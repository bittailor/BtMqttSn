/*
  Basic MQTT-SN over Serial Bridge example
  (experimental)

   Setup:
   - Sends the MQTT-SN packages not over the nRF24L01+ Transceiver but over SoftwareSerial to a PC
   - On the PC run the Serial Port Bridge from https://github.com/njh/mqtt-sn-tools to forward them via TCP to a MQTT-SN Broker

   Function:
   - reads a string from Serial and publishes the PUBLISH_TOPIC ("Bt/Ex/PubSub/Out") topic
   - subscribes to the SUBSCRIBE_TOPIC ("Bt/Ex/PubSub/In") topic and writes the reiceived data to the Serial
*/

#include <SPI.h>
#include <BtMqttSn.h>
#include <SoftwareSerial.h>

//-----

#define CLIENT_NODE_ID 1
#define GATEWAY_NODE_ID 0

#define CLIENT_ID "BtMqttPupSubExample"
#define PUBLISH_TOPIC "Bt/Ex/PubSub/Out"
#define SUBSCRIBE_TOPIC "Bt/Ex/PubSub/In"

//-----

class SerialPacketSocket : public Bt::I_RfPacketSocket {

  public:
    SerialPacketSocket(Stream& iStream): mStream(&iStream) {
    }

    virtual bool send(uint8_t* iPayload, size_t iSize, uint8_t iNodeId ) {
#if BT_LOGGING >= BT_LOG_LEVEL_DEBUG
      Serial.print(BT_LOG_STR("send to Serial Bridge > "));
      for (size_t i = 0 ; i < iSize ; i++) {
        int c = iPayload[i];
        char buffer[20];
        sprintf(buffer, "0x%x", c);
        Serial.print(buffer);
        Serial.print(" ");
      }
      Serial.println();
#endif
      return mStream->write(iPayload, iSize) == iSize;
    }

    virtual int32_t receive(uint8_t* oPayload, size_t iMaxSize, uint8_t* oNodeId) {
      if (mStream->available() < 1) {
        return 0;
      }
      uint8_t length = mStream->read();
      if (length == 0x01) {
        BT_LOG_DEBUG("Messages with length over 255 not supported");
        // todo cleanup/flush stream
        return 0;
      }
      if (length > iMaxSize) {
        BT_LOG_DEBUG("oPayload buffer too small");
        // todo cleanup/flush stream
        return 0;
      }

      *oNodeId = GATEWAY_NODE_ID;
      oPayload[0] = length;
      mStream->readBytes(oPayload + 1, length - 1);

#if BT_LOGGING >= BT_LOG_LEVEL_DEBUG
      Serial.print(BT_LOG_STR("recv from Serial Bridge < "));
      for (size_t i = 0 ; i < length ; i++) {
        int c = oPayload[i];
        char buffer[20];
        sprintf(buffer, "0x%x", c);
        Serial.print(buffer);
        Serial.print(" ");
      }
      Serial.println();
#endif
      return length;
    }

    virtual bool available() {
      return mStream->available() > 0;
    }

    virtual void suspend() {
    }

    virtual void resume() {
    }

  private:
    Stream* mStream;
};

//-----

// Comment the next line if the board has a second hardware serial
SoftwareSerial Serial1(10, 11);

SerialPacketSocket serialPacketSocket(Serial1);
Bt::MqttSnClient client(serialPacketSocket, GATEWAY_NODE_ID, CLIENT_ID, &subscribeCallback);

void setup() {
  while (!Serial);
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial << endl << endl << endl << "*** MQTT-SN PubSub example ***" << endl;
  Serial << endl;
  Serial << " - Node      = " << CLIENT_NODE_ID << endl;
  Serial << " - Pup-Topic = " << PUBLISH_TOPIC << endl;
  Serial << " - Sub-Topic = " << SUBSCRIBE_TOPIC << endl;
  Serial << "try connect ..." << endl;

  while (!client.connect(30)) {
    Serial << "... connect failed ..." << endl;
    delay(1000);
    Serial << "... retry connect ..." << endl;
  }

  Serial << "... connected" << endl;

  Serial << "subscribe" << endl;

  client.subscribe(SUBSCRIBE_TOPIC);

  Serial << "out> <Start>" << endl;

  client.publish(PUBLISH_TOPIC, "<Start>");

}

void loop() {
  client.loop();
  if (Serial.available()) {
    String out = readString();
    Serial << "out> " << out << endl;
    client.publish(PUBLISH_TOPIC, out.c_str());
    if (out == String("<quit>")) {
      quit();
    }
  }
}

void subscribeCallback(const char* iTopic, const char*  iData) {
  Serial << "in> " << iData << endl;
}

String readString() {
  Serial.setTimeout(10);
  String string = Serial.readString();
  Serial.setTimeout(1000);
  return string;
}

void quit() {
  Serial << "<quit>" << endl;
  client.disconnect();
  Serial << "Hit reboot" << endl;
  while (true) {}
}

