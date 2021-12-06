package com.mohi;

import java.util.concurrent.locks.ReentrantLock;

// A linked list (LL) node to store a queue entry
class QNode {
    Monom key;
    QNode next;

    // constructor to create a new linked list node
    public QNode(Monom key)
    {
        this.key = key;
        this.next = null;
    }
}

// Linked List Queue
class LLQ {

    private QNode front;
    private QNode rear;

    ReentrantLock lock = new ReentrantLock();

    public LLQ()
    {
        this.front = this.rear = null;
    }

    // add a key to the queue
    void push(Monom key)
    {
        lock.lock();
        // Create a new LL node
        QNode temp = new QNode(key);

        // If the queue is empty, the new node is both front and rear
        if (this.rear == null) {
            this.front = this.rear = temp;
            lock.unlock();
            return;
        }

        // Add the new node at the end of the queue and change rear
        this.rear.next = temp;
        this.rear = temp;
        lock.unlock();
    }

    public boolean isEmpty(){
        lock.lock();
        boolean isEmpty = (this.front==null);
        lock.unlock();
        return isEmpty;
    }

    Monom peek(){
        lock.lock();
        if(this.front!=null) {
            Monom key = this.front.key;
            lock.unlock();
            return key;
        }
        lock.unlock();
        return null;
    }

    // Method to remove a key from queue and return it
    Monom pop()
    {
        lock.lock();
        // If queue is empty, return null
        if (this.front == null) {
            lock.unlock();
            return null;
        }
        Monom currentFront = this.front.key;
        // move front one node ahead
        this.front = this.front.next;

        // If front becomes null, change rear to null
        if (this.front == null)
            this.rear = null;
        lock.unlock();
        return currentFront;

    }
}

