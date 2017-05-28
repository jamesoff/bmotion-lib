#include "System.h"
#include <algorithm>
#include <SimplePlugin.h>
#include <ComplexPlugin.h>
#include <Output.h>
#include <bMotion.h>
#include <time.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#endif
#include <signal.h>

/*
 * Default System constructor
 */
System::System()
: _activeLib(NULL)
, _timerThread(NULL)
, _segfault(false)
{
	// initialise the random seed
	srand((unsigned)time(NULL));
}

/*
 * Destructor
 */
System::~System()
{
	// clean up timers
	killTimers();
	// clean up dynamic loaded mess
	int i;
	int s = _libraries.size();
	for (i = 0; i < s; i++)
		delete _libraries[i];
	s = _plugins.size();
	for (i = 0; i < s; i++)
		delete _plugins[i];
	std::map< String, Abstract*, ltstr>::iterator iter = _abstracts.begin();
	while (iter != _abstracts.end())
	{
		delete iter->second;
		iter++;
	}
	s = _moods.size();
	for (i = 0; i < s; i++)
		delete _moods[i];
}

/*
 * dump the parameters of the system object to the log
 */
void System::dump()
{
	bMotionLog(1, "--System");
	bMotionLog(1, "  %d libraries loaded", _libraries.size());
	int active = 0;
	for (int i = 0; i < (int)_plugins.size(); i++)
		active += _plugins[i]->isEnabled();
	bMotionLog(1, "  %d plugins loaded (%d active, %d inactive)", 
			_plugins.size(), active, _plugins.size() - active);
	bMotionLog(1, "  %d timers active", _timers.size());
	bMotionLog(1, "  %d moods active", _moods.size());
}

/*
 * Trying to load a library from the disk
 * @param name the name of the library to load (including path)
 * @return true or false
 */
bool System::loadLibrary(const String& name)
{
	int size = _libraries.size();
	for (int i = 0; i < size; i++)
	{
		Library* lib = _libraries[i];
		if (lib->getName().equals(name))
			return false;
	}
	
	Library* lib = new Library(name);
	// register this in the settings
	Library* oldLib = _activeLib;
	_activeLib = lib;
	if (lib->openLibrary())
	{
		registerLibrary(lib);
		_activeLib = oldLib;
		return true;
	}
	_activeLib = oldLib;
	delete lib;
	return false;
}

/*
 * register a library in the system
 * @param lib the pointer to the library to register
 * @return true or false
 */
bool System::registerLibrary(Library* lib)
{
	int size = _libraries.size();
	for (int i = 0; i < size; i++)
	{
		Library* testlib = _libraries[i];
		if (testlib == lib)
			return false;
		if (testlib->getName().equals(lib->getName()))
			return false;
	}
	_libraries.push_back(lib);
	return true;
}

/*
 * disable a library loaded in the system. This will fail if the plugins that
 * rely on the library haven't been disabled or if the library doesn't exist.
 * This does not remove the library from the system, just disables it.
 * @param lib the library to disable
 * @return true or false
 */
bool System::disableLibrary(Library* lib)
{
	std::vector< Library* >::iterator pos = std::find(_libraries.begin(),
			_libraries.end(), lib);
	if (pos == _libraries.end())
		return false;
	
	// we need to check registered plugins for this lib
	int size = _plugins.size();
	for (int i = 0; i < size; i++)
	{
		Plugin* plugin = _plugins[i];
		if (plugin->getSource() == lib && plugin->isEnabled())
			return false;
	}
	
	lib->closeLibrary();
	return true;
}

/*
 * remove all of the plugin libraries and plugins. might as well be called
 * "Operation Clean Sweep"
 * @return true or false.
 */
bool System::removeAllLibraries()
{
	if (_timerThread)
		_timerThread->lock();
	bMotionLog(1, "cleaning plugins");
	int i;
	int size = _plugins.size();
	for (i = 0; i < size; i++)
	{
		Plugin* plugin = _plugins[0];
		if (plugin->getSource() == _activeLib)
			continue;
		bMotionLog(1, "deleting plugin %s", 
				(const char*)plugin->getName());
		_plugins.erase(std::find(_plugins.begin(), _plugins.end(), 
					plugin));
		delete plugin;
	}
	bMotionLog(1, "cleaning timers");
	size = _timers.size();
	for (i = 0; i < size; i++)
	{
		Timer* timer = _timers[0];
		if (timer->library() == NULL || timer->library() == _activeLib)
			continue;
		_timers.erase(std::find(_timers.begin(), _timers.end(),
					timer));
		delete timer;
	}
	bMotionLog(1, "cleaning libraries");
	size = _libraries.size();
	for (i = 0; i < size; i++)
	{
		Library* lib = _libraries[0];
		if (lib == _activeLib)
			continue;
		bMotionLog(1, "removing library '%s'", 
				(const char*)lib->getName());
		lib->closeLibrary();
		_libraries.erase(std::find(_libraries.begin(), _libraries.end(),
					lib));
		delete lib;
	}
	if (_timerThread != 0)
		_timerThread->unlock();
	return true;
}

/*
 * disable all plugins associated with a library
 * @param lib the library for association check
 * @return true or false.
 */
bool System::disableAllLibraryPlugins(Library* lib)
{
	std::vector< Library* >::iterator pos = std::find(_libraries.begin(),
			_libraries.end(), lib);
	if (pos == _libraries.end())
		return false;
	
	// we need to check registered plugins for this lib
	int size = _plugins.size();
	for (int i = 0; i < size; i++)
	{
		Plugin* plugin = _plugins[i];
		if (plugin->getSource() == lib)
			disablePlugin(plugin->getName());
	}
	return true;
}

/*
 * get the last library registered in the system
 * @return library pointer
 */
Library* System::getActiveLibrary()
{
	return _activeLib;
}

/*
 * set the active library
 * @param lib the library
 */
void System::setActiveLibrary(Library* lib)
{
	_activeLib = lib;
}

/*
 * add a plugin to the system
 * @param plugin plugin to add
 * @return true or false
 */
bool System::addPlugin(Plugin* plugin)
{
	int size = _plugins.size();
	for (int i = 0; i < size; i++)
	{
		Plugin* testplugin = _plugins[i];
		if (testplugin->getName().equals(plugin->getName()))
			return false;
	}
	_plugins.push_back(plugin);
	return true;
}

/*
 * get a plugin by name reference
 * @param name the name of the plugin
 * @return the plugin requested or NULL if not found
 */
Plugin* System::getPlugin(const String& name)
{
	int size = _plugins.size();
	for (int i = 0; i < size; i++)
	{
		Plugin* plugin = _plugins[i];
		if (plugin->getName().equals(name))
			return plugin;
	}
	return NULL;
}

/*
 * enable a plugin by a given name
 * @param name the name of the plugin
 * @return true or false.
 */
bool System::enablePlugin(const String& name)
{
	Plugin* plugin = getPlugin(name);
	if (plugin)
		return plugin->enable();
	return false;
}

/*
 * disables a plugin with the given name
 * @param name the name of the plugin
 * @return true or false.
 */ 
bool System::disablePlugin(const String& name)
{
	Plugin* plugin = getPlugin(name);
	if (plugin)
		return plugin->disable();
	return false;
}

/*
 * find a simple plugin that will run for the given text in the given language.
 * this takes the plugin "chance" into consideration.
 * @param text the testing text.
 * @param language the language for the plugin
 * @return a simple plugin or NUll if none are found
 */
SimplePlugin* System::findSimplePlugin(const String& text, Language language)
{
	int size = _plugins.size();
	for (int i = 0; i < size; i++)
	{
		Plugin* plugin = _plugins[i];
		if (plugin->getType() == Simple)
		{
			Language lang = plugin->getLanguage();
			if (lang != language && lang != any)
				continue;
			if (!text.matches(plugin->getRegexp()))
				continue;
			if (plugin->getChance() < rand() % 100)
				continue;
			return (SimplePlugin*)plugin;
		}
	}
	return NULL;
}

/*
 * find a set of complex plugins that match the given text. this takes the 
 * plugin "chance" into consideration as well 
 * @param text the testing text.
 * @param language the language for the plugin
 * @return a set of complex plugins (empty if none are found)
 */
std::vector< ComplexPlugin* > System::findComplexPlugins(const String& text,
		Language language)
{
	std::vector< ComplexPlugin* > answer;
	int size = _plugins.size();
	for (int i = 0; i < size; i++)
	{
		Plugin* plugin = _plugins[i];
		if (plugin->getType() == Complex)
		{
			Language lang = plugin->getLanguage();
			if (lang != language && lang != any)
				continue;
			if (!text.matches(plugin->getRegexp()))
				continue;
			if (plugin->getChance() < rand() % 100)
				continue;
			answer.push_back((ComplexPlugin*)plugin);
		}
	}
	return answer;
}

/*
 * find a set of event plugins that match the given text. this takes the 
 * plugin "chance" into consideration as well 
 * @param text the testing text.
 * @param language the language for the plugin
 * @return a set of event plugins (empty if none are found)
 */
std::vector< EventPlugin* > System::findEventPlugins(EventType type, 
		const String& text, Language language)
{
	std::vector< EventPlugin* > answer;
	int size = _plugins.size();
	for (int i = 0; i < size; i++)
	{
		Plugin* plugin = _plugins[i];
		if (plugin->getType() == Event)
		{
			Language lang = plugin->getLanguage();
			if (lang != language && lang != any)
				continue;
			EventPlugin* test = (EventPlugin*)plugin;
			if (test->getEventType() != type)
				continue;
			if (!text.matches(test->getRegexp()))
				continue;
			if (test->getChance() < rand() % 100)
				continue;
			answer.push_back(test);
		}
	}
	return answer;
}

