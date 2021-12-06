package com.mohi;

import java.io.*;
import java.util.Random;

public class Monomer {
    private SortedLinkedList result = new SortedLinkedList();
    private LLQ allMonoms = new LLQ();

    int threadCount;

    Monomer(int threadCount){
        this.threadCount=threadCount;
    }

    Monomer(){}

    private final int count = 5;
    int maxExp = 10000;
    int maxMonoms = 500;

    private final String baseFileName = "D:\\An2Sem2\\Lab4PPD\\src\\com\\mohi\\polynom_";


    private void readPolynoms() throws IOException {
        for(int polyNo = 0;polyNo<count;polyNo++) {
            try (BufferedReader br = new BufferedReader(new FileReader(baseFileName+polyNo+".txt"))) {
                String line;

                while ((line = br.readLine()) != null) {
                    String[] coefExp = line.split(" ");
                    int coef = Integer.parseInt(coefExp[0]);
                    int exp = Integer.parseInt(coefExp[1]);
                    allMonoms.push(new Monom(coef,exp));
                }
            }
        }
    }

    private void writeToFile(String file, SortedLinkedList polynom) throws IOException {
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(file))) {

            while(!polynom.isEmpty()) {
                Monom monom = polynom.remove().i;
                bw.write(monom.coef + " " + monom.exp);
                bw.newLine();
            }
            bw.flush();
        }
    }

    Random rnd = new Random();
    public void generatePolynoms(){

        int maxGap = maxExp/maxMonoms;
        for(int i=0;i<count;i++){
            int exp = 0;
            SortedLinkedList polynom = new SortedLinkedList();
            for(int mono=0;mono<maxMonoms;mono++){
                int coef = rnd.nextInt(100)+1;

                polynom.insert(new Monom(coef, exp));
                exp = exp+rnd.nextInt(maxMonoms)%maxGap+1;
                System.out.println(coef+" "+exp);
            }
            try {
                writeToFile(baseFileName+i+".txt", polynom);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private void addPolynoms(){
        while(!allMonoms.isEmpty()){
            // get monom from front of queue
            Monom monom = allMonoms.pop();

            // if the current exponent was already added to the result
            Monom exists = result.find(monom.exp);
            if(exists!=null){
                // just increase it's coefficient
                exists.add(monom);
            }else{
                // add it
                result.insert(monom);
            }
        }
    }

    public void run() throws InterruptedException {

        int producerCount = 2;
        int workerCount = threadCount-producerCount;

        Object queueHasElementsCondVar = new Object();

        long startTime = System.nanoTime();

        if(workerCount<=0) {// sequential
            try {
                readPolynoms();
            } catch (IOException e) {
                e.printStackTrace();
            }
            addPolynoms();
        }
        else{
            // init producers
            Thread[] producers = new Producer[producerCount];
            int start=0;
            int step = count/producerCount, remainder = count%producerCount;
            int end = step;
            for (int i = 0; i < producerCount; i++) {
                producers[i] = new Producer(allMonoms, start, end, baseFileName, queueHasElementsCondVar);
                producers[i].start();
                start=end;
                end+=step;
                if(remainder>0){
                    end++;
                    remainder--;
                }
            }

            // init consumers
            Thread[] t = new Worker[workerCount];
            for (int i = 0; i < workerCount; i++) {
                t[i] = new Worker(allMonoms, result, queueHasElementsCondVar);
                t[i].start();
            }

            for (int i = 0; i < producerCount; i++) {
                producers[i].join();
            }
            // add sentinel
            allMonoms.push(new Monom(0,0));

            for (int i = 0; i < workerCount; i++) {
                t[i].join();
            }
        }

        long stopTime = System.nanoTime();
        System.out.println((double)(stopTime-startTime)/1E6);

        try {
            writeToFile("output.txt", result);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
