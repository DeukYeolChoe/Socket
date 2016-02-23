#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

typedef int bool;
#define true 1
#define false 0
#define SIZE 1024
#define BACKLOG 10

char filePath[SIZE];

void error(char *msg)
{
	//perror: print a system error message 
    	perror(msg);
	//exit(1): indicates unsucessful termination
 	exit(1);
}

int getFilePath(char *msg)
{
	char *split;
	if (msg == NULL) return 0;
	if (strlen(msg) <= 0) return 0;

	split = strtok(msg, "\r\n");
	printf("split[0]: %s\n", split);
	if (split == NULL) return 0;

	split = strtok(split, " ");
	printf("split[1]: %s\n", split);
	if (split == NULL) return 0;

	// GET check
	if (strcmp(split, "GET") != 0) return 0;
	split = strtok(NULL, " ");
	printf("split[2]: %s\n", split);	
	if (split == NULL) return 0;	
	if (strlen(split) <= 0) return 0;
	
	split = strtok(split, "/ ");
	printf("split[3]: %s\n", split);
	
	if (split == NULL)
	{
		strcpy(filePath, "index.html");
	}
	else
	{
		strcpy(filePath, split);
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

int main(int argc, char *argv[])
{
     	int server_socket, client_socket, port;
	int bytes_count, client_len, buffer_size = 0;
     	char buffer[SIZE], data[SIZE], line[SIZE];
	char curDirectory[SIZE];
	FILE *fileReader;	
     	struct sockaddr_in server_addr, client_addr;
     	
	if (argc < 2) {
         	printf("ERROR, no port provided\n");
         	exit(1);
     	}
	//AF_INET = IPv4 protocol, SOCK_STREAM = TCP
    	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
	{
		error("setsockopt(SO_REUSEADDR) failed");
	}
     	
	if (server_socket < 0)
	{ 
        	error("ERROR opening socket\n");
	}
     	bzero((char *) &server_addr, sizeof(server_addr));
     
	//Validate a port number	
	if (validatePort(argv[1]) != false)
	{
		port = atoi(argv[1]);
	}
	else
	{	
		printf("Please, retry an input: ./Server port_number\n");
		exit(1);
	}
	
     	server_addr.sin_family = AF_INET;
     	server_addr.sin_addr.s_addr = INADDR_ANY;
	//htons function converts a u_short from host to TCP/IP network byte order (which is big-endian)
	server_addr.sin_port = htons(port);

	//It binds the socket to the specified address and TCP/UDP port
     	if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{ 
              error("ERROR on binding");
	}
	//When the server is processing a connection, "backlog" number of connections may be pending in a queue
     	listen(server_socket, BACKLOG);
	printf("Server: waiting for request!\n");
     	while(1)
     	{
     		client_len = sizeof(client_addr);
		//Accept method does not return until a remote client has established a connection
     		client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_len);
    		if (client_socket < 0)
		{ 
          		error("ERROR on accept");
		}
     		bzero(buffer,SIZE);
    		bytes_count = recv(client_socket, buffer, sizeof(buffer), 0);
     		
		if (bytes_count < 0)
		{ 
			error("ERROR reading from socket");
		}
    		printf("I got the message: %s\n", buffer);
		
		if (getFilePath(buffer) == 0)
		{
			send(client_socket, "HTTP/1.1 400 Bad request\r\n\r\n", 33, 0);
			send(client_socket, "<html><h1>400 Bad request</h1></html>\n", 40, 0);
		}
		else
		{
			getcwd(curDirectory, SIZE);
			strcat(curDirectory, "/index.html");
			fileReader = fopen(curDirectory, "r");
			printf("Cur: %s\n", curDirectory);
			if (strcmp(filePath, "index.html") == 0)
			{
				if (send(client_socket, "HTTP/1.1 200 OK\n\n", 17, 0) == -1)
					error("send");
				while (fgets(line, sizeof(line), fileReader) != NULL)
				{
					if (send(client_socket, line, strlen(line), 0) == -1)
						error("send");
				}	
			}
			else
			{
				if (send(client_socket, "HTTP/1.1 404 Not found\r\n\r\n", 26, 0) == -1)
					error("Not Found");
				if (send(client_socket, "<html><h1>404 Not Found</h1></html>\n", 36, 0) == -1)
					error("Not Found");
			}
		}
		close(client_socket);
	}
	close(fileReader);
}
