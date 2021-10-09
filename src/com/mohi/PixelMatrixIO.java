package com.mohi;

import java.io.*;
import java.util.ArrayList;

public class PixelMatrixIO {
    ArrayList<ArrayList<Integer>> readFromFile(String file) throws IOException {
        ArrayList<ArrayList<Integer>> pixelMatrix = new ArrayList<ArrayList<Integer>>();
        try (BufferedReader br = new BufferedReader(new FileReader(file))) {
            String line;
            int lineNumber = 0;
            while ((line = br.readLine()) != null) {
                String[] elements = line.split(" ");
                pixelMatrix.add(new ArrayList<>());
                for(String el : elements){
                    pixelMatrix.get(lineNumber).add(Integer.parseInt(el));
                }
                lineNumber++;
            }
        }
        return pixelMatrix;
    }

    void writeToFile(String file, ArrayList<ArrayList<Integer>> pixelMatrix) throws IOException {
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(file))) {
            for (ArrayList<Integer> line : pixelMatrix) {
                for (int j = 0; j < pixelMatrix.get(0).size(); j++)
                    bw.write(line.get(j)+" ");
                bw.newLine();
            }
            bw.flush();
        }
    }

    void displayMatrix(ArrayList<ArrayList<Integer>> pixelMatrix) {
        for (ArrayList<Integer> line : pixelMatrix) {
            for (Integer el : line) {
                System.out.print(el + " ");
            }
            System.out.println();
        }
    }
}
