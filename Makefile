CC = gcc 
CFLAGS = -g -Wall
SRCS = ./src/server.c ./src/client_slave.c ./src/sockaddrAL.c
EXECS = ./bin/server ./bin/client_slave
OBJS = $(SRCS:.c=.o)

all: clean $(SRCS) server client

server: $(OBJS)
	$(CC) $(CFLAGS) ./src/server.o -o ./bin/server

client: $(OBJS)
	$(CC) $(CFLAGS) ./src/client_slave.o -o ./bin/client_slave

# $(EXECS): $(OBJS)
# 	$(CC) $(CFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(EXECS) $(OBJS)
