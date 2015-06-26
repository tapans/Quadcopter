#ifndef rangeFinder
#define rangeFinder

#include <avr/io.h>
#include <util/delay.h>
#include "USART/USART.h"

void (*ptrParseFunction)(FIFOQueue*, FIFOQueue*);
 		
volatile uint16_t pulse_distance = 0;
volatile uint8_t pulse_flag, timing_flag;
volatile uint32_t pulse_count, pulse_time;

uint16_t toCentimeters(uint16_t result){
	return (result*15)/58;
}

uint16_t calibrate(uint16_t distance, int calibrationFactor){
	return (distance+calibrationFactor);
}


void timer_init(void){
	TCCR0 |= (1<<WGM01);
	TCCR0 |= (1<<CS00);
	OCR0=240;
	TIMSK |= (1<<OCIE0);
	
	pulse_flag=0;
	timing_flag=0;

}

void trigger(void){
	PORTB |= (1<<PIN4);
	pulse_flag=1;
	TCNT0=0;
}

ISR(TIMER0_COMP_vect){

	if (pulse_flag == 1){
		PORTB &= ~(1<<PIN4);
		pulse_flag=0;
		pulse_count=0;
	} else {
		pulse_count++;
		timing_flag=1;
	}
}

ISR(INT4_vect){
	//enableTX();
	//writeString(&writeQueue, "in INT4");
	if (timing_flag==1){
		pulse_time=pulse_count;
		timing_flag=0;
		pulse_distance=toCentimeters(pulse_time);
	}
}

void RF_init(void){
	timer_init();
	
	DDRB |= (1<<PIN4);
	//PORTB |= (1<<PIN4);
	DDRE&=~(1<<PIN4);
	PORTE |=(1<<PIN4);
	EIMSK |= (1<<INT4);
	//EICRB|=(1<<ISC41);
	EICRB|=(1<<ISC40);

}
#endif