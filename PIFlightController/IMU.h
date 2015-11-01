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

#define	PI					3.1415926535
#define	DEGREE_TO_RAD		(RTMATH_PI / 180.0)
#define	RAD_TO_DEGREE		(180.0 / RTMATH_PI)

class IMU{
    RTIMU *imu;
    
public:
    int updateIMU();
    int readIMU();
    void setup();
    double pitch;
    double roll;
    double yaw;
    void rotation(double yawShift, double rollInRad, double pitchInRad, double yawInRad)
};


#endif /* defined(__PIFlightController__IMU__) */
