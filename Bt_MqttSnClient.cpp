//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn, an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::MqttSnClient
//  
//*************************************************************************************************

#include "Bt_MqttSnClient.hpp"

#include "Bt_MqttSnConfiguration.hpp"

#include <stdio.h>
#include <string.h>

namespace {

//-------------------------------------------------------------------------------------------------

uint16_t inline bswap(const uint16_t iValue) {
    return (iValue << 8) | (iValue >> 8);
}

//-------------------------------------------------------------------------------------------------

enum ProtocolId {
      PROTOCOL_ID_1_2 = 0x01
};

//-------------------------------------------------------------------------------------------------

enum  MsgType  {
      ADVERTISE = 0x00,
      SEARCHGW,
      GWINFO,
      CONNECT = 0x04,
      CONNACK,
      WILLTOPICREQ,
      WILLTOPIC,
      WILLMSGREQ,
      WILLMSG,
      REGISTER,
      REGACK,
      PUBLISH,
      PUBACK,
      PUBCOMP,
      PUBREC,
      PUBREL,
      SUBSCRIBE = 0x12,
      SUBACK,
      UNSUBSCRIBE,
      UNSUBACK,
      PINGREQ,
      PINGRESP,
      DISCONNECT,
      WILLTOPICUPD = 0x1a,
      WILLTOPICRESP,
      WILLMSGUPD,
      WILLMSGRESP
};

//-------------------------------------------------------------------------------------------------

enum ReturnCode {
   ACCEPTED = 0x00,
   REJECTED_CONGESTION,
   REJECTED_INVALID_TOPIC_ID,
   REJECTED_NOT_SUPPORTED,
};

//-------------------------------------------------------------------------------------------------

union Flags {
      uint8_t byte;
      struct
      {
            bool dup             : 1;
            uint8_t qos          : 2;
            bool retain          : 1;
            bool will            : 1;
            bool cleanSession    : 1;
            uint8_t topicIdType  : 2;
      } bits;
};

//-------------------------------------------------------------------------------------------------

struct Header {
      uint8_t length;
      uint8_t msgType;
};

//-------------------------------------------------------------------------------------------------

struct Connect {
      Header header;
      Flags flags;
      uint8_t protocolId;
      uint16_t duration;
      char clientId[0];

      void initialize(const char* iClientId, uint16_t iKeepAliveTimerDuration, bool iCleanSession = true) {
         header.length = sizeof(Connect) + strlen(iClientId);
         header.msgType = CONNECT;
         flags.bits.dup = false;
         flags.bits.qos = 0;
         flags.bits.retain = false;
         flags.bits.will = false; //TODO (BT) implement support for will topic!
         flags.bits.cleanSession = iCleanSession;
         flags.bits.topicIdType = 0;
         protocolId = PROTOCOL_ID_1_2;
         duration = bswap(iKeepAliveTimerDuration);
         memcpy(clientId, iClientId, strlen(iClientId));

      }

      void setDuration(uint16_t iDuration) {
         duration = bswap(iDuration);
      }

      uint16_t getDuration() {
         return bswap(duration);
      }

};

//-------------------------------------------------------------------------------------------------

struct Connack {
      Header header;
      uint8_t returnCode;
};

//-------------------------------------------------------------------------------------------------

struct Register {
      Header header;
      uint16_t topicId;
      uint16_t msgId;
      char topicName[0];

      void initialize(uint16_t iMsgId, const char* iTopicName) {
         header.length = sizeof(Register) + strlen(iTopicName);
         header.msgType = REGISTER;
         topicId = 0x0000;
         msgId = bswap(iMsgId);
         memcpy(topicName, iTopicName, strlen(iTopicName));
      }

      uint16_t getTopicId() {
         return bswap(topicId);
      }

      uint16_t getMsgId() {
         return bswap(msgId);
      }

};

struct Regack {
      Header header;
      uint16_t topicId;
      uint16_t msgId;
      uint8_t returnCode;

      void initialize(uint16_t iTopicId, uint16_t iMsgId, ReturnCode iReturnCode) {
         header.length = sizeof(Regack);
         header.msgType = REGACK;
         topicId = bswap(iTopicId);
         msgId = bswap(iMsgId);
         returnCode = iReturnCode;
      }

      uint16_t getTopicId() {
         return bswap(topicId);
      }

      uint16_t getMsgId() {
         return bswap(msgId);
      }


};

//-------------------------------------------------------------------------------------------------

struct Publish {
      Header header;
      Flags flags;
      uint16_t topicId;
      uint16_t msgId;
      uint8_t data[0];

      void initialize(bool iRetain, uint16_t iTopicId, const uint8_t* iData, size_t iSize) {
         header.length = sizeof(Publish) + iSize;
         header.msgType = PUBLISH;
         flags.bits.dup = false;
         flags.bits.qos = 0;
         flags.bits.retain = iRetain;
         flags.bits.will = false;
         flags.bits.cleanSession = true;
         flags.bits.topicIdType = 0;
         topicId = bswap(iTopicId);
         msgId = 0x0000; // not relevant for QOS 0
         memcpy(data, iData, iSize);
      }


      uint16_t getTopicId() {
         return bswap(topicId);
      }

      uint16_t getMsgId() {
         return bswap(msgId);
      }


};

//-------------------------------------------------------------------------------------------------

struct Subscribe {
      Header header;
      Flags flags;
      uint16_t msgId;
      union {
         char topicName[0];
         uint16_t topicId;
      };

      void initialize(uint16_t iMsgId, const char* iTopicName) {
         header.length = sizeof(Header) + sizeof(Flags) + sizeof(uint16_t) + strlen(iTopicName);
         header.msgType = SUBSCRIBE;
         flags.bits.dup = false;
         flags.bits.qos = 0;
         flags.bits.retain = false; // not used by Subscribe
         flags.bits.will = false; // not used by Subscribe
         flags.bits.cleanSession = false; // not used by Subscribe
         flags.bits.topicIdType = 0x0; //TODO (BT) currently only supporting TopicName, implement support for other topic types
         msgId = bswap(iMsgId);
         memcpy(topicName, iTopicName, strlen(iTopicName));
      }
};

//-------------------------------------------------------------------------------------------------

struct Suback {
      Header header;
      Flags flags;
      uint16_t topicId;
      uint16_t msgId;
      uint8_t returnCode;

      uint16_t getTopicId() {
         return bswap(topicId);
      }

      uint16_t getMsgId() {
         return bswap(msgId);
      }
};

//-------------------------------------------------------------------------------------------------

struct Disconnect {
      Header header;
      uint16_t duration;

      void initialize() {
         header.length = 2;
         header.msgType = DISCONNECT;
      }

      void initialize(uint16_t iDuration) {
         header.length = 4;
         header.msgType = DISCONNECT;
         duration = bswap(iDuration);
      }

      uint16_t getDuration() {
         return bswap(duration);
      }
};

//-------------------------------------------------------------------------------------------------

struct Pingreq {
      Header header;
      char clientId[0];

      void initialize() {
         header.length = 2;
         header.msgType = PINGREQ;
      }

      void initialize(const char* iClientId) {
         header.length = 2 + strlen(iClientId);
         header.msgType = PINGREQ;
         memcpy(clientId, iClientId, strlen(iClientId));
      }

};

//-------------------------------------------------------------------------------------------------

struct Pingresp {
      Header header;

      void initialize() {
         header.length = 2;
         header.msgType = PINGRESP;
      }
};

//-------------------------------------------------------------------------------------------------


} // namespace


namespace Bt {

//-------------------------------------------------------------------------------------------------

MqttSnClient::MqttSnClient(I_RfPacketSocket& iSocket,
                           uint8_t iGatewayNodeId,
                           const char* iClientId,
                           Callback iCallback)
: mSocket(&iSocket), mGatewayNodeId(iGatewayNodeId), mMsgIdCounter(0), mCallback(iCallback)
, mKeepAliveTimerDuration(0), mLastSendActivity(millis()), mLastReveiveActivity(millis()), mCurrentState(DISCONNECTED) {
   strncpy(mClientId, iClientId, MAX_LENGTH_CLIENT_ID);

}

//-------------------------------------------------------------------------------------------------

MqttSnClient::~MqttSnClient() {

}

//-------------------------------------------------------------------------------------------------

bool MqttSnClient::connect(uint16_t iKeepAliveTimerDuration) {
   mKeepAliveTimerDuration = iKeepAliveTimerDuration * 1000;

   mTopics.clear();

   uint8_t buffer[I_RfPacketSocket::PAYLOAD_CAPACITY+1] = {0};

   Connect* connect = reinterpret_cast<Connect*>(buffer);
   connect->initialize(mClientId, iKeepAliveTimerDuration);
   if (!send(buffer, connect->header.length))
   {
      BT_LOG_WARNING("send CONNECT failed");
      return false;
   }

   if(!pollLoop(buffer, CONNACK)) {
      BT_LOG_WARNING("wait for CONNACK timeout");
      return false;
   }

   Connack* connack = reinterpret_cast<Connack*>(buffer);

   if (connack->returnCode == ACCEPTED) {
      changeState(ACTIVE);
      return true;
   }

   changeState(DISCONNECTED);
   BT_LOG_WARNING_AND_PARAMETER("connect failed with: ", connack->returnCode);
   return false;
}

//-------------------------------------------------------------------------------------------------

bool MqttSnClient::disconnect() {
   uint8_t buffer[I_RfPacketSocket::PAYLOAD_CAPACITY+1] = {0};

   Disconnect* message = reinterpret_cast<Disconnect*>(buffer);
   message->initialize();
   if (!send(buffer, message->header.length))
   {
      BT_LOG_WARNING("send DISCONNECT failed");
      return false;
   }

   changeState(DISCONNECTED);

   if(!pollLoop(buffer, DISCONNECT)) {
      BT_LOG_WARNING("wait for DISCONNECT timeout");
      return false;
   }

   return true;

}

//-------------------------------------------------------------------------------------------------

bool MqttSnClient::registerTopic(const char* iTopic) {
   if (strlen(iTopic) > MAX_LENGTH_TOPIC_NAME) {
      BT_LOG_WARNING("topic name too long");
      return false;
   }
   uint8_t buffer[I_RfPacketSocket::PAYLOAD_CAPACITY+1] = {0};
   uint16_t msgId = mMsgIdCounter++;

   Register* message = reinterpret_cast<Register*>(buffer);
   message->initialize(msgId, iTopic);
   if (!send(buffer, message->header.length))
   {
      BT_LOG_WARNING("send REGISTER failed");
      return false;
   }

   if(!pollLoop(buffer, REGACK)) {
      BT_LOG_WARNING("wait for REGACK timeout");
      return false;
   }

   Regack* regack = reinterpret_cast<Regack*>(buffer);
   if(regack->getMsgId() != msgId) {
      BT_LOG_WARNING("Regack msgId mismatch");
      return false;
   }

   if(regack->returnCode != ACCEPTED) {
      BT_LOG_WARNING_AND_PARAMETER("register failed with :", regack->returnCode);
      return false;
   }

   if(!mTopics.add(regack->getTopicId(), iTopic)) {
      BT_LOG_WARNING_AND_PARAMETER("failed adding topic id :", regack->getTopicId());
      return false;
   }

   BT_LOG_INFO("topic registered:");
   BT_LOG_INFO_AND_PARAMETER("   id    :",regack->getTopicId());
   BT_LOG_INFO_AND_PARAMETER("   topic :",iTopic);

   return true;
}

//-------------------------------------------------------------------------------------------------

bool MqttSnClient::publish(const char* iTopic, const char* iMessage, bool iRetain)
{
   return publish(iTopic, reinterpret_cast<const uint8_t*>(iMessage), strlen(iMessage));
}

//-------------------------------------------------------------------------------------------------

bool MqttSnClient::publish(const char* iTopic,const uint8_t* iData, size_t iSize, bool iRetain) {
   if (iSize > MAX_LENGTH_DATA) {
      BT_LOG_WARNING("publish data too long");
      return false;
   }

   const Topic* topic = mTopics.findTopic(iTopic);
   if(topic == 0) {
      BT_LOG_DEBUG_AND_PARAMETER("register topic since not registered yet ", iTopic);
      if (!registerTopic(iTopic)) {
         return false;
      }
      topic = mTopics.findTopic(iTopic);
      if(topic == 0) {
         BT_LOG_WARNING_AND_PARAMETER("could not find registered topic ", iTopic);
         return false;
      }
   }

   uint8_t buffer[I_RfPacketSocket::PAYLOAD_CAPACITY+1] = {0};
   Publish* message = reinterpret_cast<Publish*>(buffer);
   message->initialize(iRetain, topic->id(), iData, iSize);
   if (!send(buffer, message->header.length))
   {
      BT_LOG_WARNING("send PUBLISH failed");
      return false;
   }

   return true;
}

//-------------------------------------------------------------------------------------------------

bool MqttSnClient::subscribe(const char* iTopic) {
   if (strlen(iTopic) > MAX_LENGTH_TOPIC_NAME) {
      BT_LOG_WARNING("topic name too long");
      return false;
   }

   uint8_t buffer[I_RfPacketSocket::PAYLOAD_CAPACITY+1] = {0};
   uint16_t msgId = mMsgIdCounter++;

   Subscribe* message = reinterpret_cast<Subscribe*>(buffer);
   message->initialize(msgId, iTopic);
   if (!send(buffer, message->header.length))
   {
      BT_LOG_WARNING("send SUBSCRIBE failed");
      return false;
   }

   if(!pollLoop(buffer, SUBACK)) {
      BT_LOG_WARNING("wait for SUBACK timeout");
      return false;
   }

   Suback* suback = reinterpret_cast<Suback*>(buffer);
   if (suback->getMsgId() != msgId) {
      BT_LOG_WARNING("Suback msgId mismatch");
      return false;
   }

   if (suback->returnCode != ACCEPTED) {
      BT_LOG_WARNING_AND_PARAMETER("subscribe failed with :", suback->returnCode);
      return false;
   }

   if (suback->getTopicId() != 0x0000) {
      if (!mTopics.add(suback->getTopicId(), iTopic)) {
         BT_LOG_WARNING_AND_PARAMETER("failed adding topic id :", suback->getTopicId());
         return false;
      }
   }

   BT_LOG_INFO("topic subscribed:");
   BT_LOG_INFO_AND_PARAMETER("   id    :",suback->getTopicId());
   BT_LOG_INFO_AND_PARAMETER("   topic :",iTopic);

   return true;
}

//-------------------------------------------------------------------------------------------------

bool MqttSnClient::sleep(uint16_t iSleepDuration) {
   uint8_t buffer[I_RfPacketSocket::PAYLOAD_CAPACITY+1] = {0};

   Disconnect* message = reinterpret_cast<Disconnect*>(buffer);
   message->initialize(iSleepDuration);
   if (!send(buffer, message->header.length))
   {
      BT_LOG_WARNING("send DISCONNECT failed");
      return false;
   }

   if(!pollLoop(buffer, DISCONNECT)) {
      BT_LOG_WARNING("wait for DISCONNECT timeout");
      return false;
   }

   changeState(ASLEEP);

   mSocket->suspend();

   return true;
}

//-------------------------------------------------------------------------------------------------

bool MqttSnClient::receivePendingMessages() {
   mSocket->resume();

   uint8_t buffer[I_RfPacketSocket::PAYLOAD_CAPACITY+1] = {0};

   Pingreq* message = reinterpret_cast<Pingreq*>(buffer);
   message->initialize(mClientId);
   if (!send(buffer, message->header.length))
   {
      BT_LOG_WARNING("send PINGREQ failed");
      return false;
   }

   changeState(AWAKE);

   if(!pollLoop(buffer, PINGRESP)) {
      BT_LOG_WARNING("wait for PINGRESP timeout");
      return false;
   }

   changeState(ASLEEP);

   mSocket->suspend();
   return true;
}

//-------------------------------------------------------------------------------------------------

bool MqttSnClient::wakeUp() {
   mSocket->resume();

   uint8_t buffer[I_RfPacketSocket::PAYLOAD_CAPACITY+1] = {0};
   Connect* connect = reinterpret_cast<Connect*>(buffer);
   connect->initialize(mClientId, mKeepAliveTimerDuration/1000, false);
   if (!send(buffer, connect->header.length))
   {
      BT_LOG_WARNING("send CONNECT failed");
      return false;
   }

   if(!pollLoop(buffer, CONNACK)) {
      BT_LOG_WARNING("wait for CONNACK timeout");
      return false;
   }

   Connack* connack = reinterpret_cast<Connack*>(buffer);

   if (connack->returnCode == ACCEPTED) {
      changeState(ACTIVE);
      return true;
   }

   changeState(DISCONNECTED);
   BT_LOG_WARNING_AND_PARAMETER("connect failed with: ", connack->returnCode);

   return false;
}

//-------------------------------------------------------------------------------------------------

bool MqttSnClient::loop() {
   if(mCurrentState == DISCONNECTED || mCurrentState == LOST) {
      return false;
   }

   uint8_t buffer[I_RfPacketSocket::PAYLOAD_CAPACITY+1] = {0};
   if(handleLoop(buffer, PUBLISH)) {
      handlePublish(buffer);
   }

   unsigned long now = millis();

   if (((now - mLastReveiveActivity) > mKeepAliveTimerDuration) || ((now - mLastSendActivity) > mKeepAliveTimerDuration)) {
      BT_LOG_INFO("send PINGREQ ...");
      Pingreq* message = reinterpret_cast<Pingreq*>(buffer);
      message->initialize();
      if (!send(buffer, message->header.length))
      {
         BT_LOG_WARNING("send PINGREQ failed");
         changeState(LOST);
         return false;
      }
      if(!pollLoop(buffer, PINGRESP)) {
         BT_LOG_WARNING("wait for PINGRESP timeout");
         changeState(LOST);
         return false;
      }
      BT_LOG_INFO(" ... PINGRESP received");
   }

   return true;
}

//-------------------------------------------------------------------------------------------------

bool MqttSnClient::pollLoop(uint8_t* oBuffer, uint8_t msgType) {
   unsigned long timeout = millis() + BT_MQTTSN_T_RETRY;
   while(!handleLoop(oBuffer, msgType)){
      if((BT_MQTTSN_T_RETRY != 0) &&  (millis() > timeout) ) {
         return false;
      }
   }

   return true;
}

//-------------------------------------------------------------------------------------------------

bool MqttSnClient::handleLoop(uint8_t* oBuffer, uint8_t msgType) {
   if(!reveiveLoop(oBuffer)){
      return false;
   }

   mLastReveiveActivity = millis();

   Header* header = reinterpret_cast<Header*>(oBuffer);

   if(header->msgType == msgType) {
      return true;
   }

   handleInternal(oBuffer);
   return false;
}

//-------------------------------------------------------------------------------------------------

bool MqttSnClient::reveiveLoop(uint8_t* oBuffer) {
   if(!mSocket->available()) {
      return false;
   }

   uint8_t receiveNodeId;
   int32_t size = mSocket->receive(oBuffer, I_RfPacketSocket::PAYLOAD_CAPACITY, &receiveNodeId);
   if (size < 0) {
      BT_LOG_WARNING("receive failed");
      return false;
   }

   if (receiveNodeId != mGatewayNodeId) {
      BT_LOG_WARNING("drop not gateway packet");
      return false;
   }

   if (size < sizeof(Header)) {
      BT_LOG_WARNING("invalid message size");
      return false;
   }

   Header* header = reinterpret_cast<Header*>(oBuffer);

   if (size != header->length) {
      BT_LOG_WARNING("size length mismatch");
      return false;
   }

   return true;

}

//-------------------------------------------------------------------------------------------------

void MqttSnClient::handleInternal(uint8_t* iBuffer) {
   Header* header = reinterpret_cast<Header*>(iBuffer);
   switch(header->msgType) {
      case PUBLISH   : handlePublish(iBuffer);      return;
      case REGISTER  : handleRegister(iBuffer);     return;
      case PINGREQ   : handlePingRequest(iBuffer);  return;
      case PINGRESP  : handlePingResponse(iBuffer); return;
   }

   BT_LOG_WARNING_AND_PARAMETER("drop unexpected/unsupported message of type ", header->msgType);
}

//-------------------------------------------------------------------------------------------------

void MqttSnClient::handlePublish(uint8_t* iBuffer) {
   if (mCallback == 0) {
      BT_LOG_WARNING("no callback set => drop message");
      return;
   }

   Publish* message = reinterpret_cast<Publish*>(iBuffer);
   const Topic* topic = mTopics.findTopic(message->getTopicId());
   if (topic == 0) {
      BT_LOG_WARNING_AND_PARAMETER("topic id not found  => drop message: ", message->getTopicId());
      mCallback("?", reinterpret_cast<char*>(message->data));
      return;
   }

   mCallback(topic->name(), reinterpret_cast<char*>(message->data));
}

//-------------------------------------------------------------------------------------------------

void MqttSnClient::handleRegister(uint8_t* iBuffer) {
   Register* message = reinterpret_cast<Register*>(iBuffer);
   BT_LOG_INFO("register topic:");
   BT_LOG_INFO_AND_PARAMETER("   id:   ", message->getTopicId());
   BT_LOG_INFO_AND_PARAMETER("   name: ", message->topicName);

   ReturnCode returnCode = ACCEPTED;

   if (!mTopics.add(message->getTopicId(), message->topicName)) {
      BT_LOG_WARNING_AND_PARAMETER("failed adding topic id :", message->getTopicId());
      returnCode = REJECTED_NOT_SUPPORTED;
   }

   uint8_t buffer[I_RfPacketSocket::PAYLOAD_CAPACITY+1] = {0};
   Regack* ack = reinterpret_cast<Regack*>(buffer);
   ack->initialize(message->getTopicId(), message->getMsgId(), returnCode);
   if (!send(buffer, ack->header.length))
   {
      BT_LOG_ERROR("send REGACK failed");
   }
}

//-------------------------------------------------------------------------------------------------

void MqttSnClient::handlePingRequest(uint8_t* iBuffer) {
   uint8_t buffer[I_RfPacketSocket::PAYLOAD_CAPACITY+1] = {0};
   Pingresp* response = reinterpret_cast<Pingresp*>(buffer);
   response->initialize();
   if (!send(buffer, response->header.length))
   {
      BT_LOG_ERROR("send PINGRESP failed");
   }
}

//-------------------------------------------------------------------------------------------------

void MqttSnClient::handlePingResponse(uint8_t* iBuffer) {
   BT_LOG_INFO("handle PingResponse");
}

//-------------------------------------------------------------------------------------------------

bool MqttSnClient::send(uint8_t* iPayload, size_t iSize) {
   mLastSendActivity = millis();
   return mSocket->send(iPayload, iSize, mGatewayNodeId);
}

//-------------------------------------------------------------------------------------------------

} // namespace Bt
