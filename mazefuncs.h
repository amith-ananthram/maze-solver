/************************************************************
 *
 *	mazefuncs.h
 *
 *	header file corresponding with mazefuncs.c
 *
 * */

#ifndef _MAZEFUNCS_H_
#define _MAZEFUNCS_H_

void calculateCentroid(XYPOS* positions, int* centroid_x, int* centroid_y, int numAvatars);

int updateLocation(XYPOS* positions, int* xpos, int* ypos, int avatar_id);

int rankMoves(int xpos, int ypos, int centroid_x, int centroid_y, int* moves, int MazeWidth, int MazeHeight, unsigned int path_maze[MazeWidth][MazeHeight]);

void updateWalls(int xpos, int ypos, int direction, int MazeWidth, int MazeHeight, unsigned int *wall_maze[][MAXSIZE]);

int checkWalls(int xpos, int ypos, int direction, int MazeWidth, int MazeHeight, unsigned int *wall_maze[][MAXSIZE]);

void emptyArray(int MazeWidth, int MazeHeight, unsigned int maze[MazeWidth][MazeHeight]);

XYPOS getNewPosition(int xpos, int ypos, int direction);

void fillDeadEnds(int centroid_x, int centroid_y, int MazeWidth, int MazeHeight, unsigned int *wall_maze[][MAXSIZE]);

void establishBoundaries(int MazeWidth, int MazeHeight, unsigned int *wall_maze[][MAXSIZE]);

int onCentroid(int x, int y, int centroid_x, int centroid_y);

#endif
