/*
 File: scheduler.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

Scheduler::Scheduler() {
  
  ready_queue.size = 0;
  Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
	//if queue is not empty, then dequeue a thread and dispatch it to the CPU(give cpu to it)
	if (ready_queue.size>0){
		ready_queue.size--;
		Thread::dispatch_to(ready_queue.dequeue());
	}
  
}

void Scheduler::resume(Thread * _thread) {
	//push the thread at the end of queue so that we could start it later
	ready_queue.size++;
	ready_queue.enqueue(_thread);
}

void Scheduler::add(Thread * _thread) {
	//push the thread at the end of queue so that we could start it later
	ready_queue.size++;
	ready_queue.enqueue(_thread);
}

void Scheduler::terminate(Thread * _thread) {
  //first find the thread aka pull each thread and evaluate it if its the thread we are looking for
  //if found-terminate it
  //if not put the thread back to the queue
    for (int i=0;i<ready_queue.size;++i){
		Thread* temp=ready_queue.dequeue();
		if (temp->ThreadId()==_thread->ThreadId()){
			ready_queue.size--;
			break;
		}
		else{
			ready_queue.enqueue(temp);
		}
	}
}
