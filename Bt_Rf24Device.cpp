//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn, an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Rf24Device
//  
//*************************************************************************************************

#include "Bt_Rf24Device.hpp"

#include <Arduino.h>
#include <stdint.h>
#include <stdio.h>


namespace Bt {

namespace {

enum Masks {
   MASK_REGISTER_CMD = 0x1F,
   MASK_RF_CH = 0x7F,
   MASK_ARC = 0x0F,
   MASK_ARD = 0xF0
};

enum Commands {
   CMD_R_REGISTER    = 0x00,
   CMD_W_REGISTER    = 0x20,
   CMD_R_RX_PAYLOAD  = 0x61,
   CMD_W_TX_PAYLOAD  = 0xA0,
   CMD_FLUSH_TX      = 0xE1,
   CMD_FLUSH_RX      = 0xE2,
   CMD_R_RX_PL_WID   = 0x60,
   CMD_NOP = 0xFF
};

enum OneByteRegister
{
   REGISTER_CONFIG      = 0x00,
   REGISTER_EN_AA       = 0x01,
   REGISTER_EN_RXADDR   = 0x02,
   REGISTER_SETUP_AW    = 0x03,
   REGISTER_SETUP_RETR  = 0x04,
   REGISTER_RF_CH       = 0x05,
   REGISTER_RF_SETUP    = 0x06,
   REGISTER_STATUS      = 0x07,
   REGISTER_OBSERVE_TX  = 0x08,
   REGISTER_RPD         = 0x09,
   REGISTER_RX_ADDR_P2  = 0x0C,
   REGISTER_RX_ADDR_P3  = 0x0D,
   REGISTER_RX_ADDR_P4  = 0x0E,
   REGISTER_RX_ADDR_P5  = 0x0F,
   REGISTER_RX_PW_P0    = 0x11,
   REGISTER_RX_PW_P1    = 0x12,
   REGISTER_RX_PW_P2    = 0x13,
   REGISTER_RX_PW_P3    = 0x14,
   REGISTER_RX_PW_P4    = 0x15,
   REGISTER_RX_PW_P5    = 0x16,
   REGISTER_FIFO_STATUS = 0x17,
   REGISTER_DYNPD       = 0x1C,
   REGISTER_FEATURE     = 0x1D
};

enum FiveByteRegister
{
   REGISTER_RX_ADDR_P0  = 0x0A,
   REGISTER_RX_ADDR_P1  = 0x0B,
   REGISTER_TX_ADDR     = 0x10
};

//-------------------------------------------------------------------------------------------------

inline uint8_t calculateMask(uint8_t pBitSize, uint8_t pOffset) {
   uint8_t mask = ((1 << pBitSize) - 1) << pOffset;
   return mask;
}

//-------------------------------------------------------------------------------------------------

uint8_t readRegister(Spi& pSpi, OneByteRegister pRegister)
{
   uint8_t cmd = CMD_R_REGISTER | (pRegister & MASK_REGISTER_CMD);
   pSpi.chipSelect(true);
   pSpi.transfer(cmd);
   uint8_t value = pSpi.transfer(CMD_NOP);
   pSpi.chipSelect(false);
   return value;
}



//-------------------------------------------------------------------------------------------------

uint8_t readSubRegister(Spi& pSpi, OneByteRegister pRegister,
                        uint8_t pBitSize, uint8_t pOffset)
{
   uint8_t mask = calculateMask(pBitSize,pOffset);
   uint8_t value = readRegister(pSpi, pRegister);
   value &= mask;
   value >>= pOffset;
   return value;
}

//-------------------------------------------------------------------------------------------------

StaticArray<uint8_t,5> readRegister(Spi& pSpi, FiveByteRegister pRegister)
{
   StaticArray<uint8_t,5> value;
   uint8_t cmd = CMD_R_REGISTER | (pRegister & MASK_REGISTER_CMD);

   pSpi.chipSelect(true);
   pSpi.transfer(cmd);
   for (size_t i = 0 ; i < value.size()  ; ++i) {
      value[i] = pSpi.transfer(CMD_NOP);
   }
   pSpi.chipSelect(false);

   return value;
}

//-------------------------------------------------------------------------------------------------

uint8_t writeRegister(Spi& pSpi, OneByteRegister pRegister, uint8_t pValue)
{
   uint8_t cmd = CMD_W_REGISTER | (pRegister & MASK_REGISTER_CMD);

   pSpi.chipSelect(true);
   uint8_t status = pSpi.transfer(cmd);
   pSpi.transfer(pValue);
   pSpi.chipSelect(false);
   return status;
}

//-------------------------------------------------------------------------------------------------

uint8_t writeSubRegister(Spi& pSpi, OneByteRegister pRegister, uint8_t pValue,
                         uint8_t pBitSize, uint8_t pOffset)
{
   uint8_t mask = calculateMask(pBitSize,pOffset);
   uint8_t value = readRegister(pSpi, pRegister) ;
   value = value & ~mask;
   pValue = pValue << pOffset;
   value = value | (pValue & mask) ;
   return writeRegister(pSpi, pRegister, value);
}

//-------------------------------------------------------------------------------------------------

uint8_t writeRegister(Spi& pSpi, FiveByteRegister pRegister, StaticArray<uint8_t,5> pValue)
{
   uint8_t cmd = CMD_W_REGISTER | (pRegister & MASK_REGISTER_CMD);

   pSpi.chipSelect(true);
   uint8_t status = pSpi.transfer(cmd);
   for (size_t i = 0 ; i < pValue.size() ; ++i) {
      pSpi.transfer(pValue[i]);
   }
   pSpi.chipSelect(false);
   return status;
}

//-------------------------------------------------------------------------------------------------

} // namespace

//-------------------------------------------------------------------------------------------------

Rf24Device::Rf24Device(Spi& pSpi, Pin& pChipEnable)
   : mSpi(&pSpi), mChipEnable(&pChipEnable)  {
   mChipEnable->write(false);
   delay(5);
}

//-------------------------------------------------------------------------------------------------

Rf24Device::~Rf24Device() {

}

//-------------------------------------------------------------------------------------------------

Rf24Device::Status Rf24Device::status()
{
   uint8_t status = readRegister(*mSpi, REGISTER_STATUS);
   //BT_LOG_MESSAGE("status = %02x \n",status);
   return Status(status & 0x40, status & 0x20, status & 0x10);
}

//-------------------------------------------------------------------------------------------------

void Rf24Device::clearDataReady() {
   writeSubRegister(*mSpi, REGISTER_STATUS,1,1,6);
}

//-------------------------------------------------------------------------------------------------

void Rf24Device::clearDataSent() {
   writeSubRegister(*mSpi, REGISTER_STATUS,1,1,5);
}

//-------------------------------------------------------------------------------------------------

void Rf24Device::clearRetransmitsExceeded() {
   writeSubRegister(*mSpi, REGISTER_STATUS,1,1,4);
}

//-------------------------------------------------------------------------------------------------

bool Rf24Device::powerUp() {
   return readSubRegister(*mSpi, REGISTER_CONFIG,1,1);
}

//-------------------------------------------------------------------------------------------------

void Rf24Device::powerUp(bool pValue) {
   writeSubRegister(*mSpi, REGISTER_CONFIG,pValue,1,1);
}

//-------------------------------------------------------------------------------------------------

Rf24Device::TransceiverMode Rf24Device::transceiverMode() {
   return (readSubRegister(*mSpi, REGISTER_CONFIG,1,0)) ? RX_MODE : TX_MODE;
}

//-------------------------------------------------------------------------------------------------

void Rf24Device::transceiverMode(TransceiverMode pMode) {
   writeSubRegister(*mSpi, REGISTER_CONFIG,pMode == RX_MODE,1,0);
}

//-------------------------------------------------------------------------------------------------

void Rf24Device::chipEnable(bool pValue) {
   mChipEnable->write(pValue);
}

//-------------------------------------------------------------------------------------------------

uint8_t Rf24Device::autoRetransmitDelay() {
   return readSubRegister(*mSpi, REGISTER_SETUP_RETR,4,4);
}

//-------------------------------------------------------------------------------------------------

void Rf24Device::autoRetransmitDelay(uint8_t pDelay) {
   writeSubRegister(*mSpi, REGISTER_SETUP_RETR, pDelay, 4, 4);
}

//-------------------------------------------------------------------------------------------------

uint8_t Rf24Device::autoRetransmitCount() {
   return readSubRegister(*mSpi, REGISTER_SETUP_RETR,4,0);
}

//-------------------------------------------------------------------------------------------------

void Rf24Device::autoRetransmitCount(uint8_t pCount) {
   writeSubRegister(*mSpi, REGISTER_SETUP_RETR, pCount, 4, 0);
}

//-------------------------------------------------------------------------------------------------

uint8_t Rf24Device::autoRetransmitCounter(){
   return readSubRegister(*mSpi, REGISTER_OBSERVE_TX,4,0);
}

//-------------------------------------------------------------------------------------------------

uint8_t Rf24Device::channel() {
   uint8_t value = readRegister(*mSpi, REGISTER_RF_CH) & MASK_RF_CH;
   return value & 0x7F;
}

//-------------------------------------------------------------------------------------------------

void Rf24Device::channel(uint8_t pChannel) {
   uint8_t value = pChannel & MASK_RF_CH;
   writeRegister(*mSpi, REGISTER_RF_CH, value);
}

//-------------------------------------------------------------------------------------------------

Rf24Device::DataRate Rf24Device::dataRate() {
   uint8_t low = readSubRegister(*mSpi, REGISTER_RF_SETUP, 1, 5);
   uint8_t high = readSubRegister(*mSpi, REGISTER_RF_SETUP, 1, 3);
   if(low) {
      return Rf24Device::DR_250_KBPS;
   }
   if(high) {
      return Rf24Device::DR_2_MBPS;
   }
   return Rf24Device::DR_1_MBPS;

}

//-------------------------------------------------------------------------------------------------

void Rf24Device::dataRate(DataRate pDataRate) {
   uint8_t low = (pDataRate == Rf24Device::DR_250_KBPS);
   uint8_t high = (pDataRate == Rf24Device::DR_2_MBPS);
   writeSubRegister(*mSpi, REGISTER_RF_SETUP, low, 1, 5);
   writeSubRegister(*mSpi, REGISTER_RF_SETUP, high, 1, 3);
}

//-------------------------------------------------------------------------------------------------

Rf24Address Rf24Device::receiveAddress(Rf24Pipes::Rf24Pipe pPipe) {
   switch (pPipe) {
      case Rf24Pipes::PIPE_0:
         return readRegister(*mSpi, REGISTER_RX_ADDR_P0);
      case Rf24Pipes::PIPE_1:
         return readRegister(*mSpi, REGISTER_RX_ADDR_P1);
      default:
         break;
   }

   Rf24Address base = readRegister(*mSpi, REGISTER_RX_ADDR_P1);

   OneByteRegister oneByteRegister = REGISTER_RX_ADDR_P2;
   switch (pPipe) {
      case Rf24Pipes::PIPE_2 : oneByteRegister = REGISTER_RX_ADDR_P2; break;
      case Rf24Pipes::PIPE_3 : oneByteRegister = REGISTER_RX_ADDR_P3; break;
      case Rf24Pipes::PIPE_4 : oneByteRegister = REGISTER_RX_ADDR_P4; break;
      case Rf24Pipes::PIPE_5 : oneByteRegister = REGISTER_RX_ADDR_P5; break;
      default     : oneByteRegister = REGISTER_RX_ADDR_P2; break;
   }

   base.raw()[0] = readRegister(*mSpi, oneByteRegister);

   return base;
}

//-------------------------------------------------------------------------------------------------

void Rf24Device::receiveAddress(Rf24Pipes::Rf24Pipe pPipe, Rf24Address pAddress) {
   switch (pPipe) {
      case Rf24Pipes::PIPE_0:
         writeRegister(*mSpi, REGISTER_RX_ADDR_P0, pAddress.raw());
         return;
      case Rf24Pipes::PIPE_1:
         writeRegister(*mSpi, REGISTER_RX_ADDR_P1, pAddress.raw());
         return;
      default:
         break;
   }

   OneByteRegister oneByteRegister = REGISTER_RX_ADDR_P2;
   switch (pPipe) {
      case Rf24Pipes::PIPE_2 : oneByteRegister = REGISTER_RX_ADDR_P2; break;
      case Rf24Pipes::PIPE_3 : oneByteRegister = REGISTER_RX_ADDR_P3; break;
      case Rf24Pipes::PIPE_4 : oneByteRegister = REGISTER_RX_ADDR_P4; break;
      case Rf24Pipes::PIPE_5 : oneByteRegister = REGISTER_RX_ADDR_P5; break;
      default     : oneByteRegister = REGISTER_RX_ADDR_P2; break;
   }

   writeRegister(*mSpi, oneByteRegister, pAddress.raw()[0]);
}

//-------------------------------------------------------------------------------------------------

bool Rf24Device::receivePipeEnabled(Rf24Pipes::Rf24Pipe pPipe) {
   uint8_t offset = 0;

   switch (pPipe) {
      case Rf24Pipes::PIPE_0 : offset = 0; break;
      case Rf24Pipes::PIPE_1 : offset = 1; break;
      case Rf24Pipes::PIPE_2 : offset = 2; break;
      case Rf24Pipes::PIPE_3 : offset = 3; break;
      case Rf24Pipes::PIPE_4 : offset = 4; break;
      case Rf24Pipes::PIPE_5 : offset = 5; break;
      default     : offset = 0; break;
   }

   return readSubRegister(*mSpi, REGISTER_EN_RXADDR, 1, offset);
}

//-------------------------------------------------------------------------------------------------

void Rf24Device::receivePipeEnabled(Rf24Pipes::Rf24Pipe pPipe, bool pValue) {
   uint8_t offset = 0;

   switch (pPipe) {
      case Rf24Pipes::PIPE_0 : offset = 0; break;
      case Rf24Pipes::PIPE_1 : offset = 1; break;
      case Rf24Pipes::PIPE_2 : offset = 2; break;
      case Rf24Pipes::PIPE_3 : offset = 3; break;
      case Rf24Pipes::PIPE_4 : offset = 4; break;
      case Rf24Pipes::PIPE_5 : offset = 5; break;
      default     : offset = 0; break;
   }

   writeSubRegister(*mSpi, REGISTER_EN_RXADDR, pValue, 1, offset);
}

//-------------------------------------------------------------------------------------------------

uint8_t Rf24Device::receivePayloadSize(Rf24Pipes::Rf24Pipe pPipe) {
   OneByteRegister oneByteRegister;
   switch (pPipe) {
      case Rf24Pipes::PIPE_0 : oneByteRegister = REGISTER_RX_PW_P0; break;
      case Rf24Pipes::PIPE_1 : oneByteRegister = REGISTER_RX_PW_P1; break;
      case Rf24Pipes::PIPE_2 : oneByteRegister = REGISTER_RX_PW_P2; break;
      case Rf24Pipes::PIPE_3 : oneByteRegister = REGISTER_RX_PW_P3; break;
      case Rf24Pipes::PIPE_4 : oneByteRegister = REGISTER_RX_PW_P4; break;
      case Rf24Pipes::PIPE_5 : oneByteRegister = REGISTER_RX_PW_P5; break;
      default     : oneByteRegister = REGISTER_RX_PW_P0; break;
   }

   return readSubRegister(*mSpi,oneByteRegister,6,0);
}

//-------------------------------------------------------------------------------------------------

void Rf24Device::receivePayloadSize(Rf24Pipes::Rf24Pipe pPipe, uint8_t pSize) {
   OneByteRegister oneByteRegister;
   switch (pPipe) {
      case Rf24Pipes::PIPE_0 : oneByteRegister = REGISTER_RX_PW_P0; break;
      case Rf24Pipes::PIPE_1 : oneByteRegister = REGISTER_RX_PW_P1; break;
      case Rf24Pipes::PIPE_2 : oneByteRegister = REGISTER_RX_PW_P2; break;
      case Rf24Pipes::PIPE_3 : oneByteRegister = REGISTER_RX_PW_P3; break;
      case Rf24Pipes::PIPE_4 : oneByteRegister = REGISTER_RX_PW_P4; break;
      case Rf24Pipes::PIPE_5 : oneByteRegister = REGISTER_RX_PW_P5; break;
      default     : oneByteRegister = REGISTER_RX_PW_P0; break;
   }
   writeSubRegister(*mSpi,oneByteRegister,pSize,6,0);
}

//-------------------------------------------------------------------------------------------------

Rf24Address Rf24Device::transmitAddress() {
   return readRegister(*mSpi, REGISTER_TX_ADDR);
}


//-------------------------------------------------------------------------------------------------

void Rf24Device::transmitAddress(Rf24Address pAddress) {
   writeRegister(*mSpi, REGISTER_TX_ADDR, pAddress.raw());
}

//-------------------------------------------------------------------------------------------------

bool Rf24Device::isTransmitFifoEmpty() {
   return readSubRegister(*mSpi, REGISTER_FIFO_STATUS,1,4);
}

//-------------------------------------------------------------------------------------------------

bool Rf24Device::isTransmitFifoFull() {
   return readSubRegister(*mSpi, REGISTER_FIFO_STATUS,1,5);
}

//-------------------------------------------------------------------------------------------------

void Rf24Device::flushTransmitFifo() {
   mSpi->chipSelect(true);
   mSpi->transfer(CMD_FLUSH_TX);
   mSpi->chipSelect(false);
}

//-------------------------------------------------------------------------------------------------

bool Rf24Device::isReceiveFifoEmpty() {
   return readSubRegister(*mSpi, REGISTER_FIFO_STATUS,1,0);
}

//-------------------------------------------------------------------------------------------------

bool Rf24Device::isReceiveFifoFull() {
   return readSubRegister(*mSpi, REGISTER_FIFO_STATUS,1,1);
}

//-------------------------------------------------------------------------------------------------

void Rf24Device::flushReceiveFifo() {
   mSpi->chipSelect(true);
   mSpi->transfer(CMD_FLUSH_RX);
   mSpi->chipSelect(false);
}

//-------------------------------------------------------------------------------------------------

size_t Rf24Device::writeTransmitPayload(uint8_t* pData, size_t pSize) {

   size_t dataSize;

   if (pSize <= MAX_PAYLOAD_SIZE) {
      dataSize = pSize;
   } else {
      dataSize = MAX_PAYLOAD_SIZE;
   }

   mSpi->chipSelect(true);
   mSpi->transfer(CMD_W_TX_PAYLOAD);
   for (size_t i = 0; i < dataSize ; ++i) {
      mSpi->transfer(pData[i]);
   }
   mSpi->chipSelect(false);

   return dataSize;
}

//-------------------------------------------------------------------------------------------------

size_t Rf24Device::availableReceivePayload() {
   mSpi->chipSelect(true);
   mSpi->transfer(CMD_R_RX_PL_WID);
   size_t size = mSpi->transfer(CMD_NOP);
   mSpi->chipSelect(false);
   return size;
}

//-------------------------------------------------------------------------------------------------

size_t Rf24Device::readReceivePayload(Rf24Pipes::Rf24Pipe& pPipe, uint8_t* pData, size_t pSize) {

   switch(readSubRegister(*mSpi,REGISTER_STATUS,3,1))
   {
      case 0 : pPipe = Rf24Pipes::PIPE_0 ; break;
      case 1 : pPipe = Rf24Pipes::PIPE_1 ; break;
      case 2 : pPipe = Rf24Pipes::PIPE_2 ; break;
      case 3 : pPipe = Rf24Pipes::PIPE_3 ; break;
      case 4 : pPipe = Rf24Pipes::PIPE_4 ; break;
      case 5 : pPipe = Rf24Pipes::PIPE_5 ; break;
   }

   size_t availableSize = availableReceivePayload();
   size_t dataSize;
   size_t stuffingSize;

   if (pSize >= availableSize) {
      dataSize = availableSize;
      stuffingSize = 0;
   } else {
      dataSize = pSize;
      stuffingSize = availableSize - pSize;
   }

   mSpi->chipSelect(true);
   mSpi->transfer(CMD_R_RX_PAYLOAD);
   for (size_t i = 0; i < dataSize ; ++i) {
      pData[i] = mSpi->transfer(CMD_NOP);
   }
   for (size_t i = 0; i < stuffingSize ; ++i) {
      mSpi->transfer(CMD_NOP);
   }
   mSpi->chipSelect(false);
   return dataSize;
}

//-------------------------------------------------------------------------------------------------

bool Rf24Device::dynamicPayloadFeatureEnabled() {
   return readSubRegister(*mSpi, REGISTER_FEATURE, 1, 2);
}

//-------------------------------------------------------------------------------------------------

void Rf24Device::dynamicPayloadFeatureEnabled(bool pValue) {
   writeSubRegister(*mSpi, REGISTER_FEATURE, pValue, 1, 2);
}

//-------------------------------------------------------------------------------------------------

bool Rf24Device::dynamicPayloadEnabled(Rf24Pipes::Rf24Pipe pPipe) {
   uint8_t offset = 0;

   switch (pPipe) {
      case Rf24Pipes::PIPE_0 : offset = 0; break;
      case Rf24Pipes::PIPE_1 : offset = 1; break;
      case Rf24Pipes::PIPE_2 : offset = 2; break;
      case Rf24Pipes::PIPE_3 : offset = 3; break;
      case Rf24Pipes::PIPE_4 : offset = 4; break;
      case Rf24Pipes::PIPE_5 : offset = 5; break;
      default     : offset = 0; break;
   }

   return readSubRegister(*mSpi, REGISTER_DYNPD, 1, offset);
}

//-------------------------------------------------------------------------------------------------

void Rf24Device::dynamicPayloadEnabled(Rf24Pipes::Rf24Pipe pPipe, bool pValue) {
   uint8_t offset = 0;

   switch (pPipe) {
      case Rf24Pipes::PIPE_0 : offset = 0; break;
      case Rf24Pipes::PIPE_1 : offset = 1; break;
      case Rf24Pipes::PIPE_2 : offset = 2; break;
      case Rf24Pipes::PIPE_3 : offset = 3; break;
      case Rf24Pipes::PIPE_4 : offset = 4; break;
      case Rf24Pipes::PIPE_5 : offset = 5; break;
      default     : offset = 0; break;
   }

   writeSubRegister(*mSpi, REGISTER_DYNPD, pValue, 1, offset);
}

//-------------------------------------------------------------------------------------------------

} // namespace Bt
