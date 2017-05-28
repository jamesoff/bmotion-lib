#ifndef SYSTEM_H
#define SYSTEM_H

#include <Settings.h>
#include <System.h>

#define BMOTION_MAX_ABSTRACTS (300)
// ten minute life span for unused abstracts
#define BMOTION_MAX_ABSTRACT_AGE (600)
// abstract list end value
#define ABSTRACT_END (NULL)

// internal bMotion procedures
bool bMotionLoadPlugins();
bool bMotionUnloadAllPlugins();
bool bMotionRehash();

// support
extern "C" bool bMotionIsBotnick(const char* name);

// get the global settings and system objects
Settings& bMotionSettings();
System& bMotionSystem();

#endif

