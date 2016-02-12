//
//  MovingAverage.cpp
//  PIFlightController
//
//  Created by Matthew McGlynn on 2/12/16.
//  Copyright (c) 2016 Matthew McGlynn. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include "MovingAverage.h"

MovingAverage::MovingAverage(){
    windowSize = 10;
    double newArray[windowSize];
    array = newArray;
    array[0] = 0;
    array[1] = 0;
    array[2] = 0;
    array[3] = 0;
    array[4] = 0;
    array[5] = 0;
    array[6] = 0;
    array[7] = 0;
    array[8] = 0;
    array[9] = 0;
    counter = 0;
}

MovingAverage::MovingAverage(int newWindowSize){
    windowSize = newWindowSize;
    double newArray[windowSize];
    array = newArray;
    counter = 0;
}

double MovingAverage::getAverage(){
    double total = 0;
    double limit = windowSize;
    if (counter == 0) {
        return 0;
    }
    if (counter<windowSize) {
        limit = counter%windowSize;
    }
    for (int i = 0; i<limit; i++) {
        
        try {
            total += array[i];
            std::cout << array[i];
        } catch (int e){
            return 0;
        }
    }
    return total/limit;
}

double MovingAverage::insert(double value){
    array[counter%windowSize] = value;
    counter++;
    return value;
}
