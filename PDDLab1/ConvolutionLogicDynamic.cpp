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

	string dataFile = R"(data.txt)";
	string kernelFile = R"(kernel.txt)";
	string outputFile = R"(output.txt)";

	int getColumn(int i, int columns) { return i % columns; }
	int getLine(int i, int lines) { return i / lines; }

	void read() {
		readFromFile(pixelMatrix, pixelMatrixNSize, pixelMatrixMSize, dataFile);

		readFromFile(kernel, kernelSize, kernelSize, kernelFile);
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
		borderedMatrix = vector<vector<int>>();
		for (int i = 0; i < lines + border * 2; i++) {
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
				borderedMatrix[i].push_back(pixelMatrix[0][columns - 1]);
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

	void readFromFile(vector<vector<int>>& matrix, int n, int m, string file) {
		ifstream fin(file);
		for (int i = 0; i < n; i++) {
			matrix.push_back(vector<int>());
			for (int j = 0; j < m; j++) {
				int el = 0;
				fin >> el;
				matrix[i].push_back(el);
			}
		}
		/*for (std::string line; getline(fin, line); )
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
*/
	}

	void doWork(int start, int end) {
		for (int i = start; i < end; i++) {
			int line = getLine(i, pixelMatrixMSize);
			int column = getColumn(i, pixelMatrixMSize);
			//cout<<line<<' '<<column<<'\n';
			newPixelMatrix[line][column] = computeConvolution(line, column);
		}
	}

public:
	void runSequential(int N, int M, int n) {
		pixelMatrixNSize = N;
		pixelMatrixMSize = M;
		kernelSize = n;
		read();
		auto startTime = chrono::high_resolution_clock::now();
		//allocate newPixelMatrix
		newPixelMatrix = vector<vector<int>>(pixelMatrixNSize);
		for (int i = 0; i < pixelMatrixNSize; i++) {
			newPixelMatrix[i] = vector<int>(pixelMatrixMSize);
		}
		int border = kernelSize / 2;
		borderMatrix(border);

		int elements = pixelMatrixNSize * pixelMatrixMSize;
		doWork(0, elements);

		auto finishTime = chrono::high_resolution_clock::now();

		cout << chrono::duration_cast<chrono::nanoseconds>(finishTime - startTime).count() / 1000000.0;
		writeOutputMatrixToFile();
	}
	void runConvolutionMain(int noOfThreads) {
		auto startTime = chrono::high_resolution_clock::now();
		//allocate newPixelMatrix
		newPixelMatrix = vector<vector<int>>(pixelMatrixNSize);
		for (int i = 0; i < pixelMatrixNSize; i++) {
			newPixelMatrix[i] = vector<int>(pixelMatrixMSize);
		}
		int border = kernelSize / 2;
		borderMatrix(border);

		//create the threads
		vector<thread> t;
		//split the work
		int elements = pixelMatrixNSize * pixelMatrixMSize;
		int start = 0, end = 0;
		int chunk = elements / noOfThreads;
		int remainder = elements % noOfThreads;
		for (int i = 0; i < noOfThreads; i++) {
			end = start + chunk + (remainder-- > 0);
			t.push_back(thread([this, start, end] {doWork(start, end); }));
			start = end;
		}
		for (int i = 0; i < noOfThreads; i++) {
			t[i].join();
		}

		auto finishTime = chrono::high_resolution_clock::now();

		cout << chrono::duration_cast<chrono::nanoseconds>(finishTime - startTime).count() / 1000000.0 << "\n";
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

	void run(int noOfThreads, int N, int M, int n) {
		pixelMatrixNSize = N;
		pixelMatrixMSize = M;
		kernelSize = n;
		read();
		/*for (int i = 0; i < N; i++) {
			cout << '\n';
			for (int j = 0; j < M; j++) {
				cout << pixelMatrix[i][j] << ' ';
			}
		}*/
		/*kernelSize = 3;
		pixelMatrixMSize = 6;
		pixelMatrixNSize = 6;*/
		runConvolutionMain(noOfThreads);
	}
};

