#include "config_manager.h"

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
    for (JsonPair kv : doc.as<JsonObject>()) {
        if (configDoc_[kv.key()] != kv.value()) {
            configDoc_[kv.key()] = kv.value();
            changed = true;
        }
    }
    if (changed) {
        ensureDefaults();
        return save();
    }
    return true;
}