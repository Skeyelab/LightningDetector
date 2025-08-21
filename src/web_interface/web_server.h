#ifndef WEB_INTERFACE_WEB_SERVER_H
#define WEB_INTERFACE_WEB_SERVER_H

#include <Arduino.h>

#if defined(ROLE_RECEIVER) || defined(ENABLE_WEB_INTERFACE)
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
    void handleWifiGet();
    void handleWifiPost();
    void handleStaticFile(const String& path);
    void handleNotFound();

    static bool readJsonBody(WebServer &server, DynamicJsonDocument &doc);
};

extern WebServerManager webServerManager;

#endif // ROLE_RECEIVER || ENABLE_WEB_INTERFACE

#endif // WEB_INTERFACE_WEB_SERVER_H
