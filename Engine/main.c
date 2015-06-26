#include <avr/io.h>
#include <math.h>
#include <util/delay.h>
#include "USART/USART.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <compat/twi.h>
#include "rangeFinder.h"

#define ESC_LOW				2000	//low duty cycle count
#define ESC_HIGH			4000	//high duty cycle count
#define motors_ddr1			DDRB 	//the data direction register the motors will operate on
#define motors_ddr2 		DDRE 	//the data direction register the motors will operate on
#define north_motor 		5 		// PINB5 i.e. OCR1A
#define south_motor 		6 		// PINB6 i.e. OCR1B
#define east_motor  		7 		// PINB7 i.e. OCR1C
#define west_motor  		3 		// PINE3 i.e. OCR3A

#define SMPLRT_DIV 			0x15	//gyro register for sample rate
#define DLPF_FS 			0x16	
#define INT_CFG 			0x17
#define INT_STATUS 			0x1A	
#define TEMP_OUT_H 			0x1B	
#define TEMP_OUT_L 			0x1C
#define WHO_AM_I_W			0xD0	
#define WHO_AM_I_R			0xD1
#define GYRO_XOUT_H 		0x1D	//Address of the GYRO_XOUT_H register
#define PWR_MGM 			0x3E

#define ACCL_WHO_AM_I_W		0xA6	//Accelerometer address with Write 
#define ACCL_WHO_AM_I_R		0xA7	//Accelerometer address with read
#define ACCL_POWER_CTL		0x2D	//Accelerometer Power config register
#define ACCL_DATA_FORMAT	0x31	//Accelerometer data formatting register
#define ACCL_DATAX0			0x32	
#define ACCL_DATAX1			0x33
#define ACCL_DATAY0			0x34
#define ACCL_DATAY1			0x35
#define ACCL_DATAZ0			0x36
#define ACCL_DATAZ1			0x37

void printGyroData(void);
void printAcclData(void);


//global timer counts
volatile uint16_t count_ovf = 0; //volatile tells compiler to not optimize anything to do with this var
volatile uint16_t count_ovf_main = 0; //volatile tells compiler to not optimize anything to do with this var
int lastCount = 0;

//These values store the information needed to make an I2C read/write
void (*ptrSensCallback)(void);
volatile bool SENS_WRITE_MODE  = false;
volatile bool SENS_DATA_SENT = false;
int SENS_AD_R		=	0x00;
int SENS_AD_W		=	0x00;
int SENS_RA			=	0x00;
int SENS_RA_DATA	= 	0x00;
volatile uint8_t SENS_BURSTS = 	0;
int* SENS_DATA;

int ACCL_DATA[6]; //stores the z, y, x data respectively
int GYRO_DATA[6]; //stores the z, y, x data respectively

//these values store the degrees/sec data retrieved from the sensor
float gyro_x = 0.0;
float gyro_y = 0.0;
float gyro_z = 0.0;

//store the angles obtained by gyro data
float gyro_angle_x = 0.0;
float gyro_angle_y = 0.0;
float gyro_angle_z = 0.0;

//These are used for calibrating the gyroscope
signed int gyro_offx = 20;
signed int gyro_offy = -50;
signed int gyro_offz = 0;

//store the accelerometer current state
float accl_x = 0.0;
float accl_y = 0.0;
float accl_z = 0.0;
float accl_R = 0.0;

//store the angles given by accelerometer
float accl_angle_x = 0.0;
float accl_angle_y = 0.0;
float accl_angle_z = 0.0;

//These values store the current speed multipliers for the motors
float northMultiplier = 0.0;
float southMultiplier = 0.0;
float eastMultiplier  = 0.0;
float westMultiplier  = 0.0;

void (*ptrParseFunction)(FIFOQueue*, FIFOQueue*);
char adcResult1[16];
volatile int stop = 0;

/*
*	I2C Start command
*/
void i2c_send_start(void) {
	TWCR |= (1<<TWINT) | (1<<TWSTA) | (1<<TWEN) | (1<<TWIE);
}

