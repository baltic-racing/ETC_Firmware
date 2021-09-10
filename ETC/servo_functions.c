/*
 * servo_functions.c
 *
 * Created: 05.04.2019 19:24:26
 *  Author: Ole Hannemann
 */ 

#include "servo_functions.h"
#include <avr/io.h>

volatile uint8_t blipper_enable_Flag = 0;

volatile uint8_t servo_active = 0;

extern unsigned long sys_time;
extern uint8_t apps_is_valid;
extern uint16_t etc_time;
volatile uint16_t off_ticks = OFF_TICKS_DEFAULT;
extern volatile rpm;


void servo_timer_config(){
	
	//CONFIG FOR THE SERVO CONTROL
	//USING TIMER 1 COMPARE A INTERRUPT
	//16 bit Timer 1 config
	//CTC mode and a prescaler of 8
	TCCR1B |= (1<<CS11) | (1<<WGM12);
	TIMSK1 |= (1<<OCIE1A);
	
}

uint16_t calculate_servo_ticks(double deg){
	
	if(deg < ETC_MIN_ANGLE){
		deg = ETC_MIN_ANGLE;
	}
	if(deg > ETC_MAX_ANGLE){
		deg = ETC_MAX_ANGLE;
	}
	return (uint16_t) 4200 - (deg * (2400 / SERVO_MAXANGLE)); //Hopefully invertes the PWM signal from the ty19 to the ty 20 as the servo is mounted reversly
	//original is return (uint16_t) 4200 - (deg * (2400 / SERVO_MAXANGLE));
	//4200 = 2.1 ms => Servo PWM Signal
	//2400 = 1,2 ms => differenz von 0,9 bis 2,1 ms
}

double calculate_angle(double percentage){
	//set the Lower limit for the Percentage as not optimal calibrations leads to slight deviation of the percantage cousing neagtive or percentages over 100
	if (percentage <= 1){ 
		percentage = 0;
	}
	if(percentage > 100){
		percentage = 100;	
	}
	return (double) ETC_MIN_ANGLE+((ETC_MAX_ANGLE-ETC_MIN_ANGLE)/100.0)*percentage;

}

ISR(TIMER1_COMPA_vect){
	
	//disable interrupts
	cli();
	
	switch (servo_active)
	{	
		case 0:
			//if the APPS is is valid
			if (apps_is_valid == 1){
				SERVO_ETC_PORT |= (1<<SERVO_ETC_PIN);
			}
			//set the interrupt compare value to the desired time
			OCR1A = etc_time;
			//change var to get to the next case
			off_ticks -= etc_time;
			servo_active = 1;
			break;
		case 1:
			SERVO_ETC_PORT &= ~(1<<SERVO_ETC_PIN);
			//set the interrupt compare value to the desired time
			OCR1A = off_ticks;
			off_ticks = OFF_TICKS_DEFAULT;
			//change var to get to the next case
			servo_active = 0;
			break;

	}
	//re enable interrupts
	sei();
}
