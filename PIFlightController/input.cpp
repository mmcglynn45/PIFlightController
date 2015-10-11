//
//  input.cpp
//  PIFlightController
//
//  Created by Matthew McGlynn on 10/11/15.
//  Copyright (c) 2015 Matthew McGlynn. All rights reserved.
//

#include "input.h"
#include "wiringPi.h"

float input::getThrottle(){
    waitForInterrupt(4, 1000);
    int start = millis();
    while (digitalRead(4)) {
    }
    int now = millis();
    printf("Time returned was %i\r",(now-start));
    fflush(stdout);
    return (float)(now-start);
}