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
	printf("Extension[0]: %s\n", split);
	
	if (split == NULL) return 0;
	split = strtok(NULL, " ");
	printf("Extension[1]: %s\n", split);

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
	printf("size split[2]: %s\n", strlen(split));	
	if (split == NULL) return 0;
	if (strlen(split) > 400) return 0;	
	if (strlen(split) <= 0) return 0;
	
	split = strtok(split, "/ ");
	printf("fileName: %s\n", split);
	
	if (split == NULL)
	{
		strcpy(fileName, "index.html");
	}
	else
	{
		strcpy(fileName, split);
		int extension = getFileExtension(split);
		if (extension == 0 && strlen(fileName) > 0 && strcmp(split, "home") != 0)
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
void send_binary(int socket)
{
	FILE *picture;
	int size, read_size, stat, packet_index;
	char send_buffer[SIZE];
	char read_buffer[SIZE];
	packet_index = 1;

	picture = fopen(fileName, "rb");
	printf("Getting Picture Size\n");
	printf("fileName: %s\n", fileName);	
	if (picture == NULL)
	{
		error("Error on opening an image");
	}

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
			//stat = write(socket, send_buffer, read_size);
			stat = send(socket, send_buffer, read_size, 0);	
		} while(stat < 0);
	
		printf("Packet Number: %d\n", packet_index);
		printf("Packet Size sent: %d\n", read_size);
		printf("\n\n");
		packet_index++;
		bzero(send_buffer, sizeof(send_buffer));
	}
}

int main(int argc, char *argv[])
{
     	int server_socket, client_socket, port;
	int bytes_count, client_len, buffer_size = 0;
     	char buffer[SIZE], data[SIZE], line[SIZE];
	char curDirectory[SIZE];
	char status[SIZE] = {0,};
	char stat_msg[SIZE] = {0,};
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
        	error("ERROR on opening socket\n");
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
		
		int swtch = getFilePath(buffer);
		printf ("swtch: %d\n", swtch);
		if (swtch == 0)
		{
			strcpy(status, "HTTP/1.1 400 Bad request\r\n\r\n");
			strcpy(stat_msg, "<html><h1>400 Bad request</h1></html>");
			send(client_socket, status, strlen(status), 0);
			send(client_socket, stat_msg, strlen(stat_msg), 0);
		}
		else if (swtch == 1)
		{
			getcwd(curDirectory, SIZE);
			strcat(curDirectory, "/");
			strcat(curDirectory, fileName);
			printf("Cur: %s\n", curDirectory);
			fileReader = fopen(curDirectory, "r");
			if (fileReader == NULL)
			{
				strcpy(status, "HTTP/1.1 404 Not found\r\n\r\n");
				strcpy(stat_msg, "<html><body><h1>404 Not found</h1></body></html>");
				send(client_socket, status, strlen(status), 0);
				send(client_socket, stat_msg, strlen(stat_msg), 0);
			}
			else
			{
				strcpy(status, "HTTP/1.1 200 OK \n\n");
				if (send(client_socket, status, strlen(status), 0) == -1)
					error("send");
				while (fgets(line, sizeof(line), fileReader) != NULL)
				{
					if (send(client_socket, line, strlen(line), 0) == -1)
						error("send");
				}
			}
		}
		else if (swtch == 2)
		{// File
			strcpy(status, "HTTP/1.1 200 OK \n\n");
			if (send(client_socket, status, strlen(status), 0) == -1)
				error("send");
			send_binary(client_socket);			
		}
		close(client_socket);
	}
	close(fileReader);
}
