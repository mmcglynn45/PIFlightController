#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "Sonar.h"
#include <math.h>
#include <iostream>

#define TRUE 1

#define TRIG 12
#define ECHO 5

void Sonar::setup() {
    //printf("Made it to setup");
    wiringPiSetupGpio();
    
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    
    //TRIG pin must start LOW
    digitalWrite(TRIG, LOW);
    delay(30);
    distance = 4;
}

double Sonar::getCM() {
    double lastReading = distance;
    active = 1;
    long startTime = micros();
    //Send trig pulse
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(20);
    digitalWrite(TRIG, LOW);
    
    //Wait for echo start
    while(digitalRead(ECHO) == LOW){
        if ((micros()-startTime)>10000) { //maximum of 160cm
            active = 0;
            return distance;
        }
    }
    
    //Wait for echo end
    startTime = micros();
    while(digitalRead(ECHO) == HIGH){
        if ((micros()-startTime)>10000) { //maximum of 160cm
            active = 0;
            return distance;
        }
    }
    long travelTime = micros() - startTime;
    
    //Get distance in cm
    double newDistance = travelTime / 58.0;
    double delta = fabs(newDistance-lastReading);
    if (delta<1) {
        distance = newDistance;
    }else{
        distance = distance*.8 + newDistance*.2; //Otherwise, weight between the old and new readings
    }
    active = 0;
    return distance;
}

double Sonar::getDistance(){
    return getCM();
}

int Sonar::demo(void) {
    //printf("Made it");
    
    //printf("Distance: %f cm\n", getCM());
    
    return 0;
}