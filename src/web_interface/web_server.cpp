#include "web_server.h"

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
    if (!SPIFFS.begin(true)) {
        Serial.println("[WEB] Failed to mount SPIFFS");
        return false;
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
}

void WebServerManager::registerRoutes() {
    // Redirect root to index.html
    server_.on("/", HTTP_GET, [this]() {
        server_.sendHeader("Location", "/index.html", true);
        server_.send(302, "text/plain", "");
    });

    server_.on("/api/v1/status", HTTP_GET, [this]() { handleStatus(); });
    server_.on("/api/v1/config", HTTP_GET, [this]() { handleConfigGet(); });
    server_.on("/api/v1/config", HTTP_POST, [this]() { handleConfigPost(); });

    // Placeholder WiFi routes
    server_.on("/api/v1/wifi", HTTP_GET, [this]() { handleWifiGet(); });
    server_.on("/api/v1/wifi", HTTP_POST, [this]() { handleWifiPost(); });
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
    DynamicJsonDocument doc(1024);
    configManager_.toJson(doc);

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

    if (!configManager_.updateFromJson(doc)) {
        server_.send(500, "application/json", "{\"error\":\"Failed to save config\"}");
        return;
    }

    server_.send(200, "application/json", "{\"status\":\"ok\"}");
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
    if (path.endsWith("/")) path += "index.html";

    if (SPIFFS.exists(path)) {
        File file = SPIFFS.open(path, "r");
        if (file) {
            server_.streamFile(file, getContentType(path));
            file.close();
            return;
        }
    }
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