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
#include <Math.h>

#include "MovingAverage.h"
class MovingAverage {
    
    double * array;
    int windowSize;
    int counter;
public:
    MovingAverage(int newWindowSize){
        windowSize = newWindowSize;
        array = double[windowSize];
        counter = 0;
    }
    double getAverage(){
        double total = 0;

        for (i = 0; i<windowSize; i++) {
            try {
                total += array[i];
            } catch () {
                return total/i;
            }
        }
        return total/windowSize;
    }
    double insert(double value){
        array[counter]
    }
    
};

#endif /* defined(__PIFlightController__MovingAverage__) */
