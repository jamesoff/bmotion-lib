#ifndef SIMPLEPLUGIN_H
#define SIMPLEPLUGIN_H

#include <Plugin.h>

// declare the Simple Plugin Function type using only primitives.
typedef bool(*SimplePluginFunc)(const char*, const char*, const char*,
		const char*, const char*);

/*
 * SimplePlugin class. Child of Plugin class.
 */
class SimplePlugin : public Plugin
{
public:
	SimplePlugin(Library* source, const String& name, 
			const String& funcName, const String& regexp,
			int chance, Language lang);
	virtual ~SimplePlugin();

	// simple plugin specific execution
	virtual bool execute(const String& nick, const String& host,
			const String& handle, const String& channel,
			const String& text);
	
	// overridden from class Plugin
	virtual bool refreshCallback();
	
private:
	// callback to the plugin code
	SimplePluginFunc _callback;
	
};

#endif

