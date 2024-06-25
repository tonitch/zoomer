.PHONY: all clean run bear build
VERSION = 0.0.2

PREFIX=/usr/local

LIBS = sdl2 glew
CMACRO = -DVERSION=\"$(VERSION)\" $(if $(EMBED_SHADER),-DEMBED_SHADER,)

CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic $(shell pkg-config $(LIBS) --cflags) $(CMACRO)
LDFLAGS = $(shell pkg-config $(LIBS) --libs) -lm

all: zoomer

zoomer: $(if $(EMBED_SHADER),fshader.h vshader.h,) zoomer.o 
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.h: %.glsl
	xxd -i $< $@

clean: 
	rm -f fshader.h vshader.h
	rm -f *.o
	rm -f zoomer 

install: build
	install -Dm755 zoomer $(DESTDIR)$(PREFIX)/bin/zoomer

bear: clean
	bear -- make

build: 
	EMBED_SHADER=y $(MAKE) zoomer

run: zoomer
	./$<