/*
*	Send a byte of data via I2C
*/
void i2c_send_data(uint8_t data) {
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
}

/*
*	Stop I2C Communications
*/
void i2c_stop(void) {
	TWCR = (1<<TWINT)|(1<<TWEN)| (1<<TWSTO);
}

/*
*	Callback function for Gyroscope after data has been received
*/
void readGyroCallback(void) {
	// Put the HIGH/LOW outputs to form a 16-bit value
	// Also add an offset that will serve as the calibration factor
	gyro_x = (((SENS_DATA[5] << 8 | SENS_DATA[4]) + gyro_offx)/14.375);
	gyro_y = (((SENS_DATA[3] << 8 | SENS_DATA[2]) + gyro_offy)/14.375);
	gyro_z = (((SENS_DATA[1] << 8 | SENS_DATA[0]) + gyro_offz)/14.375);
	
//	gyro_angle_x+= (gyro_x*(count0-lastCount));
	//gyro_angle_y+= (gyro_y*(count0-lastCount));
	//gyro_angle_z+= (gyro_z*(count0-lastCount));
	//lastCount = count0;
}

void readAcclCallback(void) {
	
	int x = ((SENS_DATA[4] << 8) | SENS_DATA[5]);
	int y = ((SENS_DATA[2] << 8) | SENS_DATA[3]);
	int z = ((SENS_DATA[0] << 8) | SENS_DATA[1]);
	accl_x =  x / 256.0; // scale = 2 / (2^10), since its 10 bits
	accl_y =  y / 256.0;
	accl_z =  z / 256.0;
	accl_R = sqrt(accl_x*accl_x + accl_y*accl_y + accl_z*accl_z);
	accl_angle_x = acos(accl_x / accl_R);
	accl_angle_y = acos(accl_y / accl_R);
	accl_angle_z = acos(accl_z / accl_R);
}

/*
*	Callback function after I2C Write is complete
*/
void writeCompleteCallback(void) {
	//whatever you wanna do after a write is complete, can be done here
}

void readSensorData(int sensor_addr_r, int sensor_addr_w, int sensor_register_addr, int bursts, int* data_reg, void (*callback)(void)){
	/*
		setup generic values used in the I2C communications
		Since everything is interrupt driven we have to make sure
		all the necessary information is present for that
	*/
	SENS_AD_R = sensor_addr_r;
	SENS_AD_W = sensor_addr_w;
	SENS_RA	  = sensor_register_addr;
	SENS_BURSTS = bursts;
	SENS_DATA = data_reg;
	ptrSensCallback = callback;
	i2c_send_start();
}

void writeToSensor(int sensor_addr_w, int sensor_register_addr, int sensor_data, void (*callback)(void)) {
	
	SENS_WRITE_MODE = true; //turn on write mode
	SENS_DATA_SENT = false;
	SENS_AD_W = sensor_addr_w;
	SENS_BURSTS = 0;
	SENS_RA	  = sensor_register_addr;
	SENS_RA_DATA = sensor_data;
	ptrSensCallback = callback;
	i2c_send_start();
}

void init_i2c(void) {
	//set bitrate 100Khz
	TWBR = 0x48;
	
	/*
	TWI initialization
		TWEN - Enables two wire interface
		TWIE - Enables TWI interrupts
		TWEA - Enable Acknowledge Bit so ACK pulse is generated when slave
			   address or data bas been received
	*/
	TWCR = 1<<TWEA;
}

/*
*	Initializes the gyroscope with 125Hz sampling rate, 5Hz low pass filter
* 	and set output to 2000deg/sec
*/
void init_gyro(void) {
	//set the sample division rate for the sensor at 125Hz
	writeToSensor(WHO_AM_I_W, SMPLRT_DIV, 0x07, &writeCompleteCallback);
	//turn on 5Hz Low Pass filter, set mode to 2000deg/sec
	writeToSensor(WHO_AM_I_W, DLPF_FS, 0x1E, &writeCompleteCallback);
}

