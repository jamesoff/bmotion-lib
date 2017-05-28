#include "File.h"
#include <Output.h>

File::File(const String& filename)
: _name(filename)
, _fp(NULL)
{

}

File::~File()
{
	if (_fp)
		close();	
}

bool File::createNewFile()
{
	if (exists())
		return false;
	_fp = fopen(_name, "w+");
	if (!_fp)
	{
		bMotionLog(1, "could not create file \"%s\"", (const char*)_name);
		return false;
	}
	return true;
}

bool File::deleteFile()
{
	if (!exists())
		return false;
	close();
	// TODO: remove the file
	return false;
}

bool File::exists() const
{
	if (_fp)
		return true;
	FILE* tmp;
	if ( (tmp = fopen(_name, "r")) == NULL)
		return false;
	fclose(tmp);
	return true;
}

bool File::isEOF() const
{
	if (!_fp)
		return true;
	if (ftell(_fp) == available())
		return true;
	return (feof(_fp) != 0);
}

const String& File::getName() const
{
	return _name;
}

int File::available() const
{
	if (!_fp)
	{
		bMotionLog(1, "file is not open");
		return 0;
	}
	fpos_t pos;
	if (fgetpos(_fp, &pos) != 0)
	{
		bMotionLog(1, "file position query error");
		return 0;
	}

	if (fseek(_fp, 0, SEEK_END))
	{
		bMotionLog(1, "file seek error");
		return 0;
	}
	long answer = ftell(_fp);
	
	if (fsetpos(_fp, &pos) != 0)
	{
		bMotionLog(1, "file position set error");
		return 0;
	}
	return answer;
}

int File::read()
{
	if (!_fp)
	{
		bMotionLog(1, "cannot read from a closed stream");
		return 0;
	}
	return fgetc(_fp);
}

int File::read(char* buf, int length)
{
	if (!_fp)
	{
		bMotionLog(1, "cannot read from a closed stream");
		return 0;
	}
	if (!buf || length <= 0)
	{
		bMotionLog(1, "buffer not allocated");
		return 0;
	}
	if (feof(_fp) || ferror(_fp))
	{
		bMotionLog(1, "cannot read from end of stream");
		return 0;
	}
	int size = fread(buf, sizeof(char), length, _fp);
	buf[length] = '\0';
	return size;
}

String File::readLine()
{
	if (!_fp)
	{
		bMotionLog(1, "cannot read from a closed stream");
		return 0;
	}
	int i = 0;
	char c = '\0';
#ifdef WIN32
	fpos_t pos;
	fgetpos(_fp, &pos);
#endif
	while (c != '\n' && !isEOF())
	{
		c = (char)fgetc(_fp);
		i++;
	}
	if (!i)
		return 0;
#ifndef WIN32
	fseek(_fp, -i, SEEK_CUR);
#else
	fsetpos(_fp, &pos);
	ftell(_fp);
#endif
	char* temp = new char[i+1];
	temp[0] = '\0';
	int num = read(temp, i);
	if (num != i)
		bMotionLog(1, "WARNING");
	String str(temp);
	delete [] temp;
	return str;
}

void File::reset()
{
	if (_fp)
		rewind(_fp);
}

long File::skip(long n)
{
	if (!_fp)
	{
		bMotionLog(1, "cannot read from a closed stream");
		return 0;
	}
	return fseek(_fp, n, SEEK_CUR);
}

bool File::ready() const
{
	return (_fp && feof(_fp));
}

bool File::open()
{
	if (_fp)
	{
		bMotionLog(1, "attempting to open an open file");
		return false;
	}
	_fp = fopen(_name, "r+"); // reading and writing, will fail if
				  // the file does not exist.
	if (!_fp)
	{
		bMotionLog(1, "could not open file \"%s\"", (const char*)_name);
		return false;
	}
	fseek(_fp, 0, SEEK_SET);
	return true;
}

bool File::close()
{
	if (!_fp)
	{
		bMotionLog(1, "file is not open");
		return false;
	}
	fclose(_fp);
	_fp = NULL;
	return true;
}

bool File::writeLine(const String& line)
{
	if (!_fp)
	{
		bMotionLog(1, "file is not open");
		return false;
	}
	fprintf(_fp, "%s\n", (const char*)line);
	return true;
}

bool File::emptyFile()
{
	if (!_fp)
	{
		bMotionLog(1, "file is not open");
		return false;
	}
	fclose(_fp);
	_fp = fopen(_name, "w+");
	if (!_fp)
	{
		bMotionLog(1, "could not create emptied file");
		return false;
	}
	return true;
}

