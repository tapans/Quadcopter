#include <avr/io.h>
#include <util/delay.h>
#include "../Engine/USART/USART.h"

#define RF_port				PORTB
#define RF_pin		 		PINB
#define RF_trigger_pos		PB0 
#define RF_echo_pos			PB4 		 	
#define RF_DDR			 	DDRB 		

void (*ptrParseFunction)(FIFOQueue*, FIFOQueue*);

uint16_t getWidth(void) {
	
	/*
		Uses one time TCCR1 which has two control registers: TCCR1A and TCCR1B
		
	*/
	
	uint32_t i, result;
	
	for (i=0; i<600000; i++){
	
		// 1<<RF_echo is a mask where 1 is shifted RF_echo positions into RF_port
		// if result of the if condition is 0, RF_port's pin is actually set to 1, meaning you've hit the rising edge
		if (!(RF_pin & (1<<RF_echo_pos))){
			continue; 
		}
		else{
			break;
		}
	}
		
	if (i==600000){
		return 0xffff; // timeout
	}
	
	// High edge found
	
	// Setup timer 1
	TCCR1A=0x00;	

	// we are running at 16Mhz CPU speed, lets divide by 8 to get 2Mhz clock for our timer
	TCCR1B=(1<<CS11);
	//INIT counter
	TCNT1=0x00;
		
	
	// Now wait for the falling edge	
		
	for (i=0; i<600000; i++){
		if (RF_pin & (1<<RF_echo_pos)){
		
			// TCNT counts up to 2^16, after that it starts counting from 0, do this to prevent counter reset:
			if (TCNT1 > 60000) break; continue;
		}
		else{
			break;
		}
	}
	if (i==600000)
		return 0xffff; // timeout
	
	//found falling edge
	
	result=TCNT1; //put the timer value in result when timer finishes timing the pulse width
	TCCR1B=0x00; //stop timer
	
	if (result>60000){
		return 0xfffe; // out of range
	}
	else{
		return (result>>1);
	}
}


/*
*	Handles data that is read from RX
*/
void parseCommand(FIFOQueue* readQueue, FIFOQueue* writeQueue) {
	
	//temporarily disable RX Interrupts
	disableRX();

	//terminate the readQueue so comparison can occur
	terminate_queue(readQueue);
	
	if(matchString("HELO root root", readQueue->buffer)) {
	
		//send acknowledge
		writeString(writeQueue, "ACK");						
	
	}else if(matchString("hello", readQueue->buffer)) {
		
		writeString(writeQueue, "Hello from the atmega128");
		
	}else if(matchString("status", readQueue->buffer)) {
		
		//Demonstrating somewhat advanced User input and parsing		
		writeString(writeQueue, "ATMEGA128, CPU:16MHz, BAUD: 38400, UBRR: 25"); 
		
	}else {
	
		writeString(writeQueue, "Invalid Command");
	
	}
	//Interrupt enabled transmission
	enableTX();

}


int main(void) {
	
	uint16_t r;
	
	//store the pointer to the parseCommand() method
	ptrParseFunction = parseCommand;
	
	//begin USART session
	USART_Init(ptrParseFunction);
	sei();
	
	while(1) {
		
		//set ultra sonic port to output
		RF_DDR|=(1<<RF_trigger_pos);
		
		_delay_us(10);
		
		//give 10us High pulse
		RF_port|=(1<<RF_trigger_pos); //high
		
		_delay_us(15);
		
		RF_port&=(~(1<<RF_trigger_pos)); //low
		
		_delay_us(20);
		
		//make the pin input
		//RF_DDR&=(~(1<<RF_trigger));
		
		//Measure pulse width
		r=getWidth();
		
		char str[10];
		itoa(r, str, 10);
		
		enableTX();
		writeString(&writeQueue, str);
		_delay_ms(1000);
	}
}




