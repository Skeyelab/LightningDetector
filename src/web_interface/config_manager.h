#ifndef WEB_INTERFACE_CONFIG_MANAGER_H
#define WEB_INTERFACE_CONFIG_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Preferences.h>

class ConfigManager {
public:
    ConfigManager();
    bool load();
    bool save();

    // Convert internal config to JSON
    void toJson(JsonDocument &doc);

    // Update config from JSON, returns true if changed and saved
    bool updateFromJson(const JsonDocument &doc);

private:
    Preferences prefs_;
    DynamicJsonDocument configDoc_;
    void ensureDefaults();
};

#endif // WEB_INTERFACE_CONFIG_MANAGER_H