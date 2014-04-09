//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn, an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Rf24NetworkSocket
//  
//*************************************************************************************************

#ifndef INC__Bt_Rf24NetworkSocket__hpp
#define INC__Bt_Rf24NetworkSocket__hpp

#include "Bt_Rf24Controller.hpp"
#include "Bt_Rf24Node.hpp"
#include "Bt_Rf24NetworkRoutingAlgorithm.hpp"
#include "Bt_StaticRingBuffer.hpp"

namespace Bt {

class Rf24NetworkSocket
{
   private:
      enum { FRAME_BUFFER_SIZE = Rf24Controller::MAX_PAYLOAD_SIZE };
      enum { HEADER_SIZE = 2 };
      enum { PACKET_STORAGE_SIZE = 6 };

   public:

      class Packet {
         public:
            enum { HEADER_SIZE = 3 };
            enum { PAYLOAD_CAPACITY = Rf24Controller::Packet::BUFFER_CAPACITY - HEADER_SIZE };

            Packet() {
               mControllerPackage.size(HEADER_SIZE);
            }

            uint8_t source() {
               return mControllerPackage.buffer()[0];
            }

            uint8_t destination() {
               return mControllerPackage.buffer()[1];
            }

            uint8_t id() {
               return mControllerPackage.buffer()[2];
            }

            void destination(uint8_t destination) {
               mControllerPackage.buffer()[1] = destination;
            }

            const void* payload() {
               return payloadBuffer();
            }

            size_t size() {
               return mControllerPackage.size() - HEADER_SIZE;
            }

            size_t writePayload(const void* iBuffer, size_t iSize) {
               if (iSize > PAYLOAD_CAPACITY ) {
                  iSize = PAYLOAD_CAPACITY;
               }
               memcpy(payloadBuffer(), iBuffer, iSize);
               mControllerPackage.size(iSize + HEADER_SIZE);
               return iSize;
            }

            size_t readPayload(void* oBuffer, size_t iMaxSize) {
               size_t readSize = size();
               if (readSize > iMaxSize) {
                  readSize = iMaxSize;
               }
               memcpy(oBuffer, payload(), readSize);
               return readSize;
            }

            void copyInto(Rf24NetworkSocket::Packet& iPacket) const {
               mControllerPackage.copyInto(iPacket.mControllerPackage);
            }

            friend class Rf24NetworkSocket;

         private:
            void* payloadBuffer() {
               return mControllerPackage.buffer() + HEADER_SIZE;
            }

            void source(uint8_t pSource) {
               mControllerPackage.buffer()[0] = pSource;
            }

            void id(uint8_t pId) {
               mControllerPackage.buffer()[2] = pId;
            }

            Rf24Controller::Packet mControllerPackage;
      };

      Rf24NetworkSocket(Rf24Node pNodeId, Rf24Controller& pController);

      bool send(Packet& pPacket);
      bool receive(Packet& pPacket);
      bool available();

   private:

   	  // Constructor to prohibit copy construction.
      Rf24NetworkSocket(const Rf24NetworkSocket&);

      // Operator= to prohibit copy assignment
      Rf24NetworkSocket& operator=(const Rf24NetworkSocket&);

      void workcycle();
      bool sendInternal(Packet& pPacket);
      void receiveInternal(Packet& pPacket);


      Rf24Node mNodeId;
      Rf24Controller* mController;
      Rf24NetworkRoutingAlgorithm mRouting;
      uint8_t mIdCounter;
      StaticRingBuffer<Packet,PACKET_STORAGE_SIZE> mPackets;
};

} // namespace Bt

#endif // INC__Bt_Rf24NetworkSocket__hpp
