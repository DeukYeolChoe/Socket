#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

HttpInfo getHttpHeader(HttpInfo http)
{
	http.method = getHttpMethod(http.method);
	printf("http.method = [%s]\n", http.method);
	http.host = getHttpValue(http.host);
	printf("http.host = [%s]\n", http.host);
	http.connection = getHttpValue(http.connection);
	printf("http.connection = [%s]\n", http.connection);
	http.contentLength = getHttpValue(http.contentLength);
	printf("http.contentLength = [%s]\n", http.contentLength);
	http.cache_control = getHttpValue(http.cache_control);
	printf("http.cache_control = [%s]\n", http.cache_control);
	http.accept = getHttpValue(http.accept);
	printf("http.accept = [%s]\n", http.accept);
	http.origin = getHttpValue(http.origin);
	printf("http.origin = [%s]\n", http.origin);
	http.user_agent = getHttpValue(http.user_agent);
	printf("http.user_agent = [%s]\n", http.user_agent);
	http.content_type = getHttpValue(http.content_type);
	printf("http.content_type = [%s]\n", http.content_type);
	http.referer = getHttpValue(http.referer);
	printf("http.referer = [%s]\n", http.referer);
	http.accept_encoding = getHttpValue(http.accept_encoding);
	printf("http.accept_encoding = [%s]\n", http.accept_encoding);
	http.accept_language = getHttpValue(http.accept_language);
	printf("http.accept_language = [%s]\n", http.accept_language);
	http.content_disposition = getHttpValue(http.content_disposition);
	printf("http.content_disposition = [%s]\n", http.content_disposition);
	//printf("**(parser)contents = [%s]\n", http.contents);

	return http;	
}

HttpInfo parseHttpHeader(char *content)
{
	HttpInfo http;
	char *split;
	char *deli= "\r";
	char temp[SIZE];
	size_t size = 0;
	bool flag = false;

	http.contentLength = NULL;
	http.origin = NULL;
	http.user_agent = NULL;
	http.content_type = NULL;
	http.referer = NULL;
	http.accept_encoding = NULL;
	http.accept_language = NULL;
	http.content_disposition = NULL;
	http.upgrade_insecure_requests = NULL;
		
	split = strtok(content, deli);	
	http.method = strdup(split);
	http.fileName = strdup(split);	
	split = strtok(NULL, deli);
	
	while (split != NULL)
	{
		//printf("Parsing: %s\n", split);
		if (strstr(split, "Host:") != NULL)
			http.host = strdup(split);
		else if (strstr(split, "Connection:") != NULL)
			http.connection = strdup(split);
		else if (strstr(split, "Content-Length:") != NULL)
			http.contentLength = strdup(split);
		else if (strstr(split, "Cache-Control:") != NULL)
			http.cache_control = strdup(split);
		else if (strstr(split, "Accept:") != NULL)
			http.accept = strdup(split);
		else if (strstr(split, "Origin:") != NULL)
			http.origin = strdup(split);
		else if (strstr(split, "User-Agent:") != NULL)
			http.user_agent = strdup(split);
		else if (strstr(split, "Content-Type:") != NULL)
			http.content_type = strdup(split);
		else if (strstr(split, "Referer:") != NULL)
			http.referer = strdup(split);
		else if (strstr(split, "Accept-Encoding:") != NULL)
			http.accept_encoding = strdup(split);
		else if (strstr(split, "Accept-Language:") != NULL)
			http.accept_language = strdup(split);
		else if (strstr(split, "Content-Disposition:") != NULL)
			http.content_disposition = strdup(split);
		else if (strstr(split, "Upgrade-Insecure-Requests:") != NULL)
			http.upgrade_insecure_requests = strdup(split);
		else
		{
			http.contents = strdup(split+1);
		}
		split = strtok(NULL, deli);
	}
	http = getHttpHeader(http);
	return http; 
}

char* setHttpContents(HttpInfo http, char *content)
{
	char *split;
	char *temp;
	char *deli= "\r";
	int size;
	size = atoi(http.contentLength);
	split = strtok(content, deli);	
	http.contents = (char *) realloc (http.contents, size);
	while (split != NULL)
	{
		//if (isNullOrEmpty(split) == 1)
		//{
			//size += strlen(split);
			//http.contents = (char *) realloc (http.contents, size);
			//strcpy(temp, split);
			//strcat(temp, " ");
			strcat(http.contents, split);
		//}
		split = strtok(NULL, deli);
	}	
	return http.contents; 			
}

char* getFileExtension(char *file)
{
	char fileType[SIZE];
	char *split;
	char img[][5] = {"jpg", "jpeg", "jfif", "exif", "tiff", "gif", "bmp", "png", "ppm",
			 "pgm", "pbm", "pnm", "webp", "hdr", "heif", "bpg"};
	int array_size = 0;
	int i;
	if (isNullOrEmpty(file) == -1) return "bad";
	
	strcpy(fileType, file);
	split = strtok(fileType, ".");
	//printf("FileName[0]: %s\n", split);
	
	if (isNullOrEmpty(split) == -1) return "bad";
	split = strtok(NULL, " ");
	//printf("Extension[1]: %s\n\n", split);

	if (isNullOrEmpty(split) == -1) return "bad";

	array_size = sizeof(img)/sizeof(img[0]);
	for (i = 0; i < array_size; i++)
	{
		if (strcmp(split, img[i]) == 0)
			return split;
	}
	return "bad";
}

// flag 0 - GET, 1 - POST
char* getFileName(char *msg, int flag)
{
	char *split;
	char *fileName;
	int count_slash = 0;
	if (isNullOrEmpty(msg) == -1) return "bad";
	split = strdup(msg);
	split = strtok(split, " ");
	if (isNullOrEmpty(split) == -1) return "bad";

	split = strtok(NULL, " ");
	//printf("split[2]: %s\n", split);	
	if (isNullOrEmpty(split) == -1) return "bad";

	if (flag == 0)	
	{	
		split = strtok(split, "?");
		//printf("parser: query-string: %s\n", split);
	}	
	
	split = strtok(split, "/ ");
	while (isNullOrEmpty(split) != -1)
	{
		//printf("In while, fileName: %s\n", split);
		fileName = strdup(split);
		split = strtok(NULL, " ");
		count_slash++;
	}
	//printf("fileName: %s\n", fileName);
	
	if (count_slash > 1) return "n/a";	

	if (isNullOrEmpty(fileName) == -1)
	{
		return "index.html";
	}
	else
	{
		return fileName;			
	}
}

char* getHttpMethod(char* content)
{
	char *method;
	if (isNullOrEmpty(content) == -1) return "bad";
	method = strdup(content);
	method = strtok(method, " ");	
	return method;
}

char* getHttpValue(char* content)
{
	char *method;
	if (isNullOrEmpty(content) == -1) return "bad";
	method = strdup(content);
	method = strtok(method, " ");
	method = strtok(NULL, " ");	
	return method;
}
