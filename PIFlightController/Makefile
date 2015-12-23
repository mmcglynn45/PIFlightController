LDFLAGS = -L/usr/local/lib
LDLIBS  = -lwiringPi -lwiringPiDev -lpthread -lm -lwiringPiPca9685 -lRTIMULib
OTHER	= -std=c++0x

all:
	g++ $(LDFLAGS) $(LDLIBS) $(OTHER) main.cpp Control.cpp input.cpp IMU.cpp Sonar.cpp -o FlightController 
