//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn , an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Rf24NetworkSocket
//  
//*************************************************************************************************

#include <stdio.h>
#include <string.h>

#include "Bt_MqttSnConfiguration.hpp"
#include "Bt_Rf24NetworkSocket.hpp"

namespace Bt {

//-------------------------------------------------------------------------------------------------

Rf24NetworkSocket::Rf24NetworkSocket(Rf24Node pNodeId, Rf24Controller& pController)
: mNodeId(pNodeId), mController(&pController), mIdCounter(0) {

   Rf24Controller::Configuration configuration;

   for(size_t i = 0; i < Rf24Pipes::NUMBER_OF_PIPES; i++) {
      Rf24Pipes::Rf24Pipe pipe = Rf24Pipes::ALL_PIPES[i];
      mRouting.configurePipe(mNodeId, pipe, configuration[pipe]);
   }

   mController->configure(configuration);
   mController->startListening();

}

//-------------------------------------------------------------------------------------------------

bool Rf24NetworkSocket::send(Packet& pPacket) {
  pPacket.source(mNodeId.id());
  pPacket.id(mIdCounter++);
  if (pPacket.destination() == mNodeId.id()) {
     receiveInternal(pPacket);
     return true;
  }
  return sendInternal(pPacket);

}

//-------------------------------------------------------------------------------------------------

bool Rf24NetworkSocket::receive(Packet& pPacket) {
   while(mPackets.empty()) {
      workcycle();
   }

   Packet& received = mPackets.pop();
   received.copyInto(pPacket);

   return true;
}

//-------------------------------------------------------------------------------------------------

bool Rf24NetworkSocket::available() {
   if(mPackets.empty()) {
      workcycle();
   }
   return !mPackets.empty();
}

//-------------------------------------------------------------------------------------------------

void Rf24NetworkSocket::workcycle() {
   size_t limiter = 0;
   while (mController->isDataAvailable() && limiter < 3) {
      limiter++;
      Packet packet;
      if (mController->read(packet.mControllerPackage)) {
         if (packet.destination() != mNodeId.id()) {
            BT_LOG_MESSAGE_AND_PARAMETER(" - route from ",packet.source());
            BT_LOG_MESSAGE_AND_PARAMETER("         to   ",packet.destination());
            sendInternal(packet);
            continue;
         }
         BT_LOG_MESSAGE_AND_PARAMETER(" - receive from ",packet.source());
         BT_LOG_MESSAGE_AND_PARAMETER("           to   ",packet.destination());
         receiveInternal(packet);
      }
   }
}
//-------------------------------------------------------------------------------------------------

bool Rf24NetworkSocket::sendInternal(Packet& pPacket) {
   Rf24Pipes::Rf24Pipe pipe = mRouting.calculateRoutingPipe(mNodeId, pPacket.destination());
   return mController->write(pipe, pPacket.mControllerPackage);
}

//-------------------------------------------------------------------------------------------------

void Rf24NetworkSocket::receiveInternal(Packet& pPacket) {

   if (mPackets.full()) {
      BT_LOG_MESSAGE("drop packet");
      return;
   }

   Packet& storage = mPackets.push();
   pPacket.copyInto(storage);
}

//-------------------------------------------------------------------------------------------------

} // namespace Bt
