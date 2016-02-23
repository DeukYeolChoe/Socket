#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "Server_v3.h"
//typedef int bool;
//#define true 1
//#define false 0
//#define SIZE 1024
//#define BACKLOG 10

char fileName[SIZE];
char fileExtension[SIZE];
bool isImage;

void error(char *msg)
{
	//perror: print a system error message 
    	perror(msg);
	//exit(1): indicates unsucessful termination
 	exit(1);
}

int getFileExtension(char* file)
{
	char fileType[SIZE];
	char *split;
	char img[][5] = {"jpeg", "jfif", "exif", "tiff", "gif", "bmp", "png", "ppm",
			 "pgm", "pbm", "pnm", "webp", "hdr", "heif", "bpg"};
	int array_size = 0;
	int i;
	if (file == NULL) return 0;
	
	strcpy(fileType, file);
	split = strtok(fileType, ".");
	//printf("FileName[0]: %s\n", split);
	
	if (split == NULL) return 0;
	split = strtok(NULL, " ");
	//printf("Extension[1]: %s\n\n", split);

	if (split == NULL) return 0;

	array_size = sizeof(img)/sizeof(img[0]);
	for (i = 0; i < array_size; i++)
	{
		if (strcmp(split, img[i]) == 0)
			return 2;
	}
	return 1;
}

int getFilePath(char *msg)
{
	char *split;
	char temp_fileName[SIZE];
	int count_slash = 0;
	if (msg == NULL) return 0;
	if (strlen(msg) == 0 || strlen(msg) >= SIZE) return 0;

	split = strtok(msg, "\r\n");
	//printf("split[0]: %s\n", split);
	if (split == NULL) return 0;

	split = strtok(split, " ");
	//printf("split[1]: %s\n", split);
	if (split == NULL) return 0;

	// GET check
	if (strcmp(split, "GET") != 0) return 0;

	split = strtok(NULL, " ");
	//printf("split[2]: %s\n", split);	
	//printf("size spit[2]: %d\n", strlen(split));	
	if (split == NULL) return 0;
	
	split = strtok(split, "/ ");
	//strcpy(temp_fileName, split);
	while (split != NULL)
	{
		//printf("fileName: %s\n", split);
		strcpy(temp_fileName, split);
		split = strtok(NULL, " ");
		count_slash++;
	}
	//printf("fileName: %s\n", split);
	if (count_slash > 1) return 0;	

	if (strlen(temp_fileName) == 0)
	{
		strcpy(fileName, "index.html");
	}
	else
	{
		strcpy(fileName, temp_fileName);
		int extension = getFileExtension(fileName);
		if (extension == 0 && strlen(fileName) > 0 && strcmp(fileName, "home") != 0)
			return 1;
		else
			return extension; 				
	}
	return 1;
}

bool validatePort(char *port)
{
	if (!atoi(port))
	{
		printf("A port must be numeric\n");
		return false;	
	}
	
	if (atoi(port) < 8000 && atoi(port) >= 9000)
	{
		printf("A port must be between 8000 and 8999");
		return false;
	}
	return true;
}

/*
 * Reference: http://stackoverflow.com/questions/15445207/sending-image-jpeg-through-socket-in-c-linux
 */
void send_binary(int socket, FILE *picture)
{
	int size, read_size, stat, packet_index;
	char send_buffer[SIZE];
	char read_buffer[SIZE];
	packet_index = 1;
	
	//According to TutorialsPoint website,
	//fseek(FILE *stream, long int offset, int whence) sets the file position of the stream to the given offset
	//SEEK_END: end of file
	//SEEK_SET: Beginning of file
	//ftell(FILE *stream) returns the current file position of the given stream 	
	fseek(picture, 0, SEEK_END);
	size = ftell(picture);
	fseek(picture, 0, SEEK_SET);
	printf("Total Picture Size: %d\n", size);

	printf("Sending Picture as Byte Array\n");
	
	while (!feof(picture))
	{
		read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);
		do
		{
			stat = send(socket, send_buffer, read_size, 0);
		} while(stat < 0);
	
		//printf("Packet Number: %d\n", packet_index);
		//printf("Packet Size sent: %d\n", read_size);
		//printf("\n\n");
		//packet_index++;
		bzero(send_buffer, sizeof(send_buffer));
	}
	printf("Completed!\n");
}

