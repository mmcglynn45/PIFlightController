//
//  main.cpp
//  RaspiCam
//
//  Created by Matthew McGlynn on 3/20/16.
//  Copyright (c) 2016 Matthew McGlynn. All rights reserved.
//

#include <ctime>
#include <chrono>
#include <fstream>
#include <iostream>
#if __linux__
#include <raspicam/raspicam.h>
#elif __unix__
#include <raspicam/raspicam.h>
#elif __APPLE__
#include "raspicam.h"
#endif
#include "image.h"

std::ofstream error("error.txt");
std::streambuf *errbuf = std::cerr.rdbuf(error.rdbuf());





#include <unistd.h>

using namespace std;

int main ( int argc,char **argv ) {
    time_t timer_begin,timer_end;
    int nCount=20;
    raspicam::RaspiCam Camera; //Cmaera object
    Camera.setCaptureSize(1280, 960);
    Camera.setFormat(raspicam::RASPICAM_FORMAT_RGB);
    //Open camera
    cout<<"Opening Camera..."<<endl;
    if ( !Camera.open()) {cerr<<"Error opening camera"<<endl;return -1;}
    //wait a while until camera stabilizes
    cout<<"Sleeping for 3 secs"<<endl;
    //capture
    sleep(3);
    cout<<"Capturing "<<nCount<<" frames ...."<<endl;
    time ( &timer_begin );
    size_t imageLength =  Camera.getImageTypeSize (     raspicam::RASPICAM_FORMAT_RGB);
    printf("ImageSize is %zu\n",imageLength);
    auto t1 = std::chrono::high_resolution_clock::now();
    Camera.setHorizontalFlip(1);
    Camera.setRotation(90);

    for ( int i=0; i<nCount; i++ ) {
        unsigned char *data=new unsigned char[  Camera.getImageTypeSize (     raspicam::RASPICAM_FORMAT_RGB )];


        Camera.grab();
        Camera.retrieve (data);
        
        
        Image newImage;
        newImage.setDimensions(Camera.getHeight(), Camera.getWidth());
        newImage.setData(data);
        newImage.threshold(0, 100, 130, 255, 0, 255);
        newImage.calcCenterOfGravity();
        newImage.markTarget(newImage.cogX, newImage.cogY);
        printf("COGX = %f, COGY = %f\n", newImage.cogX, newImage.cogY);
        //newImage.saveImageToFile("testImageClass.ppm");
        delete data;
        
        //if ( i%5==0 )  cout<<"\r captured "<<i<<" images"<<std::flush;
        //printf("Camera spot R at 50,50: %i",data[1]);
        double count;
    
       // double centerHorizontal;
        /*
        for (int i = 0; i<Camera.getWidth(); i++) {
            for (int j = 0; j<Camera.getHeight()*3;j+=3 ) {
                if ((data[i*Camera.getHeight()+j+2]) > 150 && ((data[i*Camera.getHeight()+j+1]) < 100) && ((data[i*Camera.getHeight()+j]) < 100)) {
                    data[i*Camera.getHeight()+j+2] = 255;
                    data[i*Camera.getHeight()+j+1] = 0;
                    data[i*Camera.getHeight()+j] = 0;
                } else{
                    data[i*Camera.getHeight()+j+2] = 0;
                    data[i*Camera.getHeight()+j+1] = 0;
                    data[i*Camera.getHeight()+j] = 0;
                }
                count++;
            }
        }
        */
        /*
        for (int i = 0; i<(Camera.getImageTypeSize (     raspicam::RASPICAM_FORMAT_RGB )); i+=3) {
            if (data[i+2]>130&&data[i]<100) {
                data[i] = 0;
                data[i+1] = 0;
                data[i+2] = 255;
            }else{
                data[i] = 0;
                data[i+1] = 0;
                data[i+2] = 0;
            }
        }
        double COGx = 0;
        double COGy = 0;
        double sumX = 0;
        double sumY = 0;
        double totalX = 0;
        double totalY = 0;
        for (int height = 0; height<Camera.getHeight(); height++) {
            for (int i = 0; i<Camera.getWidth()*3; i+=3) {
                int red = height*Camera.getWidth()*3 + i;
                int green= height*Camera.getWidth()*3 + i+1;
                int blue = height*Camera.getWidth()*3 + i+2;
                double x = i/3;
                double y = height;
                sumX = sumX + x*data[blue];
                totalX = totalX + data[blue];
                sumY = sumY + y*data[blue];
                totalY = totalY + data[blue];
            }
        }
        if (totalX == 0 || totalY == 0) {
            printf("COG not found\n");
            continue;
        }
        
        COGx = sumX/totalX;
        COGy = sumY/totalY;
        
        printf("COGX = %f, COGY = %f\n", COGx, COGy);
        
        int base = (COGy*Camera.getWidth()*3) + COGx*3;
        data[base] = 255;
        data[base-3] = 255;
        data[base+3] = 255;
        data[base-(Camera.getWidth()*3)] = 255;
        data[base+(Camera.getWidth()*3)] = 255;
        
        printf("Count: %f",count);
        */ 
        
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    double secondsElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()/1000.0;
    cout<< secondsElapsed<<" seconds for "<< nCount<<"  frames : FPS = "<<  nCount/secondsElapsed  <<endl;
    cout<<"Stop camera..."<<endl;
    Camera.release();
    //allocate memory
    //save

    //free resrources

    return 0;
}