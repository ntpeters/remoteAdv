CC = clang
CFLAGS = -O0 -g -Wall
EXECS = ./bin/server ./bin/client_slave

all: $(EXECS)

./bin/%: ./src/%.o ./src/sockaddrAL.o
	        $(CC) $(CFLAGS) -o $@ $^

.c.o:
	        $(CC) $(CFLAGS) -c $< -o $@

clean:
	        @rm -f $(EXECS) $(OBJS)
