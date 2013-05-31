CC = clang
CFLAGS = -O0 -g -Wall
EXECS = ./bin/server ./bin/client_slave

all: $(EXECS)

.PHONY: server
server: ./bin/server

.PHONY: client_slave
client_slave: ./bin/client_slave

./bin/%: ./src/%.o ./src/sockaddrAL.o
			mkdir -p ./bin/
	        $(CC) $(CFLAGS) -o $@ $^

.c.o:
	        $(CC) $(CFLAGS) -c $< -o $@

clean:
	        @rm -f $(EXECS) ./src/*.o 
