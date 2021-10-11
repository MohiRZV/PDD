#pragma once
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <thread>
#include <vector>
using namespace std;
class ConvolutionLogic
{
	int pixelMatrix[100][100];
	int borderedMatrix[105][105];
	int kernel[100][100];
	int kernelSize;
	int pixelMatrixNSize;
	int pixelMatrixMSize;
	int newPixelMatrix[100][100];

	void read();
	int getColumn(int i, int columns) {return i % columns;}
	int getLine(int i, int lines) {return i / lines;}
	void doWork(int start, int end);
	void runConvolutionMain(int noOfThreads);
	void borderMatrix(int border);
	
	void writeKernelToFile();
	void writePixelMatrixToFile();
	void writeOutputMatrixToFile();
	void readKernelFromFile();
	void readPixelMatrixFromFile();
	void generatePixelMatrix(int N, int M);
	void generateKernelMatrix(int n);

	int computeConvolution(int line, int column);
public:
	void run(int noOfThreads);
	void generateMatrixKernel(int N, int M, int n);

};

