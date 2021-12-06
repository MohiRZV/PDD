package com.mohi;

public class Main {

    public static void main(String[] args) throws InterruptedException {

        int noOfThreads = Integer.parseInt(args[0]);
        System.out.println();
        //noOfThreads = -1;
        //new Monomer().generatePolynoms();
        new Monomer(noOfThreads).run();
    }
}
