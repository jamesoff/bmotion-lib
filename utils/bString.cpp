/*
 * mutable String class.
 * Author: Andrew Payne
 * Copyright: ATR Lab © 2004
 */

#include "bString.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

#define ADJUST_SIZE(size, len) for(size = 1; size <= len; size = size << 1);

/*
 * comparison routine for std::maps involving string keys*
 */
bool ltstr::operator()(const char* s1, const char* s2) const
{
	return strcmp(s1, s2) < 0;
}

/*
 * Default constructor.
 * creates an empty string that can be modified.
 */
String::String()
: _string(NULL)
, _alloc(0)
, _length(0)
{
	
}

/*
 * Constructor
 * Creates a new String from a given NULL terminated character array. If the
 * offset and count parameters are used, a substring of the character array
 * will be used.
 * @param value the character string.
 * @param offset the offset position from which to start the substring.
 * @param count the length of the substring from offset.
 */
String::String(const char* value, int offset, int count)
: _string(NULL)
, _alloc(0)
, _length(0)
{
	if (!value || !strlen(value))
		return;
	int len = strlen(value);
	if (offset >= len || (count > 0 && offset + count >= len))
		return;
	if (offset < 0) offset = 0;
	if (offset > 0) len -= offset;
	if (count > 0) len = count;
	if (count <= 0) count = len;
	int size = 0;
	ADJUST_SIZE(size, len);
	_string = new char[size];
	_string[0] = '\0';
	_alloc = size;
	_length = len;
	strncat(_string, (char*)(value + offset), count);
}

/*
 * Constructor.
 * Creates a new String from a String. If the offset and count parameters
 * are used, a substring of the character array will be used.
 * @param value the character string.
 * @param offset the offset position from which to start the substring.
 * @param count the length of the substring from offset.
 */
String::String(const String& original, int count, int offset)
: _string(NULL)
, _alloc(0)
, _length(0)
{
	if (!original.length())
		return;
	int len = original.length();
	if (offset >= len || (count > 0 && offset + count >= len))
		return;
	if (offset < 0) offset = 0;
	if (offset > 0) len -= offset;
	if (count > 0) len = count;
	if (count <= 0) count = len;
	int size = 0;
	ADJUST_SIZE(size, len);
	_string = new char[size];
	_string[0] = '\0';
	_alloc = size;
	_length = len;
	strncat(_string, (char*)(original._string + offset), count);
}

/*!
 * Destructor
 */
String::~String()
{
	if (_string)
		delete [] _string;
	_string = NULL;
}

/*
 * Compares this string to the specified object.
 * @param other the character string to compare this String against.
 * @return the result of UNICODE comparison
 */
int String::compareTo(const char* other) const
{
	bool otherExists = (other && strlen(other));
	bool thisExists = (length() > 0);
	if (!otherExists && !thisExists)
		return 0;
	if (thisExists && !otherExists)
		return -1;
	if (!thisExists && otherExists)
		return 1;
	return strcmp(_string, other);
}

/*
 * Compares this string to the specified object.
 * @param other the character string to compare this String against.
 * @return the result of UNICODE comparison
 */
int String::compareTo(const String& other) const
{
	return compareTo(other._string);
}

/*
 * Compares this string to the specified object without regard to case.
 * @param other the character string to compare this String against.
 * @return the result of UNICODE comparison
 */
int String::compareToIgnoreCase(const char* other) const
{
	bool otherExists = (other && strlen(other));
	bool thisExists = (length() > 0);
	if (!otherExists && !thisExists)
		return 0;
	if (thisExists && !otherExists)
		return -1;
	if (!thisExists && otherExists)
		return 1;
	int i = 0;
	int thisLen = length();
	int otherLen = strlen(other);
	if (thisLen < otherLen)
		return -1;
	else if (thisLen > otherLen)
		return 1;
	for (i = 0; i < thisLen && i < otherLen; i++)
	{
		char a = (char)tolower(_string[i]);
		char b = (char)tolower(other[i]);
		if (a < b)
			return -1;
		else if (a > b)
			return 1;
	}
	return 0;
}

