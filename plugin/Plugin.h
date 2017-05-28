#ifndef PLUGIN_H
#define PLUGIN_H

#include <Library.h>
#include <bString.h>
#include <Settings.h>

// types of the plugins. all valid plugin types should be declared here.
enum PluginType { Simple = 0, Complex, Event, Admin, Output };

/*
 * All powerful parent class of all Plugin types.
 */
class Plugin
{
public:	
	// general construction
	Plugin(Library* source, PluginType type, const String& name,
			const String& funcName, const String& regexp,
			int chance, Language lang);
	virtual ~Plugin();

	// information
	const String& getName() const;
	const Library* getSource() const;
	const String& getRegexp() const;
	Language getLanguage() const;
	int getChance() const;
	bool isEnabled() const;

	// enable / disable the plugin
	bool enable();
	bool disable();
	
	// must be overridden in children.
	virtual bool refreshCallback() = 0;

	// type of the plugin. downcasting cheat.
	PluginType getType() const;

protected:
	// source library
	Library* _source;
	// plugin type
	PluginType _type;
	// plugin name
	String _name;
	// callback function name
	String _funcName;
	// plugin regular expression
	String _regexp;
	// plugin execution chance
	int _chance;
	// plugin language
	Language _language;
	// enabled flag
	bool _enabled;
};

#endif

