#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "server.h"
#include "validation.h"
#include "log.h"
#include "parser.h"

#define OFFSET 200
/*
 * Decide to the status of http request.
 * 1 - 404 not found
 * 0 - 400 bad request
 * 2 - 200 Ok
 */
int setCommand(char *fileName)
{
		int cmd = -1;
		char *cmp;
		cmp = strdup(fileName);
	
		if (isValidImgExtension(cmp) == -1)
		{
			return 1;
		}
		else if (strcmp(cmp, "bad") == 0)
		{
			return 0;
		}
		else
			return 2;
}

/*
 * Send an image file
 */
void send_binary(int socket, FILE *picture)
{
	int size, read_size, stat, packet_index;
	char send_buffer[SIZE];
	char read_buffer[SIZE];
	packet_index = 1;

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
			//stat = write(socket, send_buffer, read_size);
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

void receive_binary(int socket, char *fileName)
{
	FILE *fd;
	char buffer[SIZE];
	int n;
	fd = fopen(fileName, "w");
	if (fd == NULL)
	{
		makeLog("server: fd == NULL\n");
		error();
	}
	printf("Ready to recevie a file!!\n");
	while(1)
	{
		bzero(buffer, SIZE);
		if ((n = recv(socket, buffer, sizeof(buffer), 0)) < 0)
		{
			makeLog("server: receive_binary() recv()\n");
			error();
		}
		printf("Receiving: %s\n", buffer);	
		fwrite(buffer, 1, sizeof(buffer), fd); 
		if (n <= 0)
			break;
	}
	
	fclose(fd);
}

char* createHttpHeader(int status)
{
	switch(status)
	{
		case 200:
			return "HTTP/1.1 200 OK\n\n";
		case 400:
			return "HTTP/1.1 400 Bad request\r\n\r\n";
		case 404:
			return "HTTP/1.1 404 Not found\r\n\r\n";
		default:
			return "The status is not implemented\r\n\r\n";
	}
}
 
char* createHttpBody(int status, char* content)
{
	char *body;
	char temp[SIZE];
	strcpy(temp, "<HTML><H1>");
	strcat(temp, content);
	strcpy(temp, "</H1></HTML>"); 
	body = strdup(temp);
	return body;
}

/*
 * Respond about the request
 * 0 - 400 Bad request
 * 1 - 404 Not found or 200 Ok for web page request
 * 2 - 400 Not found or 200 Ok for image request
 */
void response(int client_socket, char* fileName, int cmd)
{
	char *status;
	char *stat_msg;
	char curDirectory[SIZE];
	char line[SIZE];
	FILE *fileReader;
	
	switch(cmd)
	{
		case 0:
			//status = strdup("HTTP/1.1 400 Bad request\r\n\r\n");
			//stat_msg = strdup("<html><h1>400 Bad request</h1></html>");
			status = createHttpHeader(400);
			stat_msg = createHttpBody(400, "400 Bad request");
			send(client_socket, status, strlen(status), 0);
			send(client_socket, stat_msg, strlen(stat_msg), 0);
			break;
		case 1:
			getcwd(curDirectory, SIZE);
			strncat(curDirectory, "/", strlen("/"));
			strncat(curDirectory, fileName, strlen(fileName));
			printf("server: (curDirectory) %s\n", curDirectory);
			fileReader = fopen(curDirectory, "r");
			
			if (fileReader == NULL)
			{
				//status = strdup("HTTP/1.1 404 Not found\r\n\r\n");
				//stat_msg = strdup("<html><body><h1>404 Not found</h1></body></html>");
				status = createHttpHeader(404);
				stat_msg = createHttpBody(404, "404 Not found");
				send(client_socket, status, strlen(status), 0);
				send(client_socket, stat_msg, strlen(stat_msg), 0);
			}
			else
			{
				//status = strdup("HTTP/1.1 200 OK \n\n");
				status = createHttpHeader(200);
				if (send(client_socket, status, strlen(status), 0) == -1)
					error("send");
				while (fgets(line, sizeof(line), fileReader) != NULL)
				{
					if (send(client_socket, line, strlen(line), 0) == -1)
						error("send");
				}
			}
			break;
		case 2:
			//Send a file
			fileReader = fopen(fileName, "rb");
			printf("Getting Picture Size\n");
			if (fileReader == NULL)
			{
				//status = strdup("HTTP/1.1 404 Not found\r\n\r\n");
				//stat_msg = strdup("<html><body><h1>404 Not found</h1></body></html>");
				status = createHttpHeader(404);
				stat_msg = createHttpBody(404, "404 Not found");
				send(client_socket, status, strlen(status), 0);
				send(client_socket, stat_msg, strlen(stat_msg), 0);
			}			
			else
			{
				//status = strdup("HTTP/1.1 200 OK \n\n");
				status = createHttpHeader(200);
				if (send(client_socket, status, strlen(status), 0) == -1)
					error("send");
				send_binary(client_socket, fileReader);
			}			
			break;
		default:
			makeLog("response: invalid cmd\n");
			error();
	}
}

void processCGI(HttpInfo http, int socket, char *fileName)
{
	int fd1[2], fd2[2], nbytes;
	int flag = 0;
	int envValue = 0;
	pid_t pid;
	char readbuffer[SIZE] = {'\0',};
	char *status;
	pipe(fd1);
	pipe(fd2);
	
	printf("QUERY_STRING: [%s]\n", http.contents);
	printf("CONTENT_LENGTH: [%s]\n", http.contentLength);
	printf("REQUEST_METHOD: [%s]\n", http.method);
	
	if ((envValue = setenv("REQUEST_METHOD", http.method, 1)) == -1)
	{
			makeLog("server: REQUEST_METHOD\n");
	}
	if ((envValue = setenv("QUERY_STRING", http.contents, 1)) == -1)
	{
			makeLog("server: QUERY_STRING\n");
	}
	if ((envValue = setenv("CONTENT_LENGTH", http.contentLength, 1)) == -1)
	{
			makeLog("server: CONTENT_LENGTH\n");
	}

	pid = fork();
	
	if (pid == 0)
	{//A child process
		close(fd1[1]);
		dup2(fd1[0], 0);//STDIN
		close(fd2[0]);
		dup2(fd2[1], 1);//STDOUT
		
		//execl("/home/deukyeolchoe/Socket/BootCamp/calendar.cgi", NULL);
		if (strstr(fileName, ".class") != NULL)
			execlp("java", "java", "lottery", NULL);	
		else
			execl(fileName, NULL);
	}
	else if (pid > 0)
	{//A parent process
		close(fd1[0]);
		dup2(fd1[1], 1);
		close(fd2[1]);
		dup2(fd2[0], 0);
		printf("%s\n", http.contents);
		bzero(readbuffer, SIZE);
		while ((nbytes = read(0, readbuffer, SIZE)) > 0)
		{
			if (flag == 0)
			{	
				status = strdup("HTTP/1.1 200 OK\r\n");
				if (send(socket, status, strlen(status), 0) == -1)
					error("send");
				flag = 1;
			}
			send(socket, readbuffer, nbytes, 0);
			bzero(readbuffer, SIZE);
		}
		wait(NULL);
	}
}

int server(char *port)
{
	pid_t pid;
	HttpInfo http;
	struct sockaddr_in client_addr, server_addr;
	int server_sockfd, client_sockfd;
	int state, client_len;
	int server_port;
	int cmd;
	int n, nbytes;
	char buffer[SIZE];
	char *httpbody;
	char *fileName;
	char *status;

	server_port = atoi(port);
	if (server_port)
	{
		server_port = atoi(port);
	}
	else
	{
		makeLog("server: server port\n");
		error();
	}

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(server_port);

	if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		makeLog("server: socket()\n");
		error();
	}

	if ((setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &(int) { 1 }, sizeof(int))) < 0)
	{
		makeLog("server: setsockopt()\n");
		error();
	}
	
	if (bind(server_sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		makeLog("server: bind()\n");
		error();	
	}

	state = listen(server_sockfd, BACKLOG);
	printf("server: waiting for request!!\n");

	if (state == -1)
	{
		makeLog("server: listen()\n");
		error();	
	}
	
	signal(SIGCHLD, SIG_IGN);
	while (1)
	{
		client_len = sizeof(client_addr);
		
		//Accept method does not return until a remote client has established a connection
     		client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_addr, &client_len);
    		if (client_sockfd < 0)
		{ 
          		makeLog("server: accept()\n");
			error();
		}
		
		pid = fork();
		if (pid == 0)
		{//A child process
			while (1)
			{
				bzero(buffer, SIZE);
				if ((nbytes = recv(client_sockfd, buffer, sizeof(buffer), 0)) < 0)
				{
					makeLog("server: recv()\n");
					error();
				}
				else
				{
					printf("I have received data: [%s]\n", buffer);
					http = parseHttpHeader(buffer);
					if (strcmp(http.contentLength, "bad") != 0 && atoi(http.contentLength) > SIZE)
					{
						nbytes = 0;
						n = strlen(http.contents);
						while (1)
						{
							bzero(buffer, SIZE);
							nbytes = recv(client_sockfd, buffer, sizeof(buffer), 0);
							n += nbytes;
							printf("The rest of buffer = [%s]\n", buffer);
							http.contents = setHttpContents(http, buffer);
							printf("The n = [%d]\n", n);
							
							if ((atoi(http.contentLength)) < n + OFFSET)
								break;
						}
					}
					//printf("Method: [%s]\n", http.method);
					//printf("***Contents: \n[%s]\n", http.contents);

					if (strcmp(http.method, "GET") == 0)
					{
						printf("server: GET\n");
						fileName = getFileName(http.fileName, 0);
					}	
					else if(strcmp(http.method, "POST") == 0)
					{
						printf("server: POST\n");
						fileName = getFileName(http.fileName, 1);
					}	
					printf("server: (fileName) %s\n", fileName);
					cmd = setCommand(fileName);
					printf("server: (cmd): %d\n", cmd);
					
					if (isCgiScript(fileName) == 1)
					{
						printf("cgi running\n");
						processCGI(http, client_sockfd, fileName);
					}
					else
					{
						response(client_sockfd, fileName, cmd);
					}
					
					printf("server: socket closing\n");
					close(client_sockfd);
				}
				exit(0);
			}		
		}
		else if (pid > 0)
		{
			wait(NULL);
			close(client_sockfd);
		}
		else if (pid == -1)
		{
			makeLog("server: pid == -1\n");
			error();
		}
	}
}
