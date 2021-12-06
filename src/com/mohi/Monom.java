package com.mohi;

import java.util.concurrent.locks.ReentrantLock;

public class Monom {
    int coef;
    int exp;

    ReentrantLock lock = new ReentrantLock();

    public Monom(int coef, int exp) {
        this.coef = coef;
        this.exp = exp;
    }

    public void add(Monom other){
        lock.lock();
        coef+=other.coef;
        lock.unlock();
    }
}
