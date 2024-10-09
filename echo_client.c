#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#define SERVER_TCP_PORT 3000
#define BUFLEN 256
#define MAX_PACKET_SIZE 100 
#define ERROR_MARKER '0' 
#define DATA_MARKER '1'

int main(int argc, char **argv)
{
	int 	n, i, bytesRead;
	int 	sd, port;
	struct	hostent		*hp;
	struct	sockaddr_in server;
	char	*host, *bp, rbuf[BUFLEN], sbuf[BUFLEN];

	switch(argc){
	case 2:
		host = argv[1];
		port = SERVER_TCP_PORT;
		break;
	case 3:
		host = argv[1];
		port = atoi(argv[2]);
		break;
	default:
		fprintf(stderr, "Usage: %s host [port]\n", argv[0]);
		exit(1);
	}

	/* Create a stream socket */	
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "Can't creat a socket\n");
		exit(1);
	}

	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if (hp = gethostbyname(host)) 
	  bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);
	else if ( inet_aton(host, (struct in_addr *) &server.sin_addr) ){
	  fprintf(stderr, "Can't get server's address\n");
	  exit(1);
	}

	/* Connecting to the server */
	if (connect(sd, (struct sockaddr *)&server, sizeof(server)) == -1){
	  fprintf(stderr, "Can't connect\n");
	  exit(1);
	}

    char filename[BUFLEN];
	char readData[MAX_PACKET_SIZE];
	int isFileDownloaded = 0; 

	printf("Enter the requested filename: ");
	scanf("%s", filename);
	// Send filename to server
	write(sd, filename, sizeof(filename));

	while ((bytesRead = read(sd, readData, sizeof(readData))) > 0) {
		if (readData[0] == ERROR_MARKER) {
			fprintf(stderr, "Error: File cannot be found or File size is smaller than 100 bytes\n");
			isFileDownloaded = 0;
			break;
		}
		else if (readData[0] == DATA_MARKER) {
			// Open file to write data from server
			FILE *outputFile = fopen(filename, "ab");
			printf("Receiving %d bytes from server\n", bytesRead);
			// Write the data after the data marker to the file
			fwrite(readData + 1, 1, bytesRead - 1, outputFile);
			fclose(outputFile);
			isFileDownloaded = 1;
		}
	}

	if (isFileDownloaded == 1) {
		printf("File Successfully Downloaded!\n");
	}

	printf("Connection Closed\n");
	close(sd);
	return(0);
}

