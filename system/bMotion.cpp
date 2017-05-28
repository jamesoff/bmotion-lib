#include <bMotion.h>
#ifndef WIN32
#include <dirent.h>
#endif
#include <Output.h>
#include <stdarg.h>
#include <Abstract.h>
#include <Mood.h>

/*
 * initialise bMotion
 * @param confFileName a configuration filename to use instead of the default
 * @return true or false. false would indicate that bMotion is not ready to use.
 */
extern "C" bool bMotionInit(const char* confFileName)
{
	// initialise them both by calling the functions before anything else
	bMotionSystem();
	bMotionSettings();
	
	// start everything else
	bool success = false;
	if (confFileName)
		success = bMotionSettings().parseConfigFile(confFileName);
	if (!success)
		success = bMotionSettings().parseConfigFile();
	if (!success)
		return false;
	bMotionSystem().killTimers();
	if (!bMotionLoadPlugins())
	{
		bMotionLog(1, "failed to load any useable plugins");
		return false;
	}
	bMotionSystem().addTimer(300000, bMotionAbstractGarbageCollect, NULL);
	bMotionSystem().addTimer(1000, bMotionMoodDrift, NULL);
	return true;
}

/*
 * do an event response using the event plugins.
 * @param type the type of event
 * @param nick the nick associated with the event
 * @param host the host of the nick
 * @param handle the handle of the nick
 * @param channel the channel of the event
 * @param text the text of the event.
 * @return true or false.
 */
bool bMotionDoEventResponse(EventType type, const char* nick,
		const char* host, const char* handle, 
		const char* channel, const char* text)
{
	bMotionLog(1, "entering bMotionDoEventResponse");
	std::vector< EventPlugin* > plugins = bMotionSystem().findEventPlugins(
			type, text, bMotionSettings().language());
	int size = plugins.size();
	if (!size)
		return false;
	
	for (int i = 0; i < size; i++)
	{
		EventPlugin* plugin = plugins[i];
		if (plugin->execute(nick, host, handle, channel, text))
			return true;
	}
	return false;
}

/*
 * Handle a Join event.
 * @param nick the nick associated with the event
 * @param host the host of the nick
 * @param handle the handle of the nick
 * @param channel the channel of the event
 * @return true or false.
 */
extern "C" bool bMotionEventOnJoin(const char* nick, const char* host, 
		const char* handle, const char* channel)
{
	if (!bMotionSettings().isChannelAllowed(channel))
		return false;

	// ignore me
	if (bMotionIsBotnick(nick))
		return false;

	return bMotionDoEventResponse(Join, nick, host, handle, channel, "");
}

/*
 * Handle a Part event.
 * @param nick the nick associated with the event
 * @param host the host of the nick
 * @param handle the handle of the nick
 * @param channel the channel of the event
 * @param msg the message string of the part
 * @return true or false.
 */
extern "C" bool bMotionEventOnPart(const char* nick, const char* host, 
		const char* handle, const char* channel, const char* msg)
{
	if (!bMotionSettings().isChannelAllowed(channel))
		return false;

	return bMotionDoEventResponse(Part, nick, host, handle, channel, msg);
}

/*
 * Handle a Quit event.
 * @param nick the nick associated with the event
 * @param host the host of the nick
 * @param handle the handle of the nick
 * @param channel the channel of the event
 * @param reason the message string of the quit
 * @return true or false.
 */
extern "C" bool bMotionEventOnQuit(const char* nick, const char* host,
		const char* handle, const char* channel,
		const char* reason)
{
	if (!bMotionSettings().isChannelAllowed(channel))
		return false;

	return bMotionDoEventResponse(Quit, nick, host, handle, channel, 
			reason);
}

/*
 * Handle the main event type
 * @param nick the nick associated with the event
 * @param host the host of the nick
 * @param handle the handle of the nick
 * @param channel the channel of the event
 * @param text the text of the main event.
 * @return true or false.
 */
