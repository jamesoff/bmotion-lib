#include <Library.h>
#include <stdio.h>
#include <Register.h>
#include <bMotion.h>
#include <Output.h>
#include <DynamicLoader.h>

/*
 * Constructor for Library objects
 * @param name the string name of the library including path
 */
Library::Library(const String& name)
: _handle(NULL)
, _loaded(false)
, _name(name)
{
	
}

/*
 * Destructor
 */
Library::~Library()
{
	if (_handle)
		closeLibrary();
}

/*
 * open the library using the library name (path)
 * @return true or false depending if the library was opened and initialised.
 */
bool Library::openLibrary()
{
	if (_handle || _loaded)
		return true;
	if (_name.length() == 0)
		return false;
	_handle = dlLoadLibrary(_name);
	if (!_handle)
		return false;
	
	// try to find the different types of plugins
	InitFunc Init = (InitFunc)dlSymbol(_handle, "Init");
	if (Init)
	{
		if (bMotionSystem().startDangerousCode() == 0)
			Init();
		if (bMotionSystem().endDangerousCode())
		{
			bMotionLog(1, "library initialisation caused a serious error");
			_loaded = false;
		}
		else
			_loaded = true;
	}
	if (!_loaded)
	{
		dlCloseLibrary(_handle);
		_handle = NULL;
	}
	return _loaded;
}

/*
 * close the library. also disables any plugins linked to the library
 * @return true or false if the library could be close or not
 */
bool Library::closeLibrary()
{
	if (!_handle && !_loaded)
		return true;

	_loaded = false;
	// disable library plugins
	bMotionSystem().disableAllLibraryPlugins(this);
	
	// close the handle
	dlCloseLibrary(_handle);
	_handle = NULL;
	return true;
}

/*
 * Checks if the library is loaded
 * @return true or false
 */
bool Library::isLoaded() const
{
	return _loaded;
}

/*
 * gets the library internal handle. Not safe. should not be exposed outside
 * the core system.
 * @return the library handle.
 */
void* Library::getHandle()
{
	return _handle;
}

/*
 * get the name (including path) of the library
 * @return the library name
 */
const String& Library::getName() const
{
	return _name;
}

