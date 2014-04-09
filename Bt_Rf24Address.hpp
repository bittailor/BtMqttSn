//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn, an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Rf24Address
//  
//*************************************************************************************************

#ifndef INC__Bt_Rf24Address__hpp
#define INC__Bt_Rf24Address__hpp

#include "Bt_StaticArray.hpp"

namespace Bt {

class Rf24Address {
   public:
      Rf24Address(){
      }

      Rf24Address(StaticArray<uint8_t,5> pRaw) : mRaw(pRaw){
      }

      Rf24Address(uint8_t pByte4, uint8_t pByte3, uint8_t pByte2, uint8_t pByte1, uint8_t pByte0) {
         mRaw[0] = pByte0;
         mRaw[1] = pByte1;
         mRaw[2] = pByte2;
         mRaw[3] = pByte3;
         mRaw[4] = pByte4;
      }

      StaticArray<uint8_t,5>& raw() {
         return mRaw;
      }

   private:
      StaticArray<uint8_t,5> mRaw;
};

} // namespace Bt

#endif // INC__Bt_Rf24Address__hpp
