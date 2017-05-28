#include <ComplexPlugin.h>
#include <bMotion.h>
#include <Output.h>
#include <DynamicLoader.h>

/*
 * Constructor for Complex Plugin type.
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
ComplexPlugin::ComplexPlugin(Library* source, const String& name,
		const String& funcName, const String& regexp, int chance,
		Language lang)
: Plugin(source, Complex, name, funcName, regexp, chance, lang)
, _callback(NULL)
{
	
}

/*
 * Destructor
 */
ComplexPlugin::~ComplexPlugin()
{
	
}

/*
 * Complex Plugin specific invocation method.
 * @param nick The nick associated with invoking the plugin.
 * @param host The host associated with invoking the plugin.
 * @param handle The handle associated with invoking the plugin.
 * @param channel The channel in which the plugin was invoked.
 * @param text The invoking text matching the plugin regular expression.
 * @return true or false. will return false if the execution fails (either that
 * 	   there is no callback, or it's not enabled), or if the callback
 * 	   returns false which will be viewed as a failed run.
 */
bool ComplexPlugin::execute(const String& nick, const String& host, 
			const String& handle, const String& channel, 
			const String& text)
{
	if (!_callback || !_enabled)
		return false;
	bool success = false;
	Library* oldLib = bMotionSystem().getActiveLibrary();
	if (bMotionSystem().startDangerousCode() == 0)
	{
		bMotionSystem().setActiveLibrary(_source);
		success = _callback(nick, host, handle, channel, text);
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
bool ComplexPlugin::refreshCallback()
{
	_callback = (ComplexPluginFunc)dlSymbol(_source->getHandle(),
			_funcName);
	return (_callback != NULL);
}

