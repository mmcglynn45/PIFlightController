//
//  MovingAverage.h
//  PIFlightController
//
//  Created by Matthew McGlynn on 2/12/16.
//  Copyright (c) 2016 Matthew McGlynn. All rights reserved.
//

#ifndef __PIFlightController__MovingAverage__
#define __PIFlightController__MovingAverage__

#include <stdio.h>
#include <math.h>

class MovingAverage {
    
    double * array;
    int windowSize;
    int counter;
    
public:
    MovingAverage();
    MovingAverage(int newWindowSize);
    double getAverage();
    double insert(double value);
    
};

#endif /* defined(__PIFlightController__MovingAverage__) */
