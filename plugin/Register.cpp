#include <bMotion.h>
#include <SimplePlugin.h>
#include <ComplexPlugin.h>
#include <AdminPlugin.h>
#include <OutputPlugin.h>
#include <Output.h>

/*
 * Exposed Simple Plugin registration function.
 * @param pluginName the name for the plugin. Must be unique in the system.
 * @param callbackName the name of the callback function for the plugin.
 * @param regexp the plugin matching regular expression.
 * @param chance the execution chance for the plugin (0-100).
 * @param lang the language for the plugin.
 */
extern "C" bool bMotionRegisterSimple(const char* pluginName, 
		const char* callbackName, const char* regexp, int chance,
		const char* lang)
{
	Language language = Settings::getLanguageFromString(lang);
	if (language != bMotionSettings().language() && language != any)
		return false;
	Library* lib = bMotionSystem().getActiveLibrary();
	SimplePlugin* plugin = new SimplePlugin(lib, pluginName, callbackName,
			regexp, chance, language);
	if (!plugin->enable() || !bMotionSystem().addPlugin(plugin))
	{
		delete plugin;
		return false;
	}
	bMotionLog(1, "Registered Simple Plugin '%s'", pluginName);
	return true;
}

/*
 * Exposed Simple Plugin registration function.
 * @param pluginName the name for the plugin. Must be unique in the system.
 * @param callbackName the name of the callback function for the plugin.
 * @param regexp the plugin matching regular expression.
 * @param chance the execution chance for the plugin (0-100).
 * @param lang the language for the plugin.
 */
extern "C" bool bMotionRegisterComplex(const char* pluginName,
		const char* callbackName, const char* regexp, int chance,
		const char* lang)
{
	Language language = Settings::getLanguageFromString(lang);
	if (language != bMotionSettings().language() && language != any)
		return false;
	Library* lib = bMotionSystem().getActiveLibrary();
	ComplexPlugin* plugin = new ComplexPlugin(lib, pluginName, callbackName,
			regexp, chance, language);
	if (!plugin->enable() || !bMotionSystem().addPlugin(plugin))
	{
		delete plugin;
		return false;
	}
	bMotionLog(1, "Registered Complex Plugin '%s'", pluginName);
	return true;
}

/*
 * Exposed Event Plugin registration function.
 * @param pluginName the name for the plugin. Must be unique in the system.
 * @param callbackName the name of the callback function for the plugin.
 * @param type the type of event for which this plugin will trigger
 * @param regexp the plugin matching regular expression.
 * @param chance the execution chance for the plugin (0-100).
 * @param lang the language for the plugin.
 */
extern "C" bool bMotionRegisterEvent(const char* pluginName,
		const char* callbackName, const char* type, const char* regexp,
		int chance, const char* lang)
{
	Language language = Settings::getLanguageFromString(lang);
	if (language != bMotionSettings().language() && language != any)
		return false;
	Library* lib = bMotionSystem().getActiveLibrary();
	EventPlugin* plugin = new EventPlugin(lib, pluginName, type, 
			callbackName, regexp, chance, language);
	if (!plugin->enable() || !bMotionSystem().addPlugin(plugin))
	{
		delete plugin;
		return false;
	}
	bMotionLog(1, "Registered Event Plugin '%s'", pluginName);
	return true;
}

/*
 * Exposed Admin Plugin registration function.
 * @param pluginName the name for the plugin. Must be unique in the system.
 * @param callbackName the name of the callback function for the plugin.
 * @param regexp the plugin matching regular expression.
 * @param lang the language for the plugin.
 */
extern "C" bool bMotionRegisterAdmin(const char* pluginName,
		const char* callbackName, const char* command,
		const char* lang)
{
	Language language = Settings::getLanguageFromString(lang);
	if (language != bMotionSettings().language() && language != any)
		return false;
	Library* lib = bMotionSystem().getActiveLibrary();
	AdminPlugin* plugin = new AdminPlugin(lib, pluginName,
			callbackName, command, language);
	if (!plugin->enable() || !bMotionSystem().addPlugin(plugin))
	{
		delete plugin;
		return false;
	}
	bMotionLog(1, "Registered Admin Plugin '%s'", pluginName);
	return true;
}

/*
 * Exposed Output Plugin registration function.
 * @param pluginName the name for the plugin. Must be unique in the system.
 * @param callbackName the name of the callback function for the plugin.
 * @param regexp the plugin matching regular expression.
 * @param chance the execution chance for the plugin (0-100).
 * @param lang the language for the plugin.
 */
extern "C" bool bMotionRegisterOutput(const char* pluginName, 
		const char* callbackName, const char* regexp, int chance,
		const char* lang)
{
	Language language = Settings::getLanguageFromString(lang);
	if (language != bMotionSettings().language() && language != any)
		return false;
	Library* lib = bMotionSystem().getActiveLibrary();
	OutputPlugin* plugin = new OutputPlugin(lib, pluginName, callbackName,
			regexp, chance, language);
	if (!plugin->enable() || !bMotionSystem().addPlugin(plugin))
	{
		delete plugin;
		return false;
	}
	bMotionLog(1, "Registered Output Plugin '%s'", pluginName);
	return true;
}

