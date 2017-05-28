#include <Mood.h>
#include <Output.h>
#include <bMotion.h>

/*
 * Construction
 */
Mood::Mood(const String& name, int target, int lower, int upper)
: _name(name)
, _target(target)
, _value(target)
, _lower(lower)
, _upper(upper)
{
	if (_lower > _upper)
	{
		int temp = _upper;
		_upper = _lower;
		_lower = temp;
	}
	if (_upper == _lower)
	{
		_lower = target - 30;
		_upper = target + 30;
	}
	if (target < _lower || target > _upper)
		target = (_upper + _lower) / 2;
	_value = target;
}

/*
 * Destructor
 */
Mood::~Mood()
{
	
}

/*
 * get the name of the mood
 * @return reference to the name
 */
const String& Mood::getName() const
{
	return _name;
}

/*
 * get the current value of the mood
 * @return the value of the mood
 */
int Mood::getValue() const
{
	return _value;
}

/*
 * drift the mood slowly to its target value
 */
void Mood::drift()
{
	bool drifted = false;
	if (_value > _target)
	{
		_value--;
		drifted = true;
	}
	if (_value < _target)
	{
		_value += 2;
		drifted = true;
	}
	if (drifted)
		bMotionLog(1, "bMotion: drift mood '%s' now %d",
				(const char*)_name, _value);
	// check oob
	if (_value < _lower)
		_value = _lower;
	else if (_value > _upper)
		_value = _upper;
}

/*
 * increase the mood by a given value or default 1
 * @param amount the amount by which to increase (default 1)
 */
void Mood::increase(int amount)
{
	_value += amount;
}

/*
 * decrease the mood by a given value or default 1
 * @param amount the amount by which to decrease (default 1)
 */
void Mood::decrease(int amount)
{
	increase(-amount);
}

/*
 * Drift callback
 * @param **
 */
void bMotionMoodDrift(void*)
{
	bMotionSystem().moodDrift();
	bMotionSystem().addTimer(1000, bMotionMoodDrift, NULL);
}

/*
 * increase the mood by a given value or default 1
 * @param name the name of the mood
 * @param amount the amount by which to increase (default 1)
 */
extern "C" bool bMotionMoodIncrease(const char* name, int amount = 1)
{
	return bMotionSystem().moodIncrease(name, amount);
}

/*
 * decrease the mood by a given value or default 1
 * @param name the name of the mood
 * @param amount the amount by which to decrease (default 1)
 */
extern "C" bool bMotionMoodDecrease(const char* name, int amount = 1)
{
	return bMotionSystem().moodDecrease(name, amount);
}

/*
 * Create a new mood of a given name
 * @param name the name of the new mood
 * @param centre the target value of the mood
 * @param lower the lower bounds of the mood
 * @param upper the upper bounds of the mood
 * @return true if succeeded, false in error
 */
extern "C" bool bMotionMoodCreate(const char* name, int centre, int lower = -30,
		int upper = 30)
{
	return bMotionSystem().moodCreate(name, centre, lower, upper);
}

/*
 * Get the value of a mood
 * @param name the name of the mood
 * @return the value of the named mood
 */
extern "C" int bMotionMoodGet(const char* name)
{
	return bMotionSystem().moodGet(name);
}

