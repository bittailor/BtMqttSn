//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn, an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  MqttSnClient
//  
//*************************************************************************************************

#include "MqttSnClient.hpp"

#include "Bt_ReferenceWrapper.hpp"



//-------------------------------------------------------------------------------------------------

MqttSnClient::MqttSnClient() {

}

//-------------------------------------------------------------------------------------------------

MqttSnClient::~MqttSnClient() {

}

//-------------------------------------------------------------------------------------------------

void MqttSnClient::begin(uint8_t iChipEnable, uint8_t iChipSelect,
                         uint8_t iClientNodeId,  uint8_t iGatewayNodeId,
                         const char* iClientId, Callback iCallback) {
   mChipEnable.create(iChipEnable, Bt::Pin::MODE_OUTPUT);
   mChipSelect.create(iChipSelect, Bt::Pin::MODE_OUTPUT);
   mSpi.create(
            Bt::Spi::BIT_ORDER_MSBFIRST,
            Bt::Spi::MODE_0,
            Bt::Spi::SPEED_8_MHZ,
            Bt::ref(*mChipSelect));
   mDevice.create<Bt::Spi&,Bt::Pin&>(*mSpi,*mChipEnable);
   mController.create<Bt::Rf24Device&>(*mDevice);
   mNetworkSocket.create(iClientNodeId, Bt::ref(*mController));
   mPacketSocket.create(Bt::ref(*mNetworkSocket));
   mClient.create(Bt::ref(*mPacketSocket), iGatewayNodeId, iClientId, iCallback);
}

//-------------------------------------------------------------------------------------------------

void MqttSnClient::end() {
   mClient.dispose();
   mPacketSocket.dispose();
   mNetworkSocket.dispose();
   mController.dispose();
   mDevice.dispose();
   mSpi.dispose();
   mChipSelect.dispose();
   mChipEnable.dispose();
}

//-------------------------------------------------------------------------------------------------


