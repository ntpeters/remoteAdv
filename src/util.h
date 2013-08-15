#ifndef UTIL
#define UTIL

void writeLog( int loglvl, char* str, ... );
char* getDateString();
void setDebugLevel( int level );
void setLogFile( char* file );
void setSilentMode( int silent );

#endif