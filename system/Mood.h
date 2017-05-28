#ifndef MOOD_H
#define MOOD_H

#include <bString.h>

class Mood
{
public:
	Mood(const String& name, int target, int lower = -30, int upper = 30);
	~Mood();
	
	const String& getName() const;
	int getValue() const;
	
	void drift();
	void increase(int amount = 1);
	void decrease(int amount = 1);
	
private:
	String _name;
	int _target;
	int _value;
	int _lower;
	int _upper;
};

void bMotionMoodDrift(void*);

#endif

