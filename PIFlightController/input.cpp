#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "Sonar.h"
#include <math.h>
#include <iostream>
#include "input.h"

#define TRUE 1


#define ECHO 6
#define THROTTLE 6
#define PITCH 13
#define ROLL 19

void radioInput::setup() {
    wiringPiSetupGpio();
    pinMode(THROTTLE, INPUT);
    pinMode(PITCH, INPUT);
    pinMode(ROLL, INPUT);
    delay(30);
    throttleAdjustment = 1000;
    pitchAdjustment = 1000;
    rollAdjustment = 1000;
    for (int i = 0; i < 10; i++) {
        getThrottle();
        getRoll();
        getPitch();
    }
    getThrottle();
    getRoll();
    getPitch();
    std::cout<< "Throttle started at " << throttle << std::endl;
    std::cout<< "Pitch started at " << pitch << std::endl;
    std::cout<< "Roll started at" << roll << std::endl;
    throttleAdjustment = 1000 + (throttle * 1000);
    pitchAdjustment = 1000 + ((pitch-0.5)* 1000);
    rollAdjustment = 1000 + ((roll-0.5)* 1000);
    getThrottle();
    getRoll();
    getPitch();
    std::cout<< "Throttle now equals " << throttle << std::endl;
    std::cout<< "Pitch now equals " << pitch << std::endl;
    std::cout<< "Roll now equals " << roll << std::endl;
    throttle = 0;
}

/*
double radioInput::getThrottle() {
    active = 1;
    long startTime = micros();
    //Send trig pulse
    
    //Wait for echo start
    while(digitalRead(ECHO) == LOW){
        delayMicroseconds(5);
        if ((micros()-startTime)>100000) { //maximum of 160cm
            active = 0;
            return throttle;
        }
    }
    
    //Wait for echo end
    startTime = micros();
    while(digitalRead(ECHO) == HIGH){
        if ((micros()-startTime)>100000) { //maximum of 160cm
            active = 0;
            return throttle;
        }
    }
    long travelTime = micros() - startTime;
    throttle = travelTime;
    active = 0;
    return throttle;
}
 */


double radioInput::getPitch(){
    double timeCandidate = getTime(PITCH);
    if (timeCandidate!=0) {
        pitch = timeCandidate;
        pitch = (pitch - pitchAdjustment)/1000; //Normalize transmitter values
        return pitch;
    }else{
        return pitch;
    }
}

double radioInput::getRoll(){
    double timeCandidate = getTime(ROLL);
    if (timeCandidate!=0) {
        roll = timeCandidate;
        roll = (roll - rollAdjustment)/1000; //Normalize transmitter values
        return roll;
    }else{
        return roll;
    }
}

double radioInput::getThrottle(){
    double timeCandidate = getTime(THROTTLE);
    if (timeCandidate!=0) {
        throttle = timeCandidate;
        throttle = (throttle - throttleAdjustment)/1000; //Normalize transmitter values
        return throttle;
    }else{
        return throttle;
    }
}

void radioInput::updateInputs(){
    active = 1;
    //getRoll();
    //getPitch();
    //getThrottle();
    active = 0;
}

double radioInput::getTime(int pin){
    double time = 0;
    long startTime = micros();
    //Send trig pulse
    
    //Wait for echo start
    while(digitalRead(pin) == LOW){
        delayMicroseconds(5);
        if ((micros()-startTime)>100000) { //maximum of 160cm
            return time;
        }
    }
    
    //Wait for echo end
    startTime = micros();
    while(digitalRead(pin) == HIGH){
        if ((micros()-startTime)>100000) { //maximum of 160cm
            return time;
        }
    }
    long travelTime = micros() - startTime;
    time = travelTime;
    return time;

}

double radioInput::quickTime(int pin){
    double time = 0;
    long startTime = micros();
    while(digitalRead(pin) == HIGH){
        if ((micros()-startTime)>100000) { //maximum of 160cm
            return time;
        }
    }
    long travelTime = micros() - startTime;
    time = travelTime;
    return time;
    
}


