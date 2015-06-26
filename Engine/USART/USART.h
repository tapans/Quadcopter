#ifndef USART
#define USART

#include <avr/io.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdlib.h>


#define FOSC 16000000UL							//CPU Clock Speed 
#define BAUD 9600								//BAUD Rate
#define MYUBRR ((FOSC / (16UL * BAUD) ) - 1) 	//compute the UBRR prescaler
#define QUEUESIZE 1024							//Default read/write queue sizes

char recByte; 									//Stores byte that was previously received
char sendByte;									//Stores byte that is about to be transmitted

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


//parseHandler function pointer,
//this method is called when parsing needs to occur
void (*parseHandler)(FIFOQueue*, FIFOQueue*);

//Create the Queues needed for the rx/tx interrupts
FIFOQueue readQueue, writeQueue;

/*
*	Enables RX Interrupts
*	RXEN0 - Receive Enable flag
*	RXCIE0 - Receive Complete Interrupt flag
*/	
void enableRX(void) { 
	UCSR0B |= (1<<RXEN0) | (1<<RXCIE0);
}


/*
*	Enables Transmitter and UDR Receive Interrupt
*	TXEN0 - Enables Transmitter USART0
*	UDRIE0 - Enables UDR Empty Interrupt
*/
void enableTX(void) {

	UCSR0B |= (1<<TXEN0) | (1<<UDRIE0);
}
	

/*
*	Disables RX Interrupts
*	RXEN0 - Receive Enable flag
*	RXCIE0 - Receive Complete Interrupt flag
*/
void disableRX(void) {
	UCSR0B &= ~((1<<RXEN0) | (1<<RXCIE0));
}



/*
*	Disables Transmitter and UDR Receive Interrupt
*	TXEN0 - Enables Transmitter USART0
*	UDRIE0 - Enables UDR Empty Interrupt
*/
void disableTX(void) {
	UCSR0B &= ~((1<<TXEN0)|(1<<UDRIE0));
}


/*
*	Writes a single character to USART TX. 
*/
void USARTWriteChar(char data){
	
	enableTX();
	
	while(!(UCSR0A & (1<<UDRE0))) {
		//wait until UDR0 is writeable
	}
	UDR0=data;
	
	disableTX();
	
}

/*
*	Reads a single character from RX
*/
char USARTReadChar(void){
	
   while(!(UCSR0A & (1<<RXC0))){
	//wait for the character
   }
   
   return UDR0;
}



/*
*	Writes a String into a FIFOQueue's buffer
*/
void writeString(char* str) {

	int length = strlen(str);
	int i = 0;

	while((full(&writeQueue) == false) && (length-- > 0)) {
		enqueue(&writeQueue, str[i]);
		i++;
	}

	terminate_queue(&writeQueue);
	enableTX();
}

/*
*	Prints the content of a FIFOQueue's buffer
*/
void printQueue(FIFOQueue *q) {

	char c;

	while(dequeue(q, &c)) {
		//printf("%c", c);
		USARTWriteChar(c);
	}

	//printf("\n");
}


/*
*	Returns 1 if string1 matches string2, 0 otherwise
*/

bool matchString(char* string1, char* string2) {
	
	if(strncmp(string1, string2, QUEUESIZE-1) == 0) {
		
		return true;
	}
	return false;
}

/*
*	Initializes a new UART session
*/
void USART_Init(void (*ptrParseFunction)(FIFOQueue*, FIFOQueue*)) {
	
	cli();
	//set the pointer to the function that will handle parsing
	parseHandler = (*ptrParseFunction);
	
	//Set the prescaler for the BAUD as well as the BAUD
	UBRR0H = (unsigned char)(MYUBRR>>8); 
	UBRR0L = (unsigned char)MYUBRR;

	/* Set frame format: 8data, 2stop bit */ 
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	
	//initialize the readqueue
	init_queue(&readQueue);
	init_queue(&writeQueue);
	
	//disable transmit
	disableTX();
	
	//enable receiver
	enableRX();
}

/*
*	Interrupt fires when Data is available in UDR1
*/
ISR(USART0_RX_vect){ 

	
	
	//Received byte gets stored here
	recByte = UDR0; 

	
	if(full(&readQueue) == true) { 	
		
		
		parseHandler(&readQueue, &writeQueue); 		
		
	}else if(recByte != 0x0A) {
		//queue the data to be read
		enqueue(&readQueue, recByte); 	
		
	}else if(recByte == 0x0A) {
		//since the end of string was reached this must be a command
		
		parseHandler(&readQueue, &writeQueue);		
		
	}else{
		
	}
	
}	

ISR(USART0_UDRE_vect) {

	if(dequeue(&writeQueue, &sendByte) == false) {
		init_queue(&readQueue);
		init_queue(&writeQueue);
		disableTX();
		enableRX();
	}else {
		UDR0 = sendByte;
	}
	
}
#endif