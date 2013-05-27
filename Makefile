.SUFFIXES: .c .o
CC = gcc
CCFLAGS = -g -Wall
EXEC = rserver rclient1 rclient2 cTest
OBJS =

all: clean rserver

rserver: server.c
	${CC} ${CCFLAGS} sockaddrAL.c sockaddrAL.h server.c -o rserver
	
clean:
	@rm -f ${EXEC} ${OBJS}
