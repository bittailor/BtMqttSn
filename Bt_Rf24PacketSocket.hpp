//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn, an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Rf24PacketSocket
//  
//*************************************************************************************************

#ifndef INC__Bt_Rf24PacketSocket__hpp
#define INC__Bt_Rf24PacketSocket__hpp

#include "Bt_I_RfPacketSocket.hpp"
#include "Bt_Rf24NetworkSocket.hpp"

namespace Bt {

class Rf24PacketSocket : public I_RfPacketSocket
{
   public:
      enum { IMPL_PAYLOAD_CAPACITY = Rf24NetworkSocket::Packet::PAYLOAD_CAPACITY };

      Rf24PacketSocket(Rf24NetworkSocket& pNetworkSocket);
      ~Rf24PacketSocket();

      virtual bool send(uint8_t* iPayload, size_t iSize, uint8_t iNodeId );
      virtual int32_t receive(uint8_t* oPayload, size_t iMaxSize, uint8_t* oNodeId);
      virtual bool available();

      virtual void suspend();
      virtual void resume();

   private:
   	  // Constructor to prohibit copy construction.
      Rf24PacketSocket(const Rf24PacketSocket&);

      // Operator= to prohibit copy assignment
      Rf24PacketSocket& operator=(const Rf24PacketSocket&);

      Rf24NetworkSocket* mNetworkSocket;
};

} // namespace Bt

#endif // INC__Bt_Rf24PacketSocket__hpp
