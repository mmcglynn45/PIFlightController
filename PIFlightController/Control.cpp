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
#include <math.h>

#define PIN_BASE 300
#define MAX_PWM 4096
#define HERTZ 50

//Mixed layout
//#define YPMOTOR 3 //Pitch minus - Roll positive - Yaw Plus - mY neg - mx pos
//#define YNMOTOR 7 //Pitch minus - Roll minus - Yaw Minus - mY pos - mx pos
//#define XNMOTOR 8 //Pitch plus - Roll minus - Yaw Plus - mY pos - mx neg
//#define XPMOTOR 12 //Pitch plus - Roll Positive - Yaw Minus  - mY neg - mx neg

#define YPMOTOR 3 //Pitch positive
#define YNMOTOR 7 //Roll Positive
#define XNMOTOR 8 //Pitch negative
#define XPMOTOR 12 //Roll negative

#define	PI					3.1415926535
#define	DEGREE_TO_RAD		(RTMATH_PI / 180.0)
#define	RAD_TO_DEGREE		(180.0 / RTMATH_PI)
    
    
Control::Control(){
    MinTime = 1.2;
    MaxTime = 2.0;
    pitchError =  0;
    pitchIntegration = 0;
    rollError =  0;
    rollIntegration = 0;
    pitchRateError =  0;
    pitchRateIntegration = 0;
    rollRateError =  0;
    rollRateIntegration = 0;
    yawError =  0;
    yawIntegration = 0;
    altitudeError = 0;
    altitudeIntegration = 13;
    mXError = 0;
    mXIntegration = 0;
    mYError = 0;
    mYIntegration = 0;
    pitchTime = std::chrono::high_resolution_clock::now();
    rollTime = std::chrono::high_resolution_clock::now();
    yawTime = std::chrono::high_resolution_clock::now();
    altitudeTime = std::chrono::high_resolution_clock::now();
    mxTime = std::chrono::high_resolution_clock::now();
    myTime = std::chrono::high_resolution_clock::now();
    pitchRateTime = std::chrono::high_resolution_clock::now();
    rollRateTime = std::chrono::high_resolution_clock::now();
    
    rollC.setup(5);
    pitchC.setup(5);

}
    
    
void Control::setup(){
    int fd = pca9685Setup(PIN_BASE, 0x40, HERTZ);
    if (fd < 0)
    {
        printf("Error in setup\n");
    }
    
    // Reset all output
    pca9685PWMReset(fd);
    adjustYPMotor(0);
    adjustXPMotor(0);
    adjustYNMotor(0);
    adjustXNMotor(0);
    delay(1000);
    std::cout<< "Ready for test..."<<std::endl;
}


