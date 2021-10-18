#pragma once
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <thread>
#include <vector>
using namespace std;
class ConvolutionLogic
{
    int pixelMatrix[10005][10005];
    int borderedMatrix[10005][10005];
    int kernel[105][105];
    int kernelSize;
    int pixelMatrixNSize;
    int pixelMatrixMSize;
    int newPixelMatrix[10005][10005];

    string dataFile = R"(data.txt)";
    string kernelFile = R"(kernel.txt)";
    string outputFile = R"(output.txt)";

    static int getColumn(int i, int columns) { return i % columns; }
    static int getLine(int i, int lines) { return i / lines; }

    void read() {
        readPixelMatrixFromFile();
        readKernelFromFile();
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
        //top and bottom rows
        for (int i = 0; i < border; i++) {
            //first @border elements (corners)
            int col = 0;
            for (int j = 0; j < border; j++) {
                borderedMatrix[i][col] = pixelMatrix[0][0];
                borderedMatrix[i + border + lines][col] = pixelMatrix[lines - 1][0];
                col++;
            }
            for (int j = 0; j < columns; j++) {
                borderedMatrix[i][col] = pixelMatrix[0][j];
                borderedMatrix[i + border + lines][col] = pixelMatrix[lines - 1][j];
                col++;
            }
            //last @border elements (corners)
            for (int j = 0; j < border; j++) {
                borderedMatrix[i][col] = pixelMatrix[0][columns - 1];
                borderedMatrix[i + border + lines][col] = pixelMatrix[lines - 1][columns - 1];
                col++;
            }
        }
        //the rest of the matrix
        for (int i = 0; i < lines; i++) {
            int col = 0;
            for (int j = 0; j < border; j++) {
                borderedMatrix[i + border][col] = pixelMatrix[i][0];
                col++;
            }
            for (int j = 0; j < columns; j++) {
                borderedMatrix[i + border][col] = pixelMatrix[i][j];
                col++;
            }
            for (int j = 0; j < border; j++) {
                borderedMatrix[i + border][col] = pixelMatrix[i][columns - 1];
                col++;
            }
        }
       /* for (int i = 0; i < pixelMatrixNSize + border * 2; i++) {
            for (int j = 0; j < pixelMatrixMSize + border * 2; j++)
                cout << borderedMatrix[i][j] << ' ';
            cout << '\n';
        }*/
    }

    void generatePixelMatrix(int N, int M) {
        srand(time(NULL));
        int pixel;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                pixel = rand() % 256;
                pixelMatrix[i][j] = pixel;
            }
        }
    }

    void generateKernelMatrix(int n) {
        srand(time(NULL));
        int pixel;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                pixel = rand() % 256;
                kernel[i][j] = pixel;
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

    void readKernelFromFile() {
        ifstream kernelIn(kernelFile);
        for (int i = 0; i < kernelSize; i++) {
            for (int j = 0; j < kernelSize; j++) {
                kernelIn >> kernel[i][j];
            }
        }
    }

    void readPixelMatrixFromFile() {
        ifstream dataIn(dataFile);
        for (int i = 0; i < pixelMatrixNSize; i++) {
            for (int j = 0; j < pixelMatrixMSize; j++) {
                dataIn >> pixelMatrix[i][j];
            }
        }
    }

    void doWork(int start, int end) {
        for (int i = start; i < end; i++) {
            int line = getLine(i, pixelMatrixMSize);
            int column = getColumn(i, pixelMatrixMSize);
            newPixelMatrix[line][column] = computeConvolution(line, column);
        }
    }
    void runConvolutionMain(int noOfThreads) {
        auto startTime = chrono::high_resolution_clock::now();
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
            t.emplace_back([this, start, end] {doWork(start, end); });
            start = end;
        }

        for (int i = 0; i < noOfThreads; i++) {
            t[i].join();
        }

        auto finishTime = chrono::high_resolution_clock::now();

        cout << chrono::duration_cast<chrono::nanoseconds>(finishTime - startTime).count() / 1000000.0 << "\n";
        writeOutputMatrixToFile();
    }

    

public:
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

    void runSequential(int N, int M, int n) {
        kernelSize = n;
        pixelMatrixMSize = M;
        pixelMatrixNSize = N;
        read();
        auto startTime = chrono::high_resolution_clock::now();
        int border = kernelSize / 2;
        borderMatrix(border);

        int elements = pixelMatrixNSize * pixelMatrixMSize;
        doWork(0, elements-1);

        auto finishTime = chrono::high_resolution_clock::now();

        cout << chrono::duration_cast<chrono::nanoseconds>(finishTime - startTime).count() / 1000000.0 << "\n";
        writeOutputMatrixToFile();
    }

    void run(int noOfThreads, int N, int M, int n) {
        kernelSize = n;
        pixelMatrixMSize = M;
        pixelMatrixNSize = N;
        read();
        runConvolutionMain(noOfThreads);
    }
};

