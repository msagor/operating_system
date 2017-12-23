/* 
    Author: R. Bettati, Joshua Capehart
            Department of Computer Science
            Texas A&M University
			
	    A thread scheduler.

*/
#ifndef SCHEDULER_H
#define SCHEDULER_H

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "frame_pool.H"
#include "machine.H"
#include "thread.H"
#include "utils.H"



/*--------------------------------------------------------------------------*/
/* !!! IMPLEMENTATION HINT !!! */
/*--------------------------------------------------------------------------*/
/*
    One way to proceed is to implement the FIFO scheduling policy inside
    class 'Scheduler'. 

    If you plan to implement a Round-Robin Scheduler, derive it from class
    'Scheduler', say as class 'RRScheduler'. The class 'RRScheduler' is 
    really just a FIFO scheduler with THREE MODIFICATIONS:
    1. It manages a timer, which fires at the end-of-quantum (EOQ). 
    (For details on how to set up a timer and how to handle timer interrupts 
    see the 1-second timer in 'kernel.C'.)  The timer is set up in the
    constructor.
    2. It uses an additional function, the EOQ handler. This function gets
    called whenever an EOQ timer event fires. The EOQ handler forces the 
    current thread to call the scheduler's 'yield' function.
    3. The 'yield' function must be modified to account for unused quantum
    time. If a thread voluntarily yields, the EOQ timer must be reset in order
    to not penalize the next thread.
 
    (Note that this qualifies as programming at about the level of a baboon.
     Much better woudl be to have the abstract class 'Scheduler' implement 
     the basic scheduling MECHANISMS and provide abstract funtions to define
     the queue management POLICIES in derived classes, 
     such as 'FIFOScheduler'.)
    
 */
class Queue{
	
	//public variables
	public:
    Thread* thread;
    Queue* next;
	Queue * previous;
	int size ;
	//constructor
	Queue(){
        thread=NULL;
        next=NULL;
		previous = NULL;
    }
  
    //to push a new thread in the queue
    Queue(Thread* t){
        thread=t;
        next=NULL;
		previous = NULL;
    }
	
	//push a thread into the list
	 void enqueue(Thread* t){
		 
		 //if there is no thread at the beginning = add thread
        if(thread == NULL & next == NULL){
			thread = t;
			next = NULL;
			previous = NULL;
		}
		
		//if only one thread in the queue = add another thread and make 'next' point to the newly added thread
		else if (thread!=NULL & next == NULL){
			next = new Queue(t);
			//next->previous = t;
		}
		//if there are more than one thread and 
		else if(thread!=NULL & next!= NULL){
			next->enqueue(t);
			
		}
		//if there is not thread and there is a next one = doesnt exist
		else if (thread==NULL & next!=NULL){
			Console::puts("this should never happen!");
		}
    }
	
	Thread* dequeue(){
		//list if empty= nothing to dequeue
		if(thread==NULL & next==NULL){
			return NULL;
		}
		//another thread in front of the thread being pulled
		else if (thread!=NULL & next!=NULL){
			Thread* temp_t = thread;
			Queue* temp_q = next;
			thread = next->thread;
			next = next->next;
			delete temp_q;
			return temp_t;
		}
		//if there is only one thread in the list = make the list empty
		else if(thread!=NULL & next==NULL){
			Thread* temp_t = thread;
			thread = NULL;
			next = NULL;
			previous = NULL;
			return temp_t;
		}
		//no thread but there is a next that exists = isnt possible
		else if (thread ==NULL & next!= NULL){
			thread = NULL;
			next = NULL;
			previous = NULL;
			return NULL;
		}
		
		
	}
    
 
};


/*--------------------------------------------------------------------------*/
/* SCHEDULER */
/*--------------------------------------------------------------------------*/



class Scheduler {

   /* The scheduler may need private members... */
	Queue ready_queue;
	
public:

   Scheduler();
   /* Setup the scheduler. This sets up the ready queue, for example.
      If the scheduler implements some sort of round-robin scheme, then the 
      end_of_quantum handler is installed in the constructor as well. */

   /* NOTE: We are making all functions virtual. This may come in handy when
            you want to derive RRScheduler from this class. */
  
   virtual void yield();
   /* Called by the currently running thread in order to give up the CPU. 
      The scheduler selects the next thread from the ready queue to load onto 
      the CPU, and calls the dispatcher function defined in 'Thread.H' to
      do the context switch. */

   virtual void resume(Thread * _thread);
   /* Add the given thread to the ready queue of the scheduler. This is called
      for threads that were waiting for an event to happen, or that have 
      to give up the CPU in response to a preemption. */

   virtual void add(Thread * _thread);
   /* Make the given thread runnable by the scheduler. This function is called
      after thread creation. Depending on implementation, this function may 
      just add the thread to the ready queue, using 'resume'. */

   virtual void terminate(Thread * _thread);
   /* Remove the given thread from the scheduler in preparation for destruction
      of the thread. 
      Graciously handle the case where the thread wants to terminate itself.*/
  
};
	
	

#endif
