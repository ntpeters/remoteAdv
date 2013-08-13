int dbgLevel;
char* logFile;

void writeLog( int loglvl, char* str, ... );
char* getDateString();
void setDebugLevel( int level );
void setLogFile( char* file );