#ifndef THREAD_H
#define THREAD_H

#ifndef WIN32
#include <glib.h>
#else
#include <windows.h>
#endif

class Thread
{
public:
	Thread();
	virtual ~Thread();

	bool create(void (*callback)(void*), void* data);
	void runCallback();

	void lock();
	void unlock();

private:
#ifndef WIN32
	GThread* _thread;
	GMutex* _mutex;
#else
	HANDLE _thread;
	DWORD _id;
#endif
	void (*_callback)(void*);
	void* _data;
	
};

#endif

