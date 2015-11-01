//
//  Control.cpp
//  PIFlightController
//
//  Created by Matthew McGlynn on 10/3/15.
//  Copyright (c) 2015 Matthew McGlynn. All rights reserved.
//

#include "Control.h"
#include "pca9685.h"
#include "wiringPi.h"

#define PIN_BASE 300
#define MAX_PWM 4096
#define HERTZ 50
#define YPMOTOR 0
#define YNMOTOR 1
#define XPMOTOR 2
#define XNMOTOR 3

int h = 0;
    
    
Control::Control(){
    MinTime = 1.2;
    MaxTime = 1.6;
}
    
    
void Control::setup(){
    int fd = pca9685Setup(PIN_BASE, 0x40, HERTZ);
    if (fd < 0)
    {
        printf("Error in setup\n");
    }
    
    // Reset all output
    pca9685PWMReset(fd);
}

void Control::demo(){
    float millis = 1.0;
    int tick = calcTicks(millis, HERTZ);
    while(1){
        pwmWrite(PIN_BASE + 16, tick);
        delay(2000);
        millis=1.6;
        tick = calcTicks(millis, HERTZ);
        pwmWrite(PIN_BASE + 16, tick);
        delay(2000);
        millis=1.0;
        tick = calcTicks(millis, HERTZ);
        pwmWrite(PIN_BASE + 16, tick);
        delay(2000);
    }
}

void Control::adjustMotorSpeed(int motor, double speed){
    double milliseconds = speed*(MaxTime-MinTime)+MinTime;
    double tick = calcTicks(milliseconds, HERTZ);
    pwmWrite(PIN_BASE + motor, tick);
}

void Control::adjustYPMotor(double speed){
    adjustMotorSpeed(YPMOTOR, speed);
}

void Control::adjustYNMotor(double speed){
    adjustMotorSpeed(YNMOTOR, speed);
}

void Control::adjustXPMotor(double speed){
    adjustMotorSpeed(XPMOTOR, speed);
}

void Control::adjustXNMotor(double speed){
    adjustMotorSpeed(XNMOTOR, speed);
}

/**
 * Calculate the number of ticks the signal should be high for the required amount of time
 */
int Control::calcTicks(float impulseMs, int hertz)
{
    float cycleMs = 1000.0f / hertz;
    int output = (int)(MAX_PWM * impulseMs / cycleMs + 0.5f);
    return output;
}

/**
 * input is [0..1]
 * output is [min..max]
 */
float Control::map(float input, float min, float max)
{
    return (input * max) + (1 - input) * min;
}



