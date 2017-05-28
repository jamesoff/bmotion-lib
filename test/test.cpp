#include <stdio.h>
#include <bmotion_api.h>
#include <string.h>

#define BUFSIZE (1024)

int main(int argc, char** argv)
{
	bMotionLog(1, "testing app (%s) for bmotion c project", argv[0]);
	if (argc > 1)
	{
		bMotionLog(1, "no arguments needed");
		return -1;
	}

	if (!bMotionInit(NULL))
	{
		printf("exiting early\n");
		return -1;
	}

	char buf[BUFSIZE];
	bool done = false;
	while (!done)
	{
		fgets(buf, BUFSIZE, stdin);
		int size = strlen(buf);
		buf[size - 1] = '\0';
		if (strcmp(buf, "/exit") == 0 || strcmp(buf, "/quit") == 0)
			done = true;
		else
			bMotionEventMain("testing", "localhost", "user", 
					"#testing", buf);
	};
	
	return 0;
}

