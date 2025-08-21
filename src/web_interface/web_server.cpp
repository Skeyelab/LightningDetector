#include "web_server.h"
#include "lora_config_handler.h"

#if defined(ROLE_RECEIVER) || defined(ENABLE_WEB_INTERFACE)

static String getContentType(const String &path) {
    if (path.endsWith(".html")) return "text/html";
    if (path.endsWith(".css"))  return "text/css";
    if (path.endsWith(".js"))   return "application/javascript";
    if (path.endsWith(".json")) return "application/json";
    if (path.endsWith(".png"))  return "image/png";
    if (path.endsWith(".jpg"))  return "image/jpeg";
    if (path.endsWith(".ico"))  return "image/x-icon";
    if (path.endsWith(".svg"))  return "image/svg+xml";
    return "text/plain";
}

WebServerManager webServerManager;

bool WebServerManager::begin() {
    Serial.println("[WEB] Starting web server...");

    // Try to mount SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("[WEB] Failed to mount SPIFFS, trying to format...");
        if (!SPIFFS.format()) {
            Serial.println("[WEB] SPIFFS format failed, web server cannot start");
            return false;
        }
        if (!SPIFFS.begin(true)) {
            Serial.println("[WEB] SPIFFS mount failed after format, web server cannot start");
            return false;
        }
    }
    Serial.println("[WEB] SPIFFS mounted successfully");

    // List files in SPIFFS for debugging
    File root = SPIFFS.open("/");
    if (!root) {
        Serial.println("[WEB] Failed to open SPIFFS root directory");
        return false;
    }

    File file = root.openNextFile();
    Serial.println("[WEB] Files in SPIFFS:");
    int fileCount = 0;
    while (file) {
        Serial.printf("  %s (%d bytes)\n", file.name(), file.size());
        file = root.openNextFile();
        fileCount++;
    }
    root.close();

    if (fileCount == 0) {
        Serial.println("[WEB] WARNING: No files found in SPIFFS!");
    }

    // Load configuration (ensure defaults)
    configManager_.load();

    registerRoutes();

    server_.onNotFound([this]() { this->handleNotFound(); });
    server_.begin();

    Serial.println("[WEB] HTTP server started on port 80");
    return true;
}

void WebServerManager::loop() {
    server_.handleClient();

    // Periodically sync web config with device preferences (every 2 seconds)
    static uint32_t lastSync = 0;
    if (millis() - lastSync >= 2000) {
        lastSync = millis();
        configManager_.load(); // This will sync with main device preferences
    }
}

void WebServerManager::registerRoutes() {
    // Redirect root to index.html
    server_.on("/", HTTP_GET, [this]() {
        server_.sendHeader("Location", "/index.html", true);
        server_.send(302, "text/plain", "");
    });

    // Static file handler for web interface
    server_.on("/index.html", HTTP_GET, [this]() { handleStaticFile("/index.html"); });
    server_.on("/css/styles.css", HTTP_GET, [this]() { handleStaticFile("/css/styles.css"); });
    server_.on("/js/main.js", HTTP_GET, [this]() { handleStaticFile("/js/main.js"); });
    server_.on("/test_preset.html", HTTP_GET, [this]() { handleStaticFile("/test_preset.html"); });

    // Simple test endpoint
    server_.on("/test", HTTP_GET, [this]() {
        server_.send(200, "text/plain", "Web server is working!");
    });

    server_.on("/api/v1/status", HTTP_GET, [this]() { handleStatus(); });
    server_.on("/api/v1/config", HTTP_GET, [this]() { handleConfigGet(); });
    server_.on("/api/v1/config", HTTP_POST, [this]() { handleConfigPost(); });
    server_.on("/api/v1/preset", HTTP_POST, [this]() { handlePresetPost(); });

    // Placeholder WiFi routes
    server_.on("/api/v1/wifi", HTTP_GET, [this]() { handleWifiGet(); });
    server_.on("/api/v1/wifi", HTTP_POST, [this]() { handleWifiPost(); });
}

void WebServerManager::handleStaticFile(const String& path) {
    if (SPIFFS.exists(path)) {
        File file = SPIFFS.open(path, "r");
        if (file) {
            server_.streamFile(file, getContentType(path));
            file.close();
        } else {
            server_.send(500, "text/plain", "Failed to open file");
        }
    } else {
        server_.send(404, "text/plain", "File not found: " + path);
    }
}

