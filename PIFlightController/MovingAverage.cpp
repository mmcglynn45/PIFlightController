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



MovingAverage::MovingAverage(int newWindowSize){
    windowSize = newWindowSize;
    double newArray[windowSize];
    array = newArray;
    counter = 0;
}

double MovingAverage::getAverage(){
    std::cout << "array= " << array << "end";
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
    std::cout << "total" << total << "\n";
    std::cout << "limit" << limit << "\n";
    return total/limit;
}

double MovingAverage::insert(double value){
    array[(counter%windowSize)] = value;
    counter++;
    return value;
}
