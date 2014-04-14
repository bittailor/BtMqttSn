//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn, an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Rf24Controller
//  
//*************************************************************************************************

#include <stdio.h>

#include "Bt_MqttSnConfiguration.hpp"
#include "Bt_Rf24Controller.hpp"

namespace Bt {

//-------------------------------------------------------------------------------------------------

Rf24Controller::Rf24Controller(Rf24Device& pDevice)
: mDevice(&pDevice), mPowerDown(*this), mStandbyI(*this), mRxMode(*this), mTxMode(*this), mCurrentState(&mPowerDown) {
}

//-------------------------------------------------------------------------------------------------

Rf24Controller::~Rf24Controller() {

}

//-------------------------------------------------------------------------------------------------

void Rf24Controller::configure(const Configuration& pConfiguration) {

   StateBase* originalState = mCurrentState;
   mCurrentState->ToPowerDown();
   mConfiguration = pConfiguration;
   originalState->ApplyTo(*mCurrentState);
}

//-------------------------------------------------------------------------------------------------

bool Rf24Controller::write(Rf24Pipes::Rf24Pipe pPipe, Packet& pPacket) {
   size_t size = write(pPipe, pPacket.buffer(), pPacket.size());
   return size != 0;
}

//-------------------------------------------------------------------------------------------------

size_t Rf24Controller::write(Rf24Pipes::Rf24Pipe pPipe, uint8_t* pData, size_t pSize) {

   StateBase* originalState = mCurrentState;
   mCurrentState->ToStandbyI();

   Rf24Address backupPipe0 = mDevice->receiveAddress(Rf24Pipes::PIPE_0);
   Rf24Address transmitPipeAddress = mDevice->receiveAddress(pPipe);
   mDevice->transmitAddress(transmitPipeAddress);
   mDevice->receiveAddress(Rf24Pipes::PIPE_0, transmitPipeAddress);

   mDevice->writeTransmitPayload(pData, pSize);

   mCurrentState->ToTxMode();

   Rf24Device::Status status = mDevice->status();

   unsigned long start = millis();
   bool timeout = false;
   while(!(status.dataSent() || status.retransmitsExceeded() || timeout ) ) {
      status = mDevice->status();
      delayMicroseconds(10);
      timeout = (millis() - start) > 10000;
   }

   size_t sentSize = pSize;

   if (status.retransmitsExceeded()) {
      mDevice->clearRetransmitsExceeded();
      BT_LOG_MESSAGE("TxMode::ToStandbyI: send failed retransmits exceeded ! \n");
      sentSize = 0;
   }


   if (timeout) {
      mDevice->clearRetransmitsExceeded();
      mDevice->flushTransmitFifo();
      BT_LOG_MESSAGE("TxMode::ToStandbyI: send failed timeout ! \n");
      sentSize = 0;
   }

   if (status.dataSent()) {
      mDevice->clearDataSent();
   }

   mDevice->receiveAddress(Rf24Pipes::PIPE_0, backupPipe0);

   mCurrentState->ToStandbyI();
   originalState->ApplyTo(*mCurrentState);

   return sentSize;
}

//-------------------------------------------------------------------------------------------------

void Rf24Controller::startListening() {
   mCurrentState->ToRxMode();
}

//-------------------------------------------------------------------------------------------------

void Rf24Controller::stopListening() {
   mCurrentState->ToStandbyI();
}

//-------------------------------------------------------------------------------------------------

bool Rf24Controller::isDataAvailable() {
   if (mDevice->isReceiveFifoEmpty()) {
      return false;
   }

   if (mDevice->availableReceivePayload() > MAX_PAYLOAD_SIZE) {
      mDevice->flushReceiveFifo();
      return false;
   }

   return true;
}

//-------------------------------------------------------------------------------------------------

bool Rf24Controller::read(Packet& pPacket) {
   Rf24Pipes::Rf24Pipe pipe;
   return read(pPacket, pipe);
}

//-------------------------------------------------------------------------------------------------

bool Rf24Controller::read(Packet& pPacket, Rf24Pipes::Rf24Pipe& pPipe) {
   if (mDevice->isReceiveFifoEmpty())
   {
      return false;
   }
   size_t size = mDevice->readReceivePayload(pPipe, pPacket.buffer(), Packet::BUFFER_CAPACITY);
   pPacket.size(size);
   if(mDevice->isReceiveFifoEmpty()) {
      mDevice->clearDataReady();
   }
   return true;
}

//-------------------------------------------------------------------------------------------------

size_t Rf24Controller::read(uint8_t* pBuffer, size_t pSize)
{
   Rf24Pipes::Rf24Pipe pipe;
   return read(pBuffer, pSize, pipe);
}

//-------------------------------------------------------------------------------------------------

size_t Rf24Controller::read(uint8_t* pData, size_t pSize, Rf24Pipes::Rf24Pipe& pPipe) {
   if (mDevice->isReceiveFifoEmpty())
   {
      return 0;
   }
   return mDevice->readReceivePayload(pPipe, pData, pSize);
}

//-------------------------------------------------------------------------------------------------

void Rf24Controller::configureDevice() {

   mDevice->dynamicPayloadFeatureEnabled(true);
   mDevice->autoRetransmitDelay(0x6);
   mDevice->autoRetransmitCount(0xf);
   mDevice->channel(80);
   mDevice->dataRate(Rf24Device::DR_1_MBPS);

   for(size_t i = 0; i < Rf24Pipes::NUMBER_OF_PIPES; i++) {
      Rf24Pipes::Rf24Pipe pipe = Rf24Pipes::ALL_PIPES[i];
      Configuration::PipeConfiguration& pipeConfiguration = mConfiguration[pipe];
      if(pipeConfiguration.mEnabled) {
         mDevice->receivePayloadSize(pipe, Rf24Device::MAX_PAYLOAD_SIZE);
         mDevice->receivePipeEnabled(pipe, true);
         mDevice->dynamicPayloadEnabled(pipe, true);
         mDevice->receiveAddress(pipe,pipeConfiguration.mAddress);
      } else {
         mDevice->receivePipeEnabled(pipe, false);
      }
   }
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

void Rf24Controller::PowerDown::ToStandbyI() {

   mController->mDevice->flushReceiveFifo();
   mController->mDevice->flushTransmitFifo();
   mController->configureDevice();

   mController->mDevice->powerUp(true);
   delayMicroseconds(150);

   mController->mCurrentState = &mController->mStandbyI;
}

//-------------------------------------------------------------------------------------------------

void Rf24Controller::PowerDown::ToRxMode() {
   ToStandbyI();
   mController->mCurrentState->ToRxMode();
}

//-------------------------------------------------------------------------------------------------

void Rf24Controller::PowerDown::ToTxMode() {
   ToStandbyI();
   mController->mCurrentState->ToTxMode();
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

void Rf24Controller::StandbyI::ToPowerDown() {
   mController->mDevice->powerUp(false);
   mController->mCurrentState = &mController->mPowerDown;
}

//-------------------------------------------------------------------------------------------------

void Rf24Controller::StandbyI::ToRxMode() {
   mController->mDevice->clearDataReady();
   mController->mDevice->clearDataSent();
   mController->mDevice->clearRetransmitsExceeded();

   mController->mDevice->transceiverMode(Rf24Device::RX_MODE);
   mController->mDevice->chipEnable(true);
   delayMicroseconds(130);
   mController->mCurrentState = &mController->mRxMode;
}

//-------------------------------------------------------------------------------------------------

void Rf24Controller::StandbyI::ToTxMode() {
   if (mController->mDevice->isTransmitFifoEmpty())
   {
      BT_LOG_MESSAGE("StandbyI::ToTxMode: transmit fifo is empty => StandbyI ! \n");
      return;
   }

   mController->mDevice->clearDataReady();
   mController->mDevice->clearDataSent();
   mController->mDevice->clearRetransmitsExceeded();

   mController->mDevice->transceiverMode(Rf24Device::TX_MODE);
   mController->mDevice->chipEnable(true);
   delayMicroseconds(140);
   mController->mCurrentState = &mController->mTxMode;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

void Rf24Controller::RxMode::ToPowerDown() {
   ToStandbyI();
   mController->mCurrentState->ToPowerDown();
}


//-------------------------------------------------------------------------------------------------

void Rf24Controller::RxMode::ToStandbyI() {
   mController->mDevice->chipEnable(false);
   mController->mDevice->transceiverMode(Rf24Device::TX_MODE);
   delayMicroseconds(10);
   mController->mCurrentState = &mController->mStandbyI;
}

//-------------------------------------------------------------------------------------------------

void Rf24Controller::RxMode::ToTxMode() {
   ToStandbyI();
   mController->mCurrentState->ToTxMode();
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

void Rf24Controller::TxMode::ToPowerDown() {
   ToStandbyI();
   mController->mCurrentState->ToPowerDown();
}

//-------------------------------------------------------------------------------------------------

void Rf24Controller::TxMode::ToStandbyI() {
   mController->mDevice->chipEnable(false);
   delayMicroseconds(10);
   mController->mCurrentState = &mController->mStandbyI;
}

//-------------------------------------------------------------------------------------------------

void Rf24Controller::TxMode::ToRxMode() {
   ToStandbyI();
   mController->mCurrentState->ToRxMode();
}

//-------------------------------------------------------------------------------------------------

} // namespace Bt
