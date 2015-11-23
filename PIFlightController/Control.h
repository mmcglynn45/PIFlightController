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
#include <chrono>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

class Control{
    double MaxTime;
    double MinTime;
    double pitchError,pitchIntegration,pitchOutput;
    double rollError,rollIntegration,rollOutput;
    std::chrono::high_resolution_clock::time_point pitchTime;
    std::chrono::high_resolution_clock::time_point rollTime;
    
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
    double getThrottleBaseline();
    double RollPIDComputation(double current, double desired);
    double PitchPIDComputation(double current, double desired);
    double YawPIDComputation(double current, double desired);
    double inputNormalizer(double input, double min, double max);
    void MapMotorOutput(double pitchControl,double rollControl, double yawControl, double throttleBaseline);
    void ManageOrientation(double roll, double pitch, double yaw);
    double shiftNormalized(double input);
    void safetyCheck(double roll,double pitch);
    void shutdown();
};


#endif /* defined(__PIFlightController__Control__) */
