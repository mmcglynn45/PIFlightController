LDFLAGS = -L/usr/local/lib
LDLIBS  = -lwiringPi -lwiringPiDev -lpthread -lm -lwiringPiPca9685 -lRTIMULib

all:
	g++ $(LDFLAGS) $(LDLIBS) main.cpp Control.cpp IMU.cpp -o FlightController
