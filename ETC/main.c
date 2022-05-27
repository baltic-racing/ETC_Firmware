/*
 * ETC.c
 *
 * Created: 20.04.2019 20:49:57
 * Author : Ole Hannemann
 */ 



#include <avr/io.h>
#include <avr/interrupt.h>
#include "canlib.h"
#include "servo_functions.h"
#include "misc_functions.h"
#include "adc_functions.h"
#include "safety_features.h"

volatile uint16_t etc_time = 2000;

uint8_t sys_time_10 = 0;	//10ms Timer
uint8_t sys_time_50 = 0;	//50ms Timer
volatile uint16_t rpm = 0;
unsigned long time_old = 0;
extern unsigned long sys_time;

//percentages for the used sensor to store into the CAN MOB
extern float apps1_percentage;
extern float apps2_percentage;
extern float tps1_percentage;
extern float tps2_percentage;
//safety Timers for different safety functions (1 = 10ms)
extern int8_t timer_apps;
extern int8_t timer_tps;
extern int8_t timer_tps_to_apps;
extern int8_t timer_tps_hard_fail;
volatile uint8_t Blipper_Enable; 
volatile uint8_t Anti_Blipper_Enable; 

float apps1_percentage_ext;
float apps2_percentage_ext;
float tps1_percentage_ext;
float tps2_percentage_ext;


int main(void){
	//config various perepherie
    port_config();
	adc_config();
	servo_timer_config();
	sys_timer_config();
	//configure the CAN Interface for 1MBaud
	can_cfg();
	
	//definition of the mob to send
	struct CAN_MOB etc_mob;
	//mob ID for the MOB we are sending
	etc_mob.mob_id = 0x201;
	// IDMASK for the mobs to receive (see datasheet page 265)
	etc_mob.mob_idmask = 0xfff;
	//number for the CAN Controller to handle it internaly  (0->15)
	etc_mob.mob_number = 0;
	
	//data register which gets send over CAN
	uint8_t etc_data[8];
	
	struct CAN_MOB can_ECU0_mob;
	can_ECU0_mob.mob_id = 0x600;
	can_ECU0_mob.mob_idmask = 0xffff;
	can_ECU0_mob.mob_number = 1;
	uint8_t ECU0_databytes[8];
	
	struct CAN_MOB can_uFAR_mob;
	can_uFAR_mob.mob_id = 0x200;
	can_uFAR_mob.mob_idmask = 0xffff;
	can_uFAR_mob.mob_number = 2;
	uint8_t uFAR_databytes[8];
	
	uint16_t volatile adc_val = adc_get_1();
	double volatile angle = calculate_angle(adc_val);
	etc_time = calculate_servo_ticks(angle);

	sei(); // Set Interrupt Enable
	ENABLE_SHUTDOWN
	ENABLE_POWER
	
    while (1){
		
		if((sys_time - time_old) >= 1){
			
			time_old = sys_time;		//start conversion every ms
			adc_start_conversion();
			sys_time_10++;				//increment 10ms timer
		
		}
		
		if(sys_time_10 >= 10){
			
			//update the sensor percentages and check for occuring errors
			update_apps_percantage();
			update_tps_percentage();
			check_for_errors();
			
			//calculate etc angle and the resulting ticks from it			
			angle = calculate_angle(get_apps1_percentage());
			etc_time = calculate_servo_ticks(angle);
			
			sys_time_50++;			//increment 50 ms timer
			sys_time_10 = 0;		//reset 10ms timer

			//put desired Data into the CAN MOB
			
			if (apps1_percentage<=0)
			{
				apps1_percentage_ext=0;
			}else
				apps1_percentage_ext=apps1_percentage;
				
			if (apps2_percentage<=0)
			{
				apps2_percentage_ext=0;
			}else{
				apps2_percentage_ext=apps2_percentage;
			}
			
			if (tps1_percentage<=0)
			{
				tps1_percentage_ext=0;
			}else{
				tps1_percentage_ext=tps1_percentage;
			}
			
			if (tps2_percentage<=0)
			{
				tps2_percentage_ext=0;
			}else{
				tps2_percentage_ext=tps2_percentage;
			}
			etc_data[0] = apps1_percentage_ext;
			etc_data[1] = apps2_percentage_ext;
			etc_data[2] = tps1_percentage_ext;
			etc_data[3] = tps2_percentage_ext;
			etc_data[4] = timer_apps;
			etc_data[5] = timer_tps;
			etc_data[6] = timer_tps_to_apps;
			etc_data[7] = timer_tps_hard_fail;

			//transmit and recieve desired CAN MOB`s
			can_tx(&etc_mob, etc_data);
			can_rx(&can_ECU0_mob, ECU0_databytes);
			can_rx(&can_uFAR_mob, uFAR_databytes);
			rpm = ECU0_databytes[1]<<8 | ECU0_databytes[0];
			Blipper_Enable = uFAR_databytes[5];
			Anti_Blipper_Enable = uFAR_databytes[6];
		}
		if(sys_time_50 >= 5){
			sys_tick();				//blink status LED
			sys_time_50 = 0;		//reset 50 ms timer
		}
		
	}
}

