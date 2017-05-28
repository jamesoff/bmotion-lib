#include <DynamicLoader.h>

#ifndef WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif

void* dlLoadLibrary(const String& name)
{
#ifndef WIN32
	return dlopen((const char*)name, RTLD_LAZY | RTLD_GLOBAL);
#else
	return (void*)LoadLibrary((const char*)name);
#endif
}

void dlCloseLibrary(void* handle)
{
#ifndef WIN32
	dlclose(handle);
#else
	FreeLibrary((HINSTANCE)handle);
#endif
}

void* dlSymbol(void* handle, const String& name)
{
#ifndef WIN32
	return dlsym(handle, (const char*)name);
#else
	return GetProcAddress((HINSTANCE)handle, (const char*)name);
#endif
}

