/*
	INPUT: amazing i n_av diff ipaddr mport mwidth mheight filename
		where:
			i is the Avatar's id (id below)
			n_av is the number of Avatars (numAvatars)
			diff is the difficulty (difficulty)
			ipaddr is the IP address of the server (ip_address)
			mport is the port of the maze on the server (MazePort)
			mwidth is the width of the maze (MazeWidth)
			mheight is the height of the maze (MazeHeight)
			filename is the name of the log file (log_file)

	OUTPUT: messages to log file
		successful message to stdout
		

	Data Structures: utilizes shared memory segment to increase efficiency

	Definitions:

	Pseudocode: see design spec document for details
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <math.h>
#include <unistd.h>

#include "amazing.h"
#include "mazefuncs.h"
#include "utilities/messaging.h"
#include "utilities/sharedmem.h"

int main(int argc, char* argv[])
{
	// starting variable
	char* program;				// program name
	unsigned int id;			// Avatar id
	unsigned int numAvatars;
	unsigned int difficulty;
	char* ip_address;
	unsigned int MazePort;
	unsigned int MazeWidth;
	unsigned int MazeHeight;
	char* log_file;				// output file	

	// shared memory (wallmaze) variables
	int shmid;
	unsigned int *shared_top = NULL;

	// avatar location variables
	XYPOS* positions;

	// basic location variables
	int xpos = -1;
	int ypos = -1;

	// centroid variables	
	int centroid_x;
	int centroid_y;

	// move variables
	int* moves;
	int prev_move;

	// efficiency variables
	int num_moves = 0;

	// socket variables
	int sockfd;
	struct sockaddr_in servaddr;

	// message variables
	AM_MESSAGE* server_message;
	unsigned int message_type;

	// boolean flags
	int first_turn = 0;

	// file variables
	FILE* fp;

	program = argv[0];

	if(argc != 9)
	{
		fprintf(stderr, "%s: Incorrect number of arguments for the Avatar client.  Needs 8\n", program);
		return 1;
	}

	id = atoi(argv[1]);
	numAvatars = atoi(argv[2]);
	difficulty = atoi(argv[3]);
	ip_address = argv[4];
	MazePort = atoi(argv[5]);
	MazeWidth = atoi(argv[6]);
	MazeHeight = atoi(argv[7]);
	log_file = argv[8];

	unsigned int path_maze[MazeWidth][MazeHeight];	// crumbs for the path
	unsigned int *wall_maze[MAXSIZE][MAXSIZE];		// maintains the walls

	// establishes shared memory

	if(difficulty != 8 && difficulty != 9)			// because of server settings
	{
		shared_top = init_mem(MazeWidth, MazeHeight, &shmid);
		init_walls(wall_maze, shared_top, MazeWidth, MazeHeight);
	}
	else
	{
		for(int y = 0; y < MazeHeight; y++)
		{
			for(int x = 0; x < MazeWidth; x++)
			{
				wall_maze[x][y] = malloc(sizeof(unsigned int));
				*wall_maze[x][y] = 0;
			}
		}
	}

	// creates socket
	
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		fprintf(stderr, "%s: Error in creating the socket.\n", program);
		return 1;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip_address);
	servaddr.sin_port = htons(MazePort);

	if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
	{
		fprintf(stderr, "%s: Error in connecting to the server.\n", program);
		return 1;
	}	

	// sends ready message

	sendReady(id, sockfd);

	// allocates memory for server_message AM_MESSAGE and moves array

	server_message = malloc(sizeof(AM_MESSAGE));
	MALLOC_CHECK(server_message);
	
	moves = malloc(sizeof(int)*4);

	// opens file and readies it for appending
	fp = fopen(log_file, "a");

	if(id == 0)
		establishBoundaries(MazeWidth, MazeHeight, wall_maze);

	emptyArray(MazeWidth, MazeHeight, path_maze);	// sets all the values in path_maze to 0

	// the main functional loop 

	while(recv(sockfd, server_message, sizeof(AM_MESSAGE), 0) != 0)
	{
		message_type = ntohl(server_message->message_type);

		if(message_type == AM_AVATAR_TURN)
		{
			positions = (server_message->msg.avatar_turn.Pos);	// avatar tracks positions on all turns

			if(first_turn == 0)	// the centroid is calculated only once
			{
				calculateCentroid(positions, &centroid_x, &centroid_y, numAvatars);
				first_turn = 1;	
			}

			// if it's my turn to move
			if(ntohl(server_message->msg.avatar_turn.TurnId) == id)
			{		
				if(updateLocation(positions, &xpos, &ypos, id))
					path_maze[xpos][ypos] += 1;			// drops a crumb
				else
				if(onCentroid(xpos, ypos, centroid_x, centroid_y) == 0) 	// don't update walls if on the centroid - could block out others	
					updateWalls(xpos, ypos, prev_move, MazeWidth, MazeHeight, wall_maze);					

				if(rankMoves(xpos, ypos, centroid_x, centroid_y, moves, MazeWidth, MazeHeight, path_maze))	// decides which way to go
					sendMove(id, M_NULL_MOVE, sockfd);
				else
				{
					fprintf(fp, "LOCATIONS: ");
					for(int i = 0; i <= numAvatars; i++)
						fprintf(fp, "Avatar %d at (%d, %d).  ", i, ntohl(positions[i].xPos), ntohl(positions[i].yPos));
					fprintf(fp, "\n");		

					for(int i = 0; i <= M_NUM_DIRECTIONS; i++)
					{
						int move = moves[i];
						
						if(checkWalls(xpos, ypos, move, MazeWidth, MazeHeight, wall_maze))
							continue;

						fprintf(fp, "MOVE: AvatarId = %d, Direction = %d\n", id, move);
						prev_move = move;
						sendMove(id, move, sockfd);
						break;
					}
				}
			}

			// fills in dead ends while backing out to prevent redundant excursions
			fillDeadEnds(centroid_x, centroid_y, MazeWidth, MazeHeight, wall_maze);

			num_moves++;
		}

		if(message_type == AM_MAZE_SOLVED)
		{
			if(id == 0)	// success message: printed only once		
			{
				printf("Succeeded in %d turns.\n", num_moves);
				fprintf(fp, "Succeeded in %d turns.\n", num_moves);
			}

			break;
		}

		if(IS_AM_ERROR(message_type))	// error message handling
		{
			if(id == 0)
			{
				if(message_type == AM_TOO_MANY_MOVES)	// unable to solve in move limit
				{			
					fprintf(stderr, "%s: Max number of moves %d exceeded.  MazePort = %d.\n", program, num_moves, MazePort);
					fprintf(fp, "Failed to solve maze.  Max number of moves %d exceeded.\n", num_moves);
				}
				else 
				if(message_type == AM_SERVER_TIMEOUT)	// server timeout
				{
					fprintf(stderr, "%s: Server timed out.  Num moves = %d.  MazePort = %d.\n", program, num_moves, MazePort);
					fprintf(fp, "Server timed out.  Num moves = %d.\n", num_moves);
				}
				else					// nonsense
				if(message_type == AM_UNEXPECTED_MSG_TYPE || message_type == AM_UNKNOWN_MSG_TYPE || message_type == AM_NO_SUCH_AVATARID)
				{
					fprintf(stderr, "%s: Server sent bad message.\n", program);
					fprintf(fp, "Server sent bad message\n");
				}
			}

			break;
		}
	}

	// frees allocated memory and closes socket connection

	if(difficulty == 8 || difficulty == 9)
	{
		for(int y = 0; y < MazeHeight; y++)
		{
			for(int x = 0; x < MazeWidth; x++)
			{
				free(wall_maze[x][y]);
			}
		}
	}
	else
		free_mem(shared_top, &shmid, id);
	
	free(server_message);
	free(moves);

	fclose(fp);
	close(sockfd);
}
