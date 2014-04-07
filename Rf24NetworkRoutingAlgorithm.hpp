//*************************************************************************************************
//
//  BITTAILOR.CH - BtCore
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Rf24::RfNetworkRoutingAlgorithm
//  
//*************************************************************************************************

#ifndef INC__Bt_Rf24_RfNetworkRoutingAlgorithm__hpp
#define INC__Bt_Rf24_RfNetworkRoutingAlgorithm__hpp

#include "Rf24Controller.hpp"
#include "Rf24Node.hpp"



class Rf24NetworkRoutingAlgorithm 
{
   public:
      Rf24Pipes::Rf24Pipe calculateRoutingPipe(Rf24Node pSelf, Rf24Node pDestination);
      void configurePipe(Rf24Node pSelf, Rf24Pipes::Rf24Pipe pPipe, Rf24Controller::Configuration::PipeConfiguration& pPipeConfiguration);

      Rf24Address calculatePipeAddress(Rf24Node pSelf, Rf24Pipes::Rf24Pipe pPipe);
      bool isLeafNode(Rf24Node pSelf);
};


#endif // INC__Bt_Rf24_RfNetworkRoutingAlgorithm__hpp
