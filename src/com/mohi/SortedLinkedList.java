package com.mohi;

import java.util.concurrent.locks.ReentrantLock;

public class SortedLinkedList {

    ReentrantLock lock = new ReentrantLock();

    // reference to first node
    private Node head;
    SortedLinkedList(){
        head = null;
    }
    // Class for nodes
    static class Node{
        //data
        Monom i;
        Node next;
        Node(Monom i){
            this.i = i;
            this.next = null;
        }
    }

    public Monom find(int exp){
        lock.lock();
        Node current = head;

        while(current != null && current.i.exp != exp){
            current = current.next;
        }


        if(current==null) {
            lock.unlock();
            return null;
        }
        Monom cm = current.i;
        lock.unlock();
        return cm;
    }

    public void insert(Monom data){
        lock.lock();
        Node newNode = new Node(data);

        Node current = head;
        Node previous = null;
        while(current != null && data.exp > current.i.exp){
            previous = current;
            current = current.next;
        }
        // insertion at beginning of the list
        if(previous == null){
            head = newNode;
        }else{
            previous.next = newNode;
        }
        newNode.next = current;
        lock.unlock();
    }

    public Node remove(){
        if(head == null){
            throw new RuntimeException("List is empty..");
        }

        Node temp = head;


        head = head.next;

        return temp;
    }

    public boolean isEmpty(){
        lock.lock();
        boolean isEmpty = (head==null);
        lock.unlock();
        return isEmpty;
    }
}