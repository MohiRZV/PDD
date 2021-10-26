#pragma once
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <thread>
#include <vector>
#include "barrier.cpp"
#include <queue>
using namespace std;
class ConvolutionLogic
{
    int pixelMatrix[10005][10005];
    int kernel[105][105];
    int kernelSize;
    int pixelMatrixNSize;
    int pixelMatrixMSize;
    my_barrier* barrier;

    string dataFile = R"(data.txt)";
    string kernelFile = R"(kernel.txt)";
    string outputFile = R"(output.txt)";
    string checkFile = R"(check.txt)";

    int getColumn(int i) { return i % pixelMatrixMSize; }
    int getLine(int i) { return i / pixelMatrixMSize; }

    void read() {
        readPixelMatrixFromFile();
        readKernelFromFile();
    }

    bool isOnPadding(int l, int c) {
        int border = kernelSize / 2;
        return !(l >= border && l < pixelMatrixNSize - border && c >= border && c < pixelMatrixMSize - border);
    }

    int getLinearisedIndex(int line, int column) {
        return line * pixelMatrixMSize + column;
    }

    bool isInIndexes(int l, int c, int start, int end) {
        int linIndex = getLinearisedIndex(l, c);
        return linIndex >= start && linIndex < end;
    }

    pair<int, bool> computeConvolution(int line, int column, int start, int end) {
        int rez = 0;
        bool isOnBorder = false;
        for (int i = 0; i < kernelSize; i++)
            for (int j = 0; j < kernelSize; j++) {
                int l = line + i - kernelSize / 2;
                int c = column + j - kernelSize / 2;
                rez = rez + pixelMatrix[l][c] * kernel[i][j];

                if (!isOnBorder && !isOnPadding(l, c) && !isInIndexes(l, c, start, end)) {
                    isOnBorder = true;
                }
            }
        return make_pair(rez, isOnBorder);
    }

