CC = clang
CFLAGS = -O0 -g -Wall
SRCS = ./src/server.c ./src/client_slave.c ./src/sockaddrAL.c
EXECS = ./bin/server
OBJS = $(SRCS:.c=.o)

all: $(SRCS) $(EXECS)

$(EXECS): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -f $(EXECS) $(OBJS)
