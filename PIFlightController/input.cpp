#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "Sonar.h"
#include <math.h>
#include <iostream>
#include "input.h"

#define TRUE 1


#define ECHO 6

void radioInput::setup() {
    printf("Made it to setup");
    wiringPiSetupGpio();
    pinMode(ECHO, INPUT);
    delay(30);
    
    throttle = 0;
}

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

