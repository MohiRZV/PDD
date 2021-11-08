#include <mpi.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <vector>
#define MAX_SIZE 100006
using namespace std;

unsigned char number1[MAX_SIZE], number2[MAX_SIZE];
int N_1 = 0, N_2 = 0;
char line1[MAX_SIZE];
char line2[MAX_SIZE];
void readNumbers() {

    ifstream fin1("Numar1.txt");
    ifstream fin2("Numar2.txt");
    
    fin1.getline(line1, sizeof(line1) - 1);
    fin2.getline(line2, sizeof(line2) - 1);

    // the numbers will be stored in the following format:
    // number[0] = the number of digits
    // number[1] = least significant digit
    // number[number[0]] = most significant digit
    for (int i = strlen(line1) - 1; i >= 0; i--)
        number1[N_1++] = line1[i] - '0';
    for (int i = strlen(line2) - 1; i >= 0; i--)
        number2[N_2++] = line2[i] - '0';

    fin1.close();
    fin2.close();
}

void computeSumSequential() {
    // carry
    int carry = 0;
    // take the biggest number of digits
    if (N_1 < N_2)
        N_1 = N_2;
    // add each digit one by one, starting with the less significant one
    for (int i = 0; i < N_1; i++, carry /= 10)
    {
        // compute the sum for the current digits, adding the previous carry
        carry = number1[i] + number2[i] + carry;
        // store the current carry
        number1[i] = carry % 10;
    }
    // if we have carry at the end, it will become the new most significant digit
    if (carry)
        number1[++N_1] = carry;
}

void writeSumToFile() {
    ofstream fout("output.txt");
    if (number1[N_1] != 0)
        fout << (int)number1[N_1];
    for (int i = N_1 - 1; i >= 0; i--)
        fout << (int)number1[i];
    fout.close();
}

void generateNumbers(int n1, int n2) {
    ofstream fout1("Numar1.txt");
    ofstream fout2("Numar2.txt");

    srand(time(0));

    fout1 << (rand() % 10) + 1;
    for (int i = 1; i < n1; i++) {
        fout1 << (rand() % 10);
    }

    fout2 << (rand() % 9) + 1;
    for (int i = 1; i < n2; i++) {
        fout2 << (rand() % 10);
    }

    fout1.close();
    fout2.close();
}

int main(int argc, char** argv) {

    //generateNumbers(100, 100000);
    //readNumbers();
    //computeSumSequential();
    //writeSumToFile();
    //return 0;

    
   
    int nr_elems = 0;
    

    int no_proc, rank;
    MPI_Status state;
    auto startTime = chrono::high_resolution_clock::now();

    readNumbers();

    // Initialise MPI
    int rc = MPI_Init(NULL, NULL);
    if (rc != MPI_SUCCESS) {
        cout << "Err init MPI";
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    // Get number of processes and the rank
    MPI_Comm_size(MPI_COMM_WORLD, &no_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //cout << "Me the thread with rank " << rank << "\n";


    // In the parent process
    if (rank == 0) {

        // take the biggest number of digits
        int max = N_1 >= N_2 ? N_1 : N_2;
        N_1 = max;
        if (max % no_proc) {
            max = ((max / no_proc) + 1)* no_proc;
        }
        // determine how many elements will be processed by each thread
        nr_elems = max / no_proc;

        int current_process = 1;
        int current = 0;
        while (current_process < no_proc) {
            MPI_Send(number1+current, nr_elems, MPI_CHAR, current_process, 1, MPI_COMM_WORLD);
            MPI_Send(number2+current, nr_elems, MPI_CHAR, current_process, 2, MPI_COMM_WORLD);
            current = current + nr_elems;
            current_process++;
        }
        unsigned char carry = 0, recv_carry = 0;
        // add each digit one by one, starting with the less significant one
        for (int i = current; i < max; i++, carry /= 10)
        {
            // compute the sum for the current digits, adding the previous carry
            carry = number1[i] + number2[i] + carry;
            // store the current carry
            number1[i] = carry % 10;
            //cout << carry % 10;
        }
        number1[max] = carry;

        //for (int i = current; i < max; i++)
        //    cout << (int)number1[i];
        //cout << '\n';
        //for (int i = current; i < max; i++)
        //    cout << (int)number2[i];
        //cout << '\n';
    }

    // Send nr_elems through BCast
    MPI_Bcast(&nr_elems, 1, MPI_INT, 0, MPI_COMM_WORLD);
    vector<char> number1_local;
    vector<char> number2_local;

    number1_local.resize(nr_elems);
    number2_local.resize(nr_elems);
    // unsigned char number1_local[MAX_SIZE] = { 0 }, number2_local[MAX_SIZE] = { 0 };
    // Receive elements if not root
    if (rank != 0) {
        MPI_Recv(number1_local.data(), nr_elems, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &state);
        MPI_Recv(number2_local.data(), nr_elems, MPI_CHAR, 0, 2, MPI_COMM_WORLD, &state);
        //for (int i = nr_elems-1; i >= 0; i--)
        //    cout << (int)number1_local[i];
        //cout << '\n';
        //for (int i = nr_elems-1; i >= 0; i--)
        //    cout << (int)number2_local[i];
        //cout << '\n';
    }
    
    // Compute sum

    // carry
    unsigned char carry = 0, recv_carry = 0;
    // add each digit one by one, starting with the less significant one
    for (int i = 0; i < nr_elems; i++, carry /= 10)
    {
        // compute the sum for the current digits, adding the previous carry
        carry = number1_local[i] + number2_local[i] + carry;
        // store the current carry
        number1_local[i] = carry % 10;
        //cout << carry % 10;
    }

    // if not root, wait for carry
    if (rank != 0) {
        MPI_Recv(&recv_carry, 1, MPI_CHAR, rank - 1, 0, MPI_COMM_WORLD, &state);
        //cout << "received carry " << (int)recv_carry << " from " << rank - 1 << " into " << rank << '\n';
        number1_local[0] = (number1_local[0] + recv_carry) % 10;
        carry = carry + (number1_local[0] + recv_carry) / 10;
    }
    // if not last, send carry to next
    if (rank < no_proc - 1) {
        //cout << "send carry " << (int)carry << " from " << rank << " to " << rank + 1 << '\n';
        MPI_Send(&carry, 1, MPI_CHAR, rank + 1, 0, MPI_COMM_WORLD);
    }

    if (rank != 0) {
        MPI_Send(number1_local.data(), nr_elems, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    // In root
    if (rank == 0) {
        int current = 0;
        for (int current_process = 1; current_process < no_proc; current_process++) {
            MPI_Recv(number1 + current, nr_elems, MPI_CHAR, current_process, 0, MPI_COMM_WORLD, &state);
            current = current + nr_elems;
        }
        writeSumToFile();
        auto finishTime = chrono::high_resolution_clock::now();
        cout << chrono::duration_cast<chrono::nanoseconds>(finishTime - startTime).count() / 1000000.0 << "\n";
    }

    // Finalize the MPI environment.
    MPI_Finalize();
}