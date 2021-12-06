package com.mohi;

public class Worker extends Thread{
    LLQ monomials;
    SortedLinkedList result;
    Object queueHasElementsCondVar;

    public Worker(LLQ monomials, SortedLinkedList result, Object queueHasElementsCondVar){
        this.monomials=monomials;
        this.result=result;
        this.queueHasElementsCondVar=queueHasElementsCondVar;
    }

    @Override
    public void run(){
        boolean stillHaveMonomials = true;
        while(stillHaveMonomials){

            Monom front = monomials.peek();
            if (front!=null && front.exp == 0 && front.coef == 0) {
                stillHaveMonomials = false;
            }else {
                // get monom from front of queue
                Monom monom = monomials.pop();
                try {
                    synchronized (queueHasElementsCondVar) {
                        while(monom==null||(monom.exp==0&&monom.coef==0)) {
                            queueHasElementsCondVar.wait();
                            monom = monomials.pop();
                        }
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

                if (monom != null) {
                    // if the current exponent was already added to the result
                    Monom exists = result.find(monom.exp);
                    if (exists != null) {
                        // just increase it's coefficient
                        exists.add(monom);
                    } else {
                        // add it
                        result.insert(monom);
                    }
                }
            }

        }
    }
}
