/*
 * safety_features.c
 *
 * Created: 03.08.2019 16:34:46
 *  Author: Ole Hannemann
 */ 

#include "safety_features.h"
#include <avr/io.h>
#include "adc_functions.h"

extern uint16_t adc_values[4];

uint8_t apps_is_valid = 0;
uint8_t tps_valid = 0;

float apps1_percentage;
float apps2_percentage;
float tps1_percentage;
float tps2_percentage;

extern volatile rpm;

int8_t timer_apps = TIMER_APPS_DEFAULT;
int8_t timer_tps = TIMER_TPS_DEFAULT;
int8_t timer_tps_to_apps = TIMER_TPS_TO_APPS_DEFAULT;
int8_t timer_tps_hard_fail = TIMER_TPS_HARD_FAIL_DEFAULT;
uint8_t tps_idle_was_ok = 1;


// Function to check APPS Signal
// return an uint from 0 to 2
// Meanings
//   0 = System Critical Signal Error
//   1 = Deviation between Sensor Signals > +/-10%
//   2 = No error detected
uint8_t check_apps(){

	if (adc_values[0] == adc_values[1]){
		//Signal lanes shorted together
		//return 1;
	}
	if(adc_values[0] <= 30 || adc_values[1] <= 30 ){
		//if APPS shorted to ground or open circuit
		return 0;
	}
	if (adc_values[0] >= 1024 || adc_values[1] >= 1024){
		//if APPS shorted to vss
		return 0;
	}
	if (apps1_percentage > apps2_percentage+APPS_DEVIATION_ALLOWED){
		return 1; //deviation between apps +10%
	}
	if (apps1_percentage < apps2_percentage-APPS_DEVIATION_ALLOWED){
		return 1;//deviation between apps -10%
	}
	
	return 2;
	
}
uint8_t check_tps(){
	if(adc_values[2] == adc_values[3]){
		//return 0; // shorted signal lanes
	}
	if (adc_values[2] == 0 || adc_values[3] == 0){ //short circuit to GND
		return 0;
	}
	if (adc_values[2] >= 1010 || adc_values[3] >= 1010){ //short circuit to vcc
		return 0;
	}
	if(tps1_percentage > tps2_percentage + TPS_DEVIATION_ALLOWED){ //deviation +5%
		return 1;
	}
	if(tps1_percentage < tps2_percentage - TPS_DEVIATION_ALLOWED){ //deviation -5%
		return 1;
	}
	return 2;
}
uint8_t apps_to_tps(){
	
	if (tps1_percentage < -25){
		return 0;
	}
	if(apps1_percentage > tps1_percentage+TPS_APPS_DEVIATION_ALLOWED){
		return 0;
	}
	if(apps1_percentage < tps1_percentage-TPS_APPS_DEVIATION_ALLOWED){
		return 0;
	}
	if(apps2_percentage > tps2_percentage+TPS_APPS_DEVIATION_ALLOWED){
		return 0;
	}
	if(apps2_percentage < tps2_percentage-TPS_APPS_DEVIATION_ALLOWED){
		return 0;
	}
	
	return 1;
	
	
}
void update_apps_percantage(){

	apps1_percentage = (100.0/((float)APPS1_MAX_VALUE-APPS1_MIN_VALUE)) * ((float)adc_get_1()-APPS1_MIN_VALUE);
	apps2_percentage = (100.0/((float)APPS2_MAX_VALUE-APPS2_MIN_VALUE)) * ((float)adc_get_2()-APPS2_MIN_VALUE);
	if(apps1_percentage < -5.0){
		apps1_percentage = -5;
	}
	if(apps2_percentage < -5.0){
		apps2_percentage = -5;
	}
}
void update_tps_percentage(){
	
	tps1_percentage = (100.0/((float)TPS1_END-TPS1_START)) * ((float)adc_get_3()-TPS1_START);
	if(tps1_percentage > 110){
		tps1_percentage = 110;
	}
	tps2_percentage = (100.0/((float)TPS2_END-TPS2_START)) * ((float)adc_get_4()-TPS2_START);
	if(tps2_percentage > 110){
		tps2_percentage = 110;
	}
}
void reset_timer_apps(){
	timer_apps = TIMER_APPS_DEFAULT;
}
void reset_timer_tps(){
	timer_tps = TIMER_TPS_DEFAULT;
}
void reset_timer_apps_to_tps(){
	timer_tps_to_apps = TIMER_TPS_TO_APPS_DEFAULT;
}
void reset_timer_tps_hard_fail(){
	timer_tps_hard_fail = TIMER_TPS_HARD_FAIL_DEFAULT;
}

void decrement_tps_timer(){
	if(timer_tps>0){
		timer_tps --;
	}
}
void decrement_apps_timer(){
	if(timer_apps>0){
		timer_apps--;
	}
}
void decrement_tps_to_apps_timer(){
	if(timer_tps_to_apps>0){
		timer_tps_to_apps--;
	}
}
void decrement_tps_hard_fail_timer(){
	if(timer_tps_hard_fail>0){
		timer_tps_hard_fail --;
	}
}

uint8_t tps_idle_check(){
	if(tps1_percentage > 0 - TPS_DEVIATION_IDLE_ALLOWED && tps1_percentage < 0 + TPS_DEVIATION_IDLE_ALLOWED){
		return 1;
	} else {
		return 0;
	}
	
}

void check_for_errors(){	
	switch(check_tps()){
		
		case 0:
		DISABLE_SHUTDOWN
		DISABLE_POWER
		timer_tps = 0;
		break;
		case 1:
			decrement_tps_timer();
			break;
		case 2:
			reset_timer_tps();
			break;
		default:
		//this should never be executed
		DISABLE_POWER
		DISABLE_SHUTDOWN
		break;
		
	}

	switch(check_apps()){
	
		case 0:
			DISABLE_POWER
			timer_apps = 0;
			break;
		case 1:
			decrement_apps_timer();
			break;
		case 2:
			reset_timer_apps();
			break;
		default:
		//this should never be executed
		DISABLE_POWER
		DISABLE_SHUTDOWN
		break;
	}

	switch(apps_to_tps()){
	
		case 0:
			decrement_tps_to_apps_timer();
			break;
		case 1:
			reset_timer_apps_to_tps();
			break;
		default:
			//this should never happen
			DISABLE_SHUTDOWN
			DISABLE_POWER
			break;
		}
	if(POWER_DISABLED){
	
		if(tps_idle_check() == 1){
			reset_timer_tps_hard_fail();
			tps_idle_was_ok = 1;
		} else{
			decrement_tps_hard_fail_timer();
			tps_idle_was_ok = 0;
		}
	}
	if(timer_tps_to_apps > 0 && timer_tps > 0 && timer_apps > 0 && timer_tps_hard_fail > 0 && tps_idle_was_ok > 0){
		ENABLE_SHUTDOWN //Enable Shutdown Circuit if ETC System is OK
		ENABLE_POWER //Enable Power to the Servo 
	
		
		apps_is_valid = 1;
	} else {
	
		if(timer_tps_to_apps <= 0) {
			DISABLE_POWER
		}
		if(timer_apps <= 0){
			DISABLE_POWER
			apps_is_valid = 0;
		}
		if(timer_tps <= 0){
			DISABLE_POWER
		}
		if(timer_tps_hard_fail <= 0){
			DISABLE_SHUTDOWN
		}
	}
}
float get_apps1_percentage(){
	return apps1_percentage;
}