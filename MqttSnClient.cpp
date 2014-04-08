//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn , an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  MqttSnClient
//  
//*************************************************************************************************

#include "MqttSnClient.hpp"



//-------------------------------------------------------------------------------------------------

MqttSnClient::MqttSnClient(uint8_t iChipEnable, uint8_t iChipSelect,
                           uint8_t iClientNodeId, uint8_t iGatewayNodeId,
                           const char* iClientId, Callback iCallback)
: mChipEnable(iChipEnable, Bt::Pin::MODE_OUTPUT)
, mChipSelect(iChipSelect, Bt::Pin::MODE_OUTPUT)
, mSpi(Bt::Spi::BIT_ORDER_MSBFIRST,
       Bt::Spi::MODE_0,
       Bt::Spi::SPEED_8_MHZ,
       mChipSelect)
, mDevice(mSpi,mChipEnable)
, mController(mDevice)
, mNetworkSocket(iClientNodeId, mController)
, mPacketSocket(mNetworkSocket)
, mClient(mPacketSocket, iGatewayNodeId, iClientId, iCallback){

}

//-------------------------------------------------------------------------------------------------

MqttSnClient::~MqttSnClient() {

}

//-------------------------------------------------------------------------------------------------

