#ifndef _MESSAGING_H_
#define _MESSAGING_H_

void sendInitialize(int numAvatars, int difficulty, int sockfd);

void sendMove(int id, int direction, int sockfd);

void sendReady(int id, int sockfd);

#endif
