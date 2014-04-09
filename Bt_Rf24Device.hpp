//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn, an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Rf24Device
//  
//*************************************************************************************************

#ifndef INC__Bt_Rf24Device__hpp
#define INC__Bt_Rf24Device__hpp

#include "Bt_Pin.hpp"
#include "Bt_Rf24Address.hpp"
#include "Bt_Rf24Pipe.hpp"
#include "Bt_Spi.hpp"

namespace Bt {

class Rf24Device
{
   public:

      enum {
         MAX_PAYLOAD_SIZE = 32
      };

      enum TransceiverMode {
         TX_MODE,
         RX_MODE
      };

      enum DataRate {
         DR_250_KBPS,
         DR_1_MBPS,
         DR_2_MBPS
      };

      class Status {
         public:
            Status(bool pDataReady, bool pDataSent, bool pRetransmitsExceeded)
         : mDataReady(pDataReady), mDataSent(pDataSent), mRetransmitsExceeded(pRetransmitsExceeded)  {}

            bool dataReady() {return mDataReady;}
            bool dataSent() {return mDataSent;}
            bool retransmitsExceeded() {return mRetransmitsExceeded;}


         private:
            bool mDataReady;
            bool mDataSent;
            bool mRetransmitsExceeded;
      };



      Rf24Device(Spi& pSpi, Pin& pChipEnable);
      ~Rf24Device();

      Status status();

      void clearDataReady();
      void clearDataSent();
      void clearRetransmitsExceeded();

      bool powerUp();
      void powerUp(bool pValue);

      TransceiverMode transceiverMode();
      void transceiverMode(TransceiverMode pMode);

      void chipEnable(bool pValue);

      uint8_t autoRetransmitDelay();
      void autoRetransmitDelay(uint8_t pDelay);

      uint8_t autoRetransmitCount();
      void autoRetransmitCount(uint8_t pCount);

      uint8_t autoRetransmitCounter();

      uint8_t channel();
      void channel(uint8_t pChannel);

      DataRate dataRate();
      void dataRate(DataRate pDataRate);

      Rf24Address receiveAddress(Rf24Pipes::Rf24Pipe pPipe);
      void receiveAddress(Rf24Pipes::Rf24Pipe pPipe, Rf24Address pAddress);

      bool receivePipeEnabled(Rf24Pipes::Rf24Pipe pPipe);
      void receivePipeEnabled(Rf24Pipes::Rf24Pipe pPipe, bool pValue);

      uint8_t receivePayloadSize(Rf24Pipes::Rf24Pipe pPipe);
      void receivePayloadSize(Rf24Pipes::Rf24Pipe pPipe, uint8_t pSize);

      Rf24Address transmitAddress();
      void transmitAddress(Rf24Address pAddress);

      bool isTransmitFifoEmpty();
      bool isTransmitFifoFull();
      void flushTransmitFifo();

      bool isReceiveFifoEmpty();
      bool isReceiveFifoFull();
      void flushReceiveFifo();

      size_t writeTransmitPayload(uint8_t* pData, size_t pSize);

      size_t availableReceivePayload();
      size_t readReceivePayload(Rf24Pipes::Rf24Pipe& pPipe, uint8_t* pData, size_t pSize);

      bool dynamicPayloadFeatureEnabled();
      void dynamicPayloadFeatureEnabled(bool pValue);

      bool dynamicPayloadEnabled(Rf24Pipes::Rf24Pipe pPipe);
      void dynamicPayloadEnabled(Rf24Pipes::Rf24Pipe pPipe, bool pValue);


   private:
   	  // Constructor to prohibit copy construction.
      Rf24Device(const Rf24Device&);

      // Operator= to prohibit copy assignment
      Rf24Device& operator=(const Rf24Device&);

      Spi* mSpi;
      Pin* mChipEnable;

};

} // namespace Bt

#endif // INC__Bt_Rf24Device__hpp
