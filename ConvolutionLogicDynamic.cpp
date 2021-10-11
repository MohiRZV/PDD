#pragma once
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
using namespace std;
class ConvolutionLogicDynamic
{
	vector<vector<int>> pixelMatrix;
	vector<vector<int>> borderedMatrix;
	vector<vector<int>> kernel;
	int kernelSize;
	int pixelMatrixNSize;
	int pixelMatrixMSize;
	vector<vector<int>> newPixelMatrix;

    string dataFile = R"(C:\Users\razva\CLionProjects\Lab1PDD\data.txt)";
    string kernelFile = R"(C:\Users\razva\CLionProjects\Lab1PDD\kernel.txt)";
    string outputFile = R"(C:\Users\razva\CLionProjects\Lab1PDD\output.txt)";

	int getColumn(int i, int columns) { return i % columns; }
	int getLine(int i, int lines) { return i / lines; }

	void read() {
		auto sizesPM = readFromFile(pixelMatrix, dataFile);
		pixelMatrixNSize = sizesPM.first;
		pixelMatrixMSize = sizesPM.second;
		auto sizesK = readFromFile(kernel, kernelFile);
		kernelSize = sizesK.first;
	}

	int computeConvolution(int line, int column) {
		int rez = 0;
		for (int i = 0; i < kernelSize; i++)
			for (int j = 0; j < kernelSize; j++)
				rez = rez + borderedMatrix[line + i][column + j] * kernel[i][j];
		return rez;
	}

	void borderMatrix(int border) {
		int lines = pixelMatrixNSize;
		int columns = pixelMatrixMSize;
		for (int i = 0; i < lines + border*2; i++) {
			borderedMatrix.push_back(vector<int>());
		}
		//top and bottom rows
		for (int i = 0; i < border; i++) {
			//first @border elements (corners)
			for (int j = 0; j < border; j++) {
				borderedMatrix[i].push_back(pixelMatrix[0][0]);
				borderedMatrix[i + border + lines].push_back(pixelMatrix[lines - 1][0]);
			}
			for (int j = 0; j < columns; j++) {
				borderedMatrix[i].push_back(pixelMatrix[0][j]);
				borderedMatrix[i + border + lines].push_back(pixelMatrix[lines - 1][j]);
			}
			//last @border elements (corners)
			for (int j = 0; j < border; j++) {
				borderedMatrix[i].push_back(pixelMatrix[0][columns-1]);
				borderedMatrix[i + border + lines].push_back(pixelMatrix[lines - 1][columns - 1]);
			}
		}
		//the rest of the matrix
		for (int i = 0; i < lines; i++) {
			for (int j = 0; j < border; j++) {
				borderedMatrix[i + border].push_back(pixelMatrix[i][0]);
			}
			for (int j = 0; j < columns; j++) {
				borderedMatrix[i + border].push_back(pixelMatrix[i][j]);
			}
			for (int j = 0; j < border; j++) {
				borderedMatrix[i + border].push_back(pixelMatrix[i][columns - 1]);
			}
		}
//		for (int i = 0; i < pixelMatrixNSize + border * 2; i++) {
//			for (int j = 0; j < pixelMatrixMSize + border * 2; j++)
//				cout << borderedMatrix[i][j] << ' ';
//			cout << '\n';
//		}
	}

	void generatePixelMatrix(int N, int M) {
		srand(time(NULL));
		int pixel;
		for (int i = 0; i < N; i++) {
			pixelMatrix.push_back(vector<int>());
			for (int j = 0; j < M; j++) {
				pixel = rand() % 256;
				pixelMatrix[i].push_back(pixel);
			}
		}
	}

	void generateKernelMatrix(int n) {
		srand(time(NULL));
		int pixel;
		for (int i = 0; i < n; i++) {
			kernel.push_back(vector<int>());
			for (int j = 0; j < n; j++) {
				pixel = rand() % 256;
				kernel[i].push_back(pixel);
			}
		}
	}

	void writePixelMatrixToFile() {
		ofstream pixelMatrixOut(dataFile);
		for (int i = 0; i < pixelMatrixNSize; i++) {
			for (int j = 0; j < pixelMatrixMSize; j++) {
				pixelMatrixOut << pixelMatrix[i][j] << " ";
			}
			pixelMatrixOut << "\n";
		}
	}

	void writeOutputMatrixToFile() {
		ofstream pixelMatrixOut(outputFile);
		for (int i = 0; i < pixelMatrixNSize; i++) {
			for (int j = 0; j < pixelMatrixMSize; j++) {
				pixelMatrixOut << newPixelMatrix[i][j] << " ";
			}
			pixelMatrixOut << "\n";
		}
	}

	void writeKernelToFile() {
		ofstream kernelOut(kernelFile);
		for (int i = 0; i < kernelSize; i++) {
			for (int j = 0; j < kernelSize; j++) {
				kernelOut << kernel[i][j] << " ";
			}
			kernelOut << "\n";
		}
	}

	pair<int,int> readFromFile(vector<vector<int>> &matrix, string file) {
		ifstream fin(file);
		int i = 0;
        int n = 0, m = 0;
		for (std::string line; getline(fin, line); )
		{
            n++;
			matrix.push_back(vector<int>());
			istringstream iss(line);
			for (string s; iss >> s;) {
                matrix[i].push_back(stoi(s));
            }
			i++;
            m=i;
		}

		return pair<int,int>(n, m);
	}

	void doWork(int start, int end) {
		for (int i = start; i < end; i++) {
			int line = getLine(i, pixelMatrixNSize);
			int column = getColumn(i, pixelMatrixMSize);
			newPixelMatrix[line][column] = computeConvolution(line, column);
		}
	}
	
public:
	
	void runConvolutionMain(int noOfThreads) {
		int border = kernelSize / 2;
		borderMatrix(border);

        //allocate newPixelMatrix
        newPixelMatrix = vector<vector<int>>(pixelMatrixNSize);
        for(int i=0;i<pixelMatrixNSize;i++) {
            newPixelMatrix[i] = vector<int>(pixelMatrixMSize);
        }
		//create the threads
		vector<thread> t;
		//split the work
		int elements = pixelMatrixNSize * pixelMatrixMSize;
		int start = 0, end = 0;
		int chunk = elements / noOfThreads;
		int remainder = elements % noOfThreads;
		for (int i = 0; i < noOfThreads; i++) {
			end = start + chunk + (remainder-- > 0);
			//TODO
			t.push_back(thread([this, start, end] {doWork(start, end); }));
			start = end;
		}
		for (int i = 0; i < noOfThreads; i++) {
			t[i].join();
		}
		writeOutputMatrixToFile();
	}

	void generateMatrixKernel(int N, int M, int n) {
		pixelMatrixNSize = N;
		pixelMatrixMSize = M;
		kernelSize = n;
		cout << N << " " << M << " " << n << "\n";
		generatePixelMatrix(N, M);
		generateKernelMatrix(n);
		writePixelMatrixToFile();
		writeKernelToFile();
	}

	void run(int noOfThreads) {
		read();
		/*kernelSize = 3;
		pixelMatrixMSize = 6;
		pixelMatrixNSize = 6;*/
		runConvolutionMain(noOfThreads);
	}
};

