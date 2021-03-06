//
//  image.h
//  RaspiCam
//
//  Created by Matthew McGlynn on 3/23/16.
//  Copyright (c) 2016 Matthew McGlynn. All rights reserved.
//

#ifndef __RaspiCam__image__
#define __RaspiCam__image__

#include <stdio.h>
#if __linux__
#include <raspicam/raspicam.h>
#elif __unix__
#include <raspicam/raspicam.h>
#elif __APPLE__
#include "raspicam.h"
#endif
class Point {
    
public:
    int x;
    int y;
    int red;
    int blue;
    int green;
};

class Image {
    int height = 0;
    int width = 0;
    unsigned char * data;
    
    
public:
    int active = 0;
    double cogX = 0;
    double cogY = 0;
    raspicam::RaspiCam Camera; 
    void setDimensions(int h, int w);
    void setData(unsigned char * newData);
    int threshold(int redLower, int redUpper, int blueLower, int blueUpper,int greenLower, int greenUpper); //returns count of remaining pixels
    Point getPoint(int x, int y);
    void saveImageToFile(char * filename);
    void markTarget(int x, int y);
    void savePoint(Point p);
    void calcCenterOfGravity();
    void fastThresholdCOG(int redLower, int redUpper, int blueLower, int blueUpper,int greenLower, int greenUpper);
    int initialize();
    void takePicture();
    
};




#endif /* defined(__RaspiCam__image__) */
