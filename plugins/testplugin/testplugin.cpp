#include <stdio.h>
#include <bmotion_api.h>
#include <string.h>

extern "C" void Init()
{
	bMotion.RegisterComplex("bmotion TESTING plugin", "bmotion_test_function", "^rah", 100, "en");
	bMotion.AbstractRegister("testAbstract");
	bMotion.AbstractBatchAdd("testAbstract",
			"bmotion plugin test!!%|multiline%|isn't this fun",
			"yar yar yar yar",
			"pie.", 
			"/testing actions",
			ABSTRACT_END);
	bMotion.RegisterOutput("testing output plugin", "bmotion_test_output", ".*", 50, "en");
}

void testing();

void timerTest(void*)
{
	bMotion.Log(1, "timer test checks out ok!");
	bMotion.Log(1, "set 'testing' = 'true'");
	bMotion.Set("testing", "true");
	bMotion.Log(1, "get 'testing' = '%s'", bMotion.Get("testing"));
	bMotion.Log(1, "get 'empty' = '%s'", bMotion.Get("empty"));
}

extern "C" bool bmotion_test_function(const char* nick,
		const char* /*host*/, const char* /*handle*/,
		const char* channel, const char* /*text*/)
{
	bMotion.DoAction(channel, nick, "%VAR{testAbstract} harhar");
	testing();
	bMotion.AddTimer(10000, timerTest, NULL);
	return true;
}

extern "C" bool bmotion_test_output(const char* /*nick*/, 
		const char* /*channel*/,
		char* text, const char* /*moreText*/)
{
	strcat(text, " man.");
	return true;
}

