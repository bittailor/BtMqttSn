//*************************************************************************************************
//
//  BITTAILOR.CH - BtCore
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Rf24::RfNode
//  
//*************************************************************************************************

#ifndef INC__Bt_Rf24_RfNode__hpp
#define INC__Bt_Rf24_RfNode__hpp

#include <stdint.h>

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

#endif // INC__Bt_Rf24_RfNode__hpp