/*
 * Compares this string to the specified object without regard to case.
 * @param other the character string to compare this String against.
 * @return the result of UNICODE comparison
 */
int String::compareToIgnoreCase(const String& other) const
{
	return compareToIgnoreCase(other._string);
}

/*
 * Compares this string to the specified object. The result is true if and
 * only if the argument is not null and is a String object that represents
 * the same sequence of characters.
 * @param other the character string to compare this String against.
 * @return true if the String are equal; false otherwise.
 */
bool String::equals(const char* other) const
{
	return compareTo(other) == 0;
}

/*
 * Compares this string to the specified object. The result is true if and
 * only if the argument is not null and is a String object that represents
 * the same sequence of characters.
 * @param other the String to compare this String against.
 * @return true if the String are equal; false otherwise.
 */
bool String::equals(const String& other) const
{
	return equals(other._string);
}

/*
 * Compares this string to the string without regard to case. The result is
 * true if and only if the argument is not null and is a String object that
 * represents the same sequence of characters ignoring case.
 * @param other the character string to compare this String against.
 * @return true if the String are equal; false otherwise.
 */
bool String::equalsIgnoreCase(const char* other) const
{
	return compareToIgnoreCase(other) == 0;
}

/*
 * Compares this string to the string without regard to case. The result is
 * true if and only if the argument is not null and is a String object that
 * represents the same sequence of characters ignoring case.
 * @param other the String to compare this String against.
 * @return true if the String are equal; false otherwise.
 */
bool String::equalsIgnoreCase(const String& other) const
{
	return equalsIgnoreCase(other._string);
}

/*
 * add a string to this string
 * @param other the string to add
 * @return this string with the other string added.
 */
String& String::concat(const char* other)
{
	if (!other || !strlen(other))
		return *this;
	int thisLen = length();
	int otherLen = strlen(other);
	if (_alloc <= thisLen + otherLen)
	{
		int size = 0;
		ADJUST_SIZE(size, thisLen + otherLen)
		char* temp = new char[size];
		temp[0] = '\0';
		if (_string)
			strcat(temp, _string);
		strcat(temp, other);
		delete [] _string;
		_string = temp;
		_alloc = size;
		_length = thisLen + otherLen;
	}
	else
	{
		strcat(_string, other);
		_length = thisLen + otherLen;
	}
	return *this;
}

/*
 * add a string to this string
 * @param other the string to add
 * @return this string with the other string added.
 */
String& String::concat(const String& other)
{
	return concat(other._string);
}

/*
 * replace all the occurrances of the old character with the new character.
 * @param oldChar the old character to search for
 * @param newChar the replacement character
 * @return this.
 */
String& String::replace(char oldChar, char newChar)
{
	if (oldChar == '\0' || newChar == '\0')
		return *this; // exemptions
	for (int i = 0; i < _length; i++)
	{
		if (_string[i] == oldChar)
			_string[i] = newChar;
	}
	return *this;
}

/*
 * replace all the occurances of matches for the regular expression with the
 * replacement string.
 * @param regex the regular expression to match for
 * @param replacement the replacement string
 * @return this.
 */
