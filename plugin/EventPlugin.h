#ifndef EVENTPLUGIN_H
#define EVENTPLUGIN_H

#include <Plugin.h>

// declare the Event Plugin Function type using only primitives.
typedef bool(*EventPluginFunc)(const char*, const char*, const char*,
		const char*, const char*);

// Event types
enum EventType { Unknown = 0, Join, Nick, Quit, Part, Split };

/*
 * EventPlugin class. Child of Plugin.
 */
class EventPlugin : public Plugin
{
public:
	EventPlugin(Library* source, const String& name, const String& type, 
			const String& funcName, const String& regexp,
			int chance, Language lang);
	virtual ~EventPlugin();

	// event plugin specific execution
	virtual bool execute(const String& nick, const String& host, 
			const String& handle, const String& channel,
			const String& text);

	// overridden from class Plugin
	virtual bool refreshCallback();

	// information
	EventType getEventType() const;
	
private:
	// callback to the plugin code
	EventPluginFunc _callback;
	// type of this event
	EventType _eventType;
	
};

#endif

