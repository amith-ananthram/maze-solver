//
//	sharedmem.c
//
//	shared memory utilities for use with AMAZING maze solver
//	initializes shared memory block and configures interface structure
//	returns memory once completed
//

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <errno.h>
#include "sharedmem.h"

// init_mem(const int mwidth, const int mheight, int *shmid)
//	grabs access to some shared memory space for holding the shared wall array
//	called by all avatars
//	
unsigned int* init_mem(const int mwidth, const int mheight, int *shmid) {
	unsigned int *attach = 0;

	*shmid = shmget((key_t)KEY, (MDIM*MDIM*sizeof(unsigned int)), 0666 | IPC_CREAT);

	if (*shmid == -1) {
		fprintf(stderr, "ERROR: shmget failed in init_mem()! %s\n", strerror(errno));
		exit(-1);
	}

	attach = shmat(*shmid, (void *)0, 0);
	if (attach == (void *)-1) {
		fprintf(stderr, "ERROR: shmat failed in init_mem()!\n");
		exit(-1);
	}
	
	return attach;
}

// free_mem(char *shared_walls, int *shmid)
//	releases memory held for shared wall array
//	called from avatar 0 after solving maze (or failing)
//	XXX or by all avatars?  what makes sense?
//
int free_mem(unsigned int *shared_walls, int *shmid, unsigned int n_av) {

	if (shmdt((void *)shared_walls) == -1) {
		fprintf(stderr, "ERROR: shmdt failed in free_mem()!\n");
		exit(-1);
	}

	if (0 == n_av) {
		if (shmctl(*shmid, IPC_RMID, 0) == -1) {
			fprintf(stderr, "ERROR: shmctl(IPC_RMID) failed in free_mem()!\n");
			exit(-1);
		}
	}

	return 0;
}

// init_walls(char *shared_walls[][MDIM], char *array)
//	builds local interface to shared memory off of the shmget return pointer
//
int init_walls(unsigned int *shared_walls[][MDIM], unsigned int *array, int mwidth, int mheight) {
	int i, j;

	for (j = 0; j < mheight; j++) {
		for (i = 0; i < mwidth; i++) {
			shared_walls[i][j] = (array + i*sizeof(unsigned int) + j*(mwidth)*sizeof(unsigned int));
			*shared_walls[i][j] = 0;	// XXX
		}
	}

	return 0;
}