String& String::replaceAll(const char* regex, const char* replacement,
		bool ignoreCase)
{
	if (!regex || !strlen(regex) || !replacement)
		return *this;

	regex_t re;
	regmatch_t pm;
	int flags = REG_EXTENDED;
	if (ignoreCase)
		flags &= REG_ICASE;
	if (regcomp(&re, regex, flags) != 0)
		return *this;
	int error = regexec(&re, _string, 1, &pm, 0);
	while (error == 0)
	{
		int start = pm.rm_so;
		int end = pm.rm_eo;
		int newlen = _length - (end - start) + strlen(replacement);
		int size = 1;
		ADJUST_SIZE(size, newlen)
		char* temp = NULL;
		bool alloced = true;
		if (size <= _alloc)
		{
			size = _alloc;
			alloced = false;
		}
		temp = new char[size];
		temp[0] = '\0';
		if (start > 0)
			strncat(temp, _string, start);
		strcat(temp, replacement);
		if (end < _length)
			strcat(temp, _string + end);
		_length = newlen;
		if (alloced)
			_alloc = size;
		delete [] _string;
		_string = temp;
		error = regexec(&re, _string + pm.rm_eo, 1, &pm, REG_NOTBOL);
	}
	regfree(&re);
	return *this;
}

/*
 * replace all the occurances of matches for the regular expression with the
 * replacement string.
 * @param regex the regular expression to match for
 * @param replacement the replacement string
 * @return this.
 */
String& String::replaceAll(const String& regex, const String& replacement,
		bool ignoreCase)
{
	return replaceAll(regex._string, replacement._string, ignoreCase);
}

/*
 * replace the first occurance of a match for the regular expression with the
 * replacement string.
 * @param regex the regular expression to match for
 * @param replacement the replacement string
 * @return this.
 */
String& String::replaceFirst(const char* regex, const char* replacement,
		bool ignoreCase)
{
	if (!regex || !strlen(regex) || !replacement)
		return *this;
	regex_t re;
	regmatch_t pm;
	int flags = REG_EXTENDED;
	if (ignoreCase)
		flags &= REG_ICASE;
	if (regcomp(&re, regex, flags) != 0)
		return *this;
	int error = regexec(&re, _string, 1, &pm, 0);
	if (error == 0)
	{
		int start = pm.rm_so;
		int end = pm.rm_eo;
		int newlen = _length - (end - start) + strlen(replacement);
		int size = 1;
		ADJUST_SIZE(size, newlen)
		char* temp = NULL;
		bool alloced = true;
		if (size <= _alloc)
		{
			size = _alloc;
			alloced = false;
		}
		temp = new char[size];
		temp[0] = '\0';
		if (start > 0)
			strncat(temp, _string, start);
		strcat(temp, replacement);
		if (end < _length)
			strcat(temp, _string + end);
		_length = newlen;
		if (alloced)
			_alloc = size;
		delete [] _string;
		_string = temp;	
	}
	regfree(&re);
	return *this;
}

/*
 * replace the first occurance of a match for the regular expression with the
 * replacement string.
 * @param regex the regular expression to match for
 * @param replacement the replacement string
 * @return this.
 */
String& String::replaceFirst(const String& regex, const String& replacement,
		bool ignoreCase)
{
	return replaceFirst(regex._string, replacement._string, ignoreCase);
}

/*
 * make a substring from this string from beginning to end
 * @param beginning the beginning index for the substring
 * @param end the end index for the substring
 * @return the substring made from String[beginning, end]
 */
String String::substring(int beginning, int end) const
{
	if (beginning < 0)
		beginning = 0;
	if (end < beginning || end >= _length)
		end = _length;
	int sublen = end - beginning;
	if (sublen <= 0)
		return String();
	char* temp = new char[sublen + 1];
	temp[0] = '\0';
	strncat(temp, _string + beginning, sublen);
	String out(temp);
	delete [] temp;
	return out;
}

/*
 * converts all the characters in the string to lower case.
 * @return this.
 */
String& String::toLowerCase()
{
	for (int i = 0; i < _length; i++)
		_string[i] = tolower(_string[i]);
	return *this;
}

/*
 * converts all the characters in the string to upper case.
 * @return this.
 */
String& String::toUpperCase()
{
	for (int i = 0; i < _length; i++)
		_string[i] = toupper(_string[i]);
	return *this;
}

/*
 * trims whitespace from the string.
 * @return this.
 */
