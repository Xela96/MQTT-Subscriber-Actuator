#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"
#include <stdio.h>
#include <iostream>
#include <string> 
#include <jsoncpp/json/json.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "GPIO.h"

#define ADDRESS     "tcp://192.168.43.6:1883"
#define CLIENTID    "rpi2"
#define AUTHMETHOD  "alex"
#define AUTHTOKEN   "toilet11"
#define TOPIC       "ee513/ADXLdata"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

using namespace std;

volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt) {
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

void flashLED10(){
    GPIO LEDobj(24);
    LEDobj.setDirection(OUTPUT);
    LEDobj.setValue(HIGH);
    usleep(10000000); //wait for 10 seconds
    LEDobj.setValue(LOW);
}

void flashLED(){
    GPIO LEDobj(24);
    LEDobj.setDirection(OUTPUT);
    for(int i=0; i<11; i++){
        LEDobj.setValue(HIGH);
        usleep(1000000);
        LEDobj.setValue(LOW);
        usleep(1000000);
    } 
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    int i;
    string string1;
    char ch;
    char* payloadptr;
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");
    payloadptr = (char*) message->payload;
    for(i=0; i<message->payloadlen; i++) {
        ch = *payloadptr;
        string1.append(1, ch);
        putchar(*payloadptr++);
    }
    putchar('\n');
    char * charString = (char*) message->payload;
    Json::Value val;
    Json::Reader reader1;
    bool parsingStatus = reader1.parse(charString, val);

    if(!parsingStatus){
        cout<<"Error parsing string"<<endl;
    }

    int AccelerationX=val["X-Acceleration"].asInt();
    int AccelerationY=val["Y-Acceleration"].asInt();
    int AccelerationZ=val["Z-Acceleration"].asInt();
    float pitch=val["Pitch"].asFloat();
    float roll=val["Roll"].asFloat();
    string sysTime=(val["TimeDate"]).toString();;

    cout << "AccelerationX: " << AccelerationX << endl;
    cout << "AccelerationY: " << AccelerationY << endl;
    cout << "AccelerationZ: " << AccelerationZ << endl;
    cout << "Pitch: " << pitch << endl;
    cout << "Roll: " << roll << endl;
    cout << "Time: " << sysTime << endl;


    if(AccelerationX > 50 || AccelerationY > 50 || AccelerationZ > 250){
        cout<<"success"<<endl;
        flashLED10();
    }

    usleep(5000000); //waits 5 seconds

    if(pitch < 0 || roll < 0){
        cout<<"success2"<<endl;
        flashLED();
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause) {
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
    int rc;
    int ch;

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    opts.keepAliveInterval = 20;
    opts.cleansession = 0; //set to 0 for persistent connection
    opts.username = AUTHMETHOD;
    opts.password = AUTHTOKEN;

    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
    if ((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC, QOS);

    do {
        ch = getchar();
    } while(ch!='Q' && ch != 'q');
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}