#ifndef BMTIMER_H
#define BMTIMER_H

#include <time.h>
#include <Library.h>

/*
 * timer wrapper class. the timer mechanism is in System.
 */
class Timer
{
public:
	Timer(Library* lib, unsigned long interval, void(*callback)(void*), 
			void* param);
	virtual ~Timer();

	// information
	bool isReady() const;
	Library* library();

	// execution
	void dispatch();
	
private:
	Library* _library;
	unsigned long _interval;
	time_t _start;
	void (*_callback)(void*);
	void* _param;
};

#endif

