//*************************************************************************************************
//
//  BITTAILOR.CH - BtCore
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Mcu::Spi
//  
//*************************************************************************************************

#include "Spi.hpp"

#include <Arduino.h>
#include <SPI.h>


//-------------------------------------------------------------------------------------------------

Spi::Spi(BitOrder pBitOrder, Mode pSpiMode, Speed pSpeed, Pin& pChipSelectPin)
   : mChipSelectPin(&pChipSelectPin) {

   SPI.begin();

   switch (pBitOrder) {
      case Spi::BIT_ORDER_LSBFIRST : SPI.setBitOrder(LSBFIRST); break;
      case Spi::BIT_ORDER_MSBFIRST : SPI.setBitOrder(MSBFIRST); break;
   }

   switch (pSpiMode) {
      case Spi::MODE_0 : SPI.setDataMode(SPI_MODE0); break;
      case Spi::MODE_1 : SPI.setDataMode(SPI_MODE1); break;
      case Spi::MODE_2 : SPI.setDataMode(SPI_MODE2); break;
      case Spi::MODE_3 : SPI.setDataMode(SPI_MODE3); break;
   }

   //TODO (BT) check/calculate from  F_CPU

   switch (pSpeed) {
      case Spi::SPEED_4_MHZ : SPI.setClockDivider(SPI_CLOCK_DIV4); break;
      case Spi::SPEED_8_MHZ : SPI.setClockDivider(SPI_CLOCK_DIV2); break;
   }

   mChipSelectPin->mode(Pin::MODE_OUTPUT);
   mChipSelectPin->write(true);
}

//-------------------------------------------------------------------------------------------------

Spi::~Spi() {

}

//-------------------------------------------------------------------------------------------------


void Spi::chipSelect(bool iSelect) {
   mChipSelectPin->write(!iSelect);
}

//-------------------------------------------------------------------------------------------------

uint8_t Spi::transfer(uint8_t iData) {
   return SPI.transfer(iData);;
}

//-------------------------------------------------------------------------------------------------

