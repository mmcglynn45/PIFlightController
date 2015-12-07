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

using namespace std;

void* threaded(void * controller);
void* threaded2(void * controller);

int main(void)
{
    printf("Welcome to Flight Controller\n");
     
    Control controller;
    controller.setup();
    
    input radio;
    
    IMU piIMU;
    piIMU.setup();
    
    auto t1 = std::chrono::high_resolution_clock::now();
    
    while (1) {
        while(!piIMU.updateIMU()){}
        cout << "Pitch = " << piIMU.pitch << endl;
        cout << "Roll = " << piIMU.roll << endl;
        cout << "Yaw = " << piIMU.yaw << endl;
        controller.safetyCheck(piIMU.roll, piIMU.pitch);
        controller.ManageOrientation(piIMU.roll, piIMU.pitch, piIMU.yaw);
        auto t2 = std::chrono::high_resolution_clock::now();
        double count = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        cout<< count <<endl;
        if (count>2000) {
            controller.shutdown();
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

