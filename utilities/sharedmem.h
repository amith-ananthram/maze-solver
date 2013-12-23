//
//	sharedmem.h
//
//	header file for sharedmem.c
//	shared memory utilities for use with AMAZING maze solver
//

#ifndef _SHAREDMEM_H
#define _SHAREDMEM_H

#define KEY 9011
#define MDIM 100

unsigned int* init_mem(const int mwidth, const int mheight, int *shmid);
int free_mem(unsigned int *, int *shmid, unsigned int n_av);
int init_walls(unsigned int *shared_walls[][MDIM], unsigned int *array, int mwidth, int mheight);

#endif
