#include <stdio.h>
#include <bmotion_api.h>
#include <string.h>

extern "C" bool bmotion_admin_status(const char* /*nick*/,
		const char* /*host*/, const char* /*handle*/,
		const char* /*channel*/, const char* text)
{
	bMotion.Log(1, "bMotion status");
	if (!text || strlen(text) == 0)
	{
		bMotion.Status();
		return true;
	}
	return false;
}