extern "C" bool bMotionEventMain(const char* nick, const char* host,
		const char* handle, const char* channel, 
		const char* text)
{
	bMotionLog(1, "entering bMotionEventMain");
	//bMotionLog(1, "  %s", nick);
	//bMotionLog(1, "  %s", host);
	//bMotionLog(1, "  %s", handle);
	//bMotionLog(1, "  %s", channel);
	//bMotionLog(1, "  %s", text);
	
	if (!bMotionSettings().isChannelAllowed(channel))
		return false;

	// filter out bolds and stuff like that
	String processedText(text);
	processedText.replaceAll("\\002", "");
	processedText.replaceAll("\\022", "");
	processedText.replaceAll("\\037", "");
	processedText.replaceAll("\\003\\[0-9\\]+(,\\[0-9+\\])?", "");

	// trim
	processedText.trim();

	// check for admin
	if (processedText.startsWith("!bmadmin") && 
			(processedText.length() == 8 ||
			 processedText[8] == ' '))
	{
		String adminCommand;
		int pos = processedText.indexOf(' ', 9);
		if (pos > 0)
		{
			adminCommand = processedText.substring(9, pos);
			processedText = processedText.substring(pos + 1);
		}
		else
		{
			adminCommand = processedText.substring(9);
			processedText = "";
		}
		adminCommand.trim();
		processedText.trim();
		if (adminCommand.length() > 0)
		{
			if (adminCommand.equals("rehash") && 
					processedText.length() == 0)
				return bMotionRehash();
			AdminPlugin* admin = bMotionSystem().findAdminPlugin(
					adminCommand,
					bMotionSettings().language());
			if (admin)
				return admin->execute(nick, host, handle,
						channel, processedText);
		}
		return true; // not done yet
	}
	
	// run simple plugin
	SimplePlugin* simple = bMotionSystem().findSimplePlugin(processedText,
			bMotionSettings().language());
	if (simple)
	{
		simple->execute(nick, host, handle, channel, processedText);
	}
	
	// run complex plugins
	std::vector< ComplexPlugin* > complexplugins = 
		bMotionSystem().findComplexPlugins(processedText,
				bMotionSettings().language());
	int size = complexplugins.size();
	if (size > 0)
	{
		for (int i = 0; i < size; i++)
		{
			ComplexPlugin* plugin = complexplugins[i];
			if (plugin->execute(nick, host, handle, channel,
						processedText))
				break;
		}
	}
	return true;
}

/*
 * Handle Mode event type
 * @param nick the nick associated with the event
 * @param host the host of the nick
 * @param handle the handle of the nick
 * @param channel the channle of the event
 * @param mode the mode flags
 * @param victim the target of the mode
 * @return true or false.
 */
extern "C" bool bMotionEventMode(const char* /*nick*/, const char* /*host*/,
		const char* /*handle*/, const char* channel, 
		const char* /*mode*/,	const char* /*victim*/)
{
	if (!bMotionSettings().isChannelAllowed(channel))
		return false;

	return false;
}

/*
 * Handle Nick change event type
 * @param nick the nick associated with the event
 * @param host the host of the nick
 * @param handle the handle of the nick
 * @param channel the channel of the event
 * @param newnick the nick with which to replace "nick".
 * @return true or false.
 */
extern "C" bool bMotionEventNick(const char* nick, const char* host,
		const char* handle, const char* channel,
		const char* newnick)
{
	if (!bMotionSettings().isChannelAllowed(channel))
		return false;

	return bMotionDoEventResponse(Nick, nick, host, handle, channel, 
			newnick);
}

/*
 * Handle the action event type
 * @param nick the nick associated with the event
 * @param host the host of the nick
 * @param handle the handle of the nick
 * @param dest destination of the action
 * @param keyword keyword of the action
 * @param test test of the action
 * @return true or false.
 */
extern "C" bool bMotionEventAction(const char* nick, const char* host, 
		const char* handle, const char* dest, 
		const char* /*keyword*/, const char* text)
{
	if (!bMotionSettings().isChannelAllowed(dest))
		return false;

	String processedText(text);
	processedText.trim();
	processedText.replaceAll("  +", " ");

	// run complex plugins
	std::vector< ComplexPlugin* > complexplugins = 
		bMotionSystem().findComplexPlugins(processedText,
				bMotionSettings().language());
	int size = complexplugins.size();
	if (size > 0)
	{
		for (int i = 0; i < size; i++)
		{
			ComplexPlugin* plugin = complexplugins[i];
			if (plugin->execute(nick, host, handle, dest,
						processedText))
				break;
		}
	}

	return true;
}

/*
 * Load all the libraries and plugins from the plugin path from settings.
 * @return true or false.
 */
bool bMotionLoadPlugins()
{
	int loadCount = 0;
#ifndef WIN32
	DIR* directory = opendir(bMotionSettings().pluginPath());
	if (!directory)
	{
		bMotionLog(1, "could not open plugin directory \"%s\"",
				(const char*)bMotionSettings().pluginPath());
		return false;
	}
	struct dirent* entry;
	while ((entry = readdir(directory)) != NULL)
	{
		String name(entry->d_name);
		if (name.matches("lib.*\\.so"))
		{
			bMotionLog(1, "trying to load \"%s\"",
					(const char*)name);
			String fullname = bMotionSettings().pluginPath();
			if (fullname[fullname.length()-1] != '/')
				fullname.concat("/");
			fullname.concat(name);
			if (bMotionSystem().loadLibrary(fullname))
				loadCount++;
		}
	};
	closedir(directory);
	return (loadCount > 0);
#else
	WIN32_FIND_DATA data;
	String path = bMotionSettings().pluginPath();
	if (path.charAt(path.length() - 1) != '\\')
		path.concat("\\");
	path.concat("*.*");
	HANDLE dirHandle = FindFirstFile((const char*)path, &data);
	if (dirHandle == INVALID_HANDLE_VALUE)
	{
		bMotionLog(1, "could not open plugin directory \"%s\"",
				(const char*)bMotionSettings().pluginPath());
		return false;
	}
	do 
	{
		String name(data.cFileName);
		if (name.matches("lib.*\\.dll"))
		{
			bMotionLog(1, "trying to load \"%s\"",
					(const char*)name);
			String fullname = bMotionSettings().pluginPath();
			if (fullname[fullname.length()-1] != '\\')
				fullname.concat("\\");
			fullname.concat(name);
			if (bMotionSystem().loadLibrary(fullname))
				loadCount++;
		}
	}
	while (FindNextFile(dirHandle, &data) != 0);
	FindClose(dirHandle);
	return (loadCount > 0);
#endif
}

/*
 * unload all the plugins
 * @return true or false.
 */
bool bMotionUnloadAllPlugins()
{
	bMotionLog(1, "trying to unload all plugins");
	return bMotionSystem().removeAllLibraries();
}

/*
 * rehash the plugins
 * @return true or false
 */
bool bMotionRehash()
{
	bMotionLog(1, "bMotion: rehashing");
	if (!bMotionUnloadAllPlugins())
		return false;
	return bMotionLoadPlugins();
}

/*
 * check if the given name is our own
 * @param name the name for which to check
 * @return true or false.
 */
extern "C" bool bMotionIsBotnick(const char* /*name*/)
{
	return false;
}

/*
 * Add a timer event to the system level.
 * @param milli milliseconds to wait for the event
 * @param callback callback to use after milli milliseconds
 * @param param arbitrary parameter to pass to callback
 * @return true or false.
 */
extern "C" bool bMotionAddTimer(unsigned long milli, void(*callback)(void*),
		void* param)
{
	return bMotionSystem().addTimer(milli, callback, param);
}

/*
 * switch the system to a different language.
 * @param language string representation of language (i.e. "en" is english)
 * @return true or false.
 */
extern "C" bool bMotionUseLanguage(const char* language)
{
	Language lang = Settings::getLanguageFromString(language);
	if (lang == any || lang == bMotionSettings().language())
		return false;
	if (!bMotionSettings().setLanguage(lang))
		return false;
	if (!bMotionUnloadAllPlugins())
		return false;
	return bMotionLoadPlugins();
}

/*
 * Register a new abstract type with the system
 * @param type the type name of the abstract
 * @return true or false.
 */
extern "C" bool bMotionAbstractRegister(const char* type)
{
	return bMotionSystem().abstractRegister(type);
}
 
/*
 * Add a whole bunch of strings into an abstract
 * @param type the type name of the abstract
 * @param ... stuff
 * @return true or false
 */
extern "C" bool bMotionAbstractBatchAdd(const char* type, ...)
{
	va_list ap;
	va_start(ap, type);
	char* val = va_arg(ap, char*);
	while (val != ABSTRACT_END)
	{
		if (!bMotionSystem().abstractAddValue(type, val))
			return false;
		val = va_arg(ap, char*);
	};
	va_end(ap);
	return true;
}

/*
 * get the bMotion Settings object
 * @return bMotion Settings
 */
Settings& bMotionSettings()
{
	static Settings internalSettings;
	return internalSettings;
}

/*
 * get the bMotion System object
 * @return the bMotion System.
 */
System& bMotionSystem()
{
	static System internalSystem;
	return internalSystem;
}

/*
 * report a system status onto the log
 */
extern "C" void bMotionStatus()
{
	bMotionSettings().dump();
	bMotionSystem().dump();
}

/*
 * set user defined settings
 * @param setting the name of the setting
 * @param value the value to set
 */
extern "C" void bMotionSet(const char* setting, const char* value)
{
	bMotionSettings().set(setting, value);
}

/*
 * get a user setting
 * @param setting the name of the setting
 * @return the value
 */
extern "C" const char* bMotionGet(const char* setting)
{
	String& val = bMotionSettings().get(setting);
	if (val.length() == 0)
		return NULL;
	else
		return val;
}

/*
 * Get info variable from the system
 * @param name of the variable
 * @return the value
 */
extern "C" const void* bMotionInfo(const char* namestr)
{
	String name = namestr;
	if (name.equals("minRandomDelay"))
		return (const void*)bMotionSettings().minRandomDelay();
	if (name.equals("maxRandomDelay"))
		return (const void*)bMotionSettings().maxRandomDelay();
	return NULL;
}

