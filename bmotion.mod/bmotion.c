#define MODULE_NAME "bmotion"
#define MAKING_BMOTION

#include "src/mod/module.h"
#include <stdlib.h>
#include <dlfcn.h>

///////////////////////////////////////////////////////////////////////////////

typedef int bool;
typedef bool (*InitFunc)(const char*);
typedef bool (*MainFunc)(const char*, const char*, const char*, const char*, 
		const char*);
typedef void (*OutputSetFunc)(void(*)(int, const char*, const char*));
typedef void (*StatusFunc)();

static void* bMotionLib = NULL;
#define BMOTION_PRIVMSG (1)
#define BMOTION_LOG (90)
#define BMOTION_LOG1 (BMOTION_LOG+1)
#define BMOTION_LOG2 (BMOTION_LOG+1)
#define BMOTION_LOG3 (BMOTION_LOG+1)
#define BMOTION_LOG4 (BMOTION_LOG+1)
#define BMOTION_LOG5 (BMOTION_LOG+1)

///////////////////////////////////////////////////////////////////////////////

#undef global
static Function *global = NULL;
static int bmotion_open_lib();
EXPORT_SCOPE char *bmotion_start();
static char *bmotion_close();
static int bmotion_expmem();
static void bmotion_report(int idx, int details);
static void bmotion_output(int type, const char* target, const char* text);

///////////////////////////////////////////////////////////////////////////////

static int bmotion_eventMain STDVAR
{
	BADARGS(6, 6, " nick host handle channel text");
	
	if (bmotion_open_lib() != 0)
	{
		putlog(LOG_LEV1, "*", "failed to ensure library open");
		return 0;
	}
	MainFunc bMotionEventMain = (MainFunc)dlsym(bMotionLib,
			"bMotionEventMain");
	if (!bMotionEventMain)
	{
		putlog(LOG_LEV1, "*", "failed to get function from library");
		return 0;
	}
	if (bMotionEventMain(argv[1], argv[2], argv[3], argv[4], argv[5]))
		return TCL_OK;
	return TCL_ERROR;
}

///////////////////////////////////////////////////////////////////////////////

static tcl_cmds bmotion_tcl_cmds[] = {
	{ "bMotionEventMain", bmotion_eventMain },
	{ NULL, NULL }
};

static Function bmotion_table[] = {
	(Function) bmotion_start,
	(Function) bmotion_close,
	(Function) bmotion_expmem,
	(Function) bmotion_report,
};

static void bmotion_report(int idx, int details)
{
	if (details)
	{
		if (bmotion_open_lib() != 0)
		{
			putlog(LOG_LEV1, "*", "failed to ensure library open");
			return;
		}
		StatusFunc bMotionStatus = (StatusFunc)dlsym(bMotionLib,
				"bMotionStatus");
		if (!bMotionStatus)
		{
			putlog(LOG_LEV1, "*", 
					"failed to get function from library");
			return;
		}
		bMotionStatus();
	}
}

static int bmotion_expmem()
{
	int size = 0;
	return size;
}

static void bmotion_output(int type, const char* target, const char* text)
{
	if (type == BMOTION_PRIVMSG)
		dprintf(DP_HELP, "PRIVMSG %s :%s\n", target, text);
	else if (type >= BMOTION_LOG && type <= BMOTION_LOG5)
		putlog(LOG_LEV1, target, text);
}

static char *bmotion_close()
{
	module_undepend(MODULE_NAME);
	rem_tcl_commands(bmotion_tcl_cmds);
	if (bMotionLib)
	{
		putlog(LOG_LEV1, "*", "Closing bMotion library");
		dlclose(bMotionLib);
	}
	bMotionLib = NULL;
	return NULL;
}

static int bmotion_open_lib()
{
	if (bMotionLib)
		return 0;
	bMotionLib = dlopen("modules/libbmotion.so",
			RTLD_LAZY | RTLD_GLOBAL);
	if (!bMotionLib)
	{
		putlog(LOG_LEV1, "*", "Could not open bMotion core library");
		return 1;
	}
	// setup the output functions
	OutputSetFunc bMotionSetOutput = (OutputSetFunc)dlsym(bMotionLib,
			"bMotionSetOutput");
	if (bMotionSetOutput)
		bMotionSetOutput(bmotion_output);
	
	// initialise
	InitFunc bMotionInit = (InitFunc)dlsym(bMotionLib, "bMotionInit");
	if (!bMotionInit)
	{
		putlog(LOG_LEV1, "*", "Could not find bMotionInit function");
		return 1;
	}
	if (!bMotionInit(NULL))
	{
		putlog(LOG_LEV1, "*", "Could not start core bMotion library");
		return 1;
	}
	return 0;
}

char *bmotion_start(Function *global_funcs)
{
	global = global_funcs;

	// register the module
	module_register(MODULE_NAME, bmotion_table, 2, 0);

	// module depends
	if (!module_depend(MODULE_NAME, "eggdrop", 106, 0)) 
	{
		module_undepend(MODULE_NAME);
		return "This module requires Eggdrop 1.6.0 or later.";
	}
	
	putlog(LOG_LEV1, "*", "postponing opening bMotion library");
	
	add_tcl_commands(bmotion_tcl_cmds);
	return NULL;
}

