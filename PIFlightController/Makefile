LDFLAGS = -L/usr/local/lib
LDLIBS  = -lwiringPi -lwiringPiDev -lpthread -lm -lwiringPiPca9685

all:
	g++ $(LDFLAGS) $(LDLIBS) main.cpp Control.cpp -o FlightController