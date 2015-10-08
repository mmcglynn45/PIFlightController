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




int main(void)
{
    printf("Welcome to Flight Controller\n");
     
    Control controller;
    controller.setup();
    
    readIMU();
    
    for (double stepper = 0; stepper<1; stepper+=0.05) {
        printf("Stepper = %f\n",stepper);
        controller.adjustYPMotor(stepper);
        usleep(1000000);
        controller.adjustYPMotor(0);
        usleep(2000000);
    }
    
    return 0;
}


