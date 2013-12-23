#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <math.h>

#include "amazing.h"

// determines which square is most aligned with a position vector to the centroid
XYPOS getNewPosition(int xpos, int ypos, int direction)
{
	XYPOS coords;

	if(direction == M_WEST)
		xpos += -1;
	
	if(direction == M_EAST)
		xpos += 1;

	if(direction == M_NORTH)
		ypos += -1;
	
	if(direction == M_SOUTH)
		ypos += 1;

	coords.xPos = xpos;
	coords.yPos = ypos;
	
	return coords;
}

// returns the opposite compass direction
int oppositeDirection(int direction)
{
	if(direction == M_WEST)
		return M_EAST;
	else if(direction == M_NORTH)
		return M_SOUTH;
	else if(direction == M_SOUTH)
		return M_NORTH;
	else
		return M_WEST;
}

// returns the appropriate bitmask for the given direction
unsigned int getMask(int direction)
{
	if(direction == M_WEST)
		return W_MASK;
	else if(direction == M_EAST)
		return E_MASK;
	else if(direction == M_NORTH)
		return N_MASK;
	else
		return S_MASK;
}

// returns 1 if on centroid, 0 if not
int onCentroid(int x, int y, int centroid_x, int centroid_y)
{
	if(x == centroid_x && y == centroid_y)
		return 1;
	else
		return 0;
}

// initializes perimeter of wall_maze to reflect where the edges of the maze are
void establishBoundaries(int MazeWidth, int MazeHeight, unsigned int *wall_maze[][MAXSIZE])
{
	unsigned int square_val;	

	int x;
	int y;	

	// sets east and west
	for(y = 0; y < MazeHeight; y++)
	{
		x = 0;

		square_val = *wall_maze[x][y];
		*wall_maze[x][y] = square_val | W_MASK;

		x = MazeWidth - 1;

		square_val = *wall_maze[x][y];
		*wall_maze[x][y] = square_val | E_MASK;
	}
	
	// sets north and south
	for(x = 0; x < MazeWidth; x++)
	{
		y = 0;
	
		square_val = *wall_maze[x][y];
		*wall_maze[x][y] = square_val | N_MASK;
		
		y = MazeHeight - 1;
		
		square_val = *wall_maze[x][y];
		*wall_maze[x][y] = square_val | S_MASK;
	}
}

// W N E S 
// 1 1 1 0 = 14 - block N in square to S
// 0 1 1 1 = 7 	- block E in square to W
// 1 1 0 1 = 13	- block W in square to E
// 1 0 1 1 = 11 - block S in square to N
// fills in dead ends in wall_maze to prevent Avatars from making redundant treks
void fillDeadEnds(int centroid_x, int centroid_y, int MazeWidth, int MazeHeight, unsigned int *wall_maze[][MAXSIZE])
{
	unsigned int square_val;
	int direction;

	XYPOS adj;
	unsigned int mask;
	int adjx;
	int adjy;

	for(int y = 0; y < MazeHeight; y++)
	{
		for(int x = 0; x < MazeWidth; x++)
		{
			square_val = *wall_maze[x][y];

			if(x != centroid_x && y != centroid_y)
			{
				if(square_val == 7 || square_val == 11 || square_val == 13 || square_val == 14)
				{	
					if(square_val == 7) 		// block E in square to W	
						direction = M_WEST;
					else if(square_val == 11)	// block S in square to N
						direction = M_NORTH;
					else if(square_val == 13)	// block W in square to E
						direction = M_EAST;
					else
						direction = M_SOUTH;	// block N in square to S

					adj = getNewPosition(x, y, direction);
					adjx = adj.xPos;
					adjy = adj.yPos;				

					if(adjx >= 0 && adjx < MazeWidth && adjy >= 0 && adjy < MazeHeight)
					{	
						mask = getMask(oppositeDirection(direction));
						*wall_maze[adjx][adjy] = *wall_maze[adjx][adjy] | mask;
					}
				}
			}
		}
	}
}

// updates wall_maze to reflect recently encountered wall
void updateWalls(int xpos, int ypos, int direction, int MazeWidth, int MazeHeight, unsigned int *wall_maze[][MAXSIZE])
{	
	//printf("Wallmaze top, xpos = %d ypos = %d\n", xpos, ypos);
	//printf("\t%p\n", (void *)wall_maze);

	//printf("PRINTING WALLS\n");
	//int i, j;
	//for (j = 0; j < MazeHeight; j++) {
	//	for (i = 0; i < MazeWidth; i++) {
	//		printf("%u ", *wall_maze[i][j]);
	//	}
	//	printf("\n");
	//}

	unsigned int mask;
	unsigned int current_square_val;

	XYPOS opposite_square;
	int opposite_x;
	int opposite_y;
	int opposite_direction;
	unsigned int opposite_mask;
	unsigned int opposite_square_val;

	mask = getMask(direction);
	//printf("1\n");
	//printf("maze val: %u\n", *wall_maze[2][0]);
	//printf("%u\n", *wall_maze[xpos][ypos]);
	current_square_val = *wall_maze[xpos][ypos];
	//printf("2\n");
	*wall_maze[xpos][ypos] = current_square_val | mask;
	//printf("3\n");

	opposite_square = getNewPosition(xpos, ypos, direction);
	opposite_x = opposite_square.xPos;
	opposite_y = opposite_square.yPos;

	if(opposite_x >= 0 && opposite_x < MazeWidth && opposite_y >= 0 && opposite_y < MazeHeight)
	{
		opposite_direction = oppositeDirection(direction);
		opposite_mask = getMask(opposite_direction);	
		opposite_square_val = *wall_maze[opposite_x][opposite_y];
	//	printf("4\n");
		*wall_maze[opposite_x][opposite_y] = opposite_square_val | opposite_mask;	
	//	printf("5\n");
	}
}

