#include <Thread.h>

Thread::Thread()
: _thread(NULL)
#ifndef WIN32
, _mutex(NULL)
#endif
, _callback(NULL)
, _data(NULL)
{

}

Thread::~Thread()
{
#ifndef WIN32
	//g_thread_exit(0);
	g_mutex_free(_mutex);
	_mutex = NULL;
#else
	CloseHandle(_thread);
#endif
	_thread = NULL;
}

#ifndef WIN32
void* threadCallback(void* object)
#else
DWORD WINAPI threadCallback(void* object)
#endif
{
	Thread* thread = (Thread*)object;
	thread->runCallback();
	return NULL;
}

bool Thread::create(void (*callback)(void*), void* data)
{
#ifndef WIN32
	if (_thread || _mutex)
		return false;
	if (!g_thread_supported())
		g_thread_init(NULL);
	_callback = callback;
	_data = data;
	_thread = g_thread_create(threadCallback, this, FALSE, NULL);
	if (!_thread)
		return false;
	_mutex = g_mutex_new();
	if (!_mutex)
		return false;
	return true;
#else
	_thread = ::CreateThread(NULL, 0, threadCallback, this, 0, &_id);
	if (!_thread)
		return false;
	_callback = callback;
	_data = data;
	return true;
#endif
}

void Thread::runCallback()
{
	if (_callback)
		_callback(_data);
}

void Thread::lock()
{
#ifndef WIN32
	if (_mutex)
		g_mutex_lock(_mutex);
#endif
}

void Thread::unlock()
{
#ifndef WIN32
	if (_mutex)
		g_mutex_unlock(_mutex);
#endif
}

