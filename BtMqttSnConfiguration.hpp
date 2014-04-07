//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn
//
//-------------------------------------------------------------------------------------------------
//
//  BtMqttSnConfiguration
//  
//*************************************************************************************************

#ifndef INC__BtMqttSnConfiguration__hpp
#define INC__BtMqttSnConfiguration__hpp

#include <avr/pgmspace.h>
#include "Arduino.h"

#define BTMQTTSN_MAX_NUMBER_OF_REGISTERED_TOPICS 20


#define BT_PROGMEM __attribute__(( section(".progmem.data") ))
#define BT_PROGMEM_STR(s) (__extension__({static char __c[] BT_PROGMEM = (s); &__c[0];}))
#define BT_LOG_STR(string_literal) (reinterpret_cast<const __FlashStringHelper *>(BT_PROGMEM_STR(string_literal)))


#define BT_LOG_MESSAGE(message) Serial.println(BT_LOG_STR(message)) ;
#define BT_LOG_MESSAGE_AND_PARAMETER(message,parameter) Serial.print(BT_LOG_STR(message)); Serial.println(parameter) ;




#endif // INC__BtMqttSnConfiguration__hpp
