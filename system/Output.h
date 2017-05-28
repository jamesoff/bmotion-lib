#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>

#define BMOTION_PRIVMSG (1)
#define BMOTION_LOG (90)
#define BMOTION_LOG1 (BMOTION_LOG+1)
#define BMOTION_LOG2 (BMOTION_LOG+1)
#define BMOTION_LOG3 (BMOTION_LOG+1)
#define BMOTION_LOG4 (BMOTION_LOG+1)
#define BMOTION_LOG5 (BMOTION_LOG+1)

//proc bMotionDoAction {channel nick text {moreText ""} {noTypo 0} {urgent 0} }
extern "C" bool bMotionDoAction(const char* channel, const char* nick,
		const char* text, const char* moreText = NULL,
		bool urgent = false);

extern "C" void bMotionSetOutput(void(*func)(int, const char*, const char*));

// logging
extern "C" void bMotionLog(int level, const char* fmt, ...);

#endif

