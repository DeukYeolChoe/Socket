#ifndef SERVER_H
#define SERVER_H

#define SIZE 1024
#define BACKLOG 5

int setCommand(char *fileName);
void send_binary(int socket, FILE *picture);
char* createHttpHeader(int status);
char* createHttpBody(int status, char* content);
void response(int client_socket, char *fileName, int cmd);
int server(char *port);

#endif
