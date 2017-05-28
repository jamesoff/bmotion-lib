#ifndef DYNAMIC_LOADER
#define DYNAMIC_LOADER

#include <bString.h>

// HINSTANCE handle = LoadLibrary(name)
// GetProcAddress(handle, symbol_name)
// FreeLibrary(handle)

void* dlLoadLibrary(const String& name);
void dlCloseLibrary(void* handle);
void* dlSymbol(void* handle, const String& name);

#endif

