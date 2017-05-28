#include <Timer.h>
#include <math.h>

/*
 * Timer constructor. creates a new timer that starts now.
 * @param lib the library where this originated
 * @param interval milliseconds until it's time to fire.
 * @param callback the callback to fire after milliseconds
 * @param param a parameter pointer to use in callback.
 */
Timer::Timer(Library* lib, unsigned long interval, void(*callback)(void*),
		void* param)
: _library(lib)
, _interval(interval)
, _callback(callback)
, _param(param)
{
	time(&_start);	
}

/*
 * Destructor
 */
Timer::~Timer()
{

}

/*
 * check if the timer is ready to fire
 * @return true if it's ready, false if it's not.
 */
bool Timer::isReady() const
{
	time_t end;
	time(&end);
	double diff = difftime(end, _start) * 1000;
	if (_interval <= (unsigned long)floor(diff))
		return true;
	return false;
}

/*
 * get the library this timer came from, if any!
 * @return library or NULL
 */
Library* Timer::library()
{
	return _library;
}

/*
 * call the callback function now!
 */
void Timer::dispatch()
{
	if (_callback)
		_callback(_param);
}

