#include <stdio.h>
#include <bmotion_api.h>
#include <string.h>

extern "C" void Init()
{
	bMotion.RegisterAdmin("admin:lang", "bmotion_admin_lang", "lang", 
			"any");
	bMotion.RegisterAdmin("admin:status", "bmotion_admin_status", "status",
			"any");
}

