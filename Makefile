.PHONY: all clean run
VERSION = 0.0.1

PREFIX=/usr/local

LIBS = sdl2 glew
CMACRO = -DVERSION=\"$(VERSION)\"

CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic $(shell pkg-config $(LIBS) --cflags) $(CMACRO)
LDFLAGS = $(shell pkg-config $(LIBS) --libs) -lm

all: zoomer 

zoomer: zoomer.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean: 
	rm -f *.o
	rm -f zoomer 

install: zoomer
	install -Dm755 $< $(DESTDIR)$(PREFIX)/bin/$<

bear: clean
	bear -- make

run: zoomer 
	./$<
