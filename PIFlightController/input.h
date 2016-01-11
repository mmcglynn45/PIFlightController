//
//  input.h
//  PIFlightController
//
//  Created by Matthew McGlynn on 10/11/15.
//  Copyright (c) 2015 Matthew McGlynn. All rights reserved.
//

#ifndef __PIFlightController__input__
#define __PIFlightController__input__

#include <stdio.h>

class radioInput {
   
    
    
public:
    bool active;
    double getThrottle();
    double getRoll();
    double getPitch();
    void setup();
    double throttle;
    double roll;
    double pitch;
    double getTime(int input);
    void updateInputs();
    double quickTime(int pin);
    void throttleInterrupt(void);
    void pitchInterrupt(void * data);
    void rollInterrupt(void * data);
    
    
};

#endif /* defined(__PIFlightController__input__) */
