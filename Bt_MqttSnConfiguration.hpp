//*************************************************************************************************
//
//  BITTAILOR.CH - BtMqttSn, an Arduino library for MQTT-SN over nRF24L01+
//
//-------------------------------------------------------------------------------------------------
//
//  Bt::MqttSnConfiguration
//
//*************************************************************************************************

#ifndef INC__Bt_MqttSnConfiguration__hpp
#define INC__Bt_MqttSnConfiguration__hpp

#include <avr/pgmspace.h>
#include "Arduino.h"

//-------------------------------------------------------------------------------------------------
// configuration
//-------------------------------------------------------------------------------------------------

#define BT_MQTTSN_MAX_NUMBER_OF_REGISTERED_TOPICS 8

#define BT_MQTTSN_T_RETRY 15 * 1000 // 15 seconds ( use 0 to wait forever)


// The available logging levels.
#define BT_LOG_LEVEL_NONE    0  // nothing is logged
#define BT_LOG_LEVEL_ERROR   1  // only errors are logged
#define BT_LOG_LEVEL_WARNING 2  // errors and warnings are logged
#define BT_LOG_LEVEL_INFO    3  // errors and warnings and info are logged
#define BT_LOG_LEVEL_DEBUG   4  // everything is logged

// Modify this to change the logging level. Reducing the logging level reduces the code size
// the strings are removed from the source code.
#define BT_LOGGING BT_LOG_LEVEL_NONE



//-------------------------------------------------------------------------------------------------
// logging detail stuff
//-------------------------------------------------------------------------------------------------

#define BT_PROGMEM __attribute__(( section(".progmem.data") ))
#define BT_PROGMEM_STR(s) (__extension__({static char __c[] BT_PROGMEM = (s); &__c[0];}))
#define BT_LOG_STR(string_literal) (reinterpret_cast<const __FlashStringHelper *>(BT_PROGMEM_STR(string_literal)))

#if BT_LOGGING > BT_LOG_LEVEL_NONE

#define BT_LOG_MESSAGE(prefix,message) Serial.print(millis()); Serial.print(BT_LOG_STR(prefix)); Serial.println(BT_LOG_STR(message)) ;
#define BT_LOG_MESSAGE_AND_PARAMETER(prefix,message,parameter) Serial.print(millis()); Serial.print(BT_LOG_STR(prefix)); Serial.print(BT_LOG_STR(message)); Serial.println(parameter) ;

#else

#define BT_LOG_MESSAGE(prefix,message)
#define BT_LOG_MESSAGE_AND_PARAMETER(prefix,message,parameter)

#endif


// -- BT_LOG_LEVEL_ERROR
#if BT_LOGGING >= BT_LOG_LEVEL_ERROR

#define BT_LOG_ERROR(message) BT_LOG_MESSAGE(" E: ",message)
#define BT_LOG_ERROR_AND_PARAMETER(message,parameter) BT_LOG_MESSAGE_AND_PARAMETER(" E: ",message,parameter)

#else

#define BT_LOG_ERROR(message)
#define BT_LOG_ERROR_AND_PARAMETER(message,parameter)

#endif

// -- BT_LOG_LEVEL_WARNING
#if BT_LOGGING >= BT_LOG_LEVEL_WARNING

#define BT_LOG_WARNING(message) BT_LOG_MESSAGE(" W: ",message)
#define BT_LOG_WARNING_AND_PARAMETER(message,parameter) BT_LOG_MESSAGE_AND_PARAMETER(" W: ",message,parameter)

#else

#define BT_LOG_WARNING(message)
#define BT_LOG_WARNING_AND_PARAMETER(message,parameter)

#endif

// -- BT_LOG_LEVEL_INFO
#if BT_LOGGING >= BT_LOG_LEVEL_INFO

#define BT_LOG_INFO(message) BT_LOG_MESSAGE(" I: ", message)
#define BT_LOG_INFO_AND_PARAMETER(message,parameter) BT_LOG_MESSAGE_AND_PARAMETER(" I: ",message,parameter)

#else

#define BT_LOG_INFO(message)
#define BT_LOG_INFO_AND_PARAMETER(message,parameter)

#endif

// -- BT_LOG_LEVEL_DEBUG
#if BT_LOGGING >= BT_LOG_LEVEL_DEBUG

#define BT_LOG_DEBUG(message) BT_LOG_MESSAGE(" D: ", message)
#define BT_LOG_DEBUG_AND_PARAMETER(message,parameter) BT_LOG_MESSAGE_AND_PARAMETER(" D: ",message,parameter)

#else

#define BT_LOG_DEBUG(message)
#define BT_LOG_DEBUG_AND_PARAMETER(message,parameter)

#endif

//-------------------------------------------------------------------------------------------------
// fix for missing placement new
//-------------------------------------------------------------------------------------------------


inline void* operator new(size_t size, void* ptr )
{
   return ptr;
}


#endif // INC__Bt_MqttSnConfiguration__hpp
