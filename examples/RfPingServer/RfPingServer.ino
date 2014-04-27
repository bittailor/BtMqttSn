#include <BtMqttSn.h>
#include <SPI.h>

#include "Bt_Pin.hpp"
#include "Bt_Spi.hpp"
#include "Bt_Rf24Device.hpp"
#include "Bt_Rf24Controller.hpp"
#include "Bt_Rf24NetworkSocket.hpp"

using namespace Bt;

#define CHIP_ENABLE 9
#define CHIP_SELECT 10
#define NODE_ID 2

class PingServer  {
   public:

      PingServer(Rf24NetworkSocket& pSocket) : mSocket(&pSocket) {

      }

      void workcycle() {
         if(mSocket->available()) {
            Rf24NetworkSocket::Packet packet;
            mSocket->receive(packet);
            packetReceived(packet);   
         }
      }

      void packetReceived(Rf24NetworkSocket::Packet& pPacket) {
         Serial << "Packet from " << (int)pPacket.source() << " => " << (int)pPacket.destination() << ":" << (int)pPacket.size() << endl;
         Rf24NetworkSocket::Packet packet;
         packet.destination(pPacket.source());
         packet.writePayload(pPacket.payload(),pPacket.size());
         mSocket->send(packet);
      }

   private:
      Rf24NetworkSocket* mSocket;
};

void setup() {
   Serial.begin(9600);
}

void loop() {
   Pin chipEnable(CHIP_ENABLE, Pin::MODE_OUTPUT);
   Pin chipSelect(CHIP_SELECT, Pin::MODE_OUTPUT);
   Spi spi(Spi::BIT_ORDER_MSBFIRST,
           Spi::MODE_0,
           Spi::SPEED_8_MHZ,
           chipSelect);

   Rf24Device device(spi,chipEnable);
   Rf24Controller controller(device);
   Rf24NetworkSocket socket(NODE_ID, controller);
   PingServer pingServer(socket);

   Serial << "Enter ping server workcycle for node " << (int) NODE_ID << endl;
   while(true) {
      pingServer.workcycle();
   }  
}