int Control::safetyCheck(double roll,double pitch){
    if ((fabs(roll)>12) || (fabs(pitch)>12)) {
        adjustYPMotor(0);
        adjustXPMotor(0);
        adjustYNMotor(0);
        adjustXNMotor(0);
        delay(2000);
        printf("safetyExit roll: %f pitch: %f \n", roll, pitch);
        return 0;
    }
    return 1;
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

void Control::mapCameraInputs(double * cogX, double * cogY){
    double angle = 45;
    double oldCogX = *cogX-120;
    double oldCogY = *cogY-160;
    *cogX = oldCogX * cos(3.1415926535/180 * angle) + oldCogY * sin(3.1415926535/180 * angle);
    *cogY = oldCogX * -sin(3.1415926535/180 * angle) + oldCogY * cos(3.1415926535/180 * angle);
}


double Control::getThrottleBaseline(){
    double input = 4;
    input = inputNormalizer(input, 0, 4);
    return input;
}

void Control::adjustMotorSpeed(int motor, double speed){
    double milliseconds = speed*(MaxTime-MinTime)+MinTime;
    milliseconds = inputNormalizer(milliseconds, MinTime, MaxTime);
    //printf("Milliseconds: %f", milliseconds);
    double tick = calcTicks(milliseconds, HERTZ);
    pwmWrite(PIN_BASE + motor, tick);
}

void Control::ManageOrientation(double roll, double pitch, double yaw, double altitude, double mX, double mY, double rollRate, double pitchRate, double throttleInput, double rollInput, double pitchInput, double takeoffsetting, double cogX, double cogY){
    mapCameraInputs(&cogX, &cogY);
    printf("COGX = %f, COGY = %f\n", cogX, cogY);
    //double desiredPitch = (pitchInput - 0.5)*2;
    //double desiredRoll = (rollInput - 0.5)*2;
    double desiredPitch = mXPIDComputation(mX, 0);
    double desiredRoll = mYPIDComputation(mY, 0);
    
    desiredPitch = inputNormalizer(-desiredPitch, -0, 0);
    desiredRoll = inputNormalizer(-desiredRoll, -0, 0);
    
    
    pitchC.insert(PitchPIDComputation(pitch, 0));
    //double pitchControl = pitchC.getAverage();
    double desiredPitchRate = PitchPIDComputation(pitch, 0);
    double pitchControl = pitchRatePIDComputation(pitchRate, desiredPitchRate);
    //std::cout<<"Pitch Control: "<<pitchControl<<std::endl;
    
    rollC.insert(RollPIDComputation(roll, 0));
    //double rollControl = rollC.getAverage();
    double desiredRollRate = RollPIDComputation(roll, 0);
    double rollControl = rollRatePIDComputation(rollRate, desiredRollRate);
    
    
    //std::cout<<"Roll Control: "<<rollControl<<std::endl;
    double yawControl = YawPIDComputation(yaw, 60);
    //std::cout<<"Yaw Control: "<<yawControl<<std::endl;
    double altitudeControl = AltitudePIDComputation(altitude, 9);

    double mixedThrottle = 0*altitudeControl + 1*throttleInput;
    if (throttleInput<.15) {
        mixedThrottle = 0;
    }
    //printf("altitudeControl: %f \n", altitudeControl);
    if (takeoffsetting) {
        pitchControl = 0.5;
        rollControl = 0.5;
    }
    MapMotorOutput(pitchControl, rollControl, yawControl, mixedThrottle);
}

double Control::mXPIDComputation(double current, double desired){
    //printf("current: %f \n", current);
    //printf("desired: %f \n", desired);
    double Kp = 4.0;
    double Ki = 1.0;
    double Kd = 0.02;
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>> (now-mxTime);
    double deltaT = time_span.count();
    mxTime = now;
    double error = desired - current;
    mXIntegration = mXIntegration + error*deltaT;
    double deriviative = (error - mXError)/deltaT;
    double output = Kp * error + Ki * mXIntegration + Kd * deriviative;
    mXError = error;
    return output;
}


double Control::mYPIDComputation(double current, double desired){
    //printf("current: %f \n", current);
    //printf("desired: %f \n", desired);
    double Kp = 4.0;
    double Ki = 1.0;
    double Kd = 0.02;
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>> (now-myTime);
    double deltaT = time_span.count();
    myTime = now;
    double error = desired - current;
    mYIntegration = mYIntegration + error*deltaT;
    double deriviative = (error - mYError)/deltaT;
    double output = Kp * error + Ki * mYIntegration + Kd * deriviative;
    mYError = error;
    return output;
}
double Control::AltitudePIDComputation(double current, double desired){
    //printf("current: %f \n", current);
    //printf("desired: %f \n", desired);
    double Kp = 0.1;
    double Ki = 0.07;
    double Kd = 0.002;
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>> (now-altitudeTime);
    double deltaT = time_span.count();
    altitudeTime = now;
    double error = desired - current;
    altitudeIntegration = altitudeIntegration + error*deltaT;
    double deriviative = (error - altitudeError)/deltaT;
    double output = Kp * error + Ki * altitudeIntegration + Kd * deriviative;
    altitudeError = error;
    return output;
}


double Control::pitchRatePIDComputation(double current, double desired){
    double Kp = 0.5;
    double Ki = 0.05;
    double Kd = 0.10;
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>> (now-pitchRateTime);
    double deltaT = time_span.count();
    pitchRateTime = now;
    double error = desired - current;
    pitchRateIntegration = pitchRateIntegration + error*deltaT;
    double deriviative = (error - pitchRateError)/deltaT;
    double output = Kp * error + Ki *pitchRateIntegration + Kd * deriviative;
    pitchRateError = error;
    return output;
}

double Control::rollRatePIDComputation(double current, double desired){
    double Kp = 0.5;
    double Ki = 0.05;
    double Kd = 0.10;
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>> (now-rollRateTime);
    double deltaT = time_span.count();
    rollRateTime = now;
    double error = desired - current;
    rollRateIntegration = rollRateIntegration + error*deltaT;
    double deriviative = (error - rollRateError)/deltaT;
    double output = Kp * error + Ki * rollRateIntegration + Kd * deriviative;
    rollRateError = error;
    return output;
}





double Control::PitchPIDComputation(double current, double desired){
    double Kp = 0.35;
    double Ki = 0.03;
    double Kd = 0.05;
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
    double Kp = 0.35;
    double Ki = 0.03;
    double Kd = 0.05;
    double rollIntMax = 10;
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>> (now-rollTime);
    double deltaT = time_span.count();
    rollTime = now;
    double error = desired - current;
    rollIntegration = rollIntegration + error*deltaT;
    if (rollIntegration>rollIntMax) {
        rollIntegration = rollIntMax;
    }else if (rollIntegration<-rollIntMax){
        rollIntegration = -rollIntMax;
    }
    double deriviative = (error - rollError)/deltaT;
    double output = Kp * error + Ki * rollIntegration + Kd * deriviative;
    rollError = error;
    return output;
}

double Control::YawPIDComputation(double current, double desired){
    double Kp = 0.02;
    double Ki = 0.01;
    double Kd = 0.005;
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>> (now-yawTime);
    double deltaT = time_span.count();
    yawTime = now;
    double error = desired - current;
    yawIntegration = yawIntegration + error*deltaT;
    double deriviative = (error - yawError)/deltaT;
    double output = Kp * error + Ki * yawIntegration + Kd * deriviative;
    yawError = error;
    return output;
}


void Control::MapMotorOutput(double pitchControl,double rollControl, double yawControl, double throttleBaseline){
    //Basic algo is establish baseline then use roll, pitch, and yaw to modify
    throttleBaseline = throttleBaseline * 2; //Scale up for scale down of .5 across 3 inputs
    throttleBaseline = inputNormalizer(throttleBaseline, 0, 1.5);
    pitchControl = pitchControl/2;
    rollControl = rollControl/5;
    pitchControl = inputNormalizer(pitchControl,-.50,.50);
    rollControl = inputNormalizer(rollControl,-.90,.90);
    extRollControl = rollControl;
    extPitchControl = pitchControl;
    
    yawControl = inputNormalizer(yawControl,-.0,.0);//Roll and Pitch are weighted at 100%, yaw is less important weight at 10%
    double pitchP = shiftNormalized(pitchControl);
    double rollP = shiftNormalized(rollControl);
    double yawP = shiftNormalized(yawControl);
    double rollN = 1 - rollP;
    double pitchN = 1 - pitchP;
    double yawN = 1 - yawP;
    //std::cout<<"PitchP: " << pitchP << std::endl;
    //std::cout<<"RollP: " << rollP << std::endl;
    
    //Old mapping
    //double XPSpeed = throttleBaseline * pitchP * rollP *yawP;
    //double XNSpeed = throttleBaseline * pitchP * rollN *yawN;
    //double YPSpeed = throttleBaseline * pitchN * rollP *yawN;
    //double YNSpeed = throttleBaseline * pitchN * rollN *yawP;
    
    //new mapping
    double XPSpeed = throttleBaseline * rollN;
    double XNSpeed = throttleBaseline * pitchN;
    double YPSpeed = throttleBaseline * pitchP;
    double YNSpeed = throttleBaseline * rollP;
    XPSpeed = inputNormalizer(XPSpeed, 0, 1);
    XNSpeed = inputNormalizer(XNSpeed, 0, 1);
    YPSpeed = inputNormalizer(YPSpeed, 0, 1);
    YNSpeed = inputNormalizer(YNSpeed, 0, 1);
    
    extMotor12 = XPSpeed;
    extMotor3 = YPSpeed;
    extMotor7 = YNSpeed;
    extMotor8 = XNSpeed;
    
    if(rand()%1000 == 0){
        //std::cout<<XPMOTOR << " : "<< XPSpeed <<std::endl;
        //std::cout<<XNMOTOR << " : "<< XNSpeed <<std::endl;
        //std::cout<<YPMOTOR << " : "<< YPSpeed <<std::endl;
        //std::cout<<YNMOTOR << " : "<< YNSpeed <<std::endl;
        //std::cout.flush();
    }
    adjustYPMotor(YPSpeed);
    adjustYNMotor(YNSpeed);
    adjustXPMotor(XPSpeed);
    adjustXNMotor(XNSpeed);
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



