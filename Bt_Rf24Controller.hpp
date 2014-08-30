//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn, an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Rf24Controller
//  
//*************************************************************************************************

#ifndef INC__Bt_Rf24Controller__hpp
#define INC__Bt_Rf24Controller__hpp

#include <Arduino.h>
#include <stddef.h>
#include <stdint.h>

#include "Bt_Rf24Device.hpp"

namespace Bt {

class Rf24Controller
{
   public:

      class Configuration {
         public:

            struct PipeConfiguration {
               bool mEnabled;
               Rf24Address mAddress;
            };

            Configuration(uint8_t iAutoRetransmitDelay = 0x6) : mAutoRetransmitDelay(iAutoRetransmitDelay) {

            }

            uint8_t autoRetransmitDelay() const {
               return mAutoRetransmitDelay;
            }


            PipeConfiguration& operator[](Rf24Pipes::Rf24Pipe pPipe) {
               return mPipeConfigurations[static_cast<size_t>(pPipe)];
            }

         private:
            StaticArray<PipeConfiguration,Rf24Pipes::NUMBER_OF_PIPES> mPipeConfigurations;
            uint8_t mAutoRetransmitDelay;
      };

      class Packet {
         public:
            enum { BUFFER_CAPACITY = Rf24Device::MAX_PAYLOAD_SIZE };

            Packet() : mBuffer(), mSize(0) {

            }

            uint8_t* buffer() {
               return mBuffer;
            }

            void* rawBuffer() {
               return mBuffer;
            }

            size_t size() {
               return mSize;
            }

            void size(size_t pSize) {
               mSize = pSize;
            }

            void copyInto(Packet& iPacket) const {
               memcpy(iPacket.mBuffer, mBuffer, BUFFER_CAPACITY);
               iPacket.mSize = mSize;
            }

         private:
            // Constructor to prohibit copy construction.
            Packet(const Packet&);

            // Operator= to prohibit copy assignment
            Packet& operator=(const Packet&);

            uint8_t mBuffer[BUFFER_CAPACITY] ;
            size_t mSize;
      };

      enum { MAX_PAYLOAD_SIZE = Rf24Device::MAX_PAYLOAD_SIZE };

      Rf24Controller(Rf24Device& pDevice);
      ~Rf24Controller();

      void configure(const Configuration& pConfiguration);

      void startListening();
      void stopListening();

      void suspend();
      void resume();

      bool write(Rf24Pipes::Rf24Pipe pPipe, Packet& pPacket);

      size_t write(Rf24Pipes::Rf24Pipe pPipe, uint8_t* pData, size_t pSize);

      bool isDataAvailable();
      bool read(Packet& pPacket);
      bool read(Packet& pPacket, Rf24Pipes::Rf24Pipe& pPipe);

      size_t read(uint8_t* pBuffer, size_t pSize);
      size_t read(uint8_t* pBuffer, size_t pSize, Rf24Pipes::Rf24Pipe& pPipe);


   private:

      class StateBase {
         public:
         StateBase(Rf24Controller& pController) : mController(&pController){}
         virtual ~StateBase(){}
         virtual void ApplyTo(StateBase& other)= 0;
         virtual void ToPowerDown(){};
         virtual void ToStandbyI(){};
         virtual void ToRxMode(){};
         virtual void ToTxMode(){};
         protected:
         Rf24Controller* mController;
      };

      class PowerDown : public StateBase {
         public:
            PowerDown(Rf24Controller& pController) : StateBase(pController){}
            virtual void ApplyTo(StateBase& other) {other.ToPowerDown();};
            virtual void ToStandbyI();
            virtual void ToRxMode();
            virtual void ToTxMode();
      };

      class StandbyI : public StateBase {
         public:
            StandbyI(Rf24Controller& pController) : StateBase(pController){}
            virtual void ApplyTo(StateBase& other) {other.ToStandbyI();};
            virtual void ToPowerDown();
            virtual void ToRxMode();
            virtual void ToTxMode();
      };

      class RxMode : public StateBase {
         public:
            RxMode(Rf24Controller& pController) : StateBase(pController){}
            virtual void ApplyTo(StateBase& other) {other.ToRxMode();};
            virtual void ToPowerDown();
            virtual void ToStandbyI();
            virtual void ToTxMode();
      };

      class TxMode : public StateBase {
         public:
            TxMode(Rf24Controller& pController) : StateBase(pController){}
            virtual void ApplyTo(StateBase& other) {other.ToTxMode();};
            virtual void ToPowerDown();
            virtual void ToStandbyI();
            virtual void ToRxMode();
      };


      // Constructor to prohibit copy construction.
      Rf24Controller(const Rf24Controller&);

      // Operator= to prohibit copy assignment
      Rf24Controller& operator=(const Rf24Controller&);

      void configureDevice();

      Configuration mConfiguration;
      Rf24Device* mDevice;
      PowerDown mPowerDown;
      StandbyI mStandbyI;
      RxMode mRxMode;
      TxMode mTxMode;
      StateBase* mCurrentState;

};

} // namespace Bt

#endif // INC__Bt_Rf24Controller__hpp
