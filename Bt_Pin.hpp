//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn, an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Pin
//  
//*************************************************************************************************

#ifndef INC__Bt_Pin__hpp
#define INC__Bt_Pin__hpp

#include <stdint.h>

namespace Bt {

class Pin
{
   public:
      enum Mode {
         MODE_INPUT,
         MODE_INPUT_PULLUP,
         MODE_OUTPUT
      };

      Pin(uint8_t iPinId, Mode iInitialMode);
      ~Pin();

      void mode(Mode iMode);

      void write(bool iHigh);

      bool read();

   private:
   	  // Constructor to prohibit copy construction.
      Pin(const Pin&);

      // Operator= to prohibit copy assignment
      Pin& operator=(const Pin&);

      const uint8_t mPinId;
};

} // namespace Bt

#endif // INC__Bt_Pin__hpp
