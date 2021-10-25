#pragma once
#include <iostream>
#include <chrono>
#include "ConvolutionLogic.cpp"
#include "ConvolutionLogicDynamic.cpp"

using namespace std;

bool compareFiles() {
    ifstream finOutput("output.txt");
    ifstream finCheck("check.txt");

    int x, y;
    while (finOutput >> x) {
        finCheck >> y;
        if (x != y)
            return false;
    }
    return true;
}

int main(int argc, char* argv[])
{
    char params[10][10] = { "10000","10","5" };
    int noOfThreads = atoi(argv[1]);
    cout << "da"<<'\n';
    bool generate = 0;
    int N = atoi(params[0]);
    int M = atoi(params[1]);
    int n = atoi(params[2]);
    auto* runner = new ConvolutionLogic();
    if (generate)
        runner->generateMatrixKernel(N, M, n);
    else {
        if (noOfThreads > 0) {
            runner->run(noOfThreads, N, M, n);
            //cout<<compareFiles();
        }
        else {
            runner->runSequential(N, M, n);
        }
    }
}
