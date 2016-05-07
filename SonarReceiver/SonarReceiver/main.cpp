//
//  main.cpp
//  SonarReceiver
//
//  Created by Matthew McGlynn on 3/19/16.
//  Copyright (c) 2016 Matthew McGlynn. All rights reserved.
//

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include "wiringPi.h"
#include "MovingAverage.h"
#include <math.h>
#include <iostream>

#define TRUE 1

#define TRIG 20
#define ECHO 21

MovingAverage distance;
int active;
void setup();
double getDistance();
int demo();

int main(){
    printf("HELLO SONAR\n");
    setup();
    int i = 0;
    while (1) {
        i++;
        delay(3);
        double dist = getDistance();
        if (i%300==0) {
            printf("Sonar Distance = %f\n", dist/2.54);
        }
    }

}


void setup() {
    //printf("Made it to setup");
    distance.setup(10);
    wiringPiSetupGpio();
    
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    
    //TRIG pin must start LOW
    digitalWrite(TRIG, LOW);
    delay(30);
    distance.insert(4);
}

double getCM() {
    double lastReading = distance.getAverage();
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
            return distance.getAverage();
        }
    }
    
    //Wait for echo end
    startTime = micros();
    while(digitalRead(ECHO) == HIGH){
        if ((micros()-startTime)>10000) { //maximum of 160cm
            active = 0;
            return distance.getAverage();
        }
    }
    long travelTime = micros() - startTime;
    
    //Get distance in cm
    double newDistance = travelTime / 58.0;
    double delta = fabs(newDistance-lastReading);
    if (fabs(newDistance-distance.getAverage())<1) {
        distance.insert(newDistance);
    }else{
        distance.insert(newDistance*.1+distance.getAverage()*.9);
    }
    active = 0;
    return distance.getAverage();
}

double getDistance(){
    return getCM();
}
