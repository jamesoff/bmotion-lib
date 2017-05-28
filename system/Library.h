#ifndef LIBRARY_H
#define LIBRARY_H

#include <bString.h>

// initialisation function type. this function is needed in the loading library
// to be considered a successful load
typedef void (*InitFunc)();

/*
 * the library class for loading and unloading dynamic libraries for plugins.
 */
class Library
{
public:
	Library(const String& name);
	virtual ~Library();
	
	// open / close
	bool openLibrary();
	bool closeLibrary();

	// information
	bool isLoaded() const;
	void* getHandle();
	const String& getName() const;

private:
	// internal library handle
	void* _handle;
	// loaded (could also be checking if _handle != NULL)
	bool _loaded;
	// library name. stored so it can be loaded and unloaded when needed.
	String _name;
};

#endif

