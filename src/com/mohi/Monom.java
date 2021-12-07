package com.mohi;

import java.util.concurrent.locks.ReentrantLock;

public class Monom {
    private int coef;
    private int exp;

    public Monom(int coef, int exp) {
        this.coef = coef;
        this.exp = exp;
    }

    synchronized public int getCoef() {
        return coef;
    }

    public void setCoef(int coef) {
        this.coef = coef;
    }

    synchronized public int getExp() {
        return exp;
    }

    public void setExp(int exp) {
        this.exp = exp;
    }

    synchronized public void add(Monom other){
        coef+=other.coef;
    }
}
