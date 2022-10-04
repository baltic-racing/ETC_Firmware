/*
 * safety_features.h
 *
 * Created: 03.08.2019 16:34:03
 *  Author: Ole Hannemann
 */ 


#ifndef SAFETY_FEATURES_H_
#define SAFETY_FEATURES_H_
#include <avr/io.h>
#include <avr/interrupt.h>

#define TRUE 1
#define FALSE 0

// defines to enable/disable the Output Pin for the Shutdown Circuit
#define ENABLE_SHUTDOWN		PORTE |= 1<<PE1;
#define DISABLE_SHUTDOWN	PORTE &= ~(1<<PE1);
// defines to Enable/disable the relais powering the servo engine
#define ENABLE_POWER		PORTE |= 1<<PE0;
#define DISABLE_POWER		PORTE &= ~(1<<PE0);
// define to ask for the status of the relais
// 1 = off
// 0 = on
#define POWER_DISABLED		~(PINE>>PE0)&1

// Times are for the maximum allowd Deviations between Sensors and other Fail States
// These Times are defined as Follows
// 1 = 10 ms
#define TIMER_APPS_DEFAULT 50;
#define TIMER_TPS_DEFAULT 50;
#define TIMER_TPS_TO_APPS_DEFAULT 100;
#define TIMER_TPS_HARD_FAIL_DEFAULT 50;

// these are defines for the calibration of the used sensor 
// where min value stands for the position where the logical 0% is
// max value stands for the position where the logical 100% is
#define APPS1_MIN_VALUE 860
#define APPS1_MAX_VALUE 90
#define APPS2_MIN_VALUE 827
#define APPS2_MAX_VALUE 82

#define TPS1_START		519
#define TPS1_END		194
#define TPS2_START		519
#define TPS2_END		835

#define ANTI_BLIPPER_PERCENTAGE 50
#define BLIPPER_PERCENTAGE 40

//ADC Values defined as the IDLE position for the Throttle
#define THROTTLE_IDLE1	400
#define THROTTLE_IDLE2	600

//allowed Deviation between the sensors
#define APPS_DEVIATION_ALLOWED 25.0
#define TPS_DEVIATION_ALLOWED 25.0
#define TPS_DEVIATION_IDLE_ALLOWED 25.0
#define TPS_APPS_DEVIATION_ALLOWED 25.0

uint8_t check_apps();
uint8_t check_tps();
uint8_t apps_to_tps();
void update_apps_percantage();
void update_tps_percentage();
void reset_timer_apps();
void reset_timer_tps();
void reset_timer_apps_to_tps();
void reset_timer_tps_hard_fail();
void decrement_tps_timer();
void decrement_apps_timer();
void decrement_tps_to_apps_timer();
void decrement_tps_hard_fail_timer();
uint8_t tps_idle_check();
void check_for_errors();
float get_apps1_percentage();


#endif /* SAFETY_FEATURES_H_ */