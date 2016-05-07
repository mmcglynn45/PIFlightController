//
//  image.cpp
//  RaspiCam
//
//  Created by Matthew McGlynn on 3/23/16.
//  Copyright (c) 2016 Matthew McGlynn. All rights reserved.
//

#include <fstream>
#include <iostream>
#include <math.h>
#include <ctime>
#include <chrono>
#include <fstream>
#include <iostream>
#include <unistd.h>
#if __linux__
#include <raspicam/raspicam.h>
#elif __unix__
#include <raspicam/raspicam.h>
#elif __APPLE__
#include "raspicam.h"
#endif
#include "image.h"

void Image::setDimensions(int h, int w){
    height = h;
    width = w;
}

Point Image::getPoint(int x, int y){
    Point p;
    if ((x < width || x > 0) && (y < height || y > 0)){
        p.x = x;
        p.y = y;
        p.red = data[y * width * 3 + x * 3];
        p.green = data[y * width * 3 + x * 3 + 1];
        p.blue = data[y * width * 3 + x * 3 + 2];
    }
    return p;
}

int Image::threshold(int redLower, int redUpper, int blueLower, int blueUpper,int greenLower, int greenUpper){
    int count =0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Point p = getPoint(j, i);
            if ((p.red < redLower)||(p.red > redUpper)||(p.green < greenLower)||(p.green > greenUpper)||(p.blue > blueUpper)||(p.blue < blueLower)) {
                p.red = 0;
                p.blue = 0;
                p.green = 0;
                
            }else{
                p.green = 255;
                p.blue  = 255;
                p.red = 255;
                count++;
            }
            savePoint(p);
            }
    }
    return count;
}

void Image::markTarget(int x, int y){
    if (x<5||x>height-7) {
        return;
    }
    if (y<5||y>height-7) {
        return;
    }
    for (int i = x - 5; i< x + 6; i++) {
        Point p = getPoint(i, y);
        p.red = 255;
        p.green = 0;
        p.blue = 0;
        savePoint(p);
    }
    for (int i = y - 5; i< y + 6; i++) {
        Point p = getPoint(x, i);
        p.red = 255;
        p.green = 0;
        p.blue = 0;
        savePoint(p);
    }
}
void Image::calcCenterOfGravity(){
    double sumX = 0;
    double sumY = 0;
    double totalX = 0;
    double totalY = 0;
    for (int x = 0; x<width; x++) {
        for (int y = 0; y<height; y++) {
            Point p = getPoint(x, y);
            int intensity = p.blue + p.green + p.red;
            sumX = sumX + intensity * x;
            totalX = totalX + intensity;
            sumY = sumY + intensity * y;
            totalY = totalY + intensity;
        }
    }
    cogY = sumY/totalY;
    cogX = sumX/totalX;
}

void Image::savePoint(Point p){
    data[p.y * width * 3 + p.x * 3] = p.red;
    data[p.y * width * 3 + p.x * 3 + 1] = p.green;
    data[p.y * width * 3 + p.x * 3 + 2] = p.blue;
}

void Image::setData(unsigned char *newData){
    data = newData;
}

void Image::saveImageToFile(char *filename){
    std::ofstream outFile ( filename,std::ios::binary);
    outFile<<"P6\n"<<width <<" "<<height <<" 255\n";
    outFile.write ( ( char* ) data, width*height*3);
    std::cout<<"Image saved at "<<filename<<std::endl;
}

void Image::fastThresholdCOG(int redLower, int redUpper, int blueLower, int blueUpper,int greenLower, int greenUpper){
    double boxSize = width/10;
    double xTop = fmin(cogX + boxSize,width);
    double xBottom = fmax(cogX - boxSize,0);
    double yTop = fmin(cogY + boxSize,height);
    double yBottom = fmax(cogY - boxSize,0);
    //printf("xTOP : %f, xBottom: %f, yTop: %f, yBottom: %f\n",xTop,xBottom,yTop,yBottom);
    double sumX = 0;
    double sumY = 0;
    double totalX = 0;
    double totalY = 0;
    int count =0;
    for (int y = yBottom; y < yTop; y++) {
        for (int x = xBottom; x < xTop; x++) {
            Point p = getPoint(x, y);
            if ((p.red < redLower)||(p.red > redUpper)||(p.green < greenLower)||(p.green > greenUpper)||(p.blue > blueUpper)||(p.blue < blueLower)) {
                p.red = 0;
                p.blue = 0;
                p.green = 0;
                
            }else{
                p.green = 255;
                p.blue  = 255;
                p.red = 255;
                int intensity = p.blue + p.green + p.red;
                sumX = sumX + intensity * x;
                totalX = totalX + intensity;
                sumY = sumY + intensity * y;
                totalY = totalY + intensity;
                count++;
            }
            savePoint(p);
        }
    }
    
    if (count>20) {
        cogY = sumY/totalY;
        cogX = sumX/totalX;
    }else{
        printf("Failed to find in box\n");
        threshold(redLower, redUpper, blueLower, blueUpper, greenLower, greenUpper);
        calcCenterOfGravity();
    }


}


int Image::initialize(){
    using namespace std;
    Camera.setCaptureSize(320, 240);
    Camera.setFormat(raspicam::RASPICAM_FORMAT_RGB);
    //Open camera
    std::cout<<"Opening Camera..."<<endl;
    if ( !Camera.open()) {cerr<<"Error opening camera"<<endl;return -1;}
    //wait a while until camera stabilizes
    std::cout<<"Sleeping for 1 secs"<<endl;
    //capture
    sleep(1);
    Camera.setHorizontalFlip(1);
    Camera.setRotation(90);
    Camera.grab();
    setDimensions(Camera.getHeight(), Camera.getWidth());
    return 0;
}

void Image::takePicture(){
    active = 1;
    unsigned char *data=new unsigned char[  Camera.getImageTypeSize (     raspicam::RASPICAM_FORMAT_RGB )];
    Camera.grab();
    Camera.retrieve (data);
    setData(data);
    //newImage.threshold(0, 100, 130, 255, 0, 255);
    //newImage.calcCenterOfGravity();
    //saveImageToFile("myImage.ppm");
    //newImage.fastThresholdCOG(0, 100, 130, 255, 0, 255);
    fastThresholdCOG(0, 40, 0, 255, 0, 255);
    markTarget(cogX, cogY);
    printf("COGX = %f, COGY = %f\n", cogX-width/2, cogY-height/2);
    //saveImageToFile("testImageClass.ppm");
    delete data;
    active = 0;
}