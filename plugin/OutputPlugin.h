#ifndef OUTPUTPLUGIN_H
#define OUTPUTPLUGIN_H

#include <Plugin.h>

// declare OutputPlugin function type for all to see ;-P
typedef bool(*OutputPluginFunc)(const char*, const char*, char*, 
		const char*);

/*
 * Output plugin class
 */
class OutputPlugin : public Plugin
{
public:	
	OutputPlugin(Library* source, const String& name,
			const String& funcName, const String& regexp,
			int chance, Language lang);
	virtual ~OutputPlugin();

	virtual bool execute(const String& nick, const String& channel,
			String& text, const String& moreText);

	// overridden from class Plugin
	virtual bool refreshCallback();

private:
	// callback to the plugin code
	OutputPluginFunc _callback;
	
};

#endif

