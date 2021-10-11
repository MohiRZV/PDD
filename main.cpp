#include <iostream>
#include <chrono>
#include "ConvolutionLogic.cpp"
#include "ConvolutionLogicDynamic.cpp"

using namespace std;

string timesFile = R"(C:\Users\razva\CLionProjects\Lab1PDD\times.txt)";
ofstream fout(timesFile);
void runOnce(int noOfThreads, ConvolutionLogicDynamic* runner) {
    auto start = chrono::high_resolution_clock::now();

    runner->run(noOfThreads);

    auto finish = chrono::high_resolution_clock::now();

    fout << chrono::duration_cast<chrono::nanoseconds>(finish - start).count() << "\n";

}

int main()
{
    char params[10][10] = { "1","5","5","3" };
    int noOfThreads = atoi(params[0]);
    int N = atoi(params[1]);
    int M = atoi(params[2]);
    int n = atoi(params[3]);
    auto* runner = new ConvolutionLogicDynamic();
    runner->generateMatrixKernel(N, M, n);
    for (int i = 0; i < 10; i++) {
        runOnce(noOfThreads, runner);
    }
}
