/*
 * misc_functions.c
 *
 * Created: 05.04.2019 18:51:39
 *  Author: Ole Hannemann
 */ 
#include "misc_functions.h"
//var to store the system time in
volatile unsigned long sys_time = 0;

void port_config(){
	//mark PORTE as Output
	DDRE = 0xFF;
}
void sys_tick(){
	// switch Status LED state
	PORTD ^= 1<<PD0;
}

void sys_timer_config(){
	
	//8 bit Timer 0 config
	//ctc mode and 64 as prescaler
	TCCR0A = 0 | (1<<WGM01) | (1<<CS01) | (1<<CS00);
	TIMSK0 = 0 | (1<<OCF0A); //compare interrupt enable
	OCR0A = 250-1; // compare value for 1ms;<
	
}
//ISR for Timer 0 compare interrupt
ISR(TIMER0_COMP_vect){
	
	sys_time++; //system time generation

}
