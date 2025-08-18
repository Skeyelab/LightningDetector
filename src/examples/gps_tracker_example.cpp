#include "../hardware/hardware_abstraction.h"
#include "../sensors/gps_sensor.h"
#include "../config/device_config.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

// Example usage of GPS sensor with Wireless Tracker
class GPSTrackerExample {
public:
    GPSTrackerExample() : m_last_fix_time(0), m_fix_lost_time(0) {}

    bool initialize() {
        // Initialize hardware abstraction layer
        auto result = HardwareAbstraction::initialize();
        if (result != HardwareAbstraction::Result::SUCCESS) {
            #ifdef ARDUINO
            Serial.printf("HAL initialization failed: %s\n",
                         HardwareAbstraction::resultToString(result));
            #endif
            return false;
        }

        // Check if device supports GPS
        if (!DeviceConfig::DeviceManager::getCurrentCapabilities().hasGPS) {
            #ifdef ARDUINO
            Serial.println("Device does not support GPS!");
            #endif
            return false;
        }

        // Initialize GPS
        if (!GPSSensor::initializeGPS()) {
            #ifdef ARDUINO
            Serial.println("GPS initialization failed!");
            #endif
            return false;
        }

        #ifdef ARDUINO
        Serial.println("GPS Tracker initialized successfully!");
        Serial.println("Waiting for GPS fix...");
        #endif

        return true;
    }

    void update() {
        // Update GPS data
        GPSSensor::gpsSensor->update();

        const GPSSensor::GPSData& data = GPSSensor::getGPSData();
        const bool has_fix = data.isValid;

        uint32_t current_time = HardwareAbstraction::Timer::millis();

        if (has_fix) {
            if (m_last_fix_time == 0) {
                // First fix acquired
                #ifdef ARDUINO
                Serial.println("\n*** GPS FIX ACQUIRED ***");
                printGPSInfo(data);
                #endif
            }
            m_last_fix_time = current_time;
            m_fix_lost_time = 0;
        } else {
            if (m_last_fix_time > 0 && m_fix_lost_time == 0) {
                // Fix lost
                m_fix_lost_time = current_time;
                #ifdef ARDUINO
                Serial.println("GPS fix lost, searching...");
                #endif
            }
        }

        // Print status every 10 seconds
        static uint32_t last_status_print = 0;
        if (current_time - last_status_print > 10000) {
            printStatus(data, has_fix);
            last_status_print = current_time;
        }

        // Print detailed info every 30 seconds when we have a fix
        static uint32_t last_detail_print = 0;
        if (has_fix && current_time - last_detail_print > 30000) {
            printDetailedInfo(data);
            last_detail_print = current_time;
        }
    }

private:
    uint32_t m_last_fix_time;
    uint32_t m_fix_lost_time;

    void printStatus(const GPSSensor::GPSData& data, bool has_fix) {
        #ifdef ARDUINO
        Serial.printf("[%lu] Status: %s | Sats: %d | ",
                     HardwareAbstraction::Timer::millis(),
                     has_fix ? "FIX" : "NO_FIX",
                     data.satellites);

        if (has_fix) {
            Serial.printf("Pos: %.6f,%.6f | Alt: %.1fm | Speed: %.1f km/h\n",
                         data.latitude, data.longitude, data.altitude, data.speed);
        } else {
            Serial.println("Searching for satellites...");
        }
        #endif
    }

    void printGPSInfo(const GPSSensor::GPSData& data) {
        #ifdef ARDUINO
        Serial.println("=== GPS Information ===");
        Serial.printf("Position: %.6f, %.6f\n", data.latitude, data.longitude);
        Serial.printf("Altitude: %.1f meters\n", data.altitude);
        Serial.printf("Speed: %.1f km/h\n", data.speed);
        Serial.printf("Course: %.1f degrees\n", data.course);
        Serial.printf("Satellites: %d\n", data.satellites);
        Serial.printf("Time: %s %s\n", data.date, data.time);
        Serial.println("======================");
        #endif
    }

    void printDetailedInfo(const GPSSensor::GPSData& data) {
        #ifdef ARDUINO
        Serial.println("\n=== Detailed GPS Info ===");
        Serial.printf("Latitude: %.8f° (%s)\n",
                     abs(data.latitude),
                     data.latitude >= 0 ? "N" : "S");
        Serial.printf("Longitude: %.8f° (%s)\n",
                     abs(data.longitude),
                     data.longitude >= 0 ? "E" : "W");
        Serial.printf("Altitude: %.1f meters above sea level\n", data.altitude);
        Serial.printf("Speed: %.2f km/h (%.2f m/s)\n", data.speed, data.speed / 3.6);
        Serial.printf("Course: %.1f° (%.1f radians)\n", data.course, data.course * PI / 180.0);
        Serial.printf("Satellites in view: %d\n", data.satellites);
        Serial.printf("Timestamp: %s %s UTC\n", data.date, data.time);
        Serial.printf("System time: %lu ms\n", data.timestamp);
        Serial.println("========================\n");
        #endif
    }
};

// Example usage
#ifdef ARDUINO
GPSTrackerExample gpsTracker;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("GPS Tracker Example");
    Serial.println("===================");

    if (!gpsTracker.initialize()) {
        Serial.println("Failed to initialize GPS tracker!");
        while (true) {
            delay(1000);
        }
    }
}

void loop() {
    gpsTracker.update();
    delay(1000); // Update every second
}
#endif
