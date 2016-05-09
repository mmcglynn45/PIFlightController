/* A simple server in the internet domain using TCP
 The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "wiringPi.h"
#include "MovingAverage.h"
#include <math.h>
#include <iostream>
#include <chrono>

#define TRUE 1

#define TRIG 20
#define ECHO 21


int sockfd, newsockfd, portno;
socklen_t clilen;
char buffer[256];
struct sockaddr_in serv_addr, cli_addr;
int n;
MovingAverage distance;
int active;
void setup();
double getDistance();
int demo();
double timeouts = 0;


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void connectionSetup(){
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 55555;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");

}


int waitMessage()
{
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,
                       (struct sockaddr *) &cli_addr,
                       &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");
    bzero(buffer,256);
    n = read(newsockfd,buffer,255);
    if (n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n",buffer);
    n = write(newsockfd,"I got your message",18);
    if (n < 0) error("ERROR writing to socket");
    return 0;
}



int main(){
    connectionSetup();
    printf("HELLO SONAR\n");
    setup();
    int i = 0;
    while (1) {
        i++;
        delay(3);
        waitMessage();
        delayMicroseconds(20000);
        double dist = getDistance();
        printf("Sonar Distance = %f\n", dist/2.54);
        if (i%300==0) {
            printf("Sonar Distance = %f\n", dist/2.54);
            printf("Timeouts = %f\n", timeouts);
            using namespace std::chrono;
            microseconds ms = duration_cast< microseconds >(system_clock::now().time_since_epoch());
            printf("Time in milliseconds %lld\n", ms.count());
        }
    }
    
    close(newsockfd);
    close(sockfd);
}


void setup() {
    //printf("Made it to setup");
    distance.setup(10);
    wiringPiSetupGpio();
    
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    
    //TRIG pin must start LOW
    digitalWrite(TRIG, LOW);
    delay(30);
}

double getCM() {
    double lastReading = distance.getAverage();
    active = 1;
    long startTime = micros();
    //Send trig pulse
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(20);
    digitalWrite(TRIG, LOW);
    
    //Wait for echo start
    while(digitalRead(ECHO) == LOW){
        if ((micros()-startTime)>400000) { //maximum of 160cm
            active = 0;
            timeouts++;
            return distance.getAverage();
            
        }
    }
    
    //Wait for echo end
    startTime = micros();
    while(digitalRead(ECHO) == HIGH){
        if ((micros()-startTime)>40000) { //maximum of 160cm
            active = 0;
            timeouts++;
            return distance.getAverage();
            
        }
    }
    long travelTime = micros() - startTime;
    
    //Get distance in cm
    double newDistance = travelTime / 58.0;
    double delta = fabs(newDistance-lastReading);
    if (fabs(newDistance-distance.getAverage())<1) {
        distance.insert(newDistance);
    }else{
        distance.insert(newDistance*.8+distance.getAverage()*.2);
    }
    active = 0;
    return distance.getAverage();
}

double getDistance(){
    return getCM();
}
