//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn, an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Rf24PacketSocket
//  
//*************************************************************************************************

#include "Bt_Rf24PacketSocket.hpp"

#include "Bt_MqttSnConfiguration.hpp"


#define JOIN( X, Y ) JOIN2(X,Y)
#define JOIN2( X, Y ) X##Y



namespace StaticAssert
{
    template <bool> struct STATIC_ASSERT_FAILURE;
    template <> struct STATIC_ASSERT_FAILURE<true> { enum { value = 1 }; };

    template<int x> struct static_assert_test{};
}

#define COMPILE_ASSERT(x) \
    typedef ::StaticAssert::static_assert_test<\
        sizeof(::StaticAssert::STATIC_ASSERT_FAILURE< (bool)( x ) >)>\
            JOIN(_static_assert_typedef, __LINE__)


COMPILE_ASSERT(static_cast<size_t>(Bt::I_RfPacketSocket::PAYLOAD_CAPACITY) == static_cast<size_t>(Bt::Rf24PacketSocket::IMPL_PAYLOAD_CAPACITY));

namespace Bt {

//-------------------------------------------------------------------------------------------------

Rf24PacketSocket::Rf24PacketSocket(Rf24NetworkSocket& pNetworkSocket) : mNetworkSocket(&pNetworkSocket) {

}

//-------------------------------------------------------------------------------------------------

Rf24PacketSocket::~Rf24PacketSocket() {

}

//-------------------------------------------------------------------------------------------------

bool Rf24PacketSocket::send(uint8_t* iPayload, size_t iSize,  uint8_t iNodeId){
   if(iSize > Rf24NetworkSocket::Packet::PAYLOAD_CAPACITY) {
      BT_LOG_MESSAGE("size to large");
      return false;
   }

   Rf24NetworkSocket::Packet packet;
   packet.writePayload(iPayload,iSize);
   return mNetworkSocket->send(packet);
}

//-------------------------------------------------------------------------------------------------

int32_t Rf24PacketSocket::receive(uint8_t* oPayload, size_t iMaxSize, uint8_t* oNodeId){
   Rf24NetworkSocket::Packet packet;
   if (!mNetworkSocket->receive(packet)){
      return false;
   }

   if (oNodeId != 0) {
      *oNodeId = packet.source();
   }

   if(packet.size() > iMaxSize) {
      BT_LOG_MESSAGE("max size to small");
      return -1;
   }

   return packet.readPayload(oPayload, iMaxSize);
}

//-------------------------------------------------------------------------------------------------

bool Rf24PacketSocket::available(){
   return mNetworkSocket->available();
}

//-------------------------------------------------------------------------------------------------

} // namespace Bt
