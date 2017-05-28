#include <Abstract.h>
#include <algorithm>
#include <bMotion.h>
#include <File.h>
#include <Output.h>

/*
 * Default Construction
 */
Abstract::Abstract(const String& name)
: _ondisk(true)
, _language(any)
{
	if (name.length() == 0)
		_type = "undefined";
	else
		_type = name;
	char str[1024];
#ifndef WIN32
	sprintf(str, "%s/%s/%s.txt", 
			(const char*)bMotionSettings().abstractPath(),
			(const char*)Settings::getStringFromLanguage(
			bMotionSettings().language()),
	       		(const char*)_type);
#else
	sprintf(str, "%s\\%s\\%s.txt", 
			(const char*)bMotionSettings().abstractPath(),
			(const char*)Settings::getStringFromLanguage(
			bMotionSettings().language()),
	       		(const char*)_type);
#endif
	_filename = str;
}

/*
 * Destructor
 */
Abstract::~Abstract()
{

}

/*
 * create or load the abstract from file
 * @return true or false
 */
bool Abstract::create()
{
	time(&_timestamp);
	File file(_filename);
	if (file.exists())
		return loadType();
	else
	{
		file.createNewFile();	
		file.close();
		_values.clear();
		_language = bMotionSettings().language();
	}
	return true;
}

/*
 * load abstract data out of a file
 * @param filename the filename from which to read
 * @return true or false
 */
bool Abstract::loadType()
{
	if (!_ondisk)
		return false;

	File file(_filename);
	if (!file.exists())
		return false;
	
	_values.clear();
	
	// set timestamp to now
	time(&_timestamp);

	if (!file.open())
		return false;
	bMotionLog(1, "loading abstract '%s' off disk", (const char*)_type);
	String line;
	std::vector< String >::iterator item;
	bool needsReSave = false;
	while (!file.isEOF())
	{
		line = file.readLine().trim();
		if (line.length() == 0)
			continue;
		item = std::find(_values.begin(), _values.end(), line);
		if (item == _values.end())
			_values.push_back(line);
		else
			needsReSave = true;
	}
	file.close();
	if (needsReSave)
		storeType();
	
	_ondisk = false;
	return true;
}

/*
 * Store the abstract to disk
 * @return true or false.
 */
bool Abstract::storeType()
{
	File file(_filename);
	if (!file.open())
		return false;
	if (!file.emptyFile())
		return false;

	bool tidy = false;
	int count = _values.size();
	if (count > BMOTION_MAX_ABSTRACTS)
	{
		bMotionLog(1, "abstract %s has too many elements. tidying up",
				(const char*)_type);
		tidy = true;
	}
	int skipcount = 0;
	int newcount = 0;
	for (int i = 0; i < count; i++)
	{
		if (tidy && rand() % 100 < 10)
		{
			bMotionLog(1, "Dropping %s from abstract %s", 
					(const char*)_values[i], 
					(const char*)_type);
			skipcount++;
			continue;
		}
		file.writeLine(_values[i]);
		newcount++;
	}
	if (tidy)
		bMotionLog(1, "Abstract %s now has %d elements (%d fewer)",
				newcount, skipcount);
	file.close();
	return true;
}

/*
 * add a value to the abstract
 * @param value the value to add the the abstract
 * @param save force a disk save on the abstract
 * @return true or false. 
 */
bool Abstract::addValue(const String& value, bool save)
{
	if (_ondisk)
	{
		bMotionLog(1, "updating abstracts '%s' on disk", 
				(const char*)_type);
		if (!save)
			return false;
		File file(_filename);
		if (!file.open())
			return false;
		file.skip(file.available());
		file.writeLine(value);
		file.close();
		return true;
	}
	std::vector< String >::iterator item;
	item = std::find(_values.begin(), _values.end(), value);
	if (item != _values.end())
		return false;
	_values.push_back(value);
	if (save)
	{
		File file(_filename);
		if (!file.open())
			return false;
		file.skip(file.available());
		file.writeLine(value);
		file.close();
	}
	return true;
}

/*
 * Remove a specific value from the abstract
 * @param value the value to remove
 * @return true or false.
 */
bool Abstract::removeValue(const String& value, bool save)
{
	std::vector< String >::iterator item;
	item = std::find(_values.begin(), _values.end(), value);
	if (item == _values.end())
		return false;
	_values.erase(item);
	if (save)
		return storeType();
	return true;
}

/*
 * get a random value from an abstract
 * @return a string from the book of abstracts.
 */
String Abstract::getRandomValue()
{
	// set timestamp to now
	time(&_timestamp);
	if (_ondisk)
		loadType();
	int size = _values.size();
	if (size == 0)
		return "";//_values[0];
	int ind = rand() % size;
	return _values[ind];
}

/*
 * Abstract garbage collection. Simple, if enough time has gone by with no
 * activity, write to disk, free the memory.
 * @return true or false
 */
bool Abstract::garbageCollect()
{
	if (_ondisk)
		return false;
	time_t now;
	time(&now);
	if (difftime(now, _timestamp) > BMOTION_MAX_ABSTRACT_AGE ||
			_language != bMotionSettings().language())
	{
		bMotionLog(1, "Expiring abstract '%s'", (const char*)_type);
		_ondisk = true;
		_values.clear();
		return true;
	}
	return false;
}

/*
 * Timer callback for abstracts being removed
 * @param ____ nothing
 */
void bMotionAbstractGarbageCollect(void*)
{
	bMotionSystem().abstractGarbageCollect();
	bMotionSystem().addTimer(300000, bMotionAbstractGarbageCollect, NULL);
}

