//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn, an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Rf24NetworkRoutingAlgorithm
//  
//*************************************************************************************************

#ifndef INC__Bt_RfNetworkRoutingAlgorithm__hpp
#define INC__Bt_RfNetworkRoutingAlgorithm__hpp

#include "Bt_Rf24Controller.hpp"
#include "Bt_Rf24Node.hpp"

namespace Bt {

class Rf24NetworkRoutingAlgorithm 
{
   public:
      Rf24Pipes::Rf24Pipe calculateRoutingPipe(Rf24Node pSelf, Rf24Node pDestination);
      void configurePipe(Rf24Node pSelf, Rf24Pipes::Rf24Pipe pPipe, Rf24Controller::Configuration::PipeConfiguration& pPipeConfiguration);

      Rf24Address calculatePipeAddress(Rf24Node pSelf, Rf24Pipes::Rf24Pipe pPipe);
      bool isLeafNode(Rf24Node pSelf);
};

} // namespace Bt

#endif // INC__Bt_RfNetworkRoutingAlgorithm__hpp
