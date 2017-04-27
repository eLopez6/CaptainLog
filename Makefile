CC=gcc
LD=gcc
CFLAGS= -Wall -Werror -g
LDFLAGS=$(CFLAGS)

TARGETS=proj4 proj4d

all: $(TARGETS)

proj4: Project4Client.o
	$(CC) $(CFLAGS) -o $@ $<

proj4d: Project4Server.o
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o
