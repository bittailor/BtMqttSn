#include <BtMqttSn.h>
#include <SPI.h>

#include "Pin.hpp"
#include "Spi.hpp"
#include "Rf24Device.hpp"
#include "Rf24Controller.hpp"
#include "Rf24NetworkSocket.hpp"
#include "Rf24PacketSocket.hpp"




#define TEAM 1


//-----

#define xstr(s) str(s)
#define str(s) #s

#define NODE_ID (1 + TEAM)
#define CLIENT_ID "GeekTeam" xstr(TEAM)
#define TOPIC "Geek/Team/" xstr(TEAM) "/Status"

#define GATEWAY_NODE_ID 0
#define CHIP_ENABLE 9
#define CHIP_SELECT 10

//-----

#define LOG_RAM(marker) Serial << marker << freeRam() << endl 

//-----


class Timer  {
   public:
      Timer(MqttSnClient& iClient, unsigned int iTimeSpan) : mClient(&iClient), mTimeSpan(iTimeSpan), mNext(millis() + mTimeSpan) {
      }

      void loop() {
         if(millis() > mNext) {
            String message = "Time is ";
            message = message + millis();
            mClient->publish(TOPIC,message.c_str());
            mNext = millis() + mTimeSpan;
         }
      }

   private:
      MqttSnClient* mClient;
      unsigned int mTimeSpan;
      unsigned long mNext;
};

void setup() {
   Serial.begin(9600);
   Serial << endl << endl << endl << "*** MQTT-SN Example ***" << endl;
   Serial << endl; 
   Serial << "   - Team   = " << TEAM << endl;
   Serial << "   - Node   = " << NODE_ID << endl;
   Serial << "   - Client = " << CLIENT_ID << endl;
   Serial << "   - Topic  = " << TOPIC << endl;
   Serial << endl; 
   Serial << "send to start ..." << endl;
   while(!checkCommand("start")) {}
   Serial << "... start" << endl;
}

void loop() {

   LOG_RAM("a");

   Pin chipEnable(CHIP_ENABLE, Pin::MODE_OUTPUT);
   Pin chipSelect(CHIP_SELECT, Pin::MODE_OUTPUT);
   Spi spi(Spi::BIT_ORDER_MSBFIRST,
           Spi::MODE_0,
           Spi::SPEED_8_MHZ,
           chipSelect);

   Rf24Device device(spi,chipEnable);
   Rf24Controller controller(device);
   Rf24NetworkSocket networkSocket(NODE_ID, controller);

   LOG_RAM("b");

   Rf24PacketSocket packetSocket(networkSocket); 

   LOG_RAM("c");


   MqttSnClient mqttClient(packetSocket, GATEWAY_NODE_ID, CLIENT_ID);

   Timer timer(mqttClient,1000);

   LOG_RAM("d");

   Serial << "try connect ..." << endl;

   if(!mqttClient.connect()) {
      Serial << "mqtt: failed to connect" << endl;   
   }
  
   Serial << "... connected" << endl;

   Serial << "publish" << endl;

   mqttClient.publish(TOPIC,"Start");
  

   Serial << "Enter ping server workcycle for node " << NODE_ID << endl;
   LOG_RAM("e");
   bool loop = true;
   while(loop) {
      mqttClient.loop();  
      timer.loop();
      loop = !checkCommand("stop");
   }  

   mqttClient.publish(TOPIC,"Stop");

   mqttClient.disconnect();

   Serial << "Hit reboot" << endl;
   while(true) {

   }
}

bool checkCommand(String expectedCommand) {
  if (!Serial.available()) {
    return false;
  }
  Serial.setTimeout(100);
  String command = Serial.readString();
  Serial.setTimeout(1000);
  return  expectedCommand.equalsIgnoreCase(command);
}

int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

