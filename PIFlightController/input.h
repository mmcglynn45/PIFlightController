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

class input {
    float throttle;
    
public:
    float getThrottle();
};

#endif /* defined(__PIFlightController__input__) */