#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
void makeLog(char *msg)
{
	FILE *fd;
	char *buffer;

	printf("%s\n", msg);

	fd = fopen("log.txt", "aw");	
	if (fd == NULL)
	{
		printf("log: fd == NULL\n");
		exit(1);
	}	

	buffer = strdup(msg);
	fprintf(fd, "%s", buffer);
	fclose(fd);
}

void error()
{
	perror("ERROR- ");
	exit(1);
}
