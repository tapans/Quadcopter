#ifndef FIFOQueue
#define FIFOQueue

#include <avr/io.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define QUEUESIZE 1024	

/*
*	Defines a Circular FIFO Queue
*/
typedef struct FIFOQueue {

	char buffer[QUEUESIZE];
	int front;
	int back;
	int count;
	
}FIFOQueue;

/*
*	Initialize a new FIFOQueue with front at 0, back at QUEUESIZE-1 and count at 0
*/
void init_queue(FIFOQueue *q) {
	q->front = 0;
	q->back = QUEUESIZE - 1;
	q->count = 0;
}

/*
*	Returns true if the FIFOQueue is Full, false otherwise
*/
bool full(FIFOQueue *q) {
	if(q->count >= QUEUESIZE) {
		return true;
	}
	return false;
}

/*
*	Returns true if the FIFOQueue is empty, false otherwise
*/
bool empty(FIFOQueue *q) {
	if(q->count <= 0) {
		return true;
	}
	return false;
}

/*
*	Adds a new item to the FIFOQueue
*/
bool enqueue(FIFOQueue *q, char c) {

	if(full(q) == false) {
		q->back = (q->back + 1) % QUEUESIZE;
		q->buffer[q->back] = c;
		q->count = q->count + 1;
		return true;
	}

	return false;
}

/*
*	Returns true if an item was dequeued, false otherwise
*/
bool dequeue(FIFOQueue *q, char *data) {

	if(empty(q) == false) {
		*data = q->buffer[q->front];
		q->front = (q->front + 1) % QUEUESIZE;
		q->count = q->count - 1;
		return true;
	}
	return false;
}


/*
*	Null terminates a FIFOQueue's buffer
*/
void terminate_queue(FIFOQueue *q) {
	if(full(q) == false) {
		enqueue(q, 0x0A);
		return;
	}
	q->buffer[q->back] = 0x0A;
}