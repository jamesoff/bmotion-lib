#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include "bString.h"

class File
{
public:
	File(const String& filename);
	virtual ~File();

	bool createNewFile();
	bool deleteFile();
	bool exists() const;
	const String& getName() const;
	bool isEOF() const;

	int available() const;
	int read();
	int read(char* buf, int length);
	String readLine();
	void reset();
	long skip(long n);
	bool ready() const;
	
	bool open();
	bool close();

	bool writeLine(const String& line);
	bool emptyFile();

private:
	String _name;
	FILE* _fp;

};

#endif

