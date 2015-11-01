//
//  IMU.h
//  PIFlightController
//
//  Created by Matthew McGlynn on 10/7/15.
//  Copyright (c) 2015 Matthew McGlynn. All rights reserved.
//

#ifndef __PIFlightController__IMU__
#define __PIFlightController__IMU__

#include "RTIMULib/RTIMULib.h"
#include <stdio.h>
class IMU{
    RTIMU *imu;
    
public:
    int updateIMU();
    int readIMU();
    void setup();
    double pitch;
    double roll;
    double yaw;
};


#endif /* defined(__PIFlightController__IMU__) */
