package com.mohi;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.PriorityQueue;
import java.util.Queue;
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

    class Triple implements Comparable{
        int i;
        int j;
        int val;

        public Triple(int i, int j, int val) {
            this.i = i;
            this.j = j;
            this.val = val;
        }


        @Override
        public int compareTo(Object o) {
            return 0;
        }
    }
    @Override
    public void run() {
        // calculate the new values
        ArrayList<Triple> fronteerValues = new ArrayList<>();
        Queue<Triple> q = new LinkedList<>();
        for(int i=start;i<end;i++){
            int line = getLine(i);
            int column = getColumn(i);

            // if the current element is part of the padding, skip it
            if(!isOnPadding(line, column)) {
                // get new value for element [line][column], and if it is on a border or not
                Pair rez = computeConvolution(line, column);
                int value = rez.first;
                boolean isOnBorder = rez.second;
                // if on border
                if (isOnBorder) {
                    // save the value locally
                    fronteerValues.add(new Triple(line, column, value));
                } else {
                    // add the element to a queue
                    q.add(new Triple(line, column, value));
                    // if the queue is larger than the number of elements in the kernel
                    if(q.size() >= kernel.size()* kernel.size()) {
                        // the first element can be stored in the main matrix
                        Triple t = q.poll();
                        borderedMatrix.get(t.i).set(t.j, t.val);
                    }
                }
            }
        }

        // save the ramaining elements in the matrix
        while(!q.isEmpty()){
            Triple t = q.poll();
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