/*
*	Initializes the Accelerometer by first reset, standby and finally measure mode
*	This method must be called before reading x,y,z data
*/

void start_timer(void) {
	//select default clock source without prescaling
	TCCR0|=(1<<CS00);

	//Enable Overflow Interrupt (fires when max count value reached)
	TIMSK|=(1<<TOIE0);
	
	//initialize counter
	TCNT0=0;
}

void init_accl(void) {
	//reset the accelerometer
	writeToSensor(ACCL_WHO_AM_I_W, ACCL_POWER_CTL, 0x00, &writeCompleteCallback);
	//place in standby mode
	writeToSensor(ACCL_WHO_AM_I_W, ACCL_POWER_CTL, 0x10, &writeCompleteCallback);
	//set to full resolution, +-2g, left justified
	//writeToSensor(ACCL_WHO_AM_I_W, ACCL_DATA_FORMAT, 0x04, &writeCompleteCallback);
	//place in measurement mode
	writeToSensor(ACCL_WHO_AM_I_W, ACCL_POWER_CTL, 0x08, &writeCompleteCallback);
}

void i2c_test(void) {
	
	readSensorData(ACCL_WHO_AM_I_R, ACCL_WHO_AM_I_W, ACCL_DATAX0, 1, ACCL_DATA, &readAcclCallback);
	//readSensorData(WHO_AM_I_R, WHO_AM_I_W, GYRO_XOUT_H, 6, GYRO_DATA, &readGyroCallback);
}

void Wait(void){
   uint8_t i;
   for(i=0;i<60;i++) {
      _delay_loop_2(0);
      _delay_loop_2(0);
      _delay_loop_2(0);
   }
}

void init_pwm(void) {
	/*
		Uses 2 timers TCCR1 & TCCR3 with inverting mode.
		
		//Specifications
		Crystal 	- 16MHz
		PWM 		- Fast PWM
		Frequency  	- 100Hz
		Prescaler	- 8
		
		//Formula for calculating PWM stuff
		Top = [ cpu_clk_speed Hz / (Prescaler) * (Frequency Hz) ] - 1
		
	*/
	
	TCCR1A |= 1<<WGM11 | 1<<COM1A1 | 1<<COM1A0 | 1<<COM1B1 | 1<<COM1B0 | 1<<COM1C1 | 1<<COM1C0;
	TCCR1B |= 1<<WGM12 | 1<<WGM13 | 1<<CS11;
	TCCR3A |= 1<<WGM31 | 1<<COM3A1 | 1<<COM3A0; //uncomment to activate pwm for OCR3A i.e. PINE3
	TCCR3B |= 1<<WGM33 | 1<<WGM32 | 1<<CS31;
	
	ICR1 = 19999;
	ICR3 = 19999;
	
	//zero out so there is no signal
	OCR1A = ICR1; //north
	OCR1B = ICR1; //south
	OCR1C = ICR1; //east
	OCR3A = ICR1; //west
}

void init_motors(void) {
	
	motors_ddr1 |= (1<<north_motor) | (1<<south_motor) | (1<<east_motor);
	motors_ddr2 |= (1<<west_motor);
	
	//2ms pwm square wave
	OCR1A = ICR1 - ESC_HIGH;
	OCR1B = ICR1 - ESC_HIGH;
	OCR1C = ICR1 - ESC_HIGH;
	OCR3A = ICR1 - ESC_HIGH;
	Wait();
	
	//1ms pwm square wave
	OCR1A = ICR1 - ESC_LOW;
	OCR1B = ICR1 - ESC_LOW;
	OCR1C = ICR1 - ESC_LOW;
	OCR3A = ICR1 - ESC_LOW;
	Wait();
}


/*
*	Flatlines the PWM frequency
*/	
void stop_pwm(void) {
	
	stop = 1; //kill everything depending on this bit
	motors_ddr1 &= ~((1<<north_motor) | (1<<south_motor) | (1<<east_motor));
	motors_ddr2 &= ~(1<<west_motor);

}




