package com.mohi;

/**
 * Proiectare: digrama de clase in java
 * impartirea elementelor
 * padding
 * +tabelele si graficele
 */
public class Main {
    public static void main(String[] args) throws InterruptedException {
        System.out.println();
        String outputFile = "D:\\An2Sem2\\PDDLab1\\src\\com\\mohi\\output.txt";
        String checkFile = "D:\\An2Sem2\\PDDLab1\\src\\com\\mohi\\check.txt";
        String[] params = new String[]{"10000","10","5"};
        boolean generate = false;
        int noOfThreads = Integer.parseInt(args[0]);
        noOfThreads = 4;
        int N = Integer.parseInt(params[0]);
        int M = Integer.parseInt(params[1]);
        int n = Integer.parseInt(params[2]);
        if(generate) {
            ConvolutionLogic.generateMatrixKernel(N, M, n);
        } else {

            ConvolutionLogic runner = new ConvolutionLogic();
            if(noOfThreads>0) {
                runner.run(noOfThreads);
                //System.out.println(new PixelMatrixIO().checkContentsAreTheSame(checkFile, outputFile));
            }
            else
                runner.runSequential();
        }
    }
}
