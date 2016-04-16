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
#include "MovingAverage.h"
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
    
    double pitchAdjustment;
    double rollAdjustment;
    double throttleAdjustment;
    MovingAverage throttleMA;
    
    
};

#endif /* defined(__PIFlightController__input__) */
