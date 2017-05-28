#ifndef ABSTRACT_H
#define ABSTRACT_H

#include <bString.h>
#include <time.h>
#include <vector>
#include <Settings.h>

class Abstract
{
public:
	Abstract(const String& name);
	virtual ~Abstract();

	// equivalent of register
	bool create();
	
	// addition / subtraction
	bool addValue(const String& value, bool save = true);
	bool removeValue(const String& value, bool save = true);
	
	// get
	String getRandomValue();

	// cleanup
	bool garbageCollect();
	
private:
	String _type;
	bool _ondisk;
	String _filename;
	std::vector< String > _values;
	time_t _timestamp;
	Language _language;

	bool loadType();
	bool storeType();
	
};

void bMotionAbstractGarbageCollect(void*);

#endif