void init_adc(void) {
	//enable ADC and set prescaler to 128
	ADCSRA |= 1<<ADPS0 | 1<<ADPS1 | 1<<ADPS2;
	//setup volate references
	ADMUX |= 1<<REFS0;
	//enable ADC and ADC interrupts
	ADCSRA |= 1<<ADEN | 1<<ADIE;
	
	sei();
	stop = 0;
}


void poll_adc(void) {

	while(stop == 0) {
		//get new conversion from ADC
		ADCSRA |= 1<<ADSC;
		//refresh the value every 100ms
		_delay_ms(50);
	}
}

/*
*	Sets the speed of the NORTH motor
*/
void set_speed_north(float multiplier) {

	float finalSpeed = ESC_LOW + (multiplier * (ESC_HIGH - ESC_LOW));
	OCR1A = ICR1 - finalSpeed;
}

/*
*	Sets the speed of the SOUTH motor
*/
void set_speed_south(float multiplier) {
	
	float finalSpeed = ESC_LOW + (multiplier * (ESC_HIGH - ESC_LOW));
	OCR1B = ICR1 - finalSpeed;
	
}


/*
*	Sets the speed of the EAST motor
*/
void set_speed_east(float multiplier) {

	float finalSpeed = ESC_LOW + (multiplier * (ESC_HIGH - ESC_LOW));
	OCR1C = ICR1 - finalSpeed;
}

/*
*	Sets the speed of the WEST motor
*/
void set_speed_west(float multiplier) {

	float finalSpeed = ESC_LOW + (multiplier * (ESC_HIGH - ESC_LOW));
	OCR3A = ICR1 - finalSpeed;
}

/*
*	Computes the multiplier for the NORTH motor
*/
void speed_adc_north(float speed) {
	//change the speed of the motors
	float multiplier = speed / 1024;
	set_speed_north(multiplier);
}

/*
*	Computes the multiplier for the SOUTH motor
*/
void speed_adc_south(float speed) {
	
	//change the speed of the motors
	float multiplier = speed / 1024;
	set_speed_south(multiplier);
}

/*
*	Strip letters from a given string and retains 
* 	Floating point values. Remember to run free() on the return 
* 	val.
*/
char* stripLettersFromFloat(char* string) {

	char *newStr =  malloc(strlen(string) + 1);

	int count = 0;
	for(; *string; string++) {
		if(isdigit(*string) || *string == '.') {
			newStr[count] = *string;
			count++;
		}
	}
	newStr[count] = 0x00;
	return newStr;
}

