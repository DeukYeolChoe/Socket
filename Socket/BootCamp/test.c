#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	pid_t pid;


	pid = fork();

	if (pid != 0)
	{
		printf("This is a parent process\n");
		wait(NULL);
	}
	else
	{
		printf("This is a child processi\n");
		execl("/bin/echo", "echo", "Hellow World!", NULL);
	}


	return 0;	
}
