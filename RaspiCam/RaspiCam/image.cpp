//
//  image.cpp
//  RaspiCam
//
//  Created by Matthew McGlynn on 3/23/16.
//  Copyright (c) 2016 Matthew McGlynn. All rights reserved.
//

#include "image.h"
#include <fstream>
#include <iostream>
#include <algorithm>

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
    double boxSize = 200;

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

}