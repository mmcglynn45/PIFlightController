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


class Sonar {
    double distance;
    double getCM();

public:
    int active;
    void setup();
    double getDistance();
    int demo();
};

#endif /* defined(__PIFlightController__Sonar__) */
