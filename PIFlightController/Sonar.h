//
//  Sonar.h
//  PIFlightController
//
//  Created by Matthew McGlynn on 12/18/15.
//  Copyright (c) 2015 Matthew McGlynn. All rights reserved.
//

#ifndef __PIFlightController__Sonar__
#define __PIFlightController__Sonar__

#include <stdio.h>
#include "MovingAverage.h"

class Sonar {
   
    double getCM();

public:
    MovingAverage distance;
    int active;
    void setup();
    double getDistance();
    int demo();
};

#endif /* defined(__PIFlightController__Sonar__) */
