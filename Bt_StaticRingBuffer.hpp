//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn , an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::StaticRingBuffer
//  
//*************************************************************************************************

#ifndef INC__Bt_StaticRingBuffer__hpp
#define INC__Bt_StaticRingBuffer__hpp

namespace Bt {

template <typename T, size_t N>
class StaticRingBuffer {
   public:
      
      StaticRingBuffer() : mBegin(0), mEnd(0) {

      }

      T& push() {
         return mData[increment(mEnd)];
      }

      T& pop() {
         return mData[increment(mBegin)];
      }

      bool empty() {
         return mBegin == mEnd;
      }

      bool full() {
         return next(mEnd) == mBegin;
      }

   private:
      size_t increment(size_t& ioIterator) {
         size_t old = ioIterator;
         ioIterator = next(ioIterator) ;
         return old;
      }

      size_t next(size_t iIterator) {
         return (iIterator + 1) % N;
      }


      T mData[N];
      size_t mBegin;
      size_t mEnd;
};

} // namespace Bt

#endif // INC__Bt_StaticRingBuffer__hpp