/*
*	This method checks is a string starts with a substring
*	fullString - String to be checked against
*	subsString - String that should be at the beginning of fullString
*/
bool stringContains(char* fullString, char* substring) {

	if(strncasecmp(fullString, substring, strlen(substring)) == 0) {
		return true;
	}
	return false;
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
		writeString("ACK");						
	
	}else if(matchString("hello", readQueue->buffer)) {
		
		writeString("Hello from the atmega128");
		
	}else if(matchString("status", readQueue->buffer)) {
		
		//Demonstrating somewhat advanced User input and parsing		
		writeString("ATMEGA128, CPU:16MHz, BAUD: 38400, UBRR: 25"); 
	
	}else if(matchString("init pwm", readQueue->buffer)) {
		
		//initialize the particular ESC
		init_pwm();
		writeString("Pusle Width Modulation enabled and operating on 100Hz frequency");
	
	}else if(matchString("init escs", readQueue->buffer)) {
		
		//initialize the particular ESC
		init_motors();
		writeString("Initializing motors, type 'speed test' to run a constant speed test");
		
	}else if(matchString("deactivate", readQueue->buffer)) {
		
		//initialize the particular ESC
		stop_pwm();
		writeString("Killing all running processes");
		
	}else if(matchString("speed test", readQueue->buffer)) {
		
		//initialize the particular ESC
		init_adc();
		poll_adc();
		writeString("speed test complete");
		
	}else if(matchString("get gyro", readQueue->buffer)) {
	
		printGyroData();
		
	}else if(matchString("get accl", readQueue->buffer)) {
	
		printAcclData();
		
	}else if(matchString("get distance", readQueue->buffer)) {
		
		char str[20];
		itoa(calibrate(pulse_distance, -3), str, 10);
		
		writeString(str); 
		enableTX();
	
	}
	else if(stringContains(readQueue->buffer, "set speed north")) {
	
		//Update the speed of the north motor
		char *str  = stripLettersFromFloat(readQueue->buffer);
		northMultiplier = atof(str);
		writeString(str);
		free(str);
		
	}else if(stringContains(readQueue->buffer, "set speed south")) {
	
		//Update the speed of the north motor
		char *str  = stripLettersFromFloat(readQueue->buffer);
		southMultiplier = atof(str);
		writeString(str);
		free(str);
		
	}else if(stringContains(readQueue->buffer, "set speed east")) {
	
		//Update the speed of the north motor
		char *str  = stripLettersFromFloat(readQueue->buffer);
		eastMultiplier = atof(str);
		writeString(str);
		free(str);
		
	}else if(stringContains(readQueue->buffer, "set speed west")) {
	
		//Update the speed of the north motor
		char *str  = stripLettersFromFloat(readQueue->buffer);
		westMultiplier = atof(str);
		writeString(str);
		free(str);
		
	}else {
	
		writeString("Invalid Command");
	
	}
	//Interrupt enabled transmission
	enableTX();
}

/*
*	Prints a string representation of the Gyroscope Data
*/
void printGyroData(void) {

	char gyroMsg[100];

	//sprintf(gyroMsg, "(gX) %d (gY) %d (gZ) %d", (int) gyro_angle_x, (int) gyro_angle_y, (int) gyro_angle_z);
	sprintf(gyroMsg, "(gX) %d (gY) %d (gZ) %d", (int) gyro_x, (int) gyro_y, (int) gyro_z);
	
	writeString(gyroMsg);
	enableTX();
}	


/*
*	Prints a string representation of the Accelerometer Data
*/
void printAcclData(void) {
	char acclMsg[100];
	char x[10];
	char y[10];
	char z[10];
	
	dtostrf(accl_x, 1 , 2, x);
	dtostrf(accl_y, 1 , 2, y);
	dtostrf(accl_z, 1 , 2, z);
	
	sprintf(acclMsg, "(aX) %s (aY) %s (aZ) %s", x, y, z);
	
	writeString(acclMsg);
	enableTX();
}

void printRfData(void){
	char str[20];
	sprintf(str, "(rF) %d", calibrate(pulse_distance, -3));
	writeString(str);
	enableTX();

}


int main(void) {
	
	RF_init();
	//store the pointer to the parseCommand() method
	ptrParseFunction = parseCommand;
	
	//begin USART session
	USART_Init(ptrParseFunction);
	
	//set interrputs
	sei();
	
	//init_pwm();
	
	init_i2c();
	
	init_accl();
	
	//start timer for gyro
	//start_timer(); 
	
	//as long as there is no STOP signal, keep looping
	while(1) {
		trigger();
		//_delay_ms(50);
		
		
		if (count_ovf_main >= 2500){
			count_ovf_main=0;
			printRfData();
			readSensorData(WHO_AM_I_R, WHO_AM_I_W, GYRO_XOUT_H, 6, GYRO_DATA, &readGyroCallback);
			//_delay_ms(50);
		
			//printGyroData();
		
		
		
			readSensorData(ACCL_WHO_AM_I_R, ACCL_WHO_AM_I_W, ACCL_DATAX0, 6, ACCL_DATA, &readAcclCallback);
			//printAcclData();
			//_delay_ms(50);
		}
		
	}
}

