#ifndef REGISTER_H
#define REGISTER_H

// plugin registration functions.
extern "C" bool bMotionRegisterSimple(const char* pluginName, 
		const char* callbackName, const char* regexp, int chance,
		const char* lang);
extern "C" bool bMotionRegisterComplex(const char* pluginName,
		const char* callbackName, const char* regexp, int chance,
		const char* lang);
extern "C" bool bMotionRegisterEvent(const char* pluginName,
		const char* callbackName, const char* type, const char* regexp,
		int chance, const char* lang);
extern "C" bool bMotionRegisterAdmin(const char* pluginName,
		const char* callbackName, const char* command,
		const char* lang);
extern "C" bool bMotionRegisterOutput(const char* pluginName,
		const char* callbackName, const char* regexp, int chance,
		const char* lang);

#endif

