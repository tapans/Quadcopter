#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>


#define FOSC 16000000UL							//CPU Clock Speed 
#define BAUD 38400 								//BAUD Rate
#define MYUBRR ((FOSC / (16UL * BAUD) ) - 1) 	//compute the UBRR prescaler
#define QUEUESIZE 128							//Default read/write queue sizes

char recByte; 									//Stores byte that was previously received
char sendByte;									//Stores byte that is about to be transmitted

/*
*	Defines a Circular FIFO Queue
*/
typedef struct FIFOQueue {

	char buffer[128];
	int front;
	int back;
	int count;

}FIFOQueue;

//Create the Queues needed for the rx/tx interrupts
FIFOQueue readQueue, writeQueue;


void USART_Init( unsigned int ubrr ) {
	
	//Set the prescaler for the BAUD as well as the BAUD
	UBRR0H = (unsigned char)(ubrr>>8); 
	UBRR0L = (unsigned char)ubrr; 

	/* Set frame format: 8data, 2stop bit */ 
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

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

/*
*	Writes a String into a FIFOQueue's buffer
*/
void writeString(FIFOQueue *q, char* str) {

	int length = strlen(str);
	int i = 0;

	while((full(q) == false) && (length-- > 0)) {
		enqueue(q, str[i]);
		i++;
	}

	terminate_queue(q);

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

int matchString(char* string1, char* string2) {
	
	if(strncmp(string1, string2, QUEUESIZE-1) == 0) {
		
		return 1;
	}
	return 0;
}


/*
*	Handles data that is read from RX
*/
void parseCommand(FIFOQueue* readQueue, FIFOQueue* writeQueue2) {
	
	//temporarily disable RX Interrupts
	disableRX();

	//terminate the readQueue so comparison can occur
	terminate_queue(readQueue);
	
	if(matchString("HELO root root", readQueue->buffer)) {
	
		//send acknowledge
		writeString(&writeQueue, "ACK");							
	
	}else if(matchString("hello", readQueue->buffer)) {
		
		writeString(&writeQueue, "Hello from the atmega328");
		
	}else if(matchString("status", readQueue->buffer)) {
		
		//Demonstrating somewhat advanced User input and parsing		
		writeString(&writeQueue, "CPU:16MHz, BAUD: 19200, UBRR: 51"); 
	
	}else {
	
		writeString(&writeQueue, "Invalid Command");
	
	}
	//Interrupt enabled transmission
	enableTX();

}

int main( void ){

	//initialize USART
	USART_Init (MYUBRR);
	
	//initialize the readqueue
	init_queue(&readQueue);
	init_queue(&writeQueue);
	
	//disable transmit
	disableTX();
	
	//enable receiver
	enableRX();
	
	//set enable interrupts
	sei();
	
	//set pin c0 to output
	DDRC |= 1<<PINC0;
	
	//light the pin
	
	while(1) {
		PORTC ^= 1<<PINC0;
		_delay_ms(200);
	}

	return 0;
}

/*
*	Interrupt fires when Data is available in UDR1
*/
ISR(USART_RX_vect){ 
	
	//Received byte gets stored here
	recByte = UDR0; 
	
	if(full(&readQueue) == true) { 	
		//since the end of string was reached this must be a command
		parseCommand(&readQueue, &writeQueue); 		
		
	}else if(recByte != 0x0A) {
		//queue the data to be read
		enqueue(&readQueue, recByte); 	
		
	}else {
		//since the end of string was reached this must be a command
		parseCommand(&readQueue, &writeQueue);		
		
	}
}	

ISR(USART_UDRE_vect) {

	if(dequeue(&writeQueue, &sendByte) == false) {
		init_queue(&readQueue);
		init_queue(&writeQueue);
		disableTX();
		enableRX();
	}else {
		UDR0 = sendByte;
	}
	
}


