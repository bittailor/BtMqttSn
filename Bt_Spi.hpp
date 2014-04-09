//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn, an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Spi
//  
//*************************************************************************************************

#ifndef INC__Bt_Spi__hpp
#define INC__Bt_Spi__hpp

#include "Bt_Pin.hpp"

namespace Bt {

class Spi
{
   public:

      enum BitOrder {BIT_ORDER_LSBFIRST , BIT_ORDER_MSBFIRST};
      enum Mode {MODE_0,MODE_1,MODE_2,MODE_3};
      enum Speed {SPEED_4_MHZ, SPEED_8_MHZ};

      Spi(BitOrder iBitOrder, Mode iSpiMode, Speed iSpeed, Pin& iChipSelectPin);
      ~Spi();

      void chipSelect(bool iSelect);
      uint8_t transfer(uint8_t iData);
   
   private:
   	  // Constructor to prohibit copy construction.
      Spi(const Spi&);

      // Operator= to prohibit copy assignment
      Spi& operator=(const Spi&);

      Pin* mChipSelectPin;
};

} // namespace Bt

#endif // INC__Bt_Spi__hpp
