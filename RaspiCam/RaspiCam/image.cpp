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

void Image::setDimensions(int h, int w){
    height = h;
    width = w;
}

Point Image::getPoint(int x, int y){
    Point p;
    if ((x < width || x > 0) && (y < height || y > 0)){
        p.x = x;
        p.y = y;
        p.red = y * width * 3 + x * 3;
        p.green = y * width * 3 + x * 3 + 1;
        p.blue = y * width * 3 + x * 3 + 2;
    }
    return p;
}

int Image::threshold(int redLower, int redUpper, int blueLower, int blueUpper,int greenLower, int greenUpper){
    int count =0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Point p = getPoint(i, j);
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
        }
    }
    return count;
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