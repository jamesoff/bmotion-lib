#include "OutputPlugin.h"
#include <stdio.h>
#include <bMotion.h>
#include <Output.h>
#include <DynamicLoader.h>

/*
 * Constructor for Output Plugin type.
 * @param source Pointer to the library that contains the callback function
 * 		 required.
 * @param name The name assigned to the plugin. Must be unique in the system.
 * @param funcName The name of the callback function.
 * @param regexp The matching regular expression (POSIX extended) that will be
 * 		 be used to determine the invocation of the callback.
 * @param chance The percentage chance (0 - 100) that will be used in order
 * 		 to determine a random chance on invocation.
 * @param lang The language of the plugin. Details of Language are in System.
 */
OutputPlugin::OutputPlugin(Library* source, const String& name,
		const String& funcName, const String& regexp, 
		int chance, Language lang)
: Plugin(source, Output, name, funcName, regexp, chance, lang)
, _callback(NULL)
{

}

/*
 * Destructor
 */
OutputPlugin::~OutputPlugin()
{
	
}

/*
 * Output Plugin specific invocation method.
 * @param nick The nick associated with invoking the plugin.
 * @param channel The channel in which the plugin was invoked.
 * @param text The invoking text matching the plugin regular expression.
 * @param moreText additional text.
 * @return true or false. will return false if the execution fails (either that
 * 	   there is no callback, or it's not enabled), or if the callback
 * 	   returns false which will be viewed as a failed run.
 */
bool OutputPlugin::execute(const String& nick, const String& channel, 
		String& text, const String& moreText)
{
	if (!_callback || !_enabled)
		return false;
	bool success = false;
	Library* oldLib = bMotionSystem().getActiveLibrary();
	if (bMotionSystem().startDangerousCode() == 0)
	{
		bMotionSystem().setActiveLibrary(_source);
		char newtext[512];
		sprintf(newtext, "%s", (const char*)text);
		success = _callback(nick, channel, newtext, moreText);
		if (success)
			text = newtext;
	}
	bMotionSystem().setActiveLibrary(oldLib);
	if (bMotionSystem().endDangerousCode())
	{
		bMotionLog(1, "plugin \"%s\" has caused a serious error",
			(const char*)_name);
		return false;
	}
	return success;
}

/*
 * Refresh the callback method to the library. 
 * This is used because the library can be loaded and unloaded if the libraries
 * plugins are all disabled. This will attempt to get a new callback pointer.
 * @return true or false if the callback function can be refreshed from the 
 * 	   library or not.
 */
bool OutputPlugin::refreshCallback()
{
	_callback = (OutputPluginFunc)dlSymbol(_source->getHandle(), _funcName);
	return (_callback != NULL);
}

