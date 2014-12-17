//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn, an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  MqttSnClient
//  
//*************************************************************************************************

#ifndef INC__MqttSnClient__hpp
#define INC__MqttSnClient__hpp

#include "Bt_MqttSnClient.hpp"
#include "Bt_PlacementPointer.hpp"
#include "Bt_Rf24PacketSocket.hpp"


class MqttSnClient 
{
   public:
      typedef Bt::MqttSnClient::Callback Callback;

      MqttSnClient();
      ~MqttSnClient();
   

      void begin(uint8_t iChipEnable, uint8_t iChipSelect,
                 uint8_t iClientNodeId,  uint8_t iGatewayNodeId,
                 const char* iClientId, uint8_t iChannel, Callback iCallback = 0);
      bool loop();
      void end();

      bool connect(uint16_t iKeepAliveTimerDurationInSeconds = 30);
      bool disconnect();

      bool registerTopic(const char* iTopic);
      bool publish(const char* iTopic, const char* iMessage, bool iRetain = false);
      bool subscribe(const char* iTopic);

      bool sleep(uint16_t iSleepDuration);
      bool receivePendingMessages();
      bool wakeUp();

   private:
   	  // Constructor to prohibit copy construction.
      MqttSnClient(const MqttSnClient&);

      // Operator= to prohibit copy assignment
      MqttSnClient& operator=(const MqttSnClient&);

      Bt::PlacementPointer<Bt::Pin> mChipEnable;
      Bt::PlacementPointer<Bt::Pin> mChipSelect;
      Bt::PlacementPointer<Bt::Spi> mSpi;
      Bt::PlacementPointer<Bt::Rf24Device> mDevice;
      Bt::PlacementPointer<Bt::Rf24Controller> mController;
      Bt::PlacementPointer<Bt::Rf24NetworkSocket> mNetworkSocket;
      Bt::PlacementPointer<Bt::Rf24PacketSocket> mPacketSocket;
      Bt::PlacementPointer<Bt::MqttSnClient> mClient;

};

//-------------------------------------------------------------------------------------------------
// inline forwarding implementation
//-------------------------------------------------------------------------------------------------

inline bool MqttSnClient::loop() {
   return mClient->loop();
}

//-------------------------------------------------------------------------------------------------

inline bool MqttSnClient::connect(uint16_t iKeepAliveTimerDurationInSeconds) {
   return mClient->connect(iKeepAliveTimerDurationInSeconds);
}

//-------------------------------------------------------------------------------------------------

inline bool MqttSnClient::disconnect() {
   return mClient->disconnect();
}

//-------------------------------------------------------------------------------------------------

inline bool MqttSnClient::registerTopic(const char* iTopic) {
   return mClient->registerTopic(iTopic);
}

//-------------------------------------------------------------------------------------------------

inline bool MqttSnClient::publish(const char* iTopic, const char* iMessage, bool iRetain) {
   return mClient->publish(iTopic, iMessage, iRetain);
}

//-------------------------------------------------------------------------------------------------

inline bool MqttSnClient::subscribe(const char* iTopic) {
   return mClient->subscribe(iTopic);
}

//-------------------------------------------------------------------------------------------------

inline bool MqttSnClient::sleep(uint16_t iSleepDuration) {
   return mClient->sleep(iSleepDuration);
}

//-------------------------------------------------------------------------------------------------

inline bool MqttSnClient::receivePendingMessages() {
   return mClient->receivePendingMessages();
}

//-------------------------------------------------------------------------------------------------

inline bool MqttSnClient::wakeUp() {
   return mClient->wakeUp();
}

//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------

#endif // INC__MqttSnClient__hpp
