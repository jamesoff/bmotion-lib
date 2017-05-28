#include <AdminPlugin.h>
#include <bMotion.h>
#include <Output.h>
#include <DynamicLoader.h>

/*
 * Constructor for Admin Plugin type.
 * @param source Pointer to the library that contains the callback function
 * 		 required.
 * @param name The name assigned to the plugin. Must be unique in the system.
 * @param funcName The name of the callback function.
 * @param command the admin command that invokes this plugin
 * @param lang The language of the plugin. Details of Language are in System.
 */
AdminPlugin::AdminPlugin(Library* source, const String& name, 
		const String& funcName,	const String& command,
		Language lang)
: Plugin(source, Admin, name, funcName, command, 100, lang)
, _callback(NULL)
{

}

/*
 * Destructor
 */
AdminPlugin::~AdminPlugin()
{
	
}

/*
 * Admin Plugin specific invocation method.
 * @param nick The nick associated with invoking the plugin.
 * @param host The host associated with invoking the plugin.
 * @param handle The handle associated with invoking the plugin.
 * @param channel The channel in which the plugin was invoked.
 * @param text The invoking text matching the plugin regular expression.
 * @return true or false. will return false if the execution fails (either that
 * 	   there is no callback, or it's not enabled), or if the callback
 * 	   returns false which will be viewed as a failed run.
 */
bool AdminPlugin::execute(const String& nick, const String& host,
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
bool AdminPlugin::refreshCallback()
{
	_callback = (AdminPluginFunc)dlSymbol(_source->getHandle(), _funcName);
	return (_callback != NULL);
}

