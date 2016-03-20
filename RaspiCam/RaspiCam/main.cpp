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
#include "raspicam.h"
#include <unistd.h>
using namespace std;

int main ( int argc,char **argv ) {
    raspicam::RaspiCam Camera; //Cmaera object
    //Open camera
    cout<<"Opening Camera..."<<endl;
    if ( !Camera.open()) {cerr<<"Error opening camera"<<endl;return -1;}
    //wait a while until camera stabilizes
    cout<<"Sleeping for 3 secs"<<endl;
    //capture
    sleep(3);
    Camera.grab();
    //allocate memory
    unsigned char *data=new unsigned char[  Camera.getImageTypeSize ( raspicam::RASPICAM_FORMAT_RGB )];
    //extract the image in rgb format
    Camera.retrieve ( data,raspicam::RASPICAM_FORMAT_RGB );//get camera image
    //save
    std::ofstream outFile ( "raspicam_image.ppm",std::ios::binary );
    outFile<<"P6\n"<<Camera.getWidth() <<" "<<Camera.getHeight() <<" 255\n";
    outFile.write ( ( char* ) data, Camera.getImageTypeSize ( raspicam::RASPICAM_FORMAT_RGB ) );
    cout<<"Image saved at raspicam_image.ppm"<<endl;
    //free resrources
    delete data;
    return 0;
}