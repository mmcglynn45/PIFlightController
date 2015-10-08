//
//  Control.h
//  PIFlightController
//
//  Created by Matthew McGlynn on 10/3/15.
//  Copyright (c) 2015 Matthew McGlynn. All rights reserved.
//

#ifndef __PIFlightController__Control__
#define __PIFlightController__Control__

#include <stdio.h>
class Control{
    double MaxTime;
    double MinTime;
public:
    Control();
    int calcTicks(float impulseMs, int hertz);
    float map(float input, float min, float max);
    void demo();
    void setup();
    void adjustMotorSpeed(int motor, double speed);
    void adjustYPMotor(double speed);
    void adjustYNMotor(double speed);
    void adjustXPMotor(double speed);
    void adjustXNMotor(double speed);
};


#endif /* defined(__PIFlightController__Control__) */