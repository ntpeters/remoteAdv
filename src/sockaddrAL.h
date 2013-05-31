// Header of Array with functions for sockaddr_in
#ifndef SOCKADDRAL
#define SOCKADDRAL

typedef struct sockaddr_in sockaddr_in;

int getSize();
void addInfo( sockaddr_in info );
void deleteInfo( struct sockaddr_in info );
void printInfo();

#endif
