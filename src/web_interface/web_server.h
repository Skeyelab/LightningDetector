#ifndef WEB_INTERFACE_WEB_SERVER_H
#define WEB_INTERFACE_WEB_SERVER_H

#include <Arduino.h>

#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#include "config_manager.h"
#include "auth_handler.h"
#include "lora_config_handler.h"

class WebServerManager {
public:
    bool begin();
    void loop();

private:
    WebServer server_{80};
    ConfigManager configManager_;

    void registerRoutes();

    // HTTP Handlers
    void handleStatus();
    void handleConfigGet();
    void handleConfigPost();
    void handlePresetPost();
    void handleReboot();
    void handleWifiGet();
    void handleWifiPost();
    void handleStaticFile(const String& path);
    void handleNotFound();

    // Configuration management
    void handleGetConfig();
    void handleUpdateConfig();
    void handleApplyPreset();
    
    // ADC multiplier calibration for battery voltage
    void handleSetAdcMultiplier();
    void handleGetAdcMultiplier();

    static bool readJsonBody(WebServer &server, DynamicJsonDocument &doc);
};

extern WebServerManager webServerManager;

#endif // WEB_INTERFACE_WEB_SERVER_H
