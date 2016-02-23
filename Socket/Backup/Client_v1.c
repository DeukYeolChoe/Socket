#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#define SIZE 1024

void error(char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	int client_socket, port;
	struct sockaddr_in client_addr;
	char server_ip[20] = {0,};
	char request[SIZE];
	char response[SIZE];
	
	if (argc != 3)
	{
		printf("Please, follow this: ./executable_file server_ip server_port\n");
	}	
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1)
	{
		printf("Could not create client_socket\n");
	}
	
	bzero((char *)&client_addr, sizeof(client_addr));
	strcpy(server_ip, argv[1]);
	port = atoi(argv[2]);
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = inet_addr(server_ip);
	client_addr.sin_port = htons(port);
	printf("server ip: %s\n", server_ip);
	printf("server port: %d\n", port);

	if ((client_addr.sin_addr.s_addr = inet_addr(server_ip) == (unsigned long) INADDR_NONE))
	{
		error("ERROR on inet_addr");
	}
		
	if ((connect(client_socket, (struct sockaddr *)&client_addr, sizeof(client_addr))) == -1)
	{
		error("ERROR on connecting");
		//close(client_socket);
	}

	while(1)
	{
		bzero(request, SIZE);	
		bzero(response, SIZE);
		printf("\nEnter message: ");
		scanf("%s", request);
		if (send(client_socket, request, strlen(request), 0) < 0)
		{
			printf("ERROR on sending\n");
			return 1;
		}
		
		if (recv(client_socket, response, SIZE, 0) < 0)
		{
			printf("ERROR on receiving");
			break;
		}
		else
		{
			printf("Server's response: %s", response);
		}
	}
	close(client_socket);	
	//close(client_socket);	
	return 0;
}
