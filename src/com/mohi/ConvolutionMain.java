package com.mohi;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Random;

public class ConvolutionMain {

    private static final String originalFile = "D:\\An2Sem2\\PDDLab1\\src\\com\\mohi\\data.txt";
    private static final String kernelFile = "D:\\An2Sem2\\PDDLab1\\src\\com\\mohi\\kernel.txt";
    private static final String outputFile = "D:\\An2Sem2\\PDDLab1\\src\\com\\mohi\\output.txt";
    private ArrayList<ArrayList<Integer>> pixelMatrix;
    private ArrayList<ArrayList<Integer>> kernel;
    PixelMatrixIO readerWriter = new PixelMatrixIO();
    private static final int MIN_VAL = -999;

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
//        readerWriter.displayMatrix(pixelMatrix);
//        readerWriter.displayMatrix(kernel);
    }

    void run(int noOfThreads){
        read();
        runConvolutionMain(pixelMatrix, kernel);
    }

    private void runConvolutionMain(ArrayList<ArrayList<Integer>> pixelMatrix, ArrayList<ArrayList<Integer>> kernel) {
        ArrayList<ArrayList<Integer>> newPixelMatrix = new ArrayList<>();
        for (int line = 0; line < pixelMatrix.size(); line++) {
            newPixelMatrix.add(new ArrayList<>());
            for (int column = 0; column < pixelMatrix.get(0).size(); column++) {
                newPixelMatrix.get(line).add(applyConvolution(line, column, pixelMatrix, kernel));
            }
        }
//        readerWriter.displayMatrix(newPixelMatrix);
        try {
            readerWriter.writeToFile(outputFile, newPixelMatrix);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private int applyConvolution(int line, int column, ArrayList<ArrayList<Integer>> pixelMatrix, ArrayList<ArrayList<Integer>> kernel) {
        ArrayList<ArrayList<Integer>> borderedMatrix = createBorderedMatrix(pixelMatrix, kernel, line, column);
        return computeConvolution(borderedMatrix, kernel);
    }

    private int computeConvolution(ArrayList<ArrayList<Integer>> borderedMatrix, ArrayList<ArrayList<Integer>> kernel) {
        int rez = 0;
        for(int i=0;i<borderedMatrix.size();i++)
            for(int j=0;j<borderedMatrix.size();j++)
                rez = rez+borderedMatrix.get(i).get(j)*kernel.get(i).get(j);
        return rez;
    }

    private void borderForElement(int line, int column, int kernelLines, int kernelColumns, ArrayList<ArrayList<Integer>> borderedMatrix){
        int currentElement;
        int i = 0;
        int j = 0;
        //starting from the middle element, go in all directions and fill the matrix
        //down
        currentElement = pixelMatrix.get(line).get(column);
        j = kernelColumns/2;
        for(i=kernelLines/2; i<kernelLines; i++){
            if(isInPixelMatrix(pixelMatrix, line+i-kernelLines/2, column)){
                borderedMatrix.get(i).set(j, pixelMatrix.get(line+i-kernelLines/2).get(column));
            }else{
                borderedMatrix.get(i).set(j, currentElement);
            }
        }

        //up
        currentElement = pixelMatrix.get(line).get(column);
        j = kernelColumns/2;
        for(i=kernelLines/2; i>=0; i--){
            if(isInPixelMatrix(pixelMatrix, line+i-kernelLines/2, column)){
                borderedMatrix.get(i).set(j, pixelMatrix.get(line+i-kernelLines/2).get(column));
            }else{
                borderedMatrix.get(i).set(j, currentElement);
            }
        }

        //left
        currentElement = pixelMatrix.get(line).get(column);
        i = kernelLines/2;
        for(j=kernelColumns/2; j>=0; j--){
            if(isInPixelMatrix(pixelMatrix, line, column+j-kernelColumns/2)){
                borderedMatrix.get(i).set(j, pixelMatrix.get(line).get(column+j-kernelColumns/2));
            }else{
                borderedMatrix.get(i).set(j, currentElement);
            }
        }

        //right
        currentElement = pixelMatrix.get(line).get(column);
        i = kernelLines/2;
        for(j=kernelColumns/2; j<kernelColumns; j++){
            if(isInPixelMatrix(pixelMatrix, line, column+j-kernelColumns/2)){
                borderedMatrix.get(i).set(j, pixelMatrix.get(line).get(column+j-kernelColumns/2));
            }else{
                borderedMatrix.get(i).set(j, currentElement);
            }
        }
    }

    private ArrayList<ArrayList<Integer>> createBorderedMatrix(ArrayList<ArrayList<Integer>> pixelMatrix, ArrayList<ArrayList<Integer>> kernel, int line, int column) {
        ArrayList<ArrayList<Integer>> borderedMatrix = new ArrayList<>();
        // pixelMatrix[line][column] is the central element

        int kernelLines = kernel.size();
        int kernelColumns = kernel.get(0).size();
        //create a new matrix the size of the kernel
        for(int i=0;i<kernelLines;i++) {
            borderedMatrix.add(new ArrayList<>());
            for (int j = 0; j < kernelColumns; j++) {
                int val = MIN_VAL;
                //if the kernel is overlapping in that point, add the value, otherwise add the MIN_VAL
                if(isInPixelMatrix(pixelMatrix, line+i-kernelLines/2, column+j-kernelColumns/2))
                    val = pixelMatrix.get(line+i-kernelLines/2).get(column+j-kernelColumns/2);
                borderedMatrix.get(i).add(val);
            }
        }

        for(int i=0;i<kernelLines;i++) {
            for (int j = 0; j < kernelColumns; j++) {
                //if the value has to be filled
               if(borderedMatrix.get(i).get(j)==MIN_VAL)
                   fillValue(borderedMatrix, i, j);
            }
        }
        for(int i=0;i<kernelLines;i++) {
            for (int j = 0; j < kernelColumns; j++) {
                //if the value has to be filled
                if(borderedMatrix.get(i).get(j)<=MIN_VAL)
                    borderedMatrix.get(i).set(j, -(borderedMatrix.get(i).get(j)-MIN_VAL));
            }
        }
        return borderedMatrix;
    }

    //look in each direction for the value
    private void fillValue(ArrayList<ArrayList<Integer>> borderedMatrix, int line, int column) {
        int half = borderedMatrix.size()/2;
        int i,j;
        boolean found = false;
        //down
        j = column;
        for(i=line; i<borderedMatrix.size(); i++){
            if(borderedMatrix.get(i).get(j)>MIN_VAL){
                borderedMatrix.get(line).set(column, MIN_VAL-borderedMatrix.get(i).get(j));
                found = true;
                break;
            }
        }
        if(found)
            return;
        //up
        j = column;
        for(i=line; i>0; i--){
            if(borderedMatrix.get(i).get(j)>MIN_VAL){
                borderedMatrix.get(line).set(column, MIN_VAL-borderedMatrix.get(i).get(j));
                found = true;
                break;
            }
        }
        if(found)
            return;
        //right
        i = line;
        for(j=column; j<borderedMatrix.size(); j++){
            if(borderedMatrix.get(i).get(j)>MIN_VAL){
                borderedMatrix.get(line).set(column, MIN_VAL-borderedMatrix.get(i).get(j));
                found = true;
                break;
            }
        }
        if(found)
            return;
        //left
        i = line;
        for(j=column; j>=0; j--){
            if(borderedMatrix.get(i).get(j)>MIN_VAL){
                borderedMatrix.get(line).set(column, MIN_VAL-borderedMatrix.get(i).get(j));
                found = true;
                break;
            }
        }
        if(found)
            return;
        if(borderedMatrix.get(line).get(column)==MIN_VAL)
            borderedMatrix.get(line).set(column, borderedMatrix.get(half).get(half));

    }

    private boolean isInPixelMatrix(ArrayList<ArrayList<Integer>> pixelMatrix, int line, int column) {
        return line>=0 && column>=0 && line<pixelMatrix.size() && column<pixelMatrix.get(0).size();
    }
}