ISR(ADC_vect) {

	uint8_t theLow = ADCL;
	uint16_t theTenBitResult = ADCH<<8 | theLow;

	itoa(theTenBitResult, adcResult1, 10);
	writeString(adcResult1);
	enableTX();
	
	
	if ( ADMUX==0x40 ){
		writeString(adcResult1);
		enableTX();
		speed_adc_north(theTenBitResult);
		ADMUX=0x41;
	}
	else if( ADMUX==0x41 ){
		writeString(adcResult1);
		enableTX();
		speed_adc_south(theTenBitResult);
		ADMUX=0x40;
	}
}

ISR(TWI_vect) {
	
	char str[10];
	
	if (TW_STATUS==TW_START){
		//writeString("start");
		//enableTX();
		
		i2c_send_data(SENS_AD_W);
		
	}else if(TW_STATUS==TW_REP_START){
		//writeString("Repeat start");
		//enableTX();
		
		i2c_send_data(SENS_AD_R);
	}
	else if (TW_STATUS == TW_MT_SLA_ACK){ // Master transmit mode
		//writeString("Acknowledged I2c Address..sending Reg Addr");
		//enableTX();
	
		i2c_send_data(SENS_RA); //sending register address

	}else if (TW_STATUS==TW_MT_DATA_ACK){ //Sensor received data
		//writeString("Acknowledged Register Address");
		//enableTX();
		
		/*
			If write mode, SEND DATA here
			else send repeat START
		*/
		if(SENS_WRITE_MODE == true) {
			if(SENS_DATA_SENT == false) {
				i2c_send_data(SENS_RA_DATA);
				
				//set the flag after data sent
				SENS_DATA_SENT = true;
			}else {
				
				//turn off writing
				SENS_WRITE_MODE = false;
				i2c_stop();
				
				//callback after complete
				ptrSensCallback();
			}
			
		}else {
			i2c_send_start(); //second start
		}
		
		
	}else if(TW_STATUS==TW_MR_SLA_ACK){
		
		//writeString("Ack after AD+R...attempting to read data: ");
		//enableTX();
		
		SENS_BURSTS--;
		
		if(SENS_BURSTS > 0) {
			//Send ACK to Sensor to perform a BURST read
			//BURST is controlled by the TWEA bit, if set ACK is sent
			TWCR |= (1<<TWINT) | (1<<TWEN) | (1<<TWIE) | (1<<TWEA); 
		}else {
			//Send a NACK to the sensor to terminate transmission
			TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
		}
		
 	}else if(TW_STATUS==TW_MR_DATA_NACK){
	
		//writeString("Sending data....");
		//enableTX();
		
		SENS_DATA[SENS_BURSTS] = TWDR;
		
		//itoa(SENS_DATA[SENS_BURSTS], str, 10);
		//writeString(str);
		//enableTX();
		
		ptrSensCallback();
		
		i2c_stop();
		
	}else if(TW_STATUS==TW_MR_DATA_ACK){
		
		//writeString("Acknowledged received data MR_DATA_ACK");
		//enableTX();
		
		SENS_DATA[SENS_BURSTS] = TWDR;
		
		//itoa(SENS_DATA[SENS_BURSTS], str, 10);
		//writeString(str);
		//enableTX();
		
		SENS_BURSTS--;
		
		if(SENS_BURSTS > 0) {
			//Send ACK to Sensor to perform a BURST read
			//BURST is controlled by the TWEA bit, if set ACK is sent
			TWCR |= (1<<TWINT) | (1<<TWEN) | (1<<TWIE) | (1<<TWEA); 
		}else {
			//Send a NACK to the sensor to terminate transmission
			TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
		}
		
	}else{
		writeString("Unhandled State (Status Code):");
		enableTX();
	
		//Print out the unhandled status code
		itoa(TW_STATUS, str, 10);
		writeString(str);
		enableTX();
		
		i2c_stop(); //Stop I2C communications
	}

}
ISR(TIMER0_OVF_vect) {
	//timer/counter reached its max value=2^8=255, update global counter for timer0
		count_ovf++;
	count_ovf_main++;
}