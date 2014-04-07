//*************************************************************************************************
//
//  BITTAILOR.CH - BtCore
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Rf24::RfNetworkRoutingAlgorithm
//  
//*************************************************************************************************

#include "Rf24NetworkRoutingAlgorithm.hpp"



//-------------------------------------------------------------------------------------------------

Rf24Pipes::Rf24Pipe Rf24NetworkRoutingAlgorithm::calculateRoutingPipe(Rf24Node pSelf, Rf24Node pDestination) {
   if (pSelf.level() >= pDestination.level()){
      return Rf24Pipes::PIPE_0;
   }

   uint8_t idAtLevel[4] = {0};
   idAtLevel[pDestination.level()] = pDestination.id();
   for(uint8_t i = pDestination.level() ; i > pSelf.level() ; i-- ) {
      idAtLevel[i - 1] = (idAtLevel[i] - 1) / 5;
   }

   if (idAtLevel[pSelf.level()] != pSelf.id()) {
      return Rf24Pipes::PIPE_0;
   }

   switch (idAtLevel[pSelf.level() + 1] % 5) {
      case 1 : return Rf24Pipes::PIPE_1;
      case 2 : return Rf24Pipes::PIPE_2;
      case 3 : return Rf24Pipes::PIPE_3;
      case 4 : return Rf24Pipes::PIPE_4;
      case 0 : return Rf24Pipes::PIPE_5;

      default: return Rf24Pipes::PIPE_0;
   }
}

//-------------------------------------------------------------------------------------------------

void Rf24NetworkRoutingAlgorithm::configurePipe(Rf24Node pSelf, Rf24Pipes::Rf24Pipe pPipe, Rf24Controller::Configuration::PipeConfiguration& pPipeConfiguration) {
   if(isLeafNode(pSelf) && pPipe != Rf24Pipes::PIPE_0) {
      pPipeConfiguration.mEnabled = false;
      return;
   }

   pPipeConfiguration.mEnabled = true;
   pPipeConfiguration.mAddress = calculatePipeAddress(pSelf,pPipe);
}

//-------------------------------------------------------------------------------------------------

Rf24Address Rf24NetworkRoutingAlgorithm::calculatePipeAddress(Rf24Node pSelf, Rf24Pipes::Rf24Pipe pPipe) {
   uint8_t byte0 = pSelf.id();
   uint8_t child = pSelf.id() * 5;
   switch(pPipe) {
      case Rf24Pipes::PIPE_0 : byte0 = pSelf.id(); break;
      case Rf24Pipes::PIPE_1 : byte0 = child + 1; break;
      case Rf24Pipes::PIPE_2 : byte0 = child + 2; break;
      case Rf24Pipes::PIPE_3 : byte0 = child + 3; break;
      case Rf24Pipes::PIPE_4 : byte0 = child + 4; break;
      case Rf24Pipes::PIPE_5 : byte0 = child + 5; break;
   }
   return Rf24Address(0xC2,0xC2,0xC2,0xC2,byte0);
}

//-------------------------------------------------------------------------------------------------

bool Rf24NetworkRoutingAlgorithm::isLeafNode(Rf24Node pSelf) {
   return pSelf.id() > 50;
}

//-------------------------------------------------------------------------------------------------

