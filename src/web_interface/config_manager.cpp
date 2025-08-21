#include "config_manager.h"
#include <Preferences.h>

ConfigManager::ConfigManager()
    : configDoc_(1024) {
}

void ConfigManager::ensureDefaults() {
    if (!configDoc_.containsKey("device_name")) {
        configDoc_["device_name"] = "LtngDet-RX";
    }
    if (!configDoc_.containsKey("detection_sensitivity")) {
        configDoc_["detection_sensitivity"] = 5;
    }
    if (!configDoc_.containsKey("led_brightness")) {
        configDoc_["led_brightness"] = 128;
    }
    if (!configDoc_.containsKey("lora_preset")) {
        configDoc_["lora_preset"] = 0; // Default preset
    }
}

bool ConfigManager::load() {
    prefs_.begin("WebConfig", true);
    String json = prefs_.getString("json", "{}");
    prefs_.end();

    DeserializationError err = deserializeJson(configDoc_, json);
    if (err) {
        configDoc_.clear();
    }
    ensureDefaults();
    
    // Sync with main device preferences - read current preset
    Preferences mainPrefs;
    mainPrefs.begin("LtngDet", true);
    int currentPreset = mainPrefs.getInt("preset", -1);
    mainPrefs.end();
    
    if (currentPreset >= 0 && currentPreset < 8) { // Valid preset range
        if (configDoc_["lora_preset"] != currentPreset) {
            Serial.printf("[CONFIG] Syncing web config with device preset: %d\n", currentPreset);
            configDoc_["lora_preset"] = currentPreset;
            // Save the updated web config
            save();
        }
    }
    
    return true;
}

bool ConfigManager::save() {
    String json;
    serializeJson(configDoc_, json);

    prefs_.begin("WebConfig", false);
    bool ok = prefs_.putString("json", json);
    prefs_.end();
    return ok;
}

void ConfigManager::toJson(JsonDocument &doc) {
    doc.clear();
    doc.set(configDoc_); // Copy contents
}

bool ConfigManager::updateFromJson(const JsonDocument &doc) {
    bool changed = false;
    JsonObjectConst obj = doc.as<JsonObjectConst>();
    for (JsonPairConst kv : obj) {
        if (configDoc_[kv.key()] != kv.value()) {
            Serial.printf("[CONFIG] Setting %s = ", kv.key().c_str());
            if (kv.value().is<int>()) {
                Serial.printf("%d (int)\n", kv.value().as<int>());
            } else if (kv.value().is<String>()) {
                Serial.printf("%s (string)\n", kv.value().as<String>().c_str());
            } else {
                Serial.printf("(unknown type)\n");
            }
            configDoc_[kv.key()] = kv.value();
            changed = true;
        }
    }
    if (changed) {
        ensureDefaults();

        // If LoRa preset changed, save it to main device preferences
        if (doc.containsKey("lora_preset")) {
            Serial.printf("[CONFIG] LoRa preset change detected: %d\n", doc["lora_preset"].as<int>());
            Preferences mainPrefs;
            mainPrefs.begin("LtngDet", false);
            mainPrefs.putInt("preset", doc["lora_preset"].as<int>());
            mainPrefs.end();
            Serial.printf("[CONFIG] Preset saved to main preferences\n");
        }

        return save();
    }
    return true;
}
