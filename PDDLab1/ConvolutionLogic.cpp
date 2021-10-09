#include "ConvolutionLogic.h"

void ConvolutionLogic::run(int noOfThreads) {
	read();
	runConvolutionMain(noOfThreads);
}

void ConvolutionLogic::read() {
	readPixelMatrixFromFile();
	readKernelFromFile();
}

void ConvolutionLogic::runConvolutionMain(int noOfThreads) {
	int border = kernelSize / 2;
	borderMatrix(border);
	
	//create the threads
	thread t[10];
	//split the work
	int elements = pixelMatrixNSize * pixelMatrixMSize;
	int start = 0, end = 0;
	int chunk = elements / noOfThreads;
	int remainder = elements % noOfThreads;
	for (int i = 0; i < noOfThreads; i++) {
		end = start + chunk + (remainder-->0);
		//TODO
		t[i] = thread(doWork, start, end);
	}

	for (int i = 0; i < noOfThreads; i++) {
		t[i].join();
	}
	writeOutputMatrixToFile();
}

int getColumn(int i, int columns) {
	return i % columns;
}

int getLine(int i, int lines) {
	return i / lines;
}

int ConvolutionLogic::computeConvolution(int line, int column) {
	int rez = 0;
	for (int i = 0; i < kernelSize; i++)
		for (int j = 0; j < kernelSize; j++)
			rez = rez + borderedMatrix[line + i][column + j] * kernel[i][j];
	return rez;
}

void ConvolutionLogic::doWork(int start, int end) {
	for (int i = start; i < end; i++) {
		int line = getLine(i, pixelMatrixNSize);
		int column = getColumn(i, pixelMatrixMSize);
		newPixelMatrix[line][column] = computeConvolution(line, column);
	}
}

void ConvolutionLogic::borderMatrix(int border) {
	int lines = pixelMatrixNSize;
	int columns = pixelMatrixMSize;

	//top and bottom rows
	for (int i = 0; i < border; i++) {
		//first @border elements (corners)
		for (int j = 0; j < border; j++) {
			borderedMatrix[i][j] = pixelMatrix[0][0];
			borderedMatrix[i + border + lines][j] = pixelMatrix[lines - 1][0];
		}
		for (int j = 0; j < columns; j++) {
			borderedMatrix[i][j] = pixelMatrix[0][j];
			borderedMatrix[i + border + lines][j] = pixelMatrix[lines - 1][j];
		}
		//last @border elements (corners)
		for (int j = 0; j < border; j++) {
			borderedMatrix[i][j] = pixelMatrix[0][columns-1];
			borderedMatrix[i + border + lines][j] = pixelMatrix[lines - 1][columns - 1];
		}
	}
	//the rest of the matrix
	for (int i = 0; i < lines; i++) {
		int col = 0;
		for (int j = 0; j < border; j++) {
			borderedMatrix[i + border][col++] = pixelMatrix[i][0];
		}
		for (int j = 0; j < columns; j++) {
			borderedMatrix[i + border][col++] = pixelMatrix[i][j];
		}
		for (int j = 0; j < border; j++) {
			borderedMatrix[i + border][col++] = pixelMatrix[i][columns-1];
		}
	}
}

void ConvolutionLogic::generatePixelMatrix(int N, int M) {
	srand(time(NULL));
	int pixel;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			pixel = rand() % 256;
			pixelMatrix[i][j] = pixel;
		}
	}
}

void ConvolutionLogic::generateKernelMatrix(int n) {
	srand(time(NULL));
	int pixel;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			pixel = rand() % 256;
			kernel[i][j] = pixel;
		}
	}
}

void ConvolutionLogic::generateMatrixKernel(int N, int M, int n) {
	pixelMatrixNSize = N;
	pixelMatrixMSize = M;
	kernelSize = n;
	cout << N << " " << M << " " << n << "\n";
	generatePixelMatrix(N, M);
	generateKernelMatrix(n);
	writePixelMatrixToFile();
	writeKernelToFile();
}

void ConvolutionLogic::writePixelMatrixToFile() {
	ofstream pixelMatrixOut("data.txt");
	for (int i = 0; i < pixelMatrixNSize; i++) {
		for (int j = 0; j < pixelMatrixMSize; j++) {
			pixelMatrixOut << pixelMatrix[i][j] << " ";
		}
		pixelMatrixOut << "\n";
	}
}

void ConvolutionLogic::writeOutputMatrixToFile() {
	ofstream pixelMatrixOut("output.txt");
	for (int i = 0; i < pixelMatrixNSize; i++) {
		for (int j = 0; j < pixelMatrixMSize; j++) {
			pixelMatrixOut << newPixelMatrix[i][j] << " ";
		}
		pixelMatrixOut << "\n";
	}
}

void ConvolutionLogic::writeKernelToFile() {
	ofstream kernelOut("kernel.txt");
	for (int i = 0; i < kernelSize; i++) {
		for (int j = 0; j < kernelSize; j++) {
			kernelOut << kernel[i][j] << " ";
		}
		kernelOut << "\n";
	}
}

void ConvolutionLogic::readKernelFromFile() {
	ifstream kernelIn("kernel.txt");
	for (int i = 0; i < kernelSize; i++) {
		for (int j = 0; j < kernelSize; j++) {
			kernelIn >> kernel[i][j];
		}
	}
}

void ConvolutionLogic::readPixelMatrixFromFile() {
	ifstream dataIn("data.txt");
	for (int i = 0; i < pixelMatrixNSize; i++) {
		for (int j = 0; j < pixelMatrixMSize; j++) {
			dataIn >> pixelMatrix[i][j];
		}
	}
}