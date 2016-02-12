//
//  MovingAverage.cpp
//  PIFlightController
//
//  Created by Matthew McGlynn on 2/12/16.
//  Copyright (c) 2016 Matthew McGlynn. All rights reserved.
//

#include <stdio.h>
#include "MovingAverage.h"


MovingAverage::MovingAverage(int newWindowSize){
    windowSize = newWindowSize;
    double newArray[windowSize];
    array = newArray;
    counter = 0;
}

double MovingAverage::getAverage(){
    double total = 0;
    double limit = windowSize;
    if (counter<windowSize) {
        limit = counter;
    }
    for (int i = 0; i<limit; i++) {
        try {
            total += array[i];
        } catch (int e){
            return total/i;
        }
    }
    return total/limit;
}

double MovingAverage::insert(double value){
    array[counter%windowSize] = value;
    return value;
}
