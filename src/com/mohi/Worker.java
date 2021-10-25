package com.mohi;

import java.util.ArrayList;

public class Worker extends Thread{
    ArrayList<ArrayList<Integer>> borderedMatrix;
    ArrayList<ArrayList<Integer>> kernel;
    ArrayList<ArrayList<Integer>> newPixelMatrix;
    ArrayList<Integer> indexes;
    int border;

    public Worker(ArrayList<ArrayList<Integer>> borderedMatrix, ArrayList<ArrayList<Integer>> kernel, ArrayList<ArrayList<Integer>> newPixelMatrix, ArrayList<Integer> indexes) {
        this.borderedMatrix=borderedMatrix;
        this.kernel=kernel;
        this.newPixelMatrix=newPixelMatrix;
        this.border = kernel.size()/2;
        this.indexes = indexes;
    }


    private int computeConvolution(int line, int column) {
        int rez = 0;
        for(int i=0;i<kernel.size();i++)
            for(int j=0;j<kernel.size();j++)
                rez = rez+borderedMatrix.get(line+i-border).get(column+j-border)*kernel.get(i).get(j);
        return rez;
    }

    @Override
    public void run() {
        //System.out.println("worker started "+Thread.currentThread());
//        for (int i = start; i < end; i++) {
//            int line = getLine(i);
//            int column = getColumn(i);
//            newPixelMatrix.get(line).set(column, computeConvolution(line, column));
//        }
        indexes.forEach(i->{
            int line = getLine(i);
            int column = getColumn(i);
            newPixelMatrix.get(line).set(column, computeConvolution(line, column));
        });
    }

    private int getColumn(int i) {
        return i%borderedMatrix.get(0).size();
    }

    private int getLine(int i) {
        return i/borderedMatrix.get(0).size();
    }
}
