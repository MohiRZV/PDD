package com.mohi;

import java.io.*;
import java.util.ArrayList;

public class PixelMatrixIO {
    ArrayList<ArrayList<Integer>> readFromFile(String file, int padding) throws IOException {
        ArrayList<ArrayList<Integer>> pixelMatrix = new ArrayList<ArrayList<Integer>>();

        for (int i = 0; i < padding; i++) {
            pixelMatrix.add(new ArrayList<>());
        }
        int lineNumber = padding;
        try (BufferedReader br = new BufferedReader(new FileReader(file))) {
            String line;

            while ((line = br.readLine()) != null) {
                String[] elements = line.split(" ");
                pixelMatrix.add(new ArrayList<>());
                for (int i = 0; i < padding; i++) {
                    pixelMatrix.get(lineNumber).add(0);
                }
                for(String el : elements){
                    pixelMatrix.get(lineNumber).add(Integer.parseInt(el));
                }
                for (int i = 0; i < padding; i++) {
                    pixelMatrix.get(lineNumber).add(0);
                }
                lineNumber++;
            }
        }
        for (int i = 0; i < padding; i++) {
            pixelMatrix.add(new ArrayList<>());
            lineNumber++;
        }
        for (int i = 0; i < padding; i++) {
            for (int j = 0; j < pixelMatrix.get(padding + 1).size(); j++) {
                pixelMatrix.get(i).add(0);
                pixelMatrix.get(lineNumber-1-i).add(0);
            }
        }
//        for (int i = 0; i < pixelMatrix.size(); i++) {
//            for (int j = 0; j < pixelMatrix.get(0).size(); j++) {
//                System.out.print(pixelMatrix.get(i).get(j)+" ");
//            }
//            System.out.println();
//        }
        return pixelMatrix;
    }

    void writeToFile(String file, ArrayList<ArrayList<Integer>> pixelMatrix, int border) throws IOException {
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(file))) {
            for (int i = border; i < pixelMatrix.size() - border; i++) {
                for (int j = border; j < pixelMatrix.get(0).size() - border; j++)
                    bw.write(pixelMatrix.get(i).get(j)+" ");
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

    boolean checkContentsAreTheSame(String verifFile, String outputFile){
        try (BufferedReader brVerif = new BufferedReader(new FileReader(outputFile));
            BufferedReader brOut = new BufferedReader(new FileReader(outputFile))

            ) {
            String lineVerif;
            String lineOut;
            while ((lineVerif = brVerif.readLine()) != null && (lineOut=brOut.readLine())!=null) {
                if(!lineVerif.equals(lineOut))
                    return false;
            }
        } catch (IOException e) {
            return false;
        }
        return true;
    }
}
