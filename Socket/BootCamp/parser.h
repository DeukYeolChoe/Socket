#ifndef PARSER_H
#define PARSER_H

#define SIZE 1024
typedef int bool; 
#define true 1
#define false 0

struct httpinfo
{
	char *method;
	char *fileName;
	char *host;
	char *connection;
	char *contentLength;
	char *cache_control;
	char *accept;
	char *origin;
	char *user_agent;
	char *content_type;
	char *referer;
	char *accept_encoding;
	char *accept_language;
	char *content_disposition;
	char *upgrade_insecure_requests;
	char *contents;
};
typedef struct httpinfo HttpInfo;

HttpInfo getHttpHeader(HttpInfo http);
HttpInfo parseHttpHeader(char *content);
char* setHttpContents(HttpInfo http, char *content);
char* getHttpValue(char* content);
char* getFileExtensionchar(char *file);
char* getFileName(char *msg, int flag);
char* getHttpMethod(char* content);
char* getHttpValue(char* content);

#endif
