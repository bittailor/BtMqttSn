//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn, an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  BtMqttSn
//  
//*************************************************************************************************

#ifndef INC__BtMqttSn__h
#define INC__BtMqttSn__h

#include "Arduino.h"
#include "MqttSnClient.hpp"

template<class T>
inline Print &operator <<(Print &stream, T arg) {
   stream.print(arg);
   return stream;
}

enum _EndLineCode { endl };

inline Print &operator <<(Print &stream, _EndLineCode arg) {
   stream.println();
   return stream;
}




#endif // INC__BtMqttSn__h
