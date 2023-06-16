all: alien

CC=gcc
SDL_CFLAGS = `sdl2-config --cflags`
SDL_LIBS = `sdl2-config --libs`
CFLAGS=-ggdb -DENABLE_DEBUG -O2 -Wall $(SDL_CFLAGS)
OBJS=\
	client.o common.o vm.o      sprites.o decode.o   animation.o   \
	rooms.o  render.o main.o    music.o   debug.o    lzss.o        \
	sound.o  screen.o scale2x.o scale3x.o game2bin.o cd_iso.o

LIBS=$(SDL_LIBS) -lSDL2_mixer

alien: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm $(OBJS)
