/*
 * bString.h : Common string class inteface
 * MUTABLE STRING
 * Author: Andrew Payne
 * Copyright: ATR Lab © 2004
 */
 
#ifndef _COMMON_STRING_H
#define _COMMON_STRING_H

// needed for map comparison
struct ltstr
{
	bool operator()(const char* s1, const char* s2) const;
};

class String
{
public:
	String();
	String(const char* value, int offset = 0, int count = -1);
	String(const String& original, int offset = 0, int count = -1);
	virtual ~String();
	
	// comparison
	int compareTo(const char* other) const;
	int compareTo(const String& other) const;
	int compareToIgnoreCase(const char* other) const;
	int compareToIgnoreCase(const String& other) const;
	bool equals(const char* other) const;
	bool equals(const String& other) const;
	bool equalsIgnoreCase(const char* other) const;
	bool equalsIgnoreCase(const String& other) const;
	
	// operations
	String& concat(const char* other);
	String& concat(const String& other);
	String& replace(char oldChar, char newChar);
	String& replaceAll(const char* regex, const char* replacement,
			bool ignoreCase = false);
	String& replaceAll(const String& regex, const String& replacement,
			bool ignoreCase = false);
	String& replaceFirst(const char* regex, const char* replacement,
			bool ignoreCase = false);
	String& replaceFirst(const String& regex, const String& replacement,
			bool ignoreCase = false);
	String substring(int beginning, int end = -1) const;
	String& toLowerCase();
	String& toUpperCase();
	String& trim();
	
	// lookup
	char charAt(int index) const;
	bool startsWith(const char* prefix, int offset = 0) const;
	bool startsWith(const String& prefix, int offset = 0) const;
	bool endsWith(const char* suffix) const;
	bool endsWith(const String& suffix) const;
	int indexOf(char ch, int fromIndex = 0) const;
	int indexOf(const char* str, int fromIndex) const;
	int indexOf(const String& str, int fromIndex = 0) const;
	int lastIndexOf(char ch, int fromIndex = 0) const;
	int lastIndexOf(const char* str, int fromIndex = 0) const;
	int lastIndexOf(const String& str, int fromIndex = 0) const;
	int length() const;
	bool matches(const char* regex, bool ignoreCase = false) const;
	bool matches(const String& regex, bool ignoreCase = false) const;
	
	// other
	int hashCode() const;
	
	// operators
	bool operator==(const String& other) const;
	String& operator=(const char* other);
	String& operator=(const String& other);
	String& operator+=(const char* other);
	String& operator+=(const String& other);
	char& operator[](int index);
	operator const char*() const;
	
private:
	char*	_string;
	int	_alloc;
	int	_length;
};

#endif

