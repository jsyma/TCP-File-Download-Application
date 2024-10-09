#include <stdio.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <strings.h>

#define SERVER_TCP_PORT 3000
#define BUFLEN 256	
#define MAX_PACKET_SIZE 100 
#define MIN_FILE_SIZE 101 

int echod(int);
void reaper(int);

int main(int argc, char **argv)
{
	int 	sd, new_sd, client_len, port;
	struct	sockaddr_in server, client;

	switch(argc){
	case 1:
		port = SERVER_TCP_PORT; 
		break;
	case 2:
		port = atoi(argv[1]);
		break;
	default:
		fprintf(stderr, "Usage: %s [port]\n", argv[0]);
		exit(1);
	}

	/* Create a stream socket */	
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "Can't create a socket\n");
		exit(1);
	}

	/* Bind an address to the socket */
	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1){
		fprintf(stderr, "Can't bind name to socket\n");
		exit(1);
	}

	/* Queue up to 5 connect requests */
	listen(sd, 5);

	(void) signal(SIGCHLD, reaper);

	while(1) {
		client_len = sizeof(client);
		new_sd = accept(sd, (struct sockaddr *)&client, &client_len);
	  	if(new_sd < 0){
	    	fprintf(stderr, "Can't accept client \n");
	    	exit(1);
	  	}
	  	switch (fork()){
	  	case 0:		/* child */
			(void) close(sd);
		
			char filename[BUFLEN];
			read(new_sd, filename, sizeof(filename));
			
			FILE *file = fopen(filename, "rb");
			if (file == NULL) {
				char errorMsg[MAX_PACKET_SIZE];
				errorMsg[0] = '0';
				fprintf(stderr, "Error: File cannot be found\n");
				write(new_sd, errorMsg, sizeof(errorMsg));
				close(new_sd);
				exit(0); 
			}
			else {
				// Get file length 
				fseek(file, 0, SEEK_END);
				long fileSize = ftell(file);
				fseek(file, 0, SEEK_SET);

				if (fileSize < MIN_FILE_SIZE) {
					char errorMsg[MAX_PACKET_SIZE];
					fprintf(stderr, "Error: File size is smaller than 100 bytes\n");
					fclose(file);
					close(new_sd);
					exit(0);
				}

				char readData[MAX_PACKET_SIZE];
				int bytesRead;

				while ((bytesRead = (int) fread(readData + 1, 1, MAX_PACKET_SIZE - 1, file)) > 0) {
					printf("Sending %d bytes to client\n", bytesRead + 1);				
					readData[0] = '1';
					if (write(new_sd, readData, bytesRead + 1) == -1) {
						fprintf(stderr, "Error! Could not Write to Client\n");
						break;
					}
				}
				fclose(file);
			}

			/* Close the new socket */
			close(new_sd);
			exit(0);
		default:		/* parent */
			(void) close(new_sd);
			break;
		case -1:
			fprintf(stderr, "fork: error\n");
		}
	}
}

/* reaper */
void	reaper(int sig)
{
	int	status;
	while(wait3(&status, WNOHANG, (struct rusage *)0) >= 0);
}

