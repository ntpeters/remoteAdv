#include "sockaddrAL.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct sockaddr_in sArray[512];
int size;

int getSize(){
     return size;
}

// Adds Info to the array
void addInfo( struct sockaddr_in info ){
     bcopy( &info , &sArray[ size++ ] , sizeof( struct sockaddr_in ) );
}

// Unsorted, so n time to delete
void deleteInfo( struct sockaddr_in info ){
     int i = 0;
     for( i = 0; i < size ; size++ )
          if( !memcmp( &info.sin_addr , &sArray[i].sin_addr , sizeof(struct in_addr) ) )
              while( i+1 <= size )  // Delete by shifting up
                   bcopy( &sArray[i+1] , &sArray[i] , sizeof( struct sockaddr_in ) );
     size--;
}

void printInfo(){
     //TODO format output





}
