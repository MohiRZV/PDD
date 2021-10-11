// PDDLab1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono>
#include "PDDLab1.h"
#include "ConvolutionLogic.h"

using namespace std;

void runOnce(int noOfThreads, ConvolutionLogic* runner) {
    auto start = chrono::high_resolution_clock::now();

    runner->run(noOfThreads);

    auto finish = chrono::high_resolution_clock::now();
    cout << chrono::duration_cast<chrono::nanoseconds>(finish - start).count() << "\n";

}

int main()
{
    char params[10][10] = { "4","100","100","5" };
    int noOfThreads = atoi(params[0]);
    int N = atoi(params[1]);
    int M = atoi(params[2]);
    int n = atoi(params[3]);
    ConvolutionLogic* runner = new ConvolutionLogic();
    runner->generateMatrixKernel(N, M, n);
    for (int i = 0; i < 10; i++) {
        runOnce(noOfThreads, runner);
    }
}