/*
 * find an admin plugin that will run for the given text in the given language.
 * @param text the testing text.
 * @param language the language for the plugin
 * @return an admin plugin or NUll if none are found
 */
AdminPlugin* System::findAdminPlugin(const String& command, Language language)
{
	int size = _plugins.size();
	for (int i = 0; i < size; i++)
	{
		Plugin* plugin = _plugins[i];
		if (plugin->getType() == Admin)
		{
			Language lang = plugin->getLanguage();
			if (lang != language && lang != any)
				continue;
			// not reg exp
			if (!command.equals(plugin->getRegexp()))
				continue;
			return (AdminPlugin*)plugin;
		}
	}
	return NULL;
}

/*
 * find a set of output plugins that match the given text. this takes the 
 * plugin "chance" into consideration as well 
 * @param text the testing text.
 * @param language the language for the plugin
 * @return a set of output plugins (empty if none are found)
 */
std::vector< OutputPlugin* > System::findOutputPlugins(const String& text,
		Language language)
{
	std::vector< OutputPlugin* > answer;
	int size = _plugins.size();
	for (int i = 0; i < size; i++)
	{
		Plugin* plugin = _plugins[i];
		if (plugin->getType() == Output)
		{
			Language lang = plugin->getLanguage();
			if (lang != language && lang != any)
				continue;
			if (!text.matches(plugin->getRegexp()))
				continue;
			if (plugin->getChance() < rand() % 100)
				continue;
			answer.push_back((OutputPlugin*)plugin);
		}
	}
	return answer;
}

/*
 * Internal callback for timer thread.
 * @param param pointer to some parameter.
 */
void CheckTimers(void*)
{
	bMotionSystem().checkTimers();
}

/*
 * Add a timer into the system to fire after a given millisecond interval.
 * @param milli millisecond interval for the timed event.
 * @param callback the callback function for the event.
 * @param param pointer to some parameter that will be used in the callback
 * @return true or false. if false, assume that the timer will never happen.
 */
bool System::addTimer(unsigned long milli, void(*callback)(void*), void* param)
{
	// why are we using gthreads here and not alarm() or ualarm()?
	// this is because when we launch 3 alarms in sequence, only one
	// actually makes it through. This is at least true in the tests
	// that we ran. So for the time being, this will do... we create
	// a thread if one doesn't exist that polls the active timers and
	// fires off ones that are ready. If no more are left, the thread
	// dies and must be restarted when another timer is added.
	//if (_timerThread != 0)
	//	g_thread_mutex_lock(&_timerMutex);
	Timer* timer = new Timer(_activeLib, milli, callback, param);
	_timers.push_back(timer);
	//if (_timerThread != 0)
	//	g_thread_mutex_unlock(&_timerMutex);
	if (_timers.size() == 1) // if (_timerThread == 0)
	{
		bMotionLog(1, "creating new timer thread");
		_timerThread = new Thread();
		if (!_timerThread->create(CheckTimers, NULL))
		{
			bMotionLog(1, "timers will not be active");
			delete _timerThread;
			return false;
		}
	}
	return true;
}

/*
 * kill all the timers.
 * @return true or false.
 */
bool System::killTimers()
{
	if (_timerThread == NULL)
		return true; // dead already... YAY!
	_timerThread->lock();
	int size = _timers.size();
	for (int i = 0; i < size; i++)
	{
		Timer* timer = _timers[i];
		delete timer;
	}
	_timers.clear();
	_timerThread->unlock();
#ifndef WIN32
	usleep(PAUSE_LENGTH); // give it a chance to close the thread
#else
	Sleep(PAUSE_LENGTH);
#endif
	delete _timerThread;
	_timerThread = NULL;
	return true;
}

/*
 * check if the timers need to be fired or not.
 */
void System::checkTimers()
{
	while (_timers.size() > 0)
	{
		int size = _timers.size();
		for (int i = 0; i < size; i++)
		{
			_timerThread->lock();
			if ((int)_timers.size() != size)
				continue;
			Timer* timer = _timers[i];
			if (!timer->isReady())
			{
				_timerThread->unlock();
				continue;
			}
			Library* oldLib = _activeLib;
			if (bMotionSystem().startDangerousCode() == 0)
			{
				_activeLib = timer->library();
				timer->dispatch();
			}
			_activeLib = oldLib;
			if (bMotionSystem().endDangerousCode())
				bMotionLog(1, "timer trigger turned out to be naughty code");
			_timers.erase(std::find(_timers.begin(),
						_timers.end(),
						timer));
			delete timer;
			_timerThread->unlock();
			break;
		}
#ifndef WIN32
		// wait half a second
		usleep(PAUSE_LENGTH);
#else
		Sleep(PAUSE_LENGTH);
#endif
	};
	bMotionLog(1, "killing off timer thread");
	delete _timerThread;
	_timerThread = NULL;
}