// checks to see if there is a wall in a certain direction.  returns 0 if no wall, 1 if there is a wall
int checkWalls(int xpos, int ypos, int direction, int MazeWidth, int MazeHeight, unsigned int *wall_maze[][MAXSIZE])
{
	unsigned int square_val;
	unsigned int bitwised;
	unsigned int mask;

	square_val = *wall_maze[xpos][ypos];
	mask = getMask(direction);

	bitwised = square_val & mask;

	if(bitwised == 0)
		return 0;
	else
		return 1;
}

// sets all the values in maze to 0
void emptyArray(int MazeWidth, int MazeHeight, unsigned int maze[MazeWidth][MazeHeight])
{
	for(int x = 0; x < MazeWidth; x++)
		for(int y = 0; y < MazeHeight; y++)
			maze[x][y] = 0;
}

// determines which square the centroid is on.  each avatar does this once
void calculateCentroid(XYPOS* positions, int* centroid_x, int* centroid_y, int numAvatars)
{
	XYPOS current_coords;	
	int x_tally = 0;
	int y_tally = 0;

	for(int i = 0; i < numAvatars; i++)
	{
		current_coords = positions[i];

		x_tally += ntohl(current_coords.xPos);
		y_tally += ntohl(current_coords.yPos);
	}

	*centroid_x = (int)(floor(x_tally/numAvatars));
	*centroid_y = (int)(floor(y_tally/numAvatars));
}

// was the avatar's move successful?
// returns 1 if location has changed
// returns 0 if location hasn't changed
int updateLocation(XYPOS* positions, int* xpos, int* ypos, int avatar_id)
{
	XYPOS current_coords;

	int new_x;
	int new_y;
	
	current_coords = positions[avatar_id];

	new_x = ntohl(current_coords.xPos);
	new_y = ntohl(current_coords.yPos);
	
	if(new_x == *xpos && new_y == *ypos)
		return 0;
	
	*xpos = new_x;
	*ypos = new_y;
	
	return 1;
}

// weights the avatar's possible moves considering breadcrumbs and centroid direction
// returns 0 if not on centroid
// returns 1 if on centroid
int rankMoves(int xpos, int ypos, int centroid_x, int centroid_y, int* moves, int MazeWidth, int MazeHeight, unsigned int path_maze[MazeWidth][MazeHeight])
{	
	int xdist;
	int ydist;

	int xindex1;
	int xindex2;
	
	int yindex1;
	int yindex2;

	int crumb_vals[M_NUM_DIRECTIONS];
	XYPOS new_pos;
	int new_x;
	int new_y;

	int s;
	unsigned int crumb_temp;
	int move_temp;

	xdist = centroid_x - xpos;
	ydist = centroid_y - ypos;

	if(ydist == 0 && xdist == 0)
	{
		return 1;
	}

	if(abs(xdist) >= abs(ydist))
	{
		xindex1 = 0;
		xindex2 = 3;

		yindex1 = 1;
		yindex2 = 2;
	}
	else
	{
		xindex1 = 1;
		xindex2 = 2;
		
		yindex1 = 0;
		yindex2 = 3;
	}

	if(xdist < 0)
	{
		moves[xindex1] = M_WEST;
		moves[xindex2] = M_EAST;
	}
	else
	{
		moves[xindex1] = M_EAST;
		moves[xindex2] = M_WEST;
	}
	
	if(ydist < 0)
	{
		moves[yindex1] = M_NORTH;
		moves[yindex2] = M_SOUTH;
	}
	else
	{
		moves[yindex1] = M_SOUTH;
		moves[yindex2] = M_NORTH;
	}	

	for(int i = 0; i < M_NUM_DIRECTIONS; i++)
	{
		new_pos = getNewPosition(xpos, ypos, moves[i]);
		new_x = new_pos.xPos;
		new_y = new_pos.yPos;
		
		if(new_x >= 0 && new_x < MazeWidth && new_y >= 0 && new_y < MazeHeight)
			crumb_vals[i] = path_maze[new_x][new_y];
		else
			crumb_vals[i] = 10000;
	}

	s = 1;				// to break out of bubble sort

	for(int i = 0; i < M_NUM_DIRECTIONS && s != 0; i++)
	{
		s = 0;		
		
		for(int j = 0; j < M_NUM_DIRECTIONS-1; j++)
		{
			if(crumb_vals[j] > crumb_vals[j+1])
			{
				crumb_temp = crumb_vals[j];
				crumb_vals[j] = crumb_vals[j+1];
				crumb_vals[j+1] = crumb_temp;
				
				move_temp = moves[j];
				moves[j] = moves[j+1];
				moves[j+1] = move_temp;

				s++;			
			}
		}
	}	

	//printf("CRUMB VALS: %d %d %d %d\n", crumb_vals[0], crumb_vals[1], crumb_vals[2], crumb_vals[3]);
	//printf("MOVE VALS: %d %d %d %d\n", moves[0], moves[1], moves[2], moves[3]);

	return 0;		
}	
