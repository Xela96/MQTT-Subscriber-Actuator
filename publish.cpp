// Based on the Paho C code example from www.eclipse.org/paho/
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <ctime>
#include<cmath>
#include <string> 
#include <iomanip>
#include <chrono>
#include "MQTTClient.h"
#include "ADXL345.h"
#define  CPU_TEMP "/sys/class/thermal/thermal_zone0/temp"
using namespace std;
using std::chrono::system_clock;

//Please replace the following address with the address of your server
#define ADDRESS    "tcp://192.168.43.6:1883"
#define CLIENTID   "rpi1"
#define AUTHMETHOD "alex"
#define AUTHTOKEN  "toilet11"
#define TOPIC      "ee513/ADXLdata"
#define QOS        1
#define TIMEOUT    10000L

float getCPUTemperature() {        // get the CPU temperature
   int cpuTemp;                    // store as an int
   fstream fs;
   fs.open(CPU_TEMP, fstream::in); // read from the file
   fs >> cpuTemp;
   fs.close();
   return (((float)cpuTemp)/1000);
}

int main(int argc, char* argv[]) {
   unsigned int I2CBus = 1;
   unsigned int I2CAddress=0x53;
   exploringRPi::ADXL345 object(I2CBus,I2CAddress);
   object.readSensorState();
   short AccelerationX = object.getAccelerationX();
   short AccelerationY = object.getAccelerationY();
   short AccelerationZ = object.getAccelerationZ();
   float pitch = object.getPitch();
   float roll = object.getRoll();
   // time_t t = time(NULL);
   // tm* tPtr = localtime(&t);

   // string seconds = to_string(tPtr->tm_sec);
   // string minutes = to_string(tPtr->tm_min)+ ":";
   // string hours = to_string(tPtr->tm_hour)+ ":";
   // string hoursMins = hours.append(minutes.append(seconds));

   system_clock::time_point today = system_clock::now();
   std::time_t tt;

   tt = system_clock::to_time_t ( today );
   std::cout << "today is: " << ctime(&tt);




   // cout << "\n\n Display the Current Date and Time :\n";
   // cout << "----------------------------------------\n";
   // cout << " seconds = " << (tPtr->tm_sec) << endl;
   // cout << " minutes = " << (tPtr->tm_min) << endl;
   // cout << " hours = " << (tPtr->tm_hour) << endl;
   // cout << " day of month = " << (tPtr->tm_mday) << endl;
   // cout << " month of year = " << (tPtr->tm_mon)+1 << endl;
   // cout << " year = " << (tPtr->tm_year)+1900 << endl;

   
   cout << "X acceleration: " << AccelerationX << endl;
   cout << "Y acceleration: " << AccelerationY << endl;
   cout << "Z acceleration: " << AccelerationZ << endl;
   cout << "Pitch: " << pitch << endl;
   cout << "Roll: " << roll << endl;


   char str_payload[100];          // Set your max message size here
   MQTTClient client;
   MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
   MQTTClient_message pubmsg = MQTTClient_message_initializer;
   MQTTClient_deliveryToken token;
   MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

   MQTTClient_willOptions willOpts = MQTTClient_willOptions_initializer; //initialiser for MQTT will options
   opts.will = &willOpts;

   const char* LWTMessage = "Last will and testament";
   const char* LWTTopic = "LastWill";

	opts.will->qos = QOS;
   opts.will->message = LWTMessage;
   opts.will->topicName = LWTTopic;
   opts.will->retained = 0;

   opts.keepAliveInterval = 20;
   opts.cleansession = 1;
   opts.username = AUTHMETHOD;
   opts.password = AUTHTOKEN;
   int rc;
   if ((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS) {
      cout << "Failed to connect, return code " << rc << endl;
      return -1;
   }
   sprintf(str_payload, "{\"CPUTemp\": %f , \"X-Acceleration\": %d , \"Y-Acceleration\": %d , \"Z-Acceleration\": %d , \"Pitch\": %f , \"Roll\": %f  }", getCPUTemperature(), AccelerationX, AccelerationY, AccelerationZ, pitch, roll); //, \"TimeDate\": %s , ctime(&tt)
   // sprintf(str_payload, "{\"d\":{\"CPUTemp\": %f, \"AccX\": %d, \"AccY\": %d,\"AccZ\": %d, \"Pitch\": %d, \"Roll\": %d}}",
    //getCPUTemperature(), AccelerationX, AccelerationY, AccelerationZ, pitch, roll);

   pubmsg.payload = str_payload;
   pubmsg.payloadlen = strlen(str_payload);
   pubmsg.qos = QOS;
   pubmsg.retained = 0;
   MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
   cout << "Waiting for up to " << (int)(TIMEOUT/1000) <<
        " seconds for publication of " << str_payload <<
        " \non topic " << TOPIC << " for ClientID: " << CLIENTID << endl;
   int c;
   printf("Press Q<Enter> to quit\n");
   do {
      c = getchar();
      } 
   while(c!='q' && c != 'Q');
      MQTTClient_disconnect(client, 10000);
      MQTTClient_destroy(&client);
   
   rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
   cout << "Message with token " << (int)token << " delivered." << endl;
   MQTTClient_disconnect(client, 10000);
   MQTTClient_destroy(&client);
   return rc;
}