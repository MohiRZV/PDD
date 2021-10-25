package com.mohi;

import java.util.ArrayList;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

public class WorkerInPlace extends Thread{
    ArrayList<ArrayList<Integer>> borderedMatrix;
    ArrayList<ArrayList<Integer>> kernel;
    int start, end;

    int border;
    CyclicBarrier barrier;

    public WorkerInPlace(ArrayList<ArrayList<Integer>> borderedMatrix, ArrayList<ArrayList<Integer>> kernel, int start, int end, CyclicBarrier barrier) {
        this.borderedMatrix = borderedMatrix;
        this.kernel = kernel;
        this.border = kernel.size()/2;
        this.start = start;
        this.end = end;
        this.barrier = barrier;
    }

    class Pair{
        int first;
        boolean second;

        public Pair(int first, boolean second) {
            this.first = first;
            this.second = second;
        }
    }

    private boolean isOnPadding(int l, int c){
        return !(l>=border && l<borderedMatrix.size()-border && c>=border && c<borderedMatrix.get(0).size()-border);
    }

    int getLinearisedIndex(int line, int column){
        return line*borderedMatrix.get(0).size() + column;
    }

    private boolean isInIndexes(int l, int c){
        int linIndex = getLinearisedIndex(l, c);
        return linIndex >= start && linIndex < end;
    }

    private Pair computeConvolution(int line, int column) {
        int rez = 0;
        boolean isOnBorder = false;
        for(int i=0;i<kernel.size();i++)
            for(int j=0;j<kernel.size();j++) {

                int l = line + i - border;
                int c = column + j - border;
                rez = rez + borderedMatrix.get(l).get(c) * kernel.get(i).get(j);

                // if by overlapping the kernel, i need values from outside of the bounds allocated to this thread
                // the current element is on a border
                if(!isOnBorder && !isOnPadding(l,c) && !isInIndexes(l,c)){
                    isOnBorder = true;
                }
            }
        return new Pair(rez, isOnBorder);
    }

    class Triple{
        int i;
        int j;
        int val;

        public Triple(int i, int j, int val) {
            this.i = i;
            this.j = j;
            this.val = val;
        }
    }
    @Override
    public void run() {
        // calculate the new values
        ArrayList<Triple> internalValues = new ArrayList<>();
        ArrayList<Triple> fronteerValues = new ArrayList<>();
        for(int i=start;i<end;i++){
            int line = getLine(i);
            int column = getColumn(i);
            if(!isOnPadding(line, column)) {
                Pair rez = computeConvolution(line, column);
                int value = rez.first;
                boolean isOnBorder = rez.second;
                if (isOnBorder) {
                    fronteerValues.add(new Triple(line, column, value));
                } else {
                    internalValues.add(new Triple(line, column, value));
                }
            }
        }

        for(Triple t : internalValues){
            borderedMatrix.get(t.i).set(t.j, t.val);
        }

        try {
            barrier.await();
        } catch (InterruptedException | BrokenBarrierException e) {
            e.printStackTrace();
        }

        for(Triple t : fronteerValues){
            borderedMatrix.get(t.i).set(t.j, t.val);
        }

    }

    private int getColumn(int i) {
        return i%borderedMatrix.get(0).size();
    }

    private int getLine(int i) {
        return i/borderedMatrix.get(0).size();
    }
}

