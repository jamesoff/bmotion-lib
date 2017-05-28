#ifndef ADMINPLUGIN_H
#define ADMINPLUGIN_H

#include <Plugin.h>

// declare the Admin Plugin Function type using only primitives.
typedef bool(*AdminPluginFunc)(const char*, const char*, const char*,
		const char*, const char*);

/*
 * AdminPlugin class. Child of Plugin class.
 */
class AdminPlugin : public Plugin
{
public:
	AdminPlugin(Library* source, const String& name, 
			const String& funcName, const String& command,
			Language lang);
	virtual ~AdminPlugin();

	// simple plugin specific execution
	virtual bool execute(const String& nick, const String& host,
			const String& handle, const String& channel,
			const String& text);
	
	// overridden from class Plugin
	virtual bool refreshCallback();
	
private:
	// callback to the plugin code
	AdminPluginFunc _callback;
	
};

#endif