/*
 * gets called on a segfault
 */
void segfaultHandler(int /*val*/)
{
	bMotionLog(1, "caught segmentation fault signal... trying to recover");
	bMotionSystem().restoreStack();
}

/*
 * attempt to restore the stack state
 */
void System::restoreStack()
{
	_segfault = true;
#ifndef WIN32
	siglongjmp(_stack, 1);
#else
	longjmp(_stack, 1);
#endif
}

/*
 * set the tracking in case of SEGV
 * @return stack set
 */
int System::startDangerousCode()
{
	_segfault = false;
	signal(SIGSEGV, segfaultHandler);
#ifndef WIN32
	signal(SIGBUS, segfaultHandler);
	return sigsetjmp(_stack, 1);
#else
	return setjmp(_stack);
#endif
}

/*
 * check if the segfault flag is tripped
 * @return true or false.
 */
bool System::endDangerousCode()
{
	signal(SIGSEGV, SIG_DFL);
#ifndef WIN32
	signal(SIGBUS, SIG_DFL);
#endif
	return _segfault;
}

/*
 * Register a new abstract type in the system.
 * @param name the name to assign to the abstract type.
 * @return true or false.
 */
bool System::abstractRegister(const String& name)
{
	Abstract* abstract = NULL;
	if (_abstracts[name] != NULL)
		abstract = _abstracts[name];
	else
		abstract = new Abstract(name);
	bMotionLog(1, "creating abstract '%s'", (const char*)name);
	if (!abstract->create())
	{
		if (_abstracts[name] == NULL)
			delete abstract;
		return false;
	}
	_abstracts[name] = abstract;
	return true;
}

/*
 * Batch add to an abstract
 * @param name the name of the abstract to which to add.
 * @param ... stuff
 * @return true or false
 */
bool System::abstractAddValue(const String& name, const String& value)
{	
	if (_abstracts[name] == NULL)
		return false;
	_abstracts[name]->addValue(value);
	return true;
}

/*
 * Get a value from the named abstract
 * @param name the name of the abstract to which to add.
 * @return some value, empty if not there
 */
String System::abstractGetValue(const String& name)
{
	if (_abstracts[name] == NULL)
		return String();
	return _abstracts[name]->getRandomValue();
}

/*
 * Garbage collect the abstracts
 * @return true or false
 */
bool System::abstractGarbageCollect()
{
	bMotionLog(1, "Garbage collecting abstracts...");
	bool happened = false;
	std::map< String, Abstract*, ltstr >::iterator iter;
	iter = _abstracts.begin();
	while (iter != _abstracts.end())
	{
		if (iter->second->garbageCollect())
			happened = true;
		iter++;
	}
	return happened;
}

/*
 * Drift the moods according to their given targets
 * @return true or false
 */
bool System::moodDrift()
{
	if (_moods.size() == 0)
		return false;
	for (int i = 0; i < (int)_moods.size(); i++)
		_moods[i]->drift();
	return true;
}

/*
 * increase a given mood by a given amount
 * @param name the name of the mood
 * @param amount the amount by which to increase (default 1)
 * @return true or false
 */
bool System::moodIncrease(const String& name, int amount)
{
	for (int i = 0; i < (int)_moods.size(); i++)
	{
		if (name.equals(_moods[i]->getName()))
		{
			_moods[i]->increase(amount);
			return true;
		}
	}
	return false;
}

/*
 * decrease a given mood by a given amount
 * @param name the name of the mood
 * @param amount the amount by which to decrease (default 1)
 * @return true or false
 */
bool System::moodDecrease(const String& name, int amount)
{
	return moodIncrease(name, -amount);
}

/*
 * get the value of a mood
 * @param name the name of the mood
 * @return the value of the mood
 */
int System::moodGet(const String& name)
{
	for (int i = 0; i < (int)_moods.size(); i++)
		if (name.equals(_moods[i]->getName()))
			return _moods[i]->getValue();
	return 0;
}

/*
 * Create a new mood of a given name
 * @param name the name of the new mood
 * @param centre the target value of the mood
 * @param lower the lower bounds of the mood
 * @param upper the upper bounds of the mood
 * @return true if succeeded, false in error
 */
bool System::moodCreate(const String& name, int centre, int lower, int upper)
{
	for (int i = 0; i < (int)_moods.size(); i++)
		if (name.equals(_moods[i]->getName()))
			return false;
	Mood* mood = new Mood(name, centre, lower, upper);
	_moods.push_back(mood);
	return true;
}

