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


using namespace std;

void* threaded(void * controller);

int main(void)
{
    printf("Welcome to Flight Controller\n");
     
    Control controller;
    controller.setup();
    
    IMU piIMU;
    piIMU.setup();
    pthread_t thread1,thread2,thread3,thread4;
    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i<5000; i++) {
        pthread_create(&thread1, NULL, threaded, &controller);
        pthread_create(&thread2, NULL, threaded, &controller);
        pthread_create(&thread3, NULL, threaded, &controller);
        pthread_create(&thread4, NULL, threaded, &controller);
        while(!piIMU.updateIMU()){
            //i--;
        }
        pthread_join( thread1, NULL);
        pthread_join( thread2, NULL);
        pthread_join( thread3, NULL);
        pthread_join( thread4, NULL);
        //controller.adjustYPMotor(0.2);
        //controller.adjustXPMotor(0.7);
        //controller.adjustYNMotor(0.7);
        //controller.adjustXNMotor(0.7);
        //controller.adjustYPMotor(0.2);
        //controller.adjustXPMotor(0.7);
        //controller.adjustYNMotor(0.7);
        //controller.adjustXNMotor(0.7);
        //printf("hi");
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "f() took "
    << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
    << " milliseconds\n";
   

    
    for (double stepper = 0; stepper<1; stepper+=0.05) {
        printf("Stepper = %f\n",stepper);
        controller.adjustYPMotor(stepper);
        usleep(1000000);
        controller.adjustYPMotor(0);
        usleep(2000000);
    }
    
    return 0;
}

void* threaded(void * controller){
    Control * controller2;
    controller2 = (Control *)controller;
    controller2->adjustYPMotor(0.2);
    controller2->adjustYNMotor(0.2);
    //printf("Hi");
    return NULL;
}


