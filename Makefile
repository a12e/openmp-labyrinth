CC=mpicc
CFLAGS = -O2 -g -Wall
LDLIBS = -L /usr/X11R6/lib -lX11

ALL=chemin_lab gen_lab gen_lab_demo

all: $(ALL)

$(ALL): graph.o

clean:
	/bin/rm -f $(ALL) *.o laby.lab
