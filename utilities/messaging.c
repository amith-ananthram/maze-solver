#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include "../amazing.h"

void sendInitialize(int numAvatars, int difficulty, int sockfd)
{	
	AM_MESSAGE* init_message;
	
	init_message = malloc(sizeof(AM_MESSAGE));
	MALLOC_CHECK(init_message);

	init_message->message_type = htonl(AM_INITIALIZE);
	init_message->msg.initialize.nAvatars = htonl(numAvatars);
	init_message->msg.initialize.Difficulty = htonl(difficulty);

	send(sockfd, init_message, sizeof(AM_MESSAGE), 0);
}

void sendMove(int id, int direction, int sockfd)
{
	AM_MESSAGE* move_message;

	move_message = malloc(sizeof(AM_MESSAGE));
	MALLOC_CHECK(move_message);

	move_message->message_type = htonl(AM_AVATAR_MOVE);
	move_message->msg.avatar_move.AvatarId = htonl(id);
	move_message->msg.avatar_move.Direction = htonl(direction);

	send(sockfd, move_message, sizeof(AM_MESSAGE), 0);

	free(move_message);
}

void sendReady(int id, int sockfd)
{
	AM_MESSAGE* ready_message;
	
	ready_message = malloc(sizeof(AM_MESSAGE));
	MALLOC_CHECK(ready_message);
	
	ready_message->message_type = htonl(AM_AVATAR_READY);
	ready_message->msg.avatar_ready.AvatarId = htonl(id);

	send(sockfd, ready_message, sizeof(AM_MESSAGE), 0);

	free(ready_message);
}

