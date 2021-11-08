#include <mpi.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <chrono>
#include <cstdlib>
#define MAX_SIZE 100005
using namespace std;

unsigned char number1[MAX_SIZE], number2[MAX_SIZE];
int N_1 = 0, N_2 = 0;

void readNumbers() {

    ifstream fin1("Numar1.txt");
    ifstream fin2("Numar2.txt");
    
    char line1[MAX_SIZE];
    fin1.getline(line1, sizeof(line1) - 1);
    char line2[MAX_SIZE];
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

    fout1 << (rand() % 9) + 1;
    for (int i = 1; i < n1; i++) {
        fout1 << (rand() % 10);
    }

    fout2 << (rand() % 10) + 1;
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

    
    unsigned char number1_local[MAX_SIZE] = { 0 }, number2_local[MAX_SIZE] = { 0 };
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
    }

    // Send nr_elems through BCast
    MPI_Bcast(&nr_elems, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Scatter(number1, nr_elems, MPI_CHAR, number1_local, nr_elems, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Scatter(number2, nr_elems, MPI_CHAR, number2_local, nr_elems, MPI_CHAR, 0, MPI_COMM_WORLD);

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
    }
    // if not last, send carry to next
    if (rank < no_proc - 1) {
        //cout << "send carry " << (int)carry << " from " << rank << " to " << rank + 1 << '\n';
        MPI_Send(&carry, 1, MPI_CHAR, rank + 1, 0, MPI_COMM_WORLD);
    }

    number1_local[0] += recv_carry;



    MPI_Gather(number1_local, nr_elems, MPI_CHAR, number1, nr_elems, MPI_CHAR, 0, MPI_COMM_WORLD);

    // In root
    if (rank == 0) {
        writeSumToFile();
        auto finishTime = chrono::high_resolution_clock::now();
        cout << chrono::duration_cast<chrono::nanoseconds>(finishTime - startTime).count() / 1000000.0 << "\n";
    }

    // Finalize the MPI environment.
    MPI_Finalize();
}