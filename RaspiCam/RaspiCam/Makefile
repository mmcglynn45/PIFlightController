
LDFLAGS = -L/usr/local/lib
LDLIBS  = -I/usr/local/include -lraspicam -lmmal -lmmal_core -lmmal_util
OTHER   = -std=c++0x -pg

all:
	g++ $(LDFLAGS) $(LDLIBS) $(OTHER) main.cpp  -o RASPCAM