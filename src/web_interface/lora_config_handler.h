#ifndef WEB_INTERFACE_LORA_CONFIG_HANDLER_H
#define WEB_INTERFACE_LORA_CONFIG_HANDLER_H

#include <Arduino.h>

class LoRaConfigHandler {
public:
    static void sendConfig(const String &json) {
        // TODO: Implement sending config via LoRa to TX unit
        Serial.println("[LoRaCfg] sendConfig not implemented yet");
        (void)json;
    }

    static void processIncoming() {
        // TODO: Check LoRa queue for incoming config acks or status
    }
};

#endif // WEB_INTERFACE_LORA_CONFIG_HANDLER_H