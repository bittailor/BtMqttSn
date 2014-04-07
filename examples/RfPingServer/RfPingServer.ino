#include <BtMqttSn.h>
#include <SPI.h>

#include "Pin.hpp"
#include "Spi.hpp"
#include "Rf24Device.hpp"
#include "Rf24Controller.hpp"
#include "Rf24NetworkSocket.hpp"

#define CHIP_ENABLE 9
#define CHIP_SELECT 10
#define NODE_ID 2

namespace{

template<typename T>
void printArray(const T& pArray)
{
  BT_LOG_MESSAGE(" 0x");
  for(size_t i = pArray.size() ; i > 0 ; i--)
  {
    BT_LOG_MESSAGE("%02x",pArray[i-1]);
  }
}

}

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
         BT_LOG_MESSAGE("Packet from %i => %i \n [%i]", (int)pPacket.source(), (int)pPacket.destination(), (int) pPacket.size());
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
   fdevopen( &serial_putc, 0 ); 	
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

  
   for(size_t i = 0; i < Rf24Pipes::NUMBER_OF_PIPES; i++) {
      Rf24Pipes::Rf24Pipe pipe = Rf24Pipes::ALL_PIPES[i];
      BT_LOG_MESSAGE("pipe %i ",pipe);
      printArray(device.receiveAddress(pipe).raw());
      BT_LOG_MESSAGE("\n");
   }

   BT_LOG_MESSAGE("Enter ping server workcycle for node %i\n",(int) NODE_ID);
   while(true) {
      pingServer.workcycle();
   }  
}

int serial_putc( char c, FILE * )
{
  Serial.write( c );

  return c;
}

int serial_getc( char c, FILE * )
{
  return Serial.read();
}
