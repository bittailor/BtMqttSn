//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn, an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  BT::PlacementPointer
//  
//*************************************************************************************************

#ifndef INC__Bt_PlacementPointer__hpp
#define INC__Bt_PlacementPointer__hpp

namespace Bt {

template<typename T>
class PlacementPointer {
   public:
      
      PlacementPointer(){
      }

      T& operator*() {
         return *typed();
      }

      T* operator-> ()
      {
         return typed();
      }

      void create() {
         new (raw()) T();
      }

      template<typename Arg1>
      void create(Arg1 iArg1) {
         new (raw()) T(iArg1);
      }

      template<typename Arg1, typename Arg2>
      void create(Arg1 iArg1, Arg2 iArg2) {
         new (raw()) T(iArg1, iArg2);
      }

      template<typename Arg1, typename Arg2, typename Arg3>
      void create(Arg1 iArg1, Arg2 iArg2, Arg3 iArg3) {
         new (raw()) T(iArg1, iArg2, iArg3);
      }

      template<typename Arg1, typename Arg2, typename Arg3, typename Arg4>
      void create(Arg1 iArg1, Arg2 iArg2, Arg3 iArg3, Arg4 iArg4) {
         new (raw()) T(iArg1, iArg2, iArg3, iArg4);
      }

      void dispose() {
         typed()->~T();
      }

   private:
      // Constructor to prohibit copy construction.
      PlacementPointer(const PlacementPointer&);

      // Operator= to prohibit copy assignment
      PlacementPointer& operator=(const PlacementPointer&);

      T* typed() {
         return reinterpret_cast<T*>(mMemory);
      }

      void* raw() {
         return mMemory;
      }


      uint8_t mMemory[sizeof(T)];
};

} // namespace Bt

#endif // INC__Bt_PlacementPointer__hpp
