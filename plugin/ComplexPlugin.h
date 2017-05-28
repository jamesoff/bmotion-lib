#ifndef COMPLEXPLUGIN_H
#define COMPLEXPLUGIN_H

#include <Plugin.h>

// declare the Complex Plugin Function type using only primitives.
typedef bool(*ComplexPluginFunc)(const char*, const char*, const char*,
		const char*, const char*);

/*
 * ComplexPlugin class. Child of Plugin class.
 */
class ComplexPlugin : public Plugin
{
public:
	ComplexPlugin(Library* source, const String& name, 
			const String& funcName, const String& regexp,
			int chance, Language lang);
	virtual ~ComplexPlugin();

	// complex plugin specific execution
	virtual bool execute(const String& nick, const String& host, 
			const String& handle, const String& channel, 
			const String& text);
	
	// overridden from class Plugin
	virtual bool refreshCallback();
	
private:
	// callback to the plugin code
	ComplexPluginFunc _callback;
	
};

#endif

