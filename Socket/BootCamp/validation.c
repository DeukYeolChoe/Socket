#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "validation.h"

int isValidPort(char *port)
{
	if (!atoi(port))
	{
		makeLog("validation: isValidPort\n");
		return -1;
	}
	return 1;
}

int isNullOrEmpty(char *msg)
{
	if (msg == NULL)
	{
		makeLog("validation: msg == NULL\n");
		return -1;
	}
	else if (strlen(msg) == 0)
	{
		makeLog("validation: strlen(msg) == 0\n");
		return -1;
	}
	else if (strcmp(msg, "") == 0)
	{
		makeLog("validation: strcmp(msg, "") == 0\n");
		return -1;
	}
	return 1;
}

int isValidImgExtension(char *file)
{
	char img[][5] = {"jpg", "jpeg", "jfif", "exif", "tiff", "gif", "bmp", "png", "ppm",
		 "pgm", "pbm", "pnm", "webp", "hdr", "heif", "bpg"};
	int array_size = 0;
	int i;
	char *split;
	char *fileType;
	
	fileType = strdup(file);
	split = strtok(fileType, ".");
	split = strtok(NULL, " ");
	//printf("Extension: %s\n", split);

	array_size = sizeof(img)/sizeof(img[0]);
	for (i = 0; i < array_size; i++)
	{
		if (strcmp(split, img[i]) == 0)
			return 1;
	}
	return -1;
}

int isCgiScript(char* file)
{
	char cgi[][5] = {"cgi", "pl", "c", "c++", "php", "class"};
	int array_size = 0;
	int i;
	char *split;
	
	split = strdup(file);
	split = strtok(split, ".");
	split = strtok(NULL, " ");
	printf("validation: (isCgi) %s\n", split);

	array_size = sizeof(cgi)/sizeof(cgi[0]);
	for (i = 0; i < array_size; i++)
	{
		if (strcmp(split, cgi[i]) == 0)
			return 1;
	}
	
	return -1;
}

