#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "Sonar.h"

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
}

double Sonar::getCM() {
    long startTime = micros();
    //Send trig pulse
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(20);
    digitalWrite(TRIG, LOW);
    
    //Wait for echo start
    while(digitalRead(ECHO) == LOW){
        if ((micros()-startTime)>100000) {
            return distance;
        }
    }
    
    //Wait for echo end
    startTime = micros();
    while(digitalRead(ECHO) == HIGH){
        if ((micros()-startTime)>100000) {
            return distance;
        }
    }
    long travelTime = micros() - startTime;
    
    //Get distance in cm
    distance = travelTime / 58.0;
    
    return distance;
}

int Sonar::demo(void) {
    //printf("Made it");
    
    //printf("Distance: %f cm\n", getCM());
    
    return 0;
}