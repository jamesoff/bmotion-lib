#include "Output.h"
#include <stdarg.h>
#include <stdio.h>
#include <bMotion.h>
#include <bString.h>
#include <vector>
#include <OutputPlugin.h>

// external output function callback
// (type, target, message)
static void(*externalOutput)(int, const char*, const char*) = NULL;

// proc bMotionDoInterpolation { line nick moreText { channel "" } } 
bool bMotionDoInterpolation(String& line, const String& /*nick*/, 
		const String& /*moreText*/, const String& /*channel*/)
{
	//bMotionLog(1, "bMotion: bMotionDoInterpolation(%s,%s,%s,%s)",
	//		(const char*)line, (const char*)nick, 
	//	  	(const char*)moreText, (const char*)channel);

	if (line.length() == 0)
		return false;
	
	int loop = 0;
	while (line.matches("%VAR\\{.+\\}"))
	{
		loop++;
		if (loop > 10)
		{
			bMotionLog(1, "bMotion: ALERT! looping too much in %VAR code with %s", (const char*)line);
			line = "/has had a tremendous failure working something out";
			break;
		}
		int pos = line.indexOf("%VAR{");
		int pos2 = line.indexOf("}", pos);
		String type = line.substring(pos + 5, pos2);
		bMotionLog(1, "type == %s", (const char*)type);
		String val = bMotionSystem().abstractGetValue(type);
		if (val.length() == 0)
		{
			bMotionLog(1, "bMotion: ALERT! empty abstract returned");
			line = "/has had a tremendous failure working something out";
			break;
		}
		line.replaceFirst("%VAR\\{.+\\}", (const char*)val);
	};

	return true;
}

bool bMotionSayLine(const String& channel, const String& nick, 
		String& line, const String& moreText, bool /*urgent*/)
{
	if (line.equals("%STOP"))
		return false;
	
	// TODO: this would be a good place for interbot communication

	// Output Plugins
	std::vector< OutputPlugin* > outputplugins = 
		bMotionSystem().findOutputPlugins(line, 
				bMotionSettings().language());
	int size = outputplugins.size();
	bMotionLog(1, "found %d output plugins", size);
	if (size > 0)
	{
		for (int i = 0; i < size; i++)
		{
			OutputPlugin* plugin = outputplugins[i];
			plugin->execute(channel, nick, line, moreText);
		}
	}
	
	if (line.length() == 0 || line.matches("^ +$"))
		return false;

	line.replaceAll("%slash", "/");
	if (line.matches("^/")) // actions
	{
		char newline[510];
		sprintf(newline, "%cACTION %s%c", '\001', 
				(const char*)line.substring(1),
				'\001');
		line = newline;
	}
	
	// TODO: need output queue system
	bMotionLog(1, "output: %s", (const char*)line);
	if (externalOutput)
		externalOutput(BMOTION_PRIVMSG, channel, line);
	else
		printf("%s: %s\n", (const char*)channel, (const char*)line);
	return true;
}

//proc bMotionDoAction {channel nick text {moreText ""} {noTypo 0} {urgent 0} }
bool internal_DoAction(const String& channel, const String& nick, 
		const String& text, const String& moreText, bool urgent)
{
	String pText(text);
	if (!bMotionDoInterpolation(pText, nick, moreText, channel))
		return false;

	if (pText.indexOf("%|") != -1)
	{
		// multipart string
		std::vector< String > thingsToSay;
		int pos = 0;
		int loopCount = 0;
		while ((pos = pText.indexOf("%|")) != -1)
		{
			String sentence = pText.substring(0, pos);
			pText = pText.substring(pos + 2);
			if (sentence.length() != 0)
				thingsToSay.push_back(sentence);
			loopCount++;
			if (loopCount > 20)
			{
				bMotionLog(1, "bMotion ALERT: Bailed in bMotionDoAction with %s. Lost output.", (const char*)pText);
				return false;
			}
		}
		if (pText.length() != 0)
			thingsToSay.push_back(pText);
		int size = thingsToSay.size();
		for (int i = 0; i < size; i++)
		{
			if (!bMotionSayLine(channel, nick, thingsToSay[i],
						moreText, urgent))
			{
				bMotionLog(1, "bMotion: bMotionSayLine returned 1, skipping rest of output");
				break;
			}
		}
		return (size > 0);
	}
	
	return bMotionSayLine(channel, nick, pText, moreText, urgent);
}

//proc bMotionDoAction {channel nick text {moreText ""} {noTypo 0} {urgent 0} }
extern "C" bool bMotionDoAction(const char* channel, const char* nick,
		const char* text, const char* moreText, bool urgent)
{
	//bMotionLog(1, "bMotion: bMotionDoAction(%s,%s,%s,%s,%d)", channel,
	//		nick, text, moreText, urgent);

	if (channel)
	{
		if (!bMotionSettings().isChannelAllowed(channel))
			return false;
		return internal_DoAction(channel, nick, text, moreText, urgent);
	}
	std::vector< String > channels = bMotionSettings().channels();
	bool success = false;
	for (unsigned int i = 0; i < channels.size(); i++)
	{
		if (bMotionSettings().isChannelSilent(channels[i]))
			continue;
		if (internal_DoAction(channels[i], nick, text, moreText,
					urgent))
			success = true;
	}
	return success;
}

/*
 * logging for bmotion debugging and information
 * @param level the level for the logging
 * @param fmt the string format similar to printf()
 * @param ... parameter for fmt.
 */
extern "C" void bMotionLog(int level, const char* fmt, ...)
{
	char text[1024];
	va_list ap;

	if (!fmt)
		return;

	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);
	
	if (externalOutput)
		externalOutput(BMOTION_LOG + level, "*", text);
	else
		printf("%d: %s\n", level, text);
}

/*
 * Set the output function to something other than stdout
 * @param func the output function
 */
extern "C" void bMotionSetOutput(void(*func)(int, const char*, const char*))
{
	externalOutput = func;
}

