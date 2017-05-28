/*
 * This is the main API export declaration file for the bMotion library. All
 * exported functions for both implimenting programs and bMotion support plugins
 * are located here.
 * NOTE: you may use the main API *OR* the plugin support API. The default is
 * to use the main API. To use the plugin support API you must define
 * BMOTION_PLUGIN when compiling your source. All plugin support functions are
 * contained within an API structure and should be called like
 *   ...
 *   bMotion.SupportFunction();
 *   ...
 * Plugins must have an function:
 * void Init()
 * {
 * }
 * and all plugin callback registration should be done in that function.
 */

#ifndef BMOTION_API_H
#define BMOTION_API_H

#include <stdio.h>

#define ABSTRACT_END (NULL)

#ifdef BMOTION_PLUGIN
#ifndef WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif

static void* dlsymbol(const char* name)
{
#ifndef WIN32
	return dlsym(RTLD_DEFAULT, name);
#else
	return GetProcAddress(GetModuleHandle("libbmotion.dll"), name);
#endif
}

// function type declarations
typedef void (*StatusFunc)();
typedef bool (*RegisterSimpleFunc)(const char*, const char*, const char*, int, const char*);
typedef bool (*RegisterComplexFunc)(const char*, const char*, const char*, int, const char*);
typedef bool (*RegisterEventFunc)(const char*, const char*, const char*, const char*, int, const char*);
typedef bool (*RegisterAdminFunc)(const char*, const char*, const char*, const char*);
typedef bool (*RegisterOutputFunc)(const char*, const char*, const char*, int, const char*);
typedef bool (*DoActionFunc)(const char*, const char*, const char*, const char* = NULL, bool = false);
typedef void (*LogFunc)(int, const char*,...);
typedef bool (*AddTimerFunc)(unsigned long, void(*)(void*), void*);
typedef bool (*UseLanguageFunc)(const char*);
typedef bool (*AbstractRegisterFunc)(const char*);
typedef bool (*AbstractBatchAddFunc)(const char*, ... );
typedef bool (*MoodIncreaseFunc)(const char*, int = 1);
typedef bool (*MoodDecreaseFunc)(const char*, int = 1);
typedef bool (*MoodCreateFunc)(const char*, int, int = -30, int = 30);
typedef int (*MoodGetFunc)(const char*);
typedef void (*SetFunc)(const char*, const char*);
typedef const char* (*GetFunc)(const char*);
typedef const void* (*InfoFunc)(const char*);

// API function structure
typedef struct {
	// core
	StatusFunc Status;
	// plugin registration
	RegisterSimpleFunc RegisterSimple;
	RegisterComplexFunc RegisterComplex;
	RegisterEventFunc RegisterEvent;
	RegisterAdminFunc RegisterAdmin;
	RegisterOutputFunc RegisterOutput;
	// output functions
	DoActionFunc DoAction;
	LogFunc Log;
	// Timer support
	AddTimerFunc AddTimer;
	// Language support
	UseLanguageFunc UseLanguage;
	// Abstract support
	AbstractRegisterFunc AbstractRegister;
	AbstractBatchAddFunc AbstractBatchAdd;
	// mood support
	MoodIncreaseFunc MoodIncrease;
	MoodDecreaseFunc MoodDecrease;
	MoodCreateFunc MoodCreate;
	MoodGetFunc MoodGet;
	// user settings
	SetFunc Set;
	GetFunc Get;
	InfoFunc Info;
} bMotionAPI;

/*****************************************************************************/
// instantiation of API.. this is the important bit for plugin writers. This
// structure contains all the pointers to exported functions you can use in
// the bmotion API.
static bMotionAPI bMotion = {
	(StatusFunc)dlsymbol("bMotionStatus"),
	(RegisterSimpleFunc)dlsymbol("bMotionRegisterSimple"),
	(RegisterComplexFunc)dlsymbol("bMotionRegisterComplex"),
	(RegisterEventFunc)dlsymbol("bMotionRegisterEvent"),
	(RegisterAdminFunc)dlsymbol("bMotionRegisterAdmin"),
	(RegisterOutputFunc)dlsymbol("bMotionRegisterOutput"),
	(DoActionFunc)dlsymbol("bMotionDoAction"),
	(LogFunc)dlsymbol("bMotionLog"),
	(AddTimerFunc)dlsymbol("bMotionAddTimer"),
	(UseLanguageFunc)dlsymbol("bMotionUseLanguage"),
	(AbstractRegisterFunc)dlsymbol("bMotionAbstractRegister"),
	(AbstractBatchAddFunc)dlsymbol("bMotionAbstractBatchAdd"),
	(MoodIncreaseFunc)dlsymbol("bMotionMoodIncrease"),
	(MoodDecreaseFunc)dlsymbol("bMotionMoodDecrease"),
	(MoodCreateFunc)dlsymbol("bMotionMoodCreate"),
	(MoodGetFunc)dlsymbol("bMotionMoodGet"),
	(SetFunc)dlsymbol("bMotionSet"),
	(GetFunc)dlsymbol("bMotionGet"),
	(InfoFunc)dlsymbol("bMotionInfo")
};

/*****************************************************************************/

#else

/*****************************************************************************/
// this is the API interface for application level users.
#ifdef _cplusplus
extern "C" {
#endif
// initialisation
bool bMotionInit(const char* confFileName);

// main event callbacks
bool bMotionEventOnJoin(const char* nick, const char* host,
		const char* handle, const char* channel);
bool bMotionEventOnPart(const char* nick, const char* host, 
		const char* handle, const char* channel,
		const char* msg);
bool bMotionEventOnQuit(const char* nick, const char* host,
		const char* handle, const char* channel,
		const char* reason);
bool bMotionEventMain(const char* nick, const char* host,
		const char* handle, const char* channel,
		const char* text);
bool bMotionEventMode(const char* nick, const char* host,
		const char* handle, const char* channel,
		const char* mode, const char* victim);
bool bMotionEventNick(const char* nick, const char* host,
		const char* handle, const char* channel,
		const char* newnick);
bool bMotionEventAction(const char* nick, const char* host, 
		const char* handle, const char* dest,
		const char* keyword, const char* text);

// output
bool bMotionDoAction(const char* channel, const char* nick, const char* text,
		const char* moreText, bool urgent);
bool bMotionSetOutput(void(*func)(const char*));
void bMotionLog(int level, const char* fmt, ...);

// timers
bool bMotionAddTimer(unsigned long milli, void(*callback)(void*), void* param);

// utility
bool bMotionUseLanguage(const char* language);
void bMotionStatus();

// abstracts
bool bMotionAbstractRegister(const char* type);
bool bMotionAbstractBatchAdd(const char* type, ... );

// mood
bool bMotionMoodIncrease(const char* name, int amount = 1);
bool bMotionMoodDecrease(const char* name, int amount = 1);
bool bMotionMoodCreate(const char* name, int centre, int lower = -30,
		int upper = 30);
int bMotionMoodGet(const char* name);

// user defined settings
void bMotionSet(const char* setting, const char* value);
const char* bMotionGet(const char* setting);

#ifdef _cplusplus
};
#endif

/*****************************************************************************/

#endif
#endif

