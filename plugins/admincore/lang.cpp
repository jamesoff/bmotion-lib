#include <stdio.h>
#include <bmotion_api.h>
#include <string.h>

extern "C" bool bmotion_admin_lang(const char* /*nick*/,
		const char* /*host*/, const char* /*handle*/,
		const char* /*channel*/, const char* text)
{
	bMotion.Log(1, "Language Admin Control");
	if (!text || strlen(text) == 0)
	{
		// language help
		const char* lang = "none";
		bMotion.Log(1, "current language = %s", lang);
		return true;
	}
	char* line = new char[strlen(text) + 1];
	strcpy(line, text);
	char* token = strtok(line, " ");
	while (token)
	{
		if (strcmp(token, "use") == 0)
		{
			char* lang = strtok(NULL, " ");
			if (!lang)
			{
				bMotion.Log(1, "missing language specification");
				delete [] line;
				return false;
			}
			if (!bMotion.UseLanguage(lang))
			{
				bMotion.Log(1, "could not switch language to '%s'", lang);
				delete [] line;
				return false;
			}
			lang = strtok(NULL, " ");
			if (lang)
				bMotion.Log(1, "Warning: don't add anything after language request in 'lang use' command");
			token = NULL;
		}
	}
	delete [] line;
	return true;
}

