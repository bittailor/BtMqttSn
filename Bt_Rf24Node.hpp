//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn , an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::RfNode
//  
//*************************************************************************************************

#ifndef INC__Bt_Rf24Node__hpp
#define INC__Bt_Rf24Node__hpp

#include <stdint.h>

namespace Bt {

class Rf24Node 
{
   public:
      Rf24Node(uint8_t pId);

      uint8_t id() const { return mId; }
      uint8_t level() const { return mLevel; }

   private:
      static uint8_t calculateLevel(uint8_t pId);

      uint8_t mId;
      uint8_t mLevel;
};

} // namespace Bt

#endif // INC__Bt_Rf24Node__hpp
