package com.mohi;

public class Main {
    public static void runOnce(int noOfThreads, ConvolutionLogic runner) throws InterruptedException {
        long start = System.nanoTime();
        runner.run(noOfThreads);
        long stop = System.nanoTime();
        System.out.println((stop-start)/1000000.0);
    }
    public static void main(String[] args) throws InterruptedException {
        String[] params = new String[]{"8","1000","1000","5"};
        int noOfThreads = Integer.parseInt(params[0]);
        int N = Integer.parseInt(params[1]);
        int M = Integer.parseInt(params[2]);
        int n = Integer.parseInt(params[3]);
        ConvolutionMain.generateMatrixKernel(N,M,n);
        ConvolutionLogic runner = new ConvolutionLogic();
        for(int i=0;i<200;i++)
        {
            runOnce(noOfThreads, runner);
        }
    }
}
