//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn
//
//-------------------------------------------------------------------------------------------------
//
//  MqttSnClient
//  
//*************************************************************************************************

#include "MqttSnClient.hpp"
#include "BtMqttSnConfiguration.hpp"

#include <stdio.h>
#include <string.h>

//-------------------------------------------------------------------------------------------------

namespace {

uint16_t inline bswap(const uint16_t iValue) {
    return (iValue << 8) | (iValue >> 8);
}


enum ProtocolId {
      PROTOCOL_ID_1_2 = 0x01
};


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

enum ReturnCode {
   ACCEPTED = 0x00,
   REJECTED_CONGESTION,
   REJECTED_INVALID_TOPIC_ID,
   REJECTED_NOT_SUPPORTED,
};

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

struct Header {
      uint8_t length;
      uint8_t msgType;
};

struct Connect {
      Header header;
      Flags flags;
      uint8_t protocolId;
      uint16_t duration;
      char clientId[0];

      void initialize(const char* pClientId) {
         header.length = sizeof(Connect) + strlen(pClientId);
         header.msgType = CONNECT;
         flags.bits.dup = false;
         flags.bits.qos = 0;
         flags.bits.retain = false;
         flags.bits.will = false; //TODO (BT) implement support for will topic!
         flags.bits.cleanSession = true;
         flags.bits.topicIdType = 0;
         protocolId = PROTOCOL_ID_1_2;
         duration = 0xFFFF; //TODO (BT) implement support duration of Keep Alive timer
         memcpy(clientId, pClientId, strlen(pClientId));

      }

      void setDuration(uint16_t pDuration) {
         duration = bswap(pDuration);
      }

      uint16_t getDuration() {
         return bswap(duration);
      }

};

struct Connack {
      Header header;
      uint8_t returnCode;
};

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

};

struct Regack {
      Header header;
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

struct Disconnect {
      Header header;
      uint16_t duration;

      void initialize() {
         header.length = 2;
         header.msgType = DISCONNECT;
      }

      void setDuration(uint16_t pDuration) {
         duration = bswap(pDuration);
      }

      uint16_t getDuration() {
         return bswap(duration);
      }
};



} // namespace




//-------------------------------------------------------------------------------------------------

MqttSnClient::MqttSnClient(I_RfPacketSocket& iSocket, uint8_t iGatewayNodeId, const char* iClientId)
: mSocket(&iSocket), mGatewayNodeId(iGatewayNodeId), mMsgIdCounter(0), mListener(0) {
   strncpy(mClientId, iClientId, MAX_LENGTH_CLIENT_ID);

}

//-------------------------------------------------------------------------------------------------

MqttSnClient::~MqttSnClient() {

}

//-------------------------------------------------------------------------------------------------

bool MqttSnClient::connect() {
   uint8_t buffer[I_RfPacketSocket::PAYLOAD_CAPACITY] = {0};

   Connect* connect = reinterpret_cast<Connect*>(buffer);
   connect->initialize(mClientId);
   if (!mSocket->send(buffer, connect->header.length, mGatewayNodeId))
   {
      BT_LOG_MESSAGE("send Connect failed");
      return false;
   }

   pollLoop(buffer, CONNACK);

   Connack* connack = reinterpret_cast<Connack*>(buffer);

   if (connack->returnCode == ACCEPTED) {
      return true;
   }

   BT_LOG_MESSAGE_AND_PARAMETER("connect failed with: ", connack->returnCode);

   return false;
}

//-------------------------------------------------------------------------------------------------

bool MqttSnClient::disconnect() {
   uint8_t buffer[I_RfPacketSocket::PAYLOAD_CAPACITY] = {0};

   Disconnect* message = reinterpret_cast<Disconnect*>(buffer);
   message->initialize();
   if (!mSocket->send(buffer, message->header.length, mGatewayNodeId))
   {
      BT_LOG_MESSAGE("send Disconnect failed");
      return false;
   }

   pollLoop(buffer, DISCONNECT);

   return true;

}

//-------------------------------------------------------------------------------------------------

