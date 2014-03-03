CFLAGS=-I/usr/include/python2.7 -g -pg `sdl2-config --cflags` -Wall
CXXFLAGS=-I/usr/include/python2.7
LDFLAGS=-L/usr/lib/python2.7/config-x86_64-linux-gnu -pg
LIBS=-lpython2.7 -lSDL2_image `sdl2-config --libs`
DEPS=$(patsubst %.c, %.d, $(SRCS))
OBJS=python.o main.o sdl.o animation.o map.o player.o astar.o gamestate.o

.PHONY: all clean

%.o: %.c
	@echo "$(CC) $<"
	@$(CC) -c -o $@ $< $(CFLAGS)

%.d: %.c
	@echo DEPS $<
	@$(SHELL) -ec '$(CC) $(CFLAGS) -MM $< | \
		sed -e '"'"'s|$*\.o: |\$*\.o $@: |g'"'"' >$@'

all: adv

adv: $(OBJS)
	$(CC) -o adv $^ $(LDFLAGS) $(LIBS)

clean:
	-rm -f *.o adv tiles/*.pyc maps/*.pyc

dependencies: $(DEPS)

deps: dependencies
-include $(OBJS:%.o=%.d)
