package com.mohi;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Random;
import java.util.concurrent.CyclicBarrier;

public class ConvolutionLogic {
    private static final String originalFile = "D:\\An2Sem2\\PDDLab1\\src\\com\\mohi\\data.txt";
    private static final String kernelFile = "D:\\An2Sem2\\PDDLab1\\src\\com\\mohi\\kernel.txt";
    private static final String outputFile = "D:\\An2Sem2\\PDDLab1\\src\\com\\mohi\\output.txt";
    private static final String checkFile = "D:\\An2Sem2\\PDDLab1\\src\\com\\mohi\\check.txt";
    private ArrayList<ArrayList<Integer>> pixelMatrix;
    private ArrayList<ArrayList<Integer>> kernel;
    PixelMatrixIO readerWriter = new PixelMatrixIO();

    private static void generateMatrix(int N,int M, String file) {
        Random rnd = new Random();
        ArrayList<ArrayList<Integer>> matrix = new ArrayList<>();
        for(int i=0;i<N;i++){
            matrix.add(new ArrayList<>());
            for(int j=0;j<M;j++)
                matrix.get(i).add(rnd.nextInt(255));
        }
        try {
            new PixelMatrixIO().writeToFile(file, matrix, 0);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    static void generateMatrixKernel(int N, int M, int n){
        //generate pixel matrix
        generateMatrix(N,M,originalFile);
        //generate kernel
        generateMatrix(n,n, kernelFile);
    }

    private void read(){
        try {
            kernel = readerWriter.readFromFile(kernelFile,0);
            pixelMatrix = readerWriter.readFromFile(originalFile, kernel.size()/2);
        } catch (IOException ex) {
            System.out.println("Something went wrong while reading the file!\n"+ex.getMessage());
        }
    }

    void run(int noOfThreads) throws InterruptedException {
        read();
        runConvolutionMain(pixelMatrix, kernel, noOfThreads);
    }

    void runSequential() {
        read();
        long startTime = System.nanoTime();
        int border = kernel.size()/2;
        ArrayList<ArrayList<Integer>> borderedMatrix = borderMatrix(pixelMatrix, border);
        //readerWriter.displayMatrix(borderedMatrix);
        ArrayList<ArrayList<Integer>> newPixelMatrix = new ArrayList<>();
        for (int line = 0; line < pixelMatrix.size(); line++) {
            newPixelMatrix.add(new ArrayList<>());
            for (int column = 0; column < pixelMatrix.get(0).size(); column++) {
                newPixelMatrix.get(line).add(-999);
            }
        }

        for (int line = border; line < pixelMatrix.size() - border; line++) {
            for (int column = border; column < pixelMatrix.get(0).size() - border; column++) {
                newPixelMatrix.get(line).set(column, computeConvolution(borderedMatrix, kernel, line-border, column-border));
            }
        }

        long stopTime = System.nanoTime();
        System.out.println((double)(stopTime-startTime)/1E6);
        try {
            readerWriter.writeToFile(checkFile, newPixelMatrix, border);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void runConvolutionMain(ArrayList<ArrayList<Integer>> pixelMatrix, ArrayList<ArrayList<Integer>> kernel, int noOfThreads) throws InterruptedException {
        long startTime = System.nanoTime();
        int border = kernel.size()/2;
        ArrayList<ArrayList<Integer>> borderedMatrix = borderMatrix(pixelMatrix, border);
        //readerWriter.displayMatrix(borderedMatrix);

        int lines = pixelMatrix.size() - border*2;
        int columns = pixelMatrix.get(0).size() - border*2;
        Thread[] t = new WorkerInPlace[noOfThreads];
        int elements = lines * columns;
        int chunk = elements / noOfThreads;
        int start=0,end=0;
        int remainder = elements % noOfThreads;
        int currentIndexI = border;
        int currentIndexJ = border;
        CyclicBarrier barrier = new CyclicBarrier(noOfThreads);

        for (int i = 0; i < noOfThreads; i++) {
            int no = chunk;
            if(remainder>0){
                no++;
                remainder--;
            }

            if(i==noOfThreads-1)
                end=pixelMatrix.size() * pixelMatrix.get(0).size();
            else {
                for (int j = 0; j < no; j++) {

                    //indexes.add(currentIndexI* pixelMatrix.get(0).size() + currentIndexJ);
                    end = currentIndexI * pixelMatrix.get(0).size() + currentIndexJ;
                    currentIndexJ++;
                    if (currentIndexJ >= columns + border) {
                        currentIndexJ = border;
                        currentIndexI++;
                    }
                }
            }
            //t[i] = new Worker(borderedMatrix, kernel, newPixelMatrix, indexes);
            t[i] = new WorkerInPlace(borderedMatrix, kernel, start, end, barrier);
            t[i].start();

            start = end;
        }

        for (int i = 0; i < noOfThreads; i++) {
            t[i].join();
        }

        long stopTime = System.nanoTime();
        System.out.println((double)(stopTime-startTime)/1E6);
        try {
            readerWriter.writeToFile(outputFile, borderedMatrix, border);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private int computeConvolution(ArrayList<ArrayList<Integer>> borderedMatrix, ArrayList<ArrayList<Integer>> kernel, int line, int column) {
        int rez = 0;
        for(int i=0;i<kernel.size();i++)
            for(int j=0;j<kernel.size();j++)
                rez = rez+borderedMatrix.get(line+i).get(column+j)*kernel.get(i).get(j);
        return rez;
    }

    private ArrayList<ArrayList<Integer>> borderMatrix(ArrayList<ArrayList<Integer>> pixelMatrix, int border) {
        int lines = pixelMatrix.size();
        int columns = pixelMatrix.get(0).size();
        //top and bottom rows
        for (int i = 0; i < border; i++) {
            int col = 0;
            //first @border elements (corners)
            for (int j = 0; j < border; j++) {
                pixelMatrix.get(i).set(col, pixelMatrix.get(border).get(border));
                pixelMatrix.get(lines - border + i).set(col, pixelMatrix.get(lines - border - 1).get(border));
                col++;
            }
            for (int j = border; j < columns - border; j++) {
                pixelMatrix.get(i).set(col, pixelMatrix.get(border).get(j));
                pixelMatrix.get(lines - border + i).set(col, pixelMatrix.get(lines - border - 1).get(j));
                col++;
            }
            //last @border elements (corners)
            for (int j = 0; j < border; j++) {
                pixelMatrix.get(i).set(col, pixelMatrix.get(border).get(columns - border - 1));
                pixelMatrix.get(lines - border + i).set(col, pixelMatrix.get(lines - border - 1).get(columns - border - 1));
                col++;
            }
        }
        //fill rest of matrix
        for (int i = border; i < lines; i++) {
            int col=0;
            for (int j = 0; j < border; j++) {
                pixelMatrix.get(i).set(col, pixelMatrix.get(i).get(border));
                col++;
            }
            col = columns - border;
            for (int j = 0; j < border; j++) {
                pixelMatrix.get(i).set(col, pixelMatrix.get(i).get(columns - border - 1));
                col++;
            }
        }
        return pixelMatrix;
    }

}
