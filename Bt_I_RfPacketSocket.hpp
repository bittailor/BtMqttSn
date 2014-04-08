//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn , an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::I_RfPacketSocket
//  
//*************************************************************************************************

#ifndef INC__Bt_I_RfPacketSocket__hpp
#define INC__Bt_I_RfPacketSocket__hpp

#include <stddef.h>
#include <stdint.h>

namespace Bt {

class I_RfPacketSocket {
   public:

      enum { PAYLOAD_CAPACITY = 29 };

      virtual ~I_RfPacketSocket() {}
      
      virtual bool send(uint8_t* iPayload, size_t iSize, uint8_t iGatewayNodeId) = 0;
      virtual int32_t receive(uint8_t* oPayload, size_t iMaxSize, uint8_t* oGatewayNodeId) = 0;
      virtual bool available() = 0;


};

} // namespace Bt

#endif // INC__Bt_I_RfPacketSocket__hpp
