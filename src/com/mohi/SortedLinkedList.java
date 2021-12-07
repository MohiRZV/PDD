package com.mohi;

import java.util.concurrent.locks.ReentrantLock;

class Node{
    //data
    private Monom monom;
    private Node next;
    ReentrantLock lock = new ReentrantLock();
    Node(Monom i){
        this.monom = i;
        this.next = null;
    }

    public Monom getMonom() {
        Monom monom;
        lock.lock();
        monom = this.monom;
        lock.unlock();
        return monom;
    }

    public Node getNext() {
        Node nextNode;
        lock.lock();
        nextNode = this.next;
        lock.unlock();
        return nextNode;
    }

    public void setNext(Node next) {
        lock.lock();
        this.next = next;
        lock.unlock();
    }
}


public class SortedLinkedList {

    // reference to first node
    private Node head;
    private Node sentinelStart = new Node(new Monom(-1,-1));
    private Node sentinelEnd = new Node(new Monom(9999,9999));
    SortedLinkedList(){
        //set sentinels
        head = sentinelStart;
        head.setNext(sentinelEnd);
    }

    public void insert(Monom data){
        Node newNode = new Node(data);

        Node current = head;
        Node previous = null;

        current.lock.lock();
        while(data.getExp() >= current.getMonom().getExp()){
            if(previous!=null)
                previous.lock.unlock();
            current.lock.unlock();

            previous = current;
            current = current.getNext();

            previous.lock.lock();
            current.lock.lock();
        }

        if(previous!=null && previous.getMonom().getExp() == data.getExp()){
            previous.getMonom().add(data);
        }
        else if(previous!=null)
            previous.setNext(newNode);
        newNode.setNext(current);

        if(previous!=null)
            previous.lock.unlock();
        current.lock.unlock();
    }

    public Node remove(){
        if(head == null){
            throw new RuntimeException("List is empty..");
        }

        Node temp = head;


        head = head.getNext();

        return temp;
    }

    public boolean isEmpty(){
        return (head==null);
    }
}