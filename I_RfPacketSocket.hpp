//*************************************************************************************************
//
//  BITTAILOR.CH - BtCore
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Rf24::I_RfPacketSocket
//  
//*************************************************************************************************

#ifndef INC__Bt_Rf24_I_RfPacketSocket__hpp
#define INC__Bt_Rf24_I_RfPacketSocket__hpp

#include <stddef.h>
#include <stdint.h>

class I_RfPacketSocket {
   public:

      enum { PAYLOAD_CAPACITY = 29 };

      virtual ~I_RfPacketSocket() {}
      
      virtual bool send(uint8_t* iPayload, size_t iSize, uint8_t iGatewayNodeId) = 0;
      virtual int32_t receive(uint8_t* oPayload, size_t iMaxSize, uint8_t* oGatewayNodeId) = 0;
      virtual bool available() = 0;


};

#endif // INC__Bt_Rf24_I_RfPacketSocket__hpp
