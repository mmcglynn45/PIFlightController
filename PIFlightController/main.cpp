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





int main(void)
{
    printf("Welcome to Flight Controller\n");
     
    Control controller;
    printf("Loop1\n");
    controller.setup();
    
    
    printf("Loop\n");
    for (double stepper = 0; stepper<1; stepper+=0.05) {
        printf("Stepper = %f\n",stepper);
        controller.adjustYPMotor(stepper);
        usleep(100000);
        controller.adjustYPMotor(0);
        usleep(500000);
    }
    
    return 0;
}


