/*
 * Simple timer .
 *
 * Created: 3/7/2017 12:07:52 p. m.
 *  Author: jcambiasso
 *  
 *  Updated: 17/11/2017
 *  0.2: Add more utility classes
 *  
 */ 


#include "Simple_timer.h"

#ifdef __cplusplus
extern "C" 
{
#endif

uint8_t tickTimer(Timer_t* timer)
{
	if(!timer->enabled) return 0;
	uint32_t (*now)(void);

	if(timer->usbased) 
	{
		now = getMicros;
		if(usingMillis)configureHighPrecision(1);
		
	}
	else
	{ 
		now = getMillis;
		if(!usingMillis)configureHighPrecision(0);
	}

	if(!timer->running)
	{
		timer->running = 1;
		timer->lastMillis = now();
		return 0;
	}

	if(now()-timer->lastMillis >= timer->interval)
	{
		timer->running = 0;
		if(timer->singleShot)timer->enabled = 0;
		return 1;
	}
	else return 0;
}

void stopTimer(Timer_t* timer)
{
	timer->enabled = 0;
	timer->running = 0;
}

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
bool Timer::timerConfigured = false;
void Timer::init(uint32_t inter, bool us, bool ss)
{
	interval = inter;
	usbased = us;
	singleShot = ss;
	if(!singleShot)enabled = 1;
	else enabled = 0;
	if(!Timer::timerConfigured)
	{
		initTimer();
		Timer::timerConfigured = true;
	}
}
Timer::Timer(uint32_t interval)
{
	init(interval,0,0);
}
Timer::Timer(uint32_t interval, bool single)
{
	init(interval,0,single);
}
Timer::Timer(uint32_t interval, bool single, bool us)
{
	init(interval, us, single);
}
bool Timer::tick()
{
	return tickTimer(this);
}

void Timer::start(void)
{
	enabled = 1;
	running = 0;
}
void Timer::stop(void)
{
	stopTimer(this);
}
void Timer::setInterval(uint32_t t)
{
	interval = t;
}
void Timer::setSingleShot(bool ss)
{
	singleShot = ss;
}
#endif