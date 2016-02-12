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

#include "MovingAverage.h"
class MovingAverage {
    
    double * array;
    int windowSize;
    int counter;
public:
    MovingAverage(int newWindowSize){
        windowSize = newWindowSize;
        double newArray[windowSize];
        array = newArray;
        counter = 0;
    }
    double getAverage(){
        double total = 0;
        double limit = windowSize;
        if (counter<windowSize) {
            limit = counter;
        }
        for (int i = 0; i<limit; i++) {
            try {
                total += array[i];
            } catch () {
                return total/i;
            }
        }
        return total/limit;
    }
    double insert(double value){
        array[counter%windowSize] = value;
    }
    
};

#endif /* defined(__PIFlightController__MovingAverage__) */
