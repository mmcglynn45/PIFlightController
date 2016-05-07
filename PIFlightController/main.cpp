//
//  main.cpp
//  PIFlightController
//
//  Created by Matthew McGlynn on 10/3/15.
//  Copyright (c) 2015 Matthew McGlynn. All rights reserved.
//



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Control.h"
#include "IMU.h"
#include <ctime>
#include <iostream>
#include <chrono>
#include <pthread.h>
#include "input.h"
#include "Sonar.h"
#include "wiringPi.h"
#include "math.h"
#include "MovingAverage.h"
#include <iostream>
#include <fstream>
#include "image.h"


using namespace std;

void* threaded(void * controller);
void* threaded2(void * controller);
void* sonar(void * sonar);
void* getInputs(void * radio);
void throttleInterrupt();
void pitchInterrupt();
void rollInterrupt();



#define THROTTLE 6
#define PITCH 13
#define ROLL 19

radioInput radio;
int main(void)
{
    
    printf("Welcome to Flight Controller...initializing\n");
    Control controller;
    controller.setup();
    printf("Controller initialized -- Now.\n");
    Image myImage;
    myImage.initialize();
    printf("Camera initialized -- Now.\n");
    radio.setup();
    wiringPiISR(THROTTLE, INT_EDGE_RISING, &throttleInterrupt);
    //wiringPiISR(PITCH, INT_EDGE_RISING, &pitchInterrupt);
    //wiringPiISR(ROLL, INT_EDGE_RISING, &rollInterrupt);
    double totalRoll = 0;
    double totalPitch = 0;
    double xPosDrift = 0;
    double xVelDrift = 0;
    double yPosDrift = 0;
    double yVelDrift = 0;
    auto mXTime = std::chrono::high_resolution_clock::now();
    auto mYTime = std::chrono::high_resolution_clock::now();
    int index = 0;
    IMU piIMU;
    
    printf("PIIMU Setup starting...\n");
    piIMU.setup();
    printf("PIIMU Setup completed...\n");
    piIMU.pitchComp = 0;
    piIMU.rollComp = 0;
    piIMU.mXComp = 0;
    piIMU.mYComp = 0;
    for (int i = 0; i<200; i++) {
        while(!piIMU.updateIMU()){}
    }
    cout << "Pitch = " << piIMU.pitch.getAverage() << endl;
    cout << "Roll = " << piIMU.roll.getAverage() << endl;
    cout << "PitchRate = " << piIMU.pitchRate.getAverage() << endl;
    cout << "RollRate = " << piIMU.rollRate.getAverage() << endl;
    printf("First PIIMU Update\n");
    piIMU.pitchComp = -piIMU.pitch.getAverage();
    piIMU.rollComp = -piIMU.roll.getAverage();
    piIMU.mXComp = -piIMU.mX.getAverage();
    piIMU.mYComp = -piIMU.mY.getAverage();
    for (int i = 0; i<100; i++) {
        while(!piIMU.updateIMU()){}
    }
    controller.pitchIntegration = 0;
    controller.rollIntegration = 0;
    Sonar firstSonar;
    printf("Sonar Setup starting...\n");
    firstSonar.setup();
    printf("Sonar Setup Complete\n");
    long beginning = millis();
    int iterations = 0;
    pthread_t thread1,thread2;
    auto t1 = std::chrono::high_resolution_clock::now();
    int created = 0;
    int inputThreadCreated = 0;
    
    ofstream flightFile;
    flightFile.open ("flightData.csv");
    flightFile << "Time,Pitch,Roll,PitchRate,RollRate,Yaw,MX,MY,ThrottleInput,PitchControl,PitchError,PitchIntegration,RollControl,RollError,RollIntegration,Motor3,Motor12,Motor7,Motor8\n";
    
    
    
    try{
        while (1) {
            auto t2 = std::chrono::high_resolution_clock::now();
            double count = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
            //if (!created) {
            //    pthread_create(&thread1, NULL, sonar, &firstSonar);
            //    created = 1;
            //}
            
            //Radio threading
            //Temp comment out
            /*
             if (!inputThreadCreated) {
             pthread_create(&thread2, NULL, getInputs, &radio);
             inputThreadCreated = 1;
             }
             */
            
            //printf("Sonar Active: %i \n",firstSonar.active);
            while(!piIMU.updateIMU()){}
            index++;
            //cout << "Pitch = " << piIMU.pitch << endl;
            //cout << "Roll = " << piIMU.roll << endl;
            //cout << "PitchRate = " << piIMU.pitchRate << endl;
            //cout << "RollRate = " << piIMU.rollRate << endl;
            //cout << "Yaw = " << piIMU.yaw << endl;
            //cout << "MX = " << piIMU.mX << endl;
            //cout << "MY = " << piIMU.mY << endl;
            myImage.takePicture();
            cout << "COGX = " << myImage.cogX << endl;
            cout << "COGY = " << myImage.cogY << endl;
            
            
            xVelDrift += piIMU.mX.getAverage(); //(t2-mXTime).count())/1000;
            xPosDrift += xVelDrift; //* (t2-mXTime).count())/1000;
            mXTime = t2;
            
            yVelDrift += piIMU.mY.getAverage(); //(t2-mYTime).count())/1000;
            yPosDrift += yVelDrift; //(t2-mYTime).count())/1000;
            mYTime = t2;
            
            //cout << "Total mX drift (meters) = " << xPosDrift << endl;
            //cout << "Total mY drift (meters) = " << yPosDrift << endl;
            //cout << "Total Distance (meters) = " << sqrt(xPosDrift*xPosDrift + yPosDrift*yPosDrift) << endl;
            
            
            //printf("Sonar Reading: %f \n",firstSonar.distance);
            if (!controller.safetyCheck(piIMU.roll.getAverage(), piIMU.pitch.getAverage())) {
                printf("Total amount of iterations in 10 seconds is %i\n", iterations);
                cout << "TotalPitchRate = " << totalPitch/index << endl;
                cout << "TotalRollRate = " << totalRoll/index << endl;
                cout << "Total mX drift (meters) = " << xVelDrift << endl;
                cout << "Total mY drift (meters) = " << yVelDrift << endl;
                return 0;
            }
            totalPitch += fabs(piIMU.pitchRate.getAverage());
            totalRoll += fabs(piIMU.rollRate.getAverage());
            //cout << "TotalPitch = " << totalPitch/index << endl;
            //cout << "TotalRoll = " << totalRoll/index << endl;
            double takeoffSetting = 0;
            if (count<100) {
                takeoffSetting = 1;
            }
            controller.ManageOrientation(piIMU.roll.getAverage(), piIMU.pitch.getAverage(), piIMU.yaw.getAverage(),firstSonar.distance.getAverage(),piIMU.mX.getAverage(),piIMU.mY.getAverage(),piIMU.rollRate.getAverage(),piIMU.pitchRate.getAverage(), radio.throttleMA.getAverage(), radio.roll, radio.pitch, takeoffSetting);
            
            
            iterations++;
            if((int)count%10==0){
                //cout<< "Throttle equals " << radio.throttle << endl;
                //cout<< "Pitch equals " << radio.pitch << endl;
                //cout<< "Roll equals " << radio.roll << endl;
                //cout << "Pitch = " << piIMU.pitch.getAverage() << endl;
                //cout << "Roll = " << piIMU.roll.getAverage() << endl;
                //cout << "PitchRate = " << piIMU.pitchRate.getAverage() << endl;
                //cout << "RollRate = " << piIMU.rollRate.getAverage() << endl;
                //cout << "Yaw = " << piIMU.yaw << endl;
                //cout << "MX = " << piIMU.mX << endl;
                //cout << "MY = " << piIMU.mY << endl;
                //printf("Sonar Reading: %f \n",firstSonar.distance.getAverage());
                flightFile << count << "," << piIMU.pitch.getAverage()<< "," << piIMU.roll.getAverage()<< "," << piIMU.pitchRate.getAverage()<< "," << piIMU.rollRate.getAverage()<< "," << piIMU.yaw.getAverage() << "," << piIMU.mX.getAverage() << "," << piIMU.mY.getAverage() << "," << radio.throttle << "," << controller.extPitchControl << "," <<  controller.pitchError << "," << controller.pitchIntegration << "," <<  controller.extRollControl << "," << controller.rollError << "," << controller.rollIntegration << "," << controller.extMotor3 << "," << controller.extMotor12 << "," << controller.extMotor7 << "," << controller.extMotor8 <<std::endl;
            }
            
            //cout<< count <<endl;
            //firstSonar.getDistance();
            //if (!firstSonar.active){
            //    created = 0;
            //    pthread_join(thread1, NULL);
            //}
            
            //Radio threading
            /*
             if (!radio.active){
             inputThreadCreated = 0;
             pthread_join(thread2, NULL);
             }
             */
            
            double threshold = 30000;
            if (count>threshold) {
                controller.shutdown();
                long end = millis();
                printf("Total amount of iterations in %f seconds is %i\n",threshold/1000, iterations);
                printf("Flight controller cycle rate was %f per second\n",iterations/(threshold/1000));
                cout << "TotalPitchRate = " << totalPitch/index << endl;
                cout << "TotalRollRate = " << totalRoll/index << endl;
                cout << "Total mX drift (meters) = " << xPosDrift << endl;
                cout << "Total mY drift (meters) = " << yPosDrift << endl;
                return 0;
            }
        }
    }catch (int e){
        printf("\n\nShutdown executed\n\n");
        controller.shutdown();
        
        
    }
    
    
    //pthread_t thread1,thread2,thread3,thread4,thread5,thread6,thread7,thread8;
    //auto t1 = std::chrono::high_resolution_clock::now();
    //for (int i = 0; i<400; i++) {
    //pthread_create(&thread1, NULL, threaded, &controller);
    //pthread_create(&thread2, NULL, threaded, &controller);
    //pthread_create(&thread3, NULL, threaded, &controller);
    //pthread_create(&thread4, NULL, threaded, &controller);
    //pthread_create(&thread5, NULL, threaded, &controller);
    //pthread_create(&thread6, NULL, threaded, &controller);
    //pthread_create(&thread7, NULL, threaded, &controller);
    //pthread_create(&thread8, NULL, threaded, &controller);
    //pthread_create(&thread1, NULL, threaded2, &piIMU);
    //while(!piIMU.updateIMU()){
    //i--;
    //}
    //pthread_join( thread1, NULL);
    //pthread_join( thread2, NULL);
    //pthread_join( thread3, NULL);
    //pthread_join( thread4, NULL);
    //pthread_join( thread5, NULL);
    //pthread_join( thread6, NULL);
    //pthread_join( thread7, NULL);
    //pthread_join( thread8, NULL);
    //controller.
    //printf("hi");
    //}
    //auto t2 = std::chrono::high_resolution_clock::now();
    //std::cout << "f() took "
    //<< std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
    //<< " milliseconds\n";
    flightFile.close();
    return 0;

}


void* sonar(void * sonar){
    Sonar * sonar2;
    sonar2 = (Sonar *)sonar;
    sonar2->getDistance();
    return NULL;
}

void* takePicture(void * myImage){
    Image * myImage2;
    myImage2 = (Image *)myImage;
    myImage2->takePicture();
    return NULL;
}


void* getInputs(void * radio){
    radioInput * radio2;
    radio2 = (radioInput *)radio;
    radio2->updateInputs();
    return NULL;
}

void* threaded(void * controller){
    Control * controller2;
    controller2 = (Control *)controller;
    controller2->adjustYPMotor(0.2);
    //controller2->adjustYNMotor(0.2);
    //printf("Hi");
    return NULL;
}

void* threaded2(void * realIMU){
    IMU* IMUpoint;
    IMUpoint = (IMU*)realIMU;
    while(!IMUpoint->updateIMU()){}
    return NULL;
}


void throttleInterrupt(){
    radio.getThrottle();
}

void pitchInterrupt(){
    radio.getPitch();
}


void rollInterrupt(){
    radio.getRoll();
}


