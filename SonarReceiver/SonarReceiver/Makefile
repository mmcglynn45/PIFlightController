
LDFLAGS = -L/usr/local/lib
LDLIBS  = -lwiringPi 
OTHER   = -std=c++0x -pg

all:
	g++ $(LDFLAGS) $(LDLIBS) $(OTHER) main.cpp MovingAverage.cpp  -o Sonar
	g++ $(LDFLAGS) $(LDLIBS) $(OTHER) server.cpp -o Server
	g++ $(LDFLAGS) $(LDLIBS) $(OTHER) client.cpp -o Client
