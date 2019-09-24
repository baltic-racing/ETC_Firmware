/*
 * misc_functions.h
 *
 * Created: 05.04.2019 18:51:20
 *  Author: Ole Hannemann
 */ 


#ifndef MISC_FUNCTIONS_H_
#define MISC_FUNCTIONS_H_

#include <avr/io.h>
#include <avr/interrupt.h>

// configure MCU Ports
void port_config();
// configure 8 Bit timer for System time generation
void sys_timer_config();
// interrupt routine for used timer
ISR(TIMER0_COMP_vect);
//sys_tick lets an led switch state on the pcb
void sys_tick();

#endif /* MISC_FUNCTIONS_H_ */