#include <iostream>
#include <chrono>
#include "ConvolutionLogic.cpp"
#include "ConvolutionLogicDynamic.cpp"

using namespace std;

int main(int argc, char* argv[])
{
    char params[10][10] = { "10000","10","5" };
    int noOfThreads = atoi(argv[1]);
    cout << "da"<<'\n';
    bool generate = 0;

    int N = atoi(params[0]);
    int M = atoi(params[1]);
    int n = atoi(params[2]);
    auto* runner = new ConvolutionLogicDynamic();
    if (generate)
        runner->generateMatrixKernel(N, M, n);
    else {
        if (noOfThreads > 0)
            runner->run(noOfThreads, N, M, n);
        else
            runner->runSequential(N, M, n);
    }
}
