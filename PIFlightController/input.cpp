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
    printf("Made it to setup");
    wiringPiSetupGpio();
    pinMode(THROTTLE, INPUT);
    pinMode(PITCH, INPUT);
    pinMode(ROLL, INPUT);
    wiringPiISR(THROTTLE, INT_EDGE_RISING, &radioInput::throttleInterrupt);
    
    delay(30);
    
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
void radioInput::throttleInterrupt(void){
    getThrottle();
}

void radioInput::pitchInterrupt(void * data){
    getPitch();
}

void radioInput::rollInterrupt(void * data){
    getRoll();
}


double radioInput::getPitch(){
    double timeCandidate = getTime(PITCH);
    if (timeCandidate!=0) {
        pitch = timeCandidate;
        return pitch;
    }else{
        return pitch;
    }
}

double radioInput::getRoll(){
    double timeCandidate = getTime(ROLL);
    if (timeCandidate!=0) {
        roll = timeCandidate;
        return roll;
    }else{
        return roll;
    }
}

double radioInput::getThrottle(){
    double timeCandidate = getTime(THROTTLE);
    if (timeCandidate!=0) {
        throttle = timeCandidate;
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


