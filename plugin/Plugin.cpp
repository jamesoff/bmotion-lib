#include <Plugin.h>
#include <bMotion.h>
#include <Output.h>

/*
 * Constructor for Default Plugin type.
 * @param source Pointer to the library that contains the callback function
 * 		 required.
 * @param type The type of the Plugin. Must be a known type.
 * @param name The name assigned to the plugin. Must be unique in the system.
 * @param funcName The name of the callback function.
 * @param regexp The matching regular expression (POSIX extended) that will be
 * 		 be used to determine the invocation of the callback.
 * @param chance The percentage chance (0 - 100) that will be used in order
 * 		 to determine a random chance on invocation.
 * @param lang The language of the plugin. Details of Language are in System.
 */
Plugin::Plugin(Library* source, PluginType type, const String& name, 
		const String& funcName, const String& regexp, int chance, 
		Language lang)
: _source(source)
, _type(type)
, _name(name)
, _funcName(funcName)
, _regexp(regexp)
, _chance(chance)
, _language(lang)
, _enabled(false)
{
	// fix chance if it's out of range. 0 is allowed but will never run
	if (_chance < 0)
		_chance = 0;
	else if (_chance > 100)
		_chance = 100;
}

/*
 * Destructor
 */
Plugin::~Plugin()
{

}

/*
 * Get the name of the plugin
 * @return the given name of the plugin
 */
const String& Plugin::getName() const
{
	return _name;
}

/*
 * Get the pointer to the source library of the plugin callback.
 * @return the source library.
 */
const Library* Plugin::getSource() const
{
	return _source;
}

/*
 * Get the plugin regular expression
 * @return regular expression string
 */
const String& Plugin::getRegexp() const
{
	return _regexp;
}

/*
 * Get the language of the plugin. Should match system lanugage.
 * @return plugin language 
 */
Language Plugin::getLanguage() const
{
	return _language;
}

/*
 * Get the chance of this plugin running (0-100)%
 * @return plugin execution chance.
 */
int Plugin::getChance() const
{
	return _chance;
}

/*
 * check if the plugin is enabled or not
 * @return true or false for enabled or disabled.
 */
bool Plugin::isEnabled() const
{
	return _enabled;
}

/*
 * enable this plugin
 * @return true or false if enabling this plugin succeeded or not.
 */
bool Plugin::enable()
{
	if (!bMotionSettings().isPluginAllowed(_name))
		return false;
	if (_enabled)
		return false;
	if (!_source->isLoaded())
	{
		if (!_source->openLibrary())
			return false;
		else if (!refreshCallback())
			return false;
	}
	_enabled = true;
	bMotionLog(2, "plugin \"%s\" enabled", (const char*)_name);
	return true;
}

/*
 * Disable this Plugin.
 * @return true or false if disabling this plugin succeeded or not.
 */
bool Plugin::disable()
{
	if (!_enabled)
		return false;
	if (_source->isLoaded())
		bMotionSystem().disableLibrary(_source);
	_enabled = false;
	bMotionLog(2, "plugin \"%s\" disabled", (const char*)_name);
	return true;
}

/*
 * get the type of this plugin
 * @return plugin type.
 */
PluginType Plugin::getType() const
{
	return _type;
}

