#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "amazing.h"
#include "utilities/messaging.h"

#define SLEN 50

int main(int argc, char* argv[])
{
	// starting variable
	char program[SLEN];		// program name
	int numAvatars;
	int difficulty;
	char ip_address[SLEN];
	char log_file[SLEN];		// output file	
	FILE *ofile;

	// input variables
	unsigned int MPort;
	unsigned int MWidth;
	unsigned int MHeight;

	// socket variables
	int sockfd;
	struct sockaddr_in servaddr;

	// message variable
	AM_MESSAGE* server_message;

	strncpy(program, argv[0], strlen(argv[0]));

	if (argc != 5) {
		fprintf(stderr, "%s: ERROR: Improper number of files specified!", program);
		fprintf(stderr, "%s: Usage: [# Avatars] [Difficulty] [Hostname] [Logfile Name]", program);
		exit(-1);
	}

	numAvatars = atoi(argv[1]);
	difficulty = atoi(argv[2]);
	strncpy(ip_address, argv[3], strlen(argv[3]));
	strncpy(log_file, argv[4], strlen(argv[4]));

	// creates socket
	
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		fprintf(stderr, "%s: Error in creating the socket.\n", program);
		return 1;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip_address);
	servaddr.sin_port = htons(AM_SERVER_PORT);

	if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
	{
		fprintf(stderr, "%s: Error in connecting to the server.\n", program);
		return 1;
	}

	// BUILDS INITIALIZE MESSAGE (for testing purposes... remove before submission)

	sendInitialize(numAvatars, difficulty, sockfd);

	server_message = malloc(sizeof(AM_MESSAGE));
	MALLOC_CHECK(server_message);

	recv(sockfd, server_message, sizeof(AM_MESSAGE), 0);

	if(ntohl(server_message->message_type) & AM_ERROR_MASK)
		printf("ERROR\n");

	MPort = ntohl(server_message->msg.initialize_ok.MazePort);
	MWidth = ntohl(server_message->msg.initialize_ok.MazeWidth);
	MHeight = ntohl(server_message->msg.initialize_ok.MazeHeight);
	
	close(sockfd);

	free(server_message);

	ofile = fopen(log_file, "w");
	if (ofile == NULL) {
		fprintf(stderr, "%s: ERROR: Unable to create logfile %s !", program, log_file);
		exit(-1);
	}

	fprintf(ofile, "%u %u %u", MPort, MWidth, MHeight);

	fclose(ofile);

	return 0;
}

//args indexwise
//6:width
//7:height
//in between port and filename
