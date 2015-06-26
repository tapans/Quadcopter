#include <avr/io.h>
#include <util/delay.h>
#include "USART.h"

int main( void ){

	//initialize USART
	USART_Init(MYUBRR);
	
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