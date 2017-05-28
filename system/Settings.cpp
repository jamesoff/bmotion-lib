#include "Settings.h"
#include <bString.h>
#include <File.h>
#include <Output.h>
#include <algorithm>

/*
 * Default Settings constructor
 */
Settings::Settings()
: _gender(Male)
, _orientation(Straight)
, _kinky(false)
, _friendly(false)
, _minrandomdelay(2)
, _maxrandomdelay(4)
, _language(en)
, _pluginpath("./plugins")
, _abstractpath("./abstracts")
{

}

/*
 * Destructor
 */
Settings::~Settings()
{

}

/*
 * Parse default config file
 * @return true or false if the file was parsed properly or not.
 */
bool Settings::parseConfigFile() 
{
	return parseConfigFile("settings.conf");
}

/*
 * Parse given config file
 * @param filename the filename of the config file to parse
 * @return true or false if the file was parsed properly or not.
 */
bool Settings::parseConfigFile(const String& filename) 
{
	File config(filename);
	if (!config.open())
		return false;

	_channels.clear();
	_noplugin.clear();

	while (!config.isEOF())
	{
		String line = config.readLine();
		line.trim();
		if (line.length() == 0)
			continue;
		if (line.charAt(0) == '#' || line.charAt(0) == '\n')
			continue; // skipping comments and newlines
		int pos = line.indexOf('=');
		if (pos > 0)
		{
			// setting
			String token = line.substring(0, pos);
			String value = line.substring(pos + 1);
			token.trim();
			value.trim();
			if (value.length() == 0)
			{
				bMotionLog(1, "missing value");
				return false;
			}
			if (token.equals("gender"))
			{
				if (value.equals("male"))
					_gender = Male;
				else if (value.equals("female"))
					_gender = Female;
				else
				{
					bMotionLog(1, "unknown gender value \"%s\"", (const char*)value);
					return false;
				}
			}
			else if (token.equals("orientation"))
			{
				if (value.equals("straight"))
					_orientation = Straight;
				else if (value.equals("gay"))
					_orientation = Gay;
				else if (value.equals("bi"))
					_orientation = Bi;
				else
				{
					bMotionLog(1, "unknown orientation value \"%s\"", (const char*)value);
					return false;
				}
			}
			else if (token.equals("kinky"))
			{
				if (value.equals("true"))
					_kinky = true;
				else if (value.equals("false"))
					_kinky = false;
				else
				{
					bMotionLog(1, "unknown kinky value \"%s\"", (const char*)value);
					return false;
				}
			}
			else if (token.equals("friendly"))
			{
				if (value.equals("true"))
					_friendly = true;
				else if (value.equals("false"))
					_friendly = false;
				else
				{
					bMotionLog(1, "unknown friendly value \"%s\"", (const char*)value);
					return false;
				}
			}
			else if (token.equals("language"))
			{
				Language temp = Settings::getLanguageFromString(value);
				if (temp == any)
				{
					bMotionLog(1, "cannot set language to \"%s\"", (const char*)value);
					return false;
				}
				_language = temp;
			}
			else if (token.equals("plugins"))
			{
				_pluginpath = value;
			}
			else if (token.equals("abstracts"))
			{
				_abstractpath = value;
			}
			else if (token.equals("channels"))
			{
				int p = value.indexOf(',');
				while (p > 0)
				{
					String ch = value.substring(0, p);
					value = value.substring(p + 1);
					ch.trim();
					value.trim();
					_channels.push_back(ch);
					p = value.indexOf(',');
				}
				_channels.push_back(value);
			}
			else if (token.equals("silent"))
			{
				int p = value.indexOf(',');
				while (p > 0)
				{
					String ch = value.substring(0, p);
					value = value.substring(p + 1);
					ch.trim();
					value.trim();
					_silent.push_back(ch);
					p = value.indexOf(',');
				}
				_silent.push_back(value);
			}
			else if (token.equals("noplugin"))
			{
				int p = value.indexOf(',');
				while (p > 0)
				{
					String plugin = value.substring(0, p);
					value = value.substring(p + 1);
					plugin.trim();
					value.trim();
					_noplugin.push_back(plugin);
					p = value.indexOf(',');
				}
				_noplugin.push_back(value);
			}
			else if (token.equals("minRandomDelay"))
				_minrandomdelay = atoi(value);
			else if (token.equals("maxRandomDelay"))
				_minrandomdelay = atoi(value);
			else
			{
				bMotionLog(1, "unknown token \"%s\"", 
						(const char*)token);
				return false;
			}
		}
		else
		{
			// command
		}
	}
	
	config.close();
	return true;
}

/*
 * dump the contents of the settings
 */
void Settings::dump()
{
	bMotionLog(1, "Settings...");
	bMotionLog(1, "--Personality");
	bMotionLog(1, "  gender == %s", (_gender == Male ? "Male" : "Female"));
	bMotionLog(1, "  orientation == %s", (_orientation == Straight ? "Straight" : (_orientation == Gay ? "Gay" : "Bi")));
	bMotionLog(1, "  kinky == %s", (_kinky ? "true" : "false"));
	bMotionLog(1, "  friendly == %s", (_friendly ? "true" : "false"));
	bMotionLog(1, "--bMotion");
	bMotionLog(1, "  Lanuage == %s", (_language == en ? "English" : (_language == fr ? "French" : "Dutch")));
	bMotionLog(1, "  Plugins Path == %s", (const char*)_pluginpath);
	bMotionLog(1, "  Abstracts Path == %s", (const char*)_abstractpath);
	bMotionLog(1, "  minRandomDelay == %d", _minrandomdelay);
	bMotionLog(1, "  maxRandomDelay == %d", _maxrandomdelay);
	bMotionLog(1, "  Channels:");
	int size = _channels.size();
	int i;
	for (i = 0; i < size; i++)
		bMotionLog(1, "    %s", (const char*)_channels[i]);
	if (size == 0)
		bMotionLog(1, "    (none)");
	bMotionLog(1, "  Silent:");
	size = _silent.size();
	for (i = 0; i < size; i++)
		bMotionLog(1, "    %s", (const char*)_silent[i]);
	if (size == 0)
		bMotionLog(1, "    (none)");
	bMotionLog(1, "  Disallowed Plugins:");
	size = _noplugin.size();
	for (i = 0; i < size; i++)
		bMotionLog(1, "    %s", (const char*)_noplugin[i]);
	if (size == 0)
		bMotionLog(1, "    (none)");
	bMotionLog(1, "--User");
	bMotionLog(1, "  Generic Settings:");
	std::map< String, String, ltstr >::iterator iter;
	iter = _settings.begin();
	if (iter == _settings.end())
		bMotionLog(1, "    (none)");
	while (iter != _settings.end())
	{
		bMotionLog(1, "    %s == %s", (const char*)iter->first,
				(const char*)iter->second);
		iter++;
	}
}

/*
 * get the gender setting
 * @return gender of the system
 */
Gender Settings::gender() const
{
	return _gender;
}

/*
 * get the sexual orientation of the system
 * @return orientation
 */
Orientation Settings::orientation() const
{
	return _orientation;
}

/*
 * check if the system is into kinky sex or not
 * @return true or false
 */
bool Settings::kinky() const
{
	return _kinky;
}

/*
 * check if the system is friendly
 * @return true or false
 */
bool Settings::friendly() const
{
	return _friendly;
}

/*
 * get the system level language setting
 * @return language of the system
 */
Language Settings::language() const
{
	return _language;
}

/*
 * get the plugin path
 * @return plugin path
 */
const String& Settings::pluginPath() const
{
	return _pluginpath;
}

/*
 * get the abstracts path
 * @return abstracts path
 */
const String& Settings::abstractPath() const
{
	return _abstractpath;
}

/*
 * check if a given channel is in the settings as an allowed channel
 * @param channel channel name for which to test
 * @return true or false
 */
bool Settings::isChannelAllowed(const String& channel) const
{
	return (std::find(_channels.begin(), _channels.end(), channel) !=
		_channels.end());
}

/*
 * check if a given channel is in the settings as a silent channel
 * @param channel channel name for which to test
 * @return true or false
 */
bool Settings::isChannelSilent(const String& channel) const
{
	return (std::find(_silent.begin(), _silent.end(), channel) !=
		_silent.end());
}

/*
 * check if a plugin (by name) is allowed or on the disallowed list
 * @param plugin the name of the plugin
 * @return true or false
 */
bool Settings::isPluginAllowed(const String& plugin) const
{
	return (std::find(_noplugin.begin(), _noplugin.end(), plugin) ==
		_noplugin.end());
}

/*
 * get the minimum random delay
 * @return the delay in minutes
 */
unsigned int Settings::minRandomDelay() const
{
	return _minrandomdelay;
}

/*
 * get the maximum random delay
 * @return the delay in minutes
 */
unsigned int Settings::maxRandomDelay() const
{
	return _maxrandomdelay;
}

/*
 * set the language of the system
 * @param lang the new system language
 * @return true or false if the language could be set or not
 */ 
bool Settings::setLanguage(Language lang)
{
	if (lang != any)
		_language = lang;
	return (lang == _language);
}

/*
 * set a user defined setting
 * @param setting the name of the setting
 * @param value the value to which to set it
 * @return ttrue or false.
 */
bool Settings::set(const String& setting, const String& value)
{
	_settings[setting] = value;
	return true;
}

/*
 * get a user defined setting
 * @param setting the name of the setting
 * @return the value of the setting
 */
String& Settings::get(const String& setting)
{
	return _settings[setting];
}

/*
 * get the allowed channels
 * @return a vector of allowed channels
 */
std::vector< String > Settings::channels() const
{
	return _channels;
}

/*
 * convert a string language representation into and internal language type
 * @param langstr the string representation
 * @return the language for the string.
 */
Language Settings::getLanguageFromString(const String& langstr)
{
	if (langstr.equals("en"))
		return en;
	else if (langstr.equals("fr"))
		return fr;
	else if (langstr.equals("nl"))
		return nl;
	else
		return any;
}

/*
 * convert a language to a string representation
 * @param lang the language
 * @return a string representation of lang.
 */
String Settings::getStringFromLanguage(Language lang)
{
	if (lang == en)
		return String("en");
	else if (lang == fr)
		return String("fr");
	else if (lang == nl)
		return String("nl");
	else
		return String("any");
}

