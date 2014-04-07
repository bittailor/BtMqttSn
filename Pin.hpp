//*************************************************************************************************
//
//  BITTAILOR.CH - BtCore
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Mcu::Pin
//  
//*************************************************************************************************

#ifndef INC__Bt_Mcu_Pin__hpp
#define INC__Bt_Mcu_Pin__hpp

#include <stdint.h>

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


#endif // INC__Bt_Mcu_Pin__hpp
