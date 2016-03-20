//
//  main.cpp
//  RaspiCam
//
//  Created by Matthew McGlynn on 3/20/16.
//  Copyright (c) 2016 Matthew McGlynn. All rights reserved.
//

#include <ctime>
#include <fstream>
#include <iostream>
#if __linux__
#include <raspicam/raspicam.h>
#elif __unix__
#include <raspicam/raspicam.h>
#elif __APPLE__
#include "raspicam.h"
#endif


#include <unistd.h>
using namespace std;

int main ( int argc,char **argv ) {
    time_t timer_begin,timer_end;
    int nCount=100;
    raspicam::RaspiCam Camera; //Cmaera object
    Camera.setCaptureSize(100, 100);
    //Open camera
    cout<<"Opening Camera..."<<endl;
    if ( !Camera.open()) {cerr<<"Error opening camera"<<endl;return -1;}
    //wait a while until camera stabilizes
    cout<<"Sleeping for 3 secs"<<endl;
    //capture
    sleep(3);
    cout<<"Capturing "<<nCount<<" frames ...."<<endl;
    time ( &timer_begin );
    size_t imageLength =  Camera.getImageTypeSize (     raspicam::RASPICAM_FORMAT_GRAY );
    printf("ImageSize is %zu",imageLength);
    unsigned char *data=new unsigned char[  Camera.getImageTypeSize (     raspicam::RASPICAM_FORMAT_GRAY )];
    for ( int i=0; i<nCount; i++ ) {
        Camera.grab();
        Camera.retrieve ( data,    raspicam::RASPICAM_FORMAT_GRAY );
        if ( i%5==0 )  cout<<"\r captured "<<i<<" images"<<std::flush;
        printf("Camera spot R at 50,50: %i",data[1]);

    }
    time ( &timer_end );
    double secondsElapsed = difftime ( timer_end,timer_begin )/1000;
    cout<< secondsElapsed<<" seconds for "<< nCount<<"  frames : FPS = "<<  ( float ) ( ( float ) ( nCount ) /secondsElapsed ) <<endl;
    cout<<"Stop camera..."<<endl;
    Camera.release();
    //allocate memory
    //save
    std::ofstream outFile ( "raspicam_image.ppm",std::ios::binary );
    outFile<<"P6\n"<<Camera.getWidth() <<" "<<Camera.getHeight() <<" 255\n";
    outFile.write ( ( char* ) data, Camera.getImageTypeSize ( raspicam::RASPICAM_FORMAT_GRAY) );
    cout<<"Image saved at raspicam_image.ppm"<<endl;
    //free resrources
    delete data;
    return 0;
}