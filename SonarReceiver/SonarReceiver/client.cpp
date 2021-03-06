#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <chrono>
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

MovingAverage distance;
int active;
void setup();
double getDistance();
int sendMessage();
int demo();
double timeouts = 0;
int sockfd, portno, n;
struct sockaddr_in serv_addr;
struct hostent *server;
MovingAverage timeoutsCounter;
char buffer[256];

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int connectionSetup(){
    
    
    portno = 55555;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("ERROR opening socket");
        return 0;
    }
    server = gethostbyname("mattpi");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        return 0;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    
    return 1;
}

int waitMessage()
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
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
    close(newsockfd);
    close(sockfd);
    return 0;
}



int main(){

    printf("HELLO SONAR\n");
    setup();
    int i = 0;
    while (1) {
        i++;
        delay(3);
        connectionSetup();
        while (!sendMessage()) {
        }
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
    
    close(sockfd);
}


void setup() {
    //printf("Made it to setup");
    timeoutsCounter.setup(40);
    distance.setup(100);
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
            timeoutsCounter.insert(100);
            return distance.getAverage();
            
        }
    }
    
    //Wait for echo end
    startTime = micros();
    while(digitalRead(ECHO) == HIGH){
        if ((micros()-startTime)>400000) { //maximum of 160cm
            active = 0;
            timeouts++;
            timeoutsCounter.insert(100);
            return distance.getAverage();
            
        }
    }
    long travelTime = micros() - startTime;
    
    //Get distance in cm
    double newDistance = travelTime / 58.0;
    double delta = fabs(newDistance-lastReading);
    if (newDistance>2500) {
        return distance.getAverage();
    }
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



int sendMessage(){

    bzero(buffer,256);
    std::chrono::microseconds ms = std::chrono::duration_cast< std::chrono::microseconds >(std::chrono::system_clock::now().time_since_epoch());
    long long timeKey =ms.count()+30000;
    sprintf(buffer,"%lld\n", timeKey);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0){
        error("ERROR writing to socket");
        return 0;
    }

    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0){
        error("ERROR reading from socket");
        return 0;
    }
    printf("%s\n",buffer);
    /*
    int keepWaiting = 1;
    while(keepWaiting){
            std::chrono::microseconds ms = std::chrono::duration_cast< std::chrono::microseconds >(std::chrono::system_clock::now().time_since_epoch());
        long long currentTime = ms.count();
        if (currentTime>=timeKey) {
            break;
        }
    }
     */

    return 1;
}