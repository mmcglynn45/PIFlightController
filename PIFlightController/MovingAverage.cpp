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



void MovingAverage::setup(int size){
    windowSize = size;
    array = new double[size];
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
        std::cout << array[i];
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
    array[(counter%windowSize)] = value;
    counter++;
    return value;
}