bool MqttSnClient::registerTopic(const char* iTopic) {
   if (strlen(iTopic) > MAX_LENGTH_TOPIC_NAME) {
      BT_LOG_MESSAGE("topic name too long");
      return false;
   }
   uint8_t buffer[I_RfPacketSocket::PAYLOAD_CAPACITY] = {0};
   uint16_t msgId = mMsgIdCounter++;

   Register* message = reinterpret_cast<Register*>(buffer);
   message->initialize(msgId, iTopic);
   if (!mSocket->send(buffer, message->header.length, mGatewayNodeId))
   {
      BT_LOG_MESSAGE("send Register failed");
      return false;
   }

   pollLoop(buffer, REGACK);

   Regack* regack = reinterpret_cast<Regack*>(buffer);
   if(regack->getMsgId() != msgId) {
      BT_LOG_MESSAGE("Regack msgId mismatch");
      return false;
   }

   if(regack->returnCode != ACCEPTED) {
      BT_LOG_MESSAGE_AND_PARAMETER("register failed with :", regack->returnCode);
      return false;
   }

   if(!mTopics.add(regack->getTopicId(), iTopic)) {
      BT_LOG_MESSAGE_AND_PARAMETER("failed adding topic id :", regack->getTopicId());
      return false;
   }

   BT_LOG_MESSAGE("topic registered:");
   BT_LOG_MESSAGE_AND_PARAMETER("   id    :",regack->getTopicId());
   BT_LOG_MESSAGE_AND_PARAMETER("   topic :",iTopic);

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
      BT_LOG_MESSAGE("publish data too long");
      return false;
   }

   const Topic* topic = mTopics.findTopic(iTopic);
   if(topic == 0) {
      if (!registerTopic(iTopic)) {
         return false;
      }
      topic = mTopics.findTopic(iTopic);
      if(topic == 0) {
         BT_LOG_MESSAGE_AND_PARAMETER("could not find registered topic ", iTopic);
         return false;
      }
   }

   uint8_t buffer[I_RfPacketSocket::PAYLOAD_CAPACITY] = {0};
   Publish* message = reinterpret_cast<Publish*>(buffer);
   message->initialize(iRetain, topic->id(), iData, iSize);
   if (!mSocket->send(buffer, message->header.length, mGatewayNodeId))
   {
      BT_LOG_MESSAGE("send Publish failed");
      return false;
   }

   return true;
}

//-------------------------------------------------------------------------------------------------

void MqttSnClient::loop() {
   uint8_t buffer[I_RfPacketSocket::PAYLOAD_CAPACITY] = {0};
   if(!handleLoop(buffer, PUBLISH)) {
      return;
   }
   handlePublish(buffer);
}

//-------------------------------------------------------------------------------------------------

void MqttSnClient::pollLoop(uint8_t* oBuffer, uint8_t msgType) {
   while(!handleLoop(oBuffer, msgType)){}
}

//-------------------------------------------------------------------------------------------------

bool MqttSnClient::handleLoop(uint8_t* oBuffer, uint8_t msgType) {
   if(!reveiveLoop(oBuffer)){
      return false;
   }

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
      BT_LOG_MESSAGE("receive failed");
      return false;
   }

   if (receiveNodeId != mGatewayNodeId) {
      BT_LOG_MESSAGE("drop not gateway packet");
      return false;
   }

   if (size < sizeof(Header)) {
      BT_LOG_MESSAGE("invalid message size");
      return false;
   }

   Header* header = reinterpret_cast<Header*>(oBuffer);

   if (size != header->length) {
      BT_LOG_MESSAGE("size length mismatch");
      return false;
   }

   return true;

}

//-------------------------------------------------------------------------------------------------

void MqttSnClient::handleInternal(uint8_t* iBuffer) {
   Header* header = reinterpret_cast<Header*>(iBuffer);
   switch(header->msgType) {
      case PUBLISH   : handlePublish(iBuffer);     break;
      case REGISTER  : handleRegister(iBuffer);    break;


   }
}

//-------------------------------------------------------------------------------------------------

void MqttSnClient::handlePublish(uint8_t* iBuffer) {
   if (mListener == 0) {
      BT_LOG_MESSAGE("no listener drop message");
      return;
   }

   Publish* message = reinterpret_cast<Publish*>(iBuffer);
   const Topic* topic = mTopics.findTopic(message->getTopicId());
   if (topic == 0) {
      BT_LOG_MESSAGE_AND_PARAMETER("topic id not found: ", message->getTopicId());
   }

   mListener->messageArrived(topic->name(), message->data, message->header.length - sizeof(Publish));
}

//-------------------------------------------------------------------------------------------------

void MqttSnClient::handleRegister(uint8_t* iBuffer) {

}

//-------------------------------------------------------------------------------------------------

