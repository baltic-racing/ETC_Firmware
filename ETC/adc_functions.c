/*
 * adc_functions.c
 *
 * Created: 06.04.2019 10:58:39
 *  Author: Ole Hannemann
 */ 


#include "adc_functions.h"
#include "safety_features.h"

// array to store the adc data in
uint16_t adc_values[4];
// index var to know which adc will come next
uint8_t adc_next = 0;
extern volatile uint8_t Blipper_Enable;
extern volatile uint8_t Anti_Blipper_Enable;


void adc_config(){
	
	
	// AREF = AVcc
	// and PA0 as input defined
	ADMUX = (1<<REFS0) | (1<<MUX0);
	// ADEN enables ADC
	// ADC prescaler 128
	// see datasheet page 290
	// enable adc interrupt
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0) | (1<<ADIE);
	// start first conversion
	ADCSRA |= (1<<ADSC);
	
}

void adc_start_conversion(){
	
	//start next conversion with same config
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0) | (1<<ADSC) | (1<<ADIE);
	
}

ISR(ADC_vect){
	
	// store ADC result
	adc_values[adc_next] = ADC;
	adc_next++;
	
	// reset index var
	if(adc_next == 4){
		adc_next = 0;
	}
	// select other ADC Input
	ADMUX = (1<<REFS0) | adc_next;
	
}


// getter for each adc var
uint16_t adc_get_1(){
	
	if (Blipper_Enable){
		return APPS1_MAX_VALUE;
	}
	else if (Anti_Blipper_Enable){
		return APPS1_MIN_VALUE;
	}else{
		return adc_values[0];
	}
}
uint16_t adc_get_2(){
	
	if (Blipper_Enable){
		return APPS2_MAX_VALUE;
	}
	else if (Anti_Blipper_Enable){
		return APPS2_MIN_VALUE;
	}else{
		return adc_values[1];
	}
}
uint16_t adc_get_3(){
	return adc_values[2];
}
uint16_t adc_get_4(){
	return adc_values[3];
}




