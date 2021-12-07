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
            if (front!=null && front.getExp() == 0 && front.getCoef() == 0) {
                stillHaveMonomials = false;
            }else {
                // get monom from front of queue
                Monom monom = monomials.pop();

                try {
                    synchronized (queueHasElementsCondVar) {
                        while(monom==null||(monom.getCoef()==0&&monom.getExp()==0)) {
                            queueHasElementsCondVar.wait();
                            monom = monomials.pop();
                        }
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

                result.insert(monom);
            }

        }
    }
}
