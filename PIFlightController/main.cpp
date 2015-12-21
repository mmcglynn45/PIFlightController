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

using namespace std;

void* threaded(void * controller);
void* threaded2(void * controller);
void* sonar(void * sonar);

int main(void)
{
    printf("Welcome to Flight Controller\n");
     
    Control controller;
    controller.setup();
    
    input radio;
    double totalRoll = 0;
    double totalPitch = 0;
    double xPosDrift = 0;
    double xVelDrift = 0;
    double yPosDrift = 0;
    double yVelDrift = 0;
    double pitchComp = 1.81;
    double rollComp = -.4;
    auto mXTime = std::chrono::high_resolution_clock::now();
    auto mYTime = std::chrono::high_resolution_clock::now();
    int index = 0;
    IMU piIMU;
    piIMU.setup();
    piIMU.pitchComp = pitchComp;
    piIMU.rollComp = rollComp;
    Sonar firstSonar;
    firstSonar.setup();
    
    long beginning = millis();
    int iterations = 0;
    pthread_t thread1;
    auto t1 = std::chrono::high_resolution_clock::now();
    int created = 0;
    while (1) {
        auto t2 = std::chrono::high_resolution_clock::now();
        if (!created) {
            pthread_create(&thread1, NULL, sonar, &firstSonar);
            created = 1;
        }
        //printf("Sonar Active: %i \n",firstSonar.active);
        while(!piIMU.updateIMU()){}
        index++;
        cout << "Pitch = " << piIMU.pitch << endl;
        cout << "Roll = " << piIMU.roll << endl;
        cout << "Yaw = " << piIMU.yaw << endl;
        cout << "MX = " << piIMU.mX << endl;
        cout << "MY = " << piIMU.mY << endl;
        
        xVelDrift += piIMU.mX; //(t2-mXTime).count())/1000;
        xPosDrift += xVelDrift; //* (t2-mXTime).count())/1000;
        mXTime = t2;
        
        yVelDrift += piIMU.mY; //(t2-mYTime).count())/1000;
        yPosDrift += yVelDrift; //(t2-mYTime).count())/1000;
        mYTime = t2;
        
        //cout << "Total mX drift (meters) = " << xPosDrift << endl;
        //cout << "Total mY drift (meters) = " << yPosDrift << endl;
        //cout << "Total Distance (meters) = " << sqrt(xPosDrift*xPosDrift + yPosDrift*yPosDrift) << endl;
    
        
        //printf("Sonar Reading: %f \n",firstSonar.distance);
        if (!controller.safetyCheck(piIMU.roll, piIMU.pitch)) {
            return 0;
        }
        totalPitch += fabs(piIMU.pitch);
        totalRoll += fabs(piIMU.roll);
        //cout << "TotalPitch = " << totalPitch/index << endl;
        //cout << "TotalRoll = " << totalRoll/index << endl;
        controller.ManageOrientation(piIMU.roll, piIMU.pitch, piIMU.yaw,firstSonar.distance,piIMU.mX,piIMU.mY);
        
        double count = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        iterations++;
        
        //cout<< count <<endl;
        //firstSonar.getDistance();
        if (!firstSonar.active){
            created = 0;
            pthread_join( thread1, NULL);
        }
        if (count>3000) {
            controller.shutdown();
            long end = millis();
            printf("Total amount of iterations in 10 seconds is %i", iterations);
            return 0;
        }
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
   

    
    
    return 0;
}


void* sonar(void * sonar){
    Sonar * sonar2;
    sonar2 = (Sonar *)sonar;
    sonar2->getDistance();
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

