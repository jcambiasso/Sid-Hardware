/*
 * Simple timer configuration file.
 * The automatic configuration is not working right, so manually configure times in the .h file.
 *
 * Created: 3/7/2017 12:07:52 p. m.
 *  Author: jcambiasso
 *  
 *  Updated: 17/11/2017
 *  0.2: Made a little less mcu specific
 *  
 */ 

#include "Simple_timer.h"

#ifdef USING_ARDUINO_H
#include <Arduino.h>
uint32_t getMicros(){
	return micros();
}

uint32_t getMillis(){
	return millis();
}

void initTimer(){
	return;
}

void configureHighPrecision(uint8_t hp)
{
	return;
}

#endif


 #ifndef USING_ARDUINO_H
 #if defined(__AVR_ATmega328P__)
 /*This is cpu specific - atmega328p*/
 #define TIMER_CONF_A	TCCR2A
 #define TIMER_CONF_B	TCCR2B
 #define OC_REGISTER	OCR2A
 #define OC_INT_REG		TIMSK2
 #define TIMER_COUNTER TCNT2
 #define OC_INT_BIT		1
 #define INT_VECTOR		TIMER2_COMPA_vect
 #define TIMER_SIZE		8

 #define CTC_REGA_CONFIG 0b00000010 //CTC
 #define CTC_REGB_CONFIG 0b00000000

 #define RESET_PRESCALER() do {GTCCR|=1<<PSRASY; GTCCR|=1<<PSRSYNC; __asm__("NOP \n\t" ::);} while(0)
 #define SET_INTERRUPT() do{TIMSK2|=1<<1;} while(0)
 // Keep adding support
 //#elif defined (__AVR_ATmega168P__)
 //#else
 #error "Unsupported MCU"
 #endif
 /*Ends cpu specific*/

typedef struct{
	 uint8_t timerAconfig;
	 uint8_t timerBconfig;
	 uint16_t ocvalue;
	 uint32_t interval;
 }Timer_config_t;

static volatile uint32_t millis;
static volatile uint32_t micros;
uint8_t usingMillis = 1;

#ifdef __cplusplus
extern "C"{
#endif

#if defined(OC_VALUE_1ms) && defined(OC_VALUE_1us)
Timer_config_t msconfig = {.timerAconfig = CTC_REGA_CONFIG, .timerBconfig = CTC_REGB_CONFIG + PRES_VALUE_1ms, .ocvalue = OC_VALUE_1ms, .interval = 1000};
Timer_config_t usconfig = {.timerAconfig = CTC_REGA_CONFIG, .timerBconfig = CTC_REGB_CONFIG + PRES_VALUE_1us, .ocvalue = OC_VALUE_1us, .interval = 10};
#else
Timer_config_t msconfig = {.interval = 1000};
Timer_config_t usconfig = {.interval = 10};
#endif

void static initTimer_ctc(Timer_config_t* config){
	cli();
	RESET_PRESCALER();
	if(config->ocvalue!=0) //If already configured, just assign and skip.
	{
		TIMER_CONF_A= 0b00000000; //
		TIMER_CONF_B= 0b00000000;
		OC_REGISTER = config->ocvalue;
		TIMER_CONF_A=config->timerAconfig;
		TIMER_CONF_B= config->timerBconfig;
		OC_INT_REG|=1<<OC_INT_BIT;
		sei();
		return;
	}
	#if !(defined(OC_VALUE_1ms) && defined(OC_VALUE_1us))
	config->timerAconfig = CTC_REGA_CONFIG;
	config->timerBconfig = CTC_REGB_CONFIG&~2;
	uint8_t i = 0;
	uint64_t N = 0;
	for(i=0;i<5;i++){
		uint16_t pres;
		switch(i)
		{
			case 0: pres = 1;
			break;
			case 1: pres = 8;
			break;
			case 2: pres = 32;
			break;
			case 3: pres = 64;
			break;
			case 4: pres = 128;
			break;
			case 5: pres = 256;
			break;
			case 6: pres = 1024;
			break;
			default: pres = 1;
			break;
		}
		N = (CPU_CLOCK*config->interval);
		N/=pres*1000000UL;
		#if TIMER_SIZE <=8
		if(N<255 && N>0) break;
		#else
		if(N<65535 && N>0) break;
		#endif
	}
	
	config->timerBconfig+=i+1;
	config->ocvalue = (uint8_t)N;
	TIMER_CONF_A= 0b00000000;
	TIMER_CONF_B= 0b00000000;
	OC_REGISTER = config->ocvalue;
	TIMER_CONF_B= config->timerBconfig;
	SET_INTERRUPT();
	sei();
	#endif
	return;
}

void initTimer(){
	initTimer_ctc(&usconfig);
	initTimer_ctc(&msconfig);
	configureHighPrecision(0);
}

void configureHighPrecision(uint8_t hp)
{
	if(hp)
	{
		usingMillis = 0;
		initTimer_ctc(&usconfig);
	}
	else
	{
		usingMillis = 1;
		initTimer_ctc(&msconfig);
	}

}

uint32_t getMillis()
{
	return millis;
}

uint32_t getMicros()
{
	return micros;
}

ISR(INT_VECTOR)
{
	if(usingMillis) 
	{
		millis++;
		micros+=1000;
	}
	else 
	{
		static uint16_t counter;
		micros+=usconfig.interval;
		counter+=usconfig.interval;
		if(counter == 1000)
		{
			counter = 0;
			millis++;
		}
	}
}

#ifdef __cplusplus
}
#endif

#endif