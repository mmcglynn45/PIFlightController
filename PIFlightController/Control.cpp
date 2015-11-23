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
#include <iostream>


#define PIN_BASE 300
#define MAX_PWM 4096
#define HERTZ 50
#define YPMOTOR 3 //Pitch minus - Roll positive
#define YNMOTOR 7 //Pitch minus - Roll minus
#define XNMOTOR 8 //Pitch plus - Roll minus
#define XPMOTOR 12 //Pitch plus - Roll Positive

#define	PI					3.1415926535
#define	DEGREE_TO_RAD		(RTMATH_PI / 180.0)
#define	RAD_TO_DEGREE		(180.0 / RTMATH_PI)
    
    
Control::Control(){
    MinTime = 1.2;
    MaxTime = 1.3;
    pitchError =  0;
    pitchIntegration = 0;
    pitchTime = std::chrono::high_resolution_clock::now();

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


void Control::safetyCheck(double roll,double pitch){
    if ((abs(roll)>15) || (abs(pitch)>15)) {
        adjustYPMotor(0);
        adjustXPMotor(0);
        adjustYNMotor(0);
        adjustXNMotor(0);
        delay(2000);
    }
}

void Control::shutdown(){
    adjustYPMotor(0);
    adjustXPMotor(0);
    adjustYNMotor(0);
    adjustXNMotor(0);
}

void Control::demo(){
    float millis = 1.0;
    int tick = calcTicks(millis, HERTZ);
    while(1){
        pwmWrite(PIN_BASE + 16, tick);
        delay(2000);
        millis=1.6;
        tick = calcTicks(millis, HERTZ);
        pwmWrite(PIN_BASE + 16, tick);         delay(2000);
        millis=1.0;
        tick = calcTicks(millis, HERTZ);
        pwmWrite(PIN_BASE + 16, tick);
        delay(2000);
    }
}

double Control::getThrottleBaseline(){
    double input = 2;
    input = inputNormalizer(input, 0, 4);
    return input;
}

void Control::adjustMotorSpeed(int motor, double speed){
    double milliseconds = speed*(MaxTime-MinTime)+MinTime;
    milliseconds = inputNormalizer(milliseconds, MinTime, MaxTime);
    double tick = calcTicks(milliseconds, HERTZ);
    pwmWrite(PIN_BASE + motor, tick);
}

void Control::ManageOrientation(double roll, double pitch, double yaw){
    double pitchControl = PitchPIDComputation(pitch, 0);
    std::cout<<"Pitch Control: "<<pitchControl<<std::endl;
    double rollControl = PitchPIDComputation(pitch, 0);
    std::cout<<"Roll Control: "<<rollControl<<std::endl;
    MapMotorOutput(pitchControl, rollControl, 0, getThrottleBaseline());
}

double Control::PitchPIDComputation(double current, double desired){
    double Kp = 0.01;
    double Ki = 0.0001;
    double Kd = 0.005;
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>> (now-pitchTime);
    double deltaT = time_span.count();
    pitchTime = now;
    double error = desired - current;
    pitchIntegration = pitchIntegration + error*deltaT;
    double deriviative = (error - pitchError)/deltaT;
    double output = Kp * error + Ki *pitchIntegration + Kd * deriviative;
    pitchError = error;
    return output;
}

double Control::RollPIDComputation(double current, double desired){
    double Kp = 0.01;
    double Ki = 0.0001;
    double Kd = 0.005;
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>> (now-rollTime);
    double deltaT = time_span.count();
    rollTime = now;
    double error = desired - current;
    rollIntegration = rollIntegration + error*deltaT;
    double deriviative = (error - rollError)/deltaT;
    double output = Kp * error + Ki * rollIntegration + Kd * deriviative;
    rollError = error;
    return output;
}


void Control::MapMotorOutput(double pitchControl,double rollControl, double yawControl, double throttleBaseline){
    //Basic algo is establish baseline then use roll, pitch, and yaw to modify
    pitchControl = inputNormalizer(pitchControl,-1,1);
    rollControl = inputNormalizer(rollControl,-1,1);
    yawControl = inputNormalizer(yawControl,-1,1);
    double pitchP = shiftNormalized(pitchControl);
    double rollP = shiftNormalized(pitchControl);
    double rollN = 1 - rollP;
    double pitchN = 1 - pitchP;
    double XPSpeed = throttleBaseline * pitchP * rollP;
    double XNSpeed = throttleBaseline * pitchP * rollN;
    double YPSpeed = throttleBaseline * pitchN * rollN;
    double YNSpeed = throttleBaseline * pitchP * rollN;
    XPSpeed = inputNormalizer(XPSpeed, 0, 1);
    XNSpeed = inputNormalizer(XNSpeed, 0, 1);
    YPSpeed = inputNormalizer(YPSpeed, 0, 1);
    YNSpeed = inputNormalizer(XNSpeed, 0, 1);
    if(rand()%1 == 0){
        std::cout<<XPMOTOR << " : "<< XPSpeed <<std::endl;
        std::cout<<XNMOTOR << " : "<< XNSpeed <<std::endl;
        std::cout<<YPMOTOR << " : "<< YPSpeed <<std::endl;
        std::cout<<YNMOTOR << " : "<< YNSpeed <<std::endl;
        std::cout.flush();
    }
}

double Control::inputNormalizer(double input, double min, double max){
    if (input<min) {
        input= min;
    }
    if (input>max) {
        input = max;
    }
    return input;
}


double Control::shiftNormalized(double input){
    input = (input+1)/2;
    return input;
}

void Control::adjustYPMotor(double speed){
    speed = inputNormalizer(speed, 0, 1);
    adjustMotorSpeed(YPMOTOR, speed);
}

void Control::adjustYNMotor(double speed){
    speed = inputNormalizer(speed, 0, 1);
    adjustMotorSpeed(YNMOTOR, speed);
}

void Control::adjustXPMotor(double speed){
    speed = inputNormalizer(speed, 0, 1);
    adjustMotorSpeed(XPMOTOR, speed);
}

void Control::adjustXNMotor(double speed){
    speed = inputNormalizer(speed, 0, 1);
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



