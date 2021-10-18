package com.mohi;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Random;

public class ConvolutionLogic {
    private static final String originalFile = "D:\\An2Sem2\\PDDLab1\\src\\com\\mohi\\data.txt";
    private static final String kernelFile = "D:\\An2Sem2\\PDDLab1\\src\\com\\mohi\\kernel.txt";
    private static final String outputFile = "D:\\An2Sem2\\PDDLab1\\src\\com\\mohi\\output.txt";
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
            new PixelMatrixIO().writeToFile(file, matrix);
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
            pixelMatrix = readerWriter.readFromFile(originalFile);
            kernel = readerWriter.readFromFile(kernelFile);
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

        for (int line = 0; line < pixelMatrix.size(); line++) {
            for (int column = 0; column < pixelMatrix.get(0).size(); column++) {
                newPixelMatrix.get(line).set(column, computeConvolution(borderedMatrix, kernel, line, column));
            }
        }

        long stopTime = System.nanoTime();
        System.out.println((double)(stopTime-startTime)/1E6);
        try {
            readerWriter.writeToFile(outputFile, newPixelMatrix);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void runConvolutionMain(ArrayList<ArrayList<Integer>> pixelMatrix, ArrayList<ArrayList<Integer>> kernel, int noOfThreads) throws InterruptedException {
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
//        for (int line = 0; line < pixelMatrix.size(); line++) {
//            newPixelMatrix.add(new ArrayList<>());
//            for (int column = 0; column < pixelMatrix.get(0).size(); column++) {
//                newPixelMatrix.get(line).add(computeConvolution(borderedMatrix, kernel, line+border-1, column+border-1));
//            }
//        }
        Thread[] t = new Worker[noOfThreads];
        int elements = pixelMatrix.size() * pixelMatrix.get(0).size();
        int start = 0, end = 0;
        int chunk = elements/noOfThreads;
        int remainder = elements%noOfThreads;
        for (int i = 0; i < noOfThreads; i++) {
            end = start+chunk;
            if(remainder>0){
                remainder--;
                end++;
            }
            t[i] = new Worker(borderedMatrix, kernel, newPixelMatrix, start, end);
            t[i].start();
            start=end;
        }
        for (int i = 0; i < noOfThreads; i++) {
            t[i].join();
        }
        long stopTime = System.nanoTime();
        System.out.println((double)(stopTime-startTime)/1E6);
        try {
            readerWriter.writeToFile(outputFile, newPixelMatrix);
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
        ArrayList<ArrayList<Integer>> borderedMatrix = new ArrayList<>();
        int lines = pixelMatrix.size();
        int columns = pixelMatrix.get(0).size();
        for (int i = 0; i < lines + border*2; i++) {
            borderedMatrix.add(new ArrayList<>());
        }
        //top and bottom rows
        for (int i = 0; i < border; i++) {
            //first @border elements (corners)
            for (int j = 0; j < border; j++) {
                borderedMatrix.get(i).add(pixelMatrix.get(0).get(0));
                borderedMatrix.get(i + border + lines).add(pixelMatrix.get(lines-1).get(0));
            }
            for (int j = 0; j < columns; j++) {
                borderedMatrix.get(i).add(pixelMatrix.get(0).get(j));
                borderedMatrix.get(i + border + lines).add(pixelMatrix.get(lines-1).get(j));
            }
            //last @border elements (corners)
            for (int j = 0; j < border; j++) {
                borderedMatrix.get(i).add(pixelMatrix.get(0).get(columns-1));
                borderedMatrix.get(i + border + lines).add(pixelMatrix.get(lines-1).get(columns-1));
            }
        }
        //fill rest of matrix
        for (int i = 0; i < lines; i++) {
            for (int j = 0; j < border; j++) {
                borderedMatrix.get(i+border).add(pixelMatrix.get(i).get(0));
            }
            for (int j = 0; j < columns; j++) {
                borderedMatrix.get(i+border).add(pixelMatrix.get(i).get(j));
            }
            for (int j = 0; j < border; j++) {
                borderedMatrix.get(i+border).add(pixelMatrix.get(i).get(columns-1));
            }
        }
        return borderedMatrix;
    }

}
