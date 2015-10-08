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

using namespace std;

int main(void)
{
    printf("Welcome to Flight Controller\n");
     
    Control controller;
    controller.setup();
    
    IMU piIMU;
    piIMU.setup();
    
    std::clock_t    start;
    
    start = std::clock();
    for (int i = 0; i<10000; i++) {
        if(!piIMU.updateIMU()){
            i--;
        }
    }
    cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;
    
    

    
    for (double stepper = 0; stepper<1; stepper+=0.05) {
        printf("Stepper = %f\n",stepper);
        controller.adjustYPMotor(stepper);
        usleep(1000000);
        controller.adjustYPMotor(0);
        usleep(2000000);
    }
    
    return 0;
}


