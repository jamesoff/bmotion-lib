#ifndef SETTINGS_H
#define SETTINGS_H

#include <vector>
#include <map>
#include <bString.h>

// gender type
enum Gender { Male = 0, Female };
// orientation type
enum Orientation { Straight = 0, Gay, Bi };
// language type
enum Language { any = 0, en, fr, nl };

/*
 * Settings class for system wide settings. NOTE: individual plugin settings
 * should be handled by the plugin and not the system.
 */
class Settings
{
public:
	Settings();
	virtual ~Settings();
	
	bool parseConfigFile();
	bool parseConfigFile(const String& filename);
	
	// diagnostics
	void dump();

	// information
	Gender gender() const;
	Orientation orientation() const;
	bool kinky() const;
	bool friendly() const;
	Language language() const;
	const String& pluginPath() const;
	const String& abstractPath() const;
	bool isChannelAllowed(const String& channel) const;
	bool isChannelSilent(const String& channel) const;
	bool isPluginAllowed(const String& plugin) const;
	unsigned int maxRandomDelay() const;
	unsigned int minRandomDelay() const;

	// set
	bool setLanguage(Language lang);
	bool set(const String& setting, const String& value);

	// get
	String& get(const String& setting);
	std::vector< String > channels() const;

	// utility
	static Language getLanguageFromString(const String& langstr);
	static String getStringFromLanguage(Language lang);

private:
	// personality stuff
	Gender _gender;
	Orientation _orientation;
	bool _kinky;
	bool _friendly;
	
	// behaviour stuff
	unsigned int _minrandomdelay;
	unsigned int _maxrandomdelay;
	
	// system stuff
	Language _language;
	String _pluginpath;
	String _abstractpath;
	std::vector< String > _channels;
	std::vector< String > _silent;
	std::vector< String > _noplugin;

	// generic settings
	std::map< String, String, ltstr > _settings;
};

#endif

