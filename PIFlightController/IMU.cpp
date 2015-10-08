#include "IMU.h"
#include "math.h"

inline double to_degrees(double radians) {
    return radians * (180.0 / M_PI);
}

void IMU::setup(){
    RTIMUSettings *settings = new RTIMUSettings("RTIMULib");
    
    imu = RTIMU::createIMU(settings);
    
    if ((imu == NULL) || (imu->IMUType() == RTIMU_TYPE_NULL)) {
        printf("No IMU found\n");
        exit(1);
    }
    
    //  This is an opportunity to manually override any settings before the call IMUInit
    imu->IMUInit();
    
    //  this is a convenient place to change fusion parameters
    
    imu->setSlerpPower(0.02);
    imu->setGyroEnable(true);
    imu->setAccelEnable(true);
    imu->setCompassEnable(true);
}

int IMU::updateIMU(){
    int sampleCount = 0;
    int sampleRate = 0;
    if (imu->IMURead()) {
        RTIMU_DATA imuData = imu->getIMUData();
        sampleCount++;
        roll = to_degrees(imuData.fusionPose.data(0));
        //printf("Test one piece: Roll = %f\n",to_degrees(imuData.fusionPose.data(0)));
        //printf("Sample rate %d: %s\r", sampleRate, RTMath::displayDegrees("", imuData.fusionPose));
        return 1;
    }else{
        return 0;
    }

}


int IMU::readIMU()
{
    int sampleCount = 0;
    int sampleRate = 0;
    uint64_t rateTimer;
    uint64_t displayTimer;
    uint64_t now;
    
    //  Using RTIMULib here allows it to use the .ini file generated by RTIMULib
    //Demo.
    //  Or, you can create the .ini in some other directory by using:
    //      RTIMUSettings *settings = new RTIMUSettings("<directory path>", "RTIMULib");
    //  where <directory path> is the path to where the .ini file is to be loaded/saved
    
    RTIMUSettings *settings = new RTIMUSettings("RTIMULib");
    
    RTIMU *imu = RTIMU::createIMU(settings);
    
    if ((imu == NULL) || (imu->IMUType() == RTIMU_TYPE_NULL)) {
        printf("No IMU found\n");
        exit(1);
    }
    
    //  This is an opportunity to manually override any settings before the call IMUInit
    imu->IMUInit();
    
    //  this is a convenient place to change fusion parameters
    
    imu->setSlerpPower(0.02);
    imu->setGyroEnable(true);
    imu->setAccelEnable(true);
    imu->setCompassEnable(true);
    
    //  set up for rate timer
    
    rateTimer = displayTimer = RTMath::currentUSecsSinceEpoch();
    
    //  now just process data
    
    while (1) {
        //  poll at the rate recommended by the IMU
        
        usleep(imu->IMUGetPollInterval() * 1000);
        
        while (imu->IMURead()) {
            RTIMU_DATA imuData = imu->getIMUData();
            sampleCount++;
            
            
            
            now = RTMath::currentUSecsSinceEpoch();
            
            //  display 10 times per second
            
            if ((now - displayTimer) > 100000) {
                printf("Sample rate %d: %s\r", sampleRate, RTMath::displayDegrees("", imuData.fusionPose));
                fflush(stdout);
                displayTimer = now;
            }
            
            //  update rate every second
            
            if ((now - rateTimer) > 1000000) {
                sampleRate = sampleCount;
                sampleCount = 0;
                rateTimer = now;
            }
        }
    }
}