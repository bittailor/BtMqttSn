//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn , an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  MqttSnClient
//  
//*************************************************************************************************

#ifndef INC__MqttSnClient__hpp
#define INC__MqttSnClient__hpp

#include "Bt_MqttSnClient.hpp"
#include "Bt_Rf24PacketSocket.hpp"


class MqttSnClient 
{
   public:
      typedef Bt::MqttSnClient::Callback Callback;

      MqttSnClient(uint8_t iChipEnable, uint8_t iChipSelect,
                   uint8_t iClientNodeId,  uint8_t iGatewayNodeId,
                   const char* iClientId, Callback iCallback = 0);
      ~MqttSnClient();
   

      void begin();
      void loop();

      bool connect();
      bool disconnect();

      bool registerTopic(const char* iTopic);
      bool publish(const char* iTopic, const char* iMessage, bool iRetain = false);
      bool subscribe(const char* iTopic);


   private:
   	  // Constructor to prohibit copy construction.
      MqttSnClient(const MqttSnClient&);

      // Operator= to prohibit copy assignment
      MqttSnClient& operator=(const MqttSnClient&);

      Bt::Pin mChipEnable;
      Bt::Pin mChipSelect;
      Bt::Spi mSpi;
      Bt::Rf24Device mDevice;
      Bt::Rf24Controller mController;
      Bt::Rf24NetworkSocket mNetworkSocket;
      Bt::Rf24PacketSocket mPacketSocket;
      Bt::MqttSnClient mClient;

};

//-------------------------------------------------------------------------------------------------
// inline forwarding implementation
//-------------------------------------------------------------------------------------------------

inline void MqttSnClient::loop() {
   mClient.loop();
}

//-------------------------------------------------------------------------------------------------

inline bool MqttSnClient::connect() {
   return mClient.connect();
}

//-------------------------------------------------------------------------------------------------

inline bool MqttSnClient::disconnect() {
   return mClient.disconnect();
}

//-------------------------------------------------------------------------------------------------

inline bool MqttSnClient::registerTopic(const char* iTopic) {
   return mClient.registerTopic(iTopic);
}

//-------------------------------------------------------------------------------------------------

inline bool MqttSnClient::publish(const char* iTopic, const char* iMessage, bool iRetain) {
   return mClient.publish(iTopic, iMessage, iRetain);
}

//-------------------------------------------------------------------------------------------------

inline bool MqttSnClient::subscribe(const char* iTopic) {
   return mClient.subscribe(iTopic);
}

//-------------------------------------------------------------------------------------------------

#endif // INC__MqttSnClient__hpp
