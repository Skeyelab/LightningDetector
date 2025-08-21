#ifndef WIFI_MOCKS_H
#define WIFI_MOCKS_H

#ifdef ARDUINO_MOCK

#include <cstdint>
#include <cstring>
#include <cstdio>

// Mock WiFi status constants
#define WL_CONNECTED 3
#define WL_DISCONNECTED 6
#define WL_CONNECT_FAILED 4

// Mock WiFi class
class WiFiClass {
public:
    static bool begin(const char* ssid, const char* password) {
        (void)ssid;
        (void)password;
        return true; // Mock successful connection
    }

    static int status() {
        return WL_CONNECTED; // Mock connected status
    }

    static void disconnect() {}

    static void print(const char* str) {
        (void)str;
    }

    static void println(const char* str) {
        (void)str;
    }

    static void printf(const char* format, ...) {
        (void)format;
    }

    // Mock IP address class
    class IPAddress {
    public:
        IPAddress() : addr{0, 0, 0, 0} {}
        IPAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d) : addr{a, b, c, d} {}

        const char* toString() const {
            static char buffer[16];
            snprintf(buffer, sizeof(buffer), "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
            return buffer;
        }

        uint8_t addr[4];
    };

    static IPAddress localIP() {
        return IPAddress(192, 168, 1, 100); // Mock IP address
    }
};

// Global WiFi instance
extern WiFiClass WiFi;

#endif // ARDUINO_MOCK

#endif // WIFI_MOCKS_H
