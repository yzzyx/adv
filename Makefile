CFLAGS=-I/usr/include/python2.7 -g -pg `sdl2-config --cflags` -Wall
CXXFLAGS=-I/usr/include/python2.7
LDFLAGS=-L/usr/lib/python2.7/config-x86_64-linux-gnu -pg
LIBS=-lpython2.7 -lSDL2_image `sdl2-config --libs`

all: adv

adv: python.o main.o sdl.o animation.o map.o player.o astar.o
	$(CC) -o adv $^ $(LDFLAGS) $(LIBS)

clean:
	rm -f *.o adv tiles/*.pyc maps/*.pyc
