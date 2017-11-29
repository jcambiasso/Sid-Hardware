#ifndef SIMPLE_TIMER_H
#define SIMPLE_TIMER_H 1

#include <util/atomic.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//Define CPU Clock
#define CPU_CLOCK 16000000UL
//Comment to use standard AVR-GCC.
#define USING_ARDUINO_H
//You may manually define the OC value for 1ms and 1us. This saves memory and makes for a faster startup.
//PRES_VALUE_X is the prescaling value according to the datasheet.
#define OC_VALUE_1ms 250
#define PRES_VALUE_1ms 4
#define OC_VALUE_1us 160
#define PRES_VALUE_1us 1

//end of configuration
#ifdef __cplusplus
extern "C"{
#endif

typedef struct {
	uint32_t interval;
	uint32_t lastMillis;
	uint8_t enabled;
	uint8_t	singleShot;
	uint8_t running;
	uint8_t usbased;
	//Each timer takes up to 12 bytes of RAM.
}Timer_t;

uint8_t tickTimer(Timer_t* timer);
void stopTimer(Timer_t* timer);

void initTimer();
void stopTimer(Timer_t* timer);
uint32_t getMillis();
uint32_t getMicros();
void configureHighPrecision(uint8_t hp);
extern uint8_t usingMillis;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
//friendly wrapper to delay without blocking
#define WAIT(a) if(a.tick())
//opaque C++ wrapper class
class Timer : private Timer_t{
	public:
	Timer(uint32_t interval);
	Timer(uint32_t interval, bool single);
	Timer(uint32_t interval, bool single, bool us);
	bool tick(void);
	void start(void);
	void stop(void);
	void setInterval(uint32_t t);
	void setSingleShot(bool ss);
	operator ::Timer_t*();
	operator const ::Timer_t*();
	
	
	private:
	void init(uint32_t inter, bool us, bool ss);
	static bool timerConfigured;
};
#else
#define NewTimer(in,ss,us)\
{\
	.interval = in, \
	.singleShot = ss, \
	.usbased = us, \
	.enabled = 1 \
}
#endif

#endif //[SIMPLE_TIMER_H]