String& String::trim()
{
	// first look for the beginning white space
	int i;
	for (i = 0; i < _length; i++)
		if (!isspace(_string[i]))
			break;
	if (i > 0 && i < _length)
	{
		char* temp = new char[_alloc];
		memset(temp, '\0', sizeof(char) * _alloc);
		strcpy(temp, _string + i);
		_length -= i;
		delete [] _string;
		_string = temp;
	}

	// now the tail bit
	for (i = _length - 1; i >= 0; i--)
		if (!isspace(_string[i]))
			break;
	if (i < _length - 1 && i >= 0)	
	{
		_string[i + 1] = '\0';
		_length = i + 1;
	}
		
	return *this;
}

/*
 * Returns the character at the specified index. An index ranges from 0 to
 * length() - 1. The first character of the sequence is at index 0, the next
 * at index 1, and so on, as for array indexing.
 * @param index the index of the character.
 * @return the character at the specified index of this string. The first
 * character is at index 0.
 */
char String::charAt(int index) const
{
	if (index >= 0 && index < _length)
		return _string[index];
	return '\0';
}

/*
 * check if this string starts with another string, starting at a given offset
 * @param prefix the string to search for
 * @param offset the offset from which to start the search
 * @return true or false.
 */
bool String::startsWith(const char* prefix, int offset) const
{
	if (!_length || !prefix || !strlen(prefix) || _length - offset <= 0)
		return false;
	if (offset < 0)
		return false;
	int index = indexOf(prefix, offset);
	return index == offset;
}

/*
 * check if this string starts with another string, starting at a given offset
 * @param prefix the string to search for
 * @param offset the offset from which to start the search
 * @return true or false.
 */
bool String::startsWith(const String& prefix, int offset) const
{
	return startsWith(prefix._string, offset);
}

/*
 * checks if the string ends with another string
 * @param suffix the string to check for.
 * @return true or false.
 */
bool String::endsWith(const char* suffix) const
{
	if (!_length || !suffix || !strlen(suffix))
		return false;
	int len = strlen(suffix);
	int index = lastIndexOf(suffix);
	return index == _length - len;
}

/*
 * checks if the string ends with another string
 * @param suffix the string to check for.
 * @return true or false.
 */
bool String::endsWith(const String& suffix) const
{
	return endsWith(suffix._string);
}

/*
 * find the index of a character in the string
 * @param ch the character to look for
 * @param fromIndex the index to start the search from
 * @return the index of the character or -1 if not found
 */
int String::indexOf(char ch, int fromIndex) const
{
	if (!_length || _length - fromIndex <= 0 || ch == '\0')
		return -1;
	char* result = strchr(_string + fromIndex, ch);
	if (!result)
		return -1;
	return _length - strlen(result);
}

/*
 * find the index of a string in the string
 * @param str the string to look for
 * @param fromIndex the index to start the search from
 * @return the index of the start of the string or -1 if not found
 */
int String::indexOf(const char* str, int fromIndex) const
{
	if (!_length || !str || !strlen(str) || _length - fromIndex <= 0)
		return -1;
	if (fromIndex < 0)
		return false;
	//int len = strlen(_string + fromIndex);
	char* result = strstr(_string + fromIndex, str);
	if (!result)
		return -1;
	return _length - strlen(result);
}

/*
 * find the index of a string in the string
 * @param str the string to look for
 * @param fromIndex the index to start the search from
 * @return the index of the start of the string or -1 if not found
 */
int String::indexOf(const String& str, int fromIndex) const
{
	return indexOf(str._string, fromIndex);
}

/*
 * find the index of a character in the string starting from the end of the
 * string forward
 * @param ch the character to look for
 * @param fromIndex the index to start the search from
 * @return the index of the character or -1 if not found
 */
int String::lastIndexOf(char ch, int fromIndex) const
{
	if (fromIndex <= 0)
		fromIndex = _length;
	if (!_length || _length - fromIndex < 0)
		return -1;
	char* cpy = new char[fromIndex + 1];
	cpy[0] = '\0';
	strncpy(cpy, _string, fromIndex);
	char* result = strrchr(cpy, ch);
	int index = -1;
	if (!result)
		index = -1;
	else
		index = fromIndex - strlen(result);
	delete [] cpy;
	return index;
}