    void borderMatrix(int border) {
        int lines = pixelMatrixNSize;
        int columns = pixelMatrixMSize;
        //top and bottom rows
        for (int i = 0; i < border; i++) {
            //first @border elements (corners)
            int col = 0;
            for (int j = 0; j < border; j++) {
                pixelMatrix[i][col] = pixelMatrix[border][border];
                pixelMatrix[lines - border + i][col] = pixelMatrix[lines - border - 1][border];
                col++;
            }
            for (int j = border; j < columns - border; j++) {
                pixelMatrix[i][col] = pixelMatrix[border][j];
                pixelMatrix[lines - border + i][col] = pixelMatrix[lines - border - 1][j];
                col++;
            }
            //last @border elements (corners)
            for (int j = 0; j < border; j++) {
                pixelMatrix[i][col] = pixelMatrix[0][columns - border - 1];
                pixelMatrix[lines - border + i][col] = pixelMatrix[lines - border - 1][columns - border - 1];
                col++;
            }
        }
        //the rest of the matrix
        for (int i = border; i < lines; i++) {
            int col = 0;
            for (int j = 0; j < border; j++) {
                pixelMatrix[i][col] = pixelMatrix[i][border];
                col++;
            }
            col = columns - border;
            for (int j = 0; j < border; j++) {
                pixelMatrix[i][col] = pixelMatrix[i][columns - border - 1];
                col++;
            }
        }
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

    void writeOutputMatrixToFile(string file) {
        ofstream pixelMatrixOut(file);
        int border = kernelSize / 2;
        for (int i = border; i < pixelMatrixNSize - border; i++) {
            for (int j = border; j < pixelMatrixMSize - border; j++) {
                pixelMatrixOut << pixelMatrix[i][j] << " ";
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
        for (int i = kernelSize / 2; i < pixelMatrixNSize - kernelSize / 2; i++) {
            for (int j = kernelSize / 2; j < pixelMatrixMSize - kernelSize / 2; j++) {
                dataIn >> pixelMatrix[i][j];
            }
        }
    }

    struct triple {
        int i, j, value;
    };

    void doWork(int start, int end) {
        vector<triple> fronteerValues = vector<triple>();
        queue<triple> q;
        for (int i = start; i < end; i++)
        {
            //cout << i << ' ';
            int line = getLine(i);
            int column = getColumn(i);
            if (!isOnPadding(line, column)) {
                pair<int, bool> rez = computeConvolution(line, column, start, end);
                int value = rez.first;
                bool isOnBorder = rez.second;
                triple el;
                el.i = line;
                el.j = column;
                el.value = value;
                if (isOnBorder) {
                    fronteerValues.push_back(el);
                }
                else {
                    q.push(el);
                    if (q.size() >= kernelSize * kernelSize) {
                        triple t = q.front();
                        q.pop();
                        pixelMatrix[t.i][t.j] = t.value;

                    }
                }
            }
        }

        while (!q.empty()) {
            triple t = q.front();
            q.pop();
            pixelMatrix[t.i][t.j] = t.value;
        }
        

        barrier->wait();

        for (triple t : fronteerValues) {
            pixelMatrix[t.i][t.j] = t.value;
        }
    }
    void runConvolutionMain(int noOfThreads) {
        auto startTime = chrono::high_resolution_clock::now();

        int border = kernelSize / 2;
        borderMatrix(border);
        //create the threads
        vector<thread> t;
        //split the work
        int lines = pixelMatrixNSize - border * 2;
        int columns = pixelMatrixMSize - border * 2;
        int elements = lines * columns;
        int chunk = elements / noOfThreads;
        int remainder = elements % noOfThreads;
        int start = 0, end = 0;
        int currentIndexI = border;
        int currentIndexJ = border;
        barrier = new my_barrier(noOfThreads);


        for (int i = 0; i < noOfThreads; i++) {
            int no = chunk;
            if (remainder > 0) {
                no++;
                remainder--;
            }
            if (i == noOfThreads - 1)
                end = pixelMatrixNSize * pixelMatrixMSize;
            else {
                for (int j = 0; j < no; j++) {
                    end = currentIndexI * pixelMatrixMSize + currentIndexJ;
                    currentIndexJ++;
                    if (currentIndexJ >= columns + border) {
                        currentIndexJ = border;
                        currentIndexI++;
                    }

                }
            }
            t.emplace_back([this, start, end] {doWork(start, end); });
            start = end;
        }

        for (int i = 0; i < noOfThreads; i++) {
            t[i].join();
        }
        auto finishTime = chrono::high_resolution_clock::now();

        cout << chrono::duration_cast<chrono::nanoseconds>(finishTime - startTime).count() / 1000000.0 << "\n";
        writeOutputMatrixToFile(outputFile);
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
        pixelMatrixMSize = M + kernelSize/2*2;
        pixelMatrixNSize = N + kernelSize/2*2;
        read();
        vector<vector<int>> newPixelMatrix;

        newPixelMatrix = vector<vector<int>>(pixelMatrixNSize);
        for (int i = 0; i < pixelMatrixNSize; i++) {
            newPixelMatrix[i] = vector<int>(pixelMatrixMSize);
        }
        auto startTime = chrono::high_resolution_clock::now();
        int border = kernelSize / 2;
        borderMatrix(border);

        int elements = pixelMatrixNSize * pixelMatrixMSize;
        for (int line = border; line < pixelMatrixNSize - border; line++) {
            for (int column = border; column < pixelMatrixMSize - border; column++) {
                newPixelMatrix[line][column] = computeConvolution(line, column, 0, pixelMatrixMSize*pixelMatrixNSize).first;
            }
        }


        auto finishTime = chrono::high_resolution_clock::now();

        cout << chrono::duration_cast<chrono::nanoseconds>(finishTime - startTime).count() / 1000000.0 << "\n";
        
        ofstream pixelMatrixOut(checkFile);
        for (int i = border; i < pixelMatrixNSize - border; i++) {
            for (int j = border; j < pixelMatrixMSize - border; j++) {
                pixelMatrixOut << newPixelMatrix[i][j] << " ";
            }
            pixelMatrixOut << "\n";
        }
    }

    void run(int noOfThreads, int N, int M, int n) {
        kernelSize = n;
        pixelMatrixMSize = M + (kernelSize / 2) * 2;
        pixelMatrixNSize = N + (kernelSize / 2) * 2;
        read();
      
        runConvolutionMain(noOfThreads);
    }
};

