package com.mohi;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

public class Producer extends Thread{

    LLQ queue;
    int start, end;
    String baseFileName;
    Object queueHasElementsCondVar;
    Producer(LLQ queue, int start, int end, String baseFileName, Object queueHasElementsCondVar){
        this.queue=queue;
        this.start=start;
        this.end=end;
        this.baseFileName=baseFileName;
        this.queueHasElementsCondVar = queueHasElementsCondVar;
    }

    @Override
    public void run() {
        for(int polyNo = start;polyNo<end;polyNo++) {
            try (BufferedReader br = new BufferedReader(new FileReader(baseFileName+polyNo+".txt"))) {
                String line;

                while ((line = br.readLine()) != null) {
                    String[] coefExp = line.split(" ");
                    int coef = Integer.parseInt(coefExp[0]);
                    int exp = Integer.parseInt(coefExp[1]);
                    queue.push(new Monom(coef,exp));
                    synchronized (queueHasElementsCondVar) {
                        queueHasElementsCondVar.notifyAll();
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