void WebServerManager::handleStatus() {
    DynamicJsonDocument doc(512);
    doc["uptime_ms"] = millis();
    doc["free_heap"] = ESP.getFreeHeap();
    doc["core_temperature"] = (int)temperatureRead();

#if defined(ENABLE_WIFI_OTA)
    doc["wifi_connected"] = (WiFi.status() == WL_CONNECTED);
    if (WiFi.status() == WL_CONNECTED) {
        doc["ip"] = WiFi.localIP().toString();
        doc["ssid"] = WiFi.SSID();
        doc["rssi"] = WiFi.RSSI();
    }
#endif

    String json;
    serializeJson(doc, json);
    server_.send(200, "application/json", json);
}

void WebServerManager::handleConfigGet() {
    // Sync with device preferences before returning config
    configManager_.load();

    DynamicJsonDocument doc(1024);
    configManager_.toJson(doc);

    // Debug output
    Serial.printf("[WEB] Config GET - returning lora_preset: %d\n", doc["lora_preset"].as<int>());

    String json;
    serializeJson(doc, json);
    server_.send(200, "application/json", json);
}

void WebServerManager::handleConfigPost() {
    DynamicJsonDocument doc(1024);
    if (!readJsonBody(server_, doc)) {
        server_.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
    }

    // Debug output
    Serial.printf("[WEB] Config POST received: ");
    serializeJson(doc, Serial);
    Serial.println();

    if (doc.containsKey("lora_preset")) {
        Serial.printf("[WEB] LoRa preset change detected: %d\n", doc["lora_preset"].as<int>());
    }

    if (!configManager_.updateFromJson(doc)) {
        server_.send(500, "application/json", "{\"error\":\"Failed to save config\"}");
        return;
    }

    // Forward new configuration to LoRa layer (TX unit)
    String cfgJson;
    serializeJson(doc, cfgJson);
    LoRaConfigHandler::sendConfig(cfgJson);

    // Apply preset locally if provided
    // The preset will be applied on main loop via preference reload

    server_.send(200, "application/json", "{\"status\":\"ok\"}");
}

void WebServerManager::handlePresetPost() {
    DynamicJsonDocument doc(256);
    if (!readJsonBody(server_, doc)) {
        server_.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
    }

    if (doc.containsKey("preset")) {
        int preset = doc["preset"].as<int>();
        Serial.printf("[WEB] Direct preset application requested: %d\n", preset);

        // Send preset to main device via preferences
        Preferences mainPrefs;
        mainPrefs.begin("LtngDet", false);
        mainPrefs.putInt("preset", preset);
        mainPrefs.end();

        Serial.printf("[WEB] Preset %d saved to main preferences\n", preset);

        // Set a flag in preferences to notify main loop to reload
        mainPrefs.begin("LtngDet", false);
        mainPrefs.putBool("web_cfg", true);
        mainPrefs.end();

        // Trigger preset broadcasting (this will happen when main loop processes the change)
        Serial.printf("[WEB] Preset change will be broadcast to TX devices\n");

        server_.send(200, "application/json", "{\"status\":\"preset_saved\"}");
    } else {
        server_.send(400, "application/json", "{\"error\":\"Missing preset parameter\"}");
    }
}

void WebServerManager::handleWifiGet() {
    DynamicJsonDocument doc(256);
#if defined(ENABLE_WIFI_OTA)
    doc["connected"] = (WiFi.status() == WL_CONNECTED);
    if (WiFi.status() == WL_CONNECTED) {
        doc["ip"] = WiFi.localIP().toString();
        doc["ssid"] = WiFi.SSID();
    }
#endif
    String json; serializeJson(doc, json);
    server_.send(200, "application/json", json);
}

void WebServerManager::handleWifiPost() {
    // TODO: Implement WiFi update handling
    server_.send(501, "application/json", "{\"error\":\"Not implemented\"}");
}

void WebServerManager::handleNotFound() {
    String path = server_.uri();
    Serial.printf("[WEB] 404 - Requested: %s\n", path.c_str());

    if (path.endsWith("/")) path += "index.html";

    Serial.printf("[WEB] Looking for file: %s\n", path.c_str());
    if (SPIFFS.exists(path)) {
        Serial.printf("[WEB] File found: %s\n", path.c_str());
        File file = SPIFFS.open(path, "r");
        if (file) {
            Serial.printf("[WEB] Serving file: %s (%d bytes)\n", path.c_str(), file.size());
            server_.streamFile(file, getContentType(path));
            file.close();
            return;
        }
    }
    Serial.printf("[WEB] File not found: %s\n", path.c_str());
    server_.send(404, "text/plain", "Not Found");
}

bool WebServerManager::readJsonBody(WebServer &server, DynamicJsonDocument &doc) {
    if (server.hasArg("plain")) {
        DeserializationError err = deserializeJson(doc, server.arg("plain"));
        return !err;
    }
    return false;
}

#endif // ROLE_RECEIVER || ENABLE_WEB_INTERFACE
