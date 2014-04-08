//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn , an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::Rf24Pipe
//  
//*************************************************************************************************

#ifndef INC__BT_Rf24Pipe__hpp
#define INC__BT_Rf24Pipe__hpp

namespace Bt {

class Rf24Pipes {
   public:

      enum Rf24Pipe
      {
         PIPE_0,
         PIPE_1,
         PIPE_2,
         PIPE_3,
         PIPE_4,
         PIPE_5,
      };

      enum { NUMBER_OF_PIPES = 6 };

      static Rf24Pipe ALL_PIPES[NUMBER_OF_PIPES];
};

} // namespace Bt

#endif // INC__BT_Rf24Pipe__hpp
