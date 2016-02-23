#ifndef DEUK_TEST_H
#define DEUK_TEST_H

typedef int bool;
#define true 1
#define false 0
#define SIZE 1024
#define BACKLOG 10

void error(char* msg);
int getFileExtension(char* file);
int getFilePath(char* msg);
bool validatePort(char* port);
//void send_binary(int socket, FILE* picture);

#endif
