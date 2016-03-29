
LDFLAGS = -L/usr/local/lib
LDLIBS  = -I/usr/local/include -lraspicam 
OTHER   = -std=c++0x -pg

all:
	g++ $(LDFLAGS) $(LDLIBS) $(OTHER) main.cpp image.cpp  -o RASPCAM