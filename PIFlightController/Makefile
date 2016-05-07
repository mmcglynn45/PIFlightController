LDFLAGS = -L/usr/local/lib
LDLIBS  = -lwiringPi -lwiringPiDev -lpthread -lm -lwiringPiPca9685 -lRTIMULib -lraspicam
OTHER	= -std=c++0x -pg

all:
	g++ $(LDFLAGS) $(LDLIBS) $(OTHER) main.cpp Control.cpp MovingAverage.cpp input.cpp IMU.cpp image.cpp Sonar.cpp -o FlightController
