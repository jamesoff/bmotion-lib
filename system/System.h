#ifndef BMSYSTEM_H
#define BMSYSTEM_H

#include <map>
#include <vector>
#include <Library.h>
#include <vector>
#include <SimplePlugin.h>
#include <ComplexPlugin.h>
#include <EventPlugin.h>
#include <AdminPlugin.h>
#include <OutputPlugin.h>
#include <Timer.h>
#include <setjmp.h>
#include <Abstract.h>
#include <bString.h>
#include <Mood.h>
#include <Thread.h>

// lapse for waiting in the timer thread
#define PAUSE_LENGTH (500)

class Plugin;

/*
 * the bmotion system class.
 */
class System
{
public:
	System();
	virtual ~System();

	// diagnostics
	void dump();

	// dynamic library tracking
	bool loadLibrary(const String& name);
	bool removeAllLibraries();
	bool disableLibrary(Library* lib);
	bool disableAllLibraryPlugins(Library* lib);

	// hmmm
	Library* getActiveLibrary();
	void setActiveLibrary(Library* lib);

	// plugins
	Plugin* getPlugin(const String& name);
	bool addPlugin(Plugin* plugin);
	bool enablePlugin(const String& name);
	bool disablePlugin(const String& name);

	// plugin type specific methods
	SimplePlugin* findSimplePlugin(const String& text, Language language);
	std::vector< ComplexPlugin* > findComplexPlugins(const String& text,
			Language language);
	std::vector< EventPlugin* > findEventPlugins(EventType type,
			const String& text, Language language);
	AdminPlugin* findAdminPlugin(const String& text, Language language);
	std::vector< OutputPlugin* > findOutputPlugins(const String& text,
			Language language);

	// Timers
	bool addTimer(unsigned long milli, void(*callback)(void*), void* param);
	bool killTimers();
	void checkTimers();

	// handling
	void restoreStack();
	int startDangerousCode();
	bool endDangerousCode();

	// abstracts
	bool abstractRegister(const String& name);
	bool abstractAddValue(const String& name, const String& value);
	String abstractGetValue(const String& name);
	bool abstractGarbageCollect();

	// moods
	bool moodDrift();
	bool moodIncrease(const String& name, int amount = 1);
	bool moodDecrease(const String& name, int amount = 1);
	int moodGet(const String& name);
	bool moodCreate(const String& name, int centre, int lower = -30,
			int upper = 30);

private:
	// collections
	std::vector< Library* > _libraries;
	std::vector< Plugin* > _plugins;
	std::vector< Timer* > _timers;
	std::map< String, Abstract*, ltstr> _abstracts;
	std::vector< Mood* > _moods;

	// active lib
	Library* _activeLib;

	// timer thread
	Thread* _timerThread;

	// stack
#ifndef WIN32
	sigjmp_buf _stack;
#else
	jmp_buf _stack;
#endif
	bool _segfault;

	// internal functions
	bool registerLibrary(Library* lib);

};

#endif

