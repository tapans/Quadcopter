# Quadcopter
Original Contributors: Raashad Ramdeen, Tapan Shah

This repository holds the source code for the Quadcopter project from 2012-2013.

The project goals were:
- constructing a quadcopter prototype from scratch (see "initial demo.mp4" video) 
- program the AtMega128 Microcontroller to contro motor speeds and interact with the sensors to essentially self control the quadcopter
- Implement the low level details such as serial communications, PWM, I2C communication, ADC, etc without using existing libraries
(Though, we later realized that that implementing all these from scratch made the project a little too ambitious and that going with Arduino would have been a much simpler choice.
Regardless, it was an immense learning experience and we are now re-implementing the project using Arduino).

To understand the architecture of this project, please see the powerpoint presentation "architecture.pptx".

The latest repository for the project with an Arduino implementation can be found here:
https://github.com/holiday/imu

