CC = clang
CFLAGS = -O0 -g -Wall
SRCS = ./src/client_slave.c ./src/server.c ./src/sockaddrAL.c
EXECS = ./bin/server ./bin/client
OBJS = $(SRCS:.c=.o)

all: $(SRCS) $(EXECS)

$(EXECS): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	@rm -f $(EXECS) $(OBJS)