/*
 * find the index of a string in the string starting from the end of the string
 * searching forward.
 * @param str the string to look for
 * @param fromIndex the index to start the search from
 * @return the index of the start of the string or -1 if not found
 */
int String::lastIndexOf(const char* str, int fromIndex) const
{
	if (fromIndex <= 0)
		fromIndex = _length;
	if (!_length || _length - fromIndex < 0 || !str || !strlen(str))
		return -1;
	char* cpy = new char[fromIndex + 1];
	cpy[0] = '\0';
	strncpy(cpy, _string, fromIndex);

	int len = strlen(str);
	int pos = fromIndex - len;
	while (pos >= 0)
	{
		if (strncmp(cpy + pos, str, len) == 0)
			break;
		pos--;
	}

	delete [] cpy;
	return pos;
}

/*
 * find the index of a string in the string starting from the end of the string
 * searching forward.
 * @param str the string to look for
 * @param fromIndex the index to start the search from
 * @return the index of the start of the string or -1 if not found
 */
int String::lastIndexOf(const String& str, int fromIndex) const
{
	return lastIndexOf(str._string, fromIndex);
}

/*
 * Returns the length of this string. The length is equal to the number of
 * characters in the string.
 * @return the length of the sequence of characters represented by this object.
 */
int String::length() const
{
	return _length;
}

/*
 * find if there is a match for a regular expression in the string
 * @param regex the regular expression to match
 * @return true or false
 */
bool String::matches(const char* regex, bool ignoreCase) const
{
	if (!regex || !strlen(regex))
		return false;
	regex_t re;
	regmatch_t pm;
	int flags = REG_EXTENDED;
	if (ignoreCase)
		flags &= REG_ICASE;
	if (regcomp(&re, regex, flags) != 0)
		return false;
	int error = regexec(&re, _string, 1, &pm, 0);
	regfree(&re);
	if (error == 0)
		return true;
	return false;
}

/*
 * find if there is a match for a regular expression in the string
 * @param regex the regular expression to match
 * @return true or false
 */
bool String::matches(const String& regex, bool ignoreCase) const
{
	return matches(regex._string, ignoreCase);
}

/*
 * generate a hash code for the string. This is no longer valid when the string
 * is modified.
 * @return the hash code based on the contents of the string.
 */
int String::hashCode() const
{
	int hash = 0;
	for (int i = 0; i < _length; i++)
	{
		char c = _string[i];
		if (c >= 0140)
			c -= 40;
		hash = ((hash << 3) + (hash >> 28) + c);
	}
	return hash & 07777777777;
}

// operators
bool String::operator==(const String& other) const
{
	return equals(other);
}

String& String::operator=(const char* other)
{
	delete [] _string;
	_string = NULL;
	_length = 0;
	_alloc = 0;
	if (!other)
		return *this;
	int len = strlen(other);
	if (len > 0)
	{
		int size;
		ADJUST_SIZE(size, len)
		_string = new char[size];
		_string[0] = '\0';
		strcat(_string, other);
		_alloc = size;
		_length = len;
	}
	return *this;
}

String& String::operator=(const String& other)
{
	return operator=(other._string);
}

String& String::operator+=(const char* other)
{
	return concat(other);
}

String& String::operator+=(const String& other)
{
	return concat(other);
}

/*
 * access the string as an array
 */
char& String::operator[](int index)
{
	if (!_alloc)
	{
		_string = new char[2];
		_string[0] = '\0';
		_length = 1;
		_alloc = 2;
		return _string[0];
	}
	if (index >= _length)
		index = _length - 1;
	if (index < 0)
		index = 0;
	return _string[index];
}

String::operator const char*() const
{
	return _string;
}

