#include <avr/io.h> 
#include <util/delay.h>

void main(void){
	DDRB|=1<<PINB6; // Data Direction register assigning only pin6 of port B to output
	
	
	while(1){
	
		PORTB^=1<<PINB6;	// sets PINB6 to 0 or 1 (toggle)
		_delay_ms(1000); // delay of 1 second between the toggling of bit B6``
	}
}