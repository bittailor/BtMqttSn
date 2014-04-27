//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn, an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::MqttSnClient
//  
//*************************************************************************************************

#ifndef INC__Bt_MqttSnClient__hpp
#define INC__Bt_MqttSnClient__hpp

#include <stdio.h>
#include <string.h>

#include "Bt_MqttSnConfiguration.hpp"
#include "Bt_I_RfPacketSocket.hpp"

namespace Bt {

class MqttSnClient
{
   public:

      typedef void (*Callback)(const char* iTopic, const char* iData);


      enum { MAX_LENGTH_CLIENT_ID = 23 };
      enum { MAX_LENGTH_TOPIC_NAME = I_RfPacketSocket::PAYLOAD_CAPACITY - 7 };
      enum { MAX_LENGTH_DATA = I_RfPacketSocket::PAYLOAD_CAPACITY - 7 };

      MqttSnClient(I_RfPacketSocket& iSocket,
                   uint8_t iGatewayNodeId,
                   const char* iClientId,
                   Callback iCallback = 0);

      ~MqttSnClient();

      void loop();

      bool connect(uint16_t iKeepAliveTimerDuration);
      bool disconnect();

      bool registerTopic(const char* iTopic);
      bool publish(const char* iTopic, const char* iMessage, bool iRetain = false);
      bool publish(const char* iTopic, const uint8_t* iData, size_t iSize, bool iRetain = false);
      bool subscribe(const char* iTopic);







   
   private:

      class Topic {
         public:
            Topic() : mRegistered(false), mId(0), mName(){}

            bool define(uint16_t iId, const char* iName) {
               if(strlen(iName) > MAX_LENGTH_TOPIC_NAME) {
                  BT_LOG_WARNING_AND_PARAMETER("topic name too long: ", iName);
                  return false;
               }
               mRegistered = true;
               mId = iId;
               strncpy(mName,iName,sizeof(mName));
               return true;
            }

            bool isRegistred() const { return mRegistered; }
            uint16_t id() const { return mId; }
            const char* name() const { return mName; }

            bool matches(const char* iName) const { return strcmp(mName, iName) == 0; }
            bool matches(uint16_t iId) const { return mId == iId; }

         private:
            bool mRegistered;
            uint16_t mId;
            char mName[MAX_LENGTH_TOPIC_NAME + 1];
      };

      class Topics {
         public:

            bool add(uint16_t iId, const char* iName) {
               Topic* freeTopic = nextFree();
               if(freeTopic == 0) {
                  BT_LOG_WARNING("no free topic storage");
                  return false;
               }
               return freeTopic->define(iId, iName);
            }

            const Topic* findTopic(const char* iName) {
               for (size_t i = 0 ; i < BT_MQTTSN_MAX_NUMBER_OF_REGISTERED_TOPICS ; i++) {
                  if(mTopics[i].isRegistred() && mTopics[i].matches(iName) ) {
                     return &mTopics[i];
                  }
               }
               return 0;
            }

            const Topic* findTopic(uint16_t iId) {
               for (size_t i = 0 ; i < BT_MQTTSN_MAX_NUMBER_OF_REGISTERED_TOPICS ; i++) {
                  if(mTopics[i].isRegistred() && mTopics[i].matches(iId) ) {
                     return &mTopics[i];
                  }
               }
               return 0;
            }


         private:
            Topic* nextFree() {
               for (size_t i = 0 ; i < BT_MQTTSN_MAX_NUMBER_OF_REGISTERED_TOPICS ; i++) {
                  if(!mTopics[i].isRegistred()) {
                     return &mTopics[i];
                  }
               }
               return 0;
            }

            Topic mTopics[BT_MQTTSN_MAX_NUMBER_OF_REGISTERED_TOPICS];
      };


        // Constructor to prohibit copy construction.
      MqttSnClient(const MqttSnClient&);

      // Operator= to prohibit copy assignment
      MqttSnClient& operator=(const MqttSnClient&);

      bool pollLoop(uint8_t* oBuffer, uint8_t msgType);

      bool handleLoop(uint8_t* oBuffer, uint8_t msgType);
      bool reveiveLoop(uint8_t* oBuffer);

      void handleInternal(uint8_t* iBuffer);
      void handlePublish(uint8_t* iBuffer);
      void handleRegister(uint8_t* iBuffer);
      void handlePingRequest(uint8_t* iBuffer);
      void handlePingResponse(uint8_t* iBuffer);

      bool send(uint8_t* iPayload, size_t iSize);

      I_RfPacketSocket* mSocket;
      uint8_t mGatewayNodeId;
      char mClientId[MAX_LENGTH_CLIENT_ID + 1];
      Topics mTopics;
      uint16_t mMsgIdCounter;
      Callback mCallback;
      unsigned long mKeepAliveTimerDuration;
      unsigned long mKeepAliveTimerLastSend;

};

} // namespace Bt

#endif // INC__Bt_MqttSnClient__hpp
