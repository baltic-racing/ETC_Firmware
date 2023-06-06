/*
 * servo_functions.h
 *
 * Created: 05.04.2019 19:24:07
 *  Author: Ole Hannemann
 */ 


#ifndef SERVO_FUNCTIONS_H_
#define SERVO_FUNCTIONS_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define SERVO_MAXANGLE 130
#define OFF_TICKS_DEFAULT 20000
#define SERVO_ETC_PORT PORTE
#define SERVO_ETC_PIN PE2

//calibrate to 0% tps
#define ETC_MIN_ANGLE 25
//calibrate to 100%tps
#define ETC_MAX_ANGLE 50 

//function to calculate the servo angle
double calculate_angle(double percentage);
//function to calculate the servo ticks from given angle
uint16_t calculate_servo_ticks(double deg);
//configure the 16 bit timer for the servo signal generation
void servo_timer_config();

//interrupt service routine for the timer1 compare a interrupt which we are using to generate the servo signal
ISR(TIMER1_COMPA_vect);


#endif /* SERVO_FUNCTIONS_H_ */