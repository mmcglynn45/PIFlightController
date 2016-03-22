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


std::ofstream error("error.txt");
std::streambuf *errbuf = std::cerr.rdbuf(error.rdbuf());





#include <unistd.h>

using namespace std;

int main ( int argc,char **argv ) {
    time_t timer_begin,timer_end;
    int nCount=1;
    raspicam::RaspiCam Camera; //Cmaera object
    Camera.setCaptureSize(640, 480);
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

    unsigned char *data=new unsigned char[  Camera.getImageTypeSize (     raspicam::RASPICAM_FORMAT_RGB )];
    for ( int i=0; i<nCount; i++ ) {
        Camera.grab();
        Camera.setHorizontalFlip(1);
        Camera.setRotation(90);
        Camera.retrieve (data);
        //if ( i%5==0 )  cout<<"\r captured "<<i<<" images"<<std::flush;
        //printf("Camera spot R at 50,50: %i",data[1]);
        double count;
    
       // double centerHorizontal;
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
        
        printf("Count: %f",count);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    double secondsElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()/1000.0;
    cout<< secondsElapsed<<" seconds for "<< nCount<<"  frames : FPS = "<<  nCount/secondsElapsed  <<endl;
    cout<<"Stop camera..."<<endl;
    Camera.release();
    //allocate memory
    //save
    std::ofstream outFile ( "raspicam_image.ppm",std::ios::binary);
    outFile<<"P6\n"<<Camera.getWidth() <<" "<<Camera.getHeight() <<" 255\n";
    outFile.write ( ( char* ) data, Camera.getImageTypeSize ( raspicam::RASPICAM_FORMAT_RGB) );
    FILE * pFile;
    pFile = fopen ("newImage.ppm","w");
    fprintf(pFile, "P4\n");
    fprintf(pFile, "%i %i 255\n",Camera.getWidth(),Camera.getHeight());
    for (int i = 0; i<Camera.getHeight(); i++) {
        for (int j =0 ; j<Camera.getWidth()*3; j++) {
            fprintf(pFile, "%i ", data[i*Camera.getWidth()+j]);
        }
        fprintf(pFile,"\n");
    }
    fclose (pFile);
    cout<<"Image saved at raspicam_image.ppm"<<endl;
    //free resrources
    delete data;
    return 0;
}