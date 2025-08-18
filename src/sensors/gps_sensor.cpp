#include "gps_sensor.h"
#include <Arduino.h>
#include "../config/device_config.h"

namespace GPSSensor {

    // Global GPS instance
    GPSSensorInterface* gpsSensor = nullptr;

    // GPS factory function
    GPSSensorInterface* createGPSSensor(bool isBuiltIn) {
        if (isBuiltIn) {
            return new BuiltInGPS();
        } else {
            return new ExternalGPSModule();
        }
    }

    // Initialize GPS based on device capabilities
    bool initializeGPS() {
        // Check if device supports GPS
        if (!DeviceConfig::DeviceManager::getCurrentCapabilities().hasGPS) {
            return false;
        }

        // Create appropriate GPS sensor
        bool isBuiltIn = DeviceConfig::DeviceManager::getCurrentDevice() ==
                        DeviceConfig::DeviceType::HELTEC_WIRELESS_TRACKER_V1_1;

        gpsSensor = createGPSSensor(isBuiltIn);
        if (!gpsSensor) {
            return false;
        }

        // Get pin configuration
        const DeviceConfig::PinConfig& pins = DeviceConfig::DeviceManager::getCurrentPinConfig();

        // Configure GPS
        GPSConfig config;
        config.txPin = pins.gpsTx;
        config.rxPin = pins.gpsRx;
        config.ppsPin = pins.gpsPps;
        config.baudRate = 9600;
        config.updateInterval = 1000;
        config.enablePPS = true;

        return gpsSensor->initialize(config);
    }

    // Get current GPS data
    GPSData getGPSData() {
        if (gpsSensor) {
            return gpsSensor->getData();
        }

        // Return invalid data if no GPS sensor
        GPSData data = {};
        data.isValid = false;
        return data;
    }

    // Check if GPS is available
    bool isGPSAvailable() {
        return gpsSensor != nullptr && gpsSensor->isAvailable();
    }

    // External GPS Module Implementation
    ExternalGPSModule::ExternalGPSModule() : initialized(false), lastUpdate(0) {
        memset(&currentData, 0, sizeof(currentData));
        currentData.isValid = false;
    }

    ExternalGPSModule::~ExternalGPSModule() {
        // Cleanup if needed
    }

    bool ExternalGPSModule::initialize(const GPSConfig& config) {
        this->config = config;

        // Initialize UART for external GPS module
        Serial2.begin(config.baudRate, SERIAL_8N1, config.rxPin, config.txPin);

        initialized = true;
        return true;
    }

    bool ExternalGPSModule::isAvailable() const {
        return initialized;
    }

    GPSData ExternalGPSModule::getData() const {
        return currentData;
    }

    void ExternalGPSModule::update() {
        if (!initialized) return;

        // Read NMEA data from Serial2
        while (Serial2.available()) {
            String nmea = Serial2.readStringUntil('\n');
            if (nmea.length() > 0) {
                parseNMEA(nmea);
            }
        }

        lastUpdate = millis();
    }

    void ExternalGPSModule::reset() {
        memset(&currentData, 0, sizeof(currentData));
        currentData.isValid = false;
        lastUpdate = 0;
    }

    void ExternalGPSModule::parseNMEA(const String& nmea) {
        // Basic NMEA parsing - this is a simplified implementation
        if (nmea.startsWith("$GPGGA")) {
            // Parse GGA sentence for position data
            // This is a basic implementation - in production you'd want more robust parsing
            currentData.isValid = true;
            currentData.timestamp = millis();
            // Parse latitude, longitude, altitude, satellites from GGA sentence
            // For now, just mark as valid
        } else if (nmea.startsWith("$GPRMC")) {
            // Parse RMC sentence for time, date, speed, course
            // This is a basic implementation
        }
    }

    bool ExternalGPSModule::validateChecksum(const String& nmea) {
        // Basic checksum validation
        if (nmea.length() < 4) return false;

        int asteriskPos = nmea.lastIndexOf('*');
        if (asteriskPos == -1) return false;

        // Calculate checksum and compare
        // This is a simplified implementation
        return true;
    }

    // Built-in GPS Implementation
    BuiltInGPS::BuiltInGPS() : initialized(false), lastUpdate(0) {
        memset(&currentData, 0, sizeof(currentData));
        currentData.isValid = false;
    }

    BuiltInGPS::~BuiltInGPS() {
        // Cleanup if needed
    }

    bool BuiltInGPS::initialize(const GPSConfig& config) {
        this->config = config;

        // Initialize UART for built-in GPS
        Serial2.begin(config.baudRate, SERIAL_8N1, config.rxPin, config.txPin);

        // Configure PPS pin if enabled
        if (config.enablePPS && config.ppsPin != 255) {
            pinMode(config.ppsPin, INPUT);
        }

        initialized = true;
        return true;
    }

    bool BuiltInGPS::isAvailable() const {
        return initialized;
    }

    GPSData BuiltInGPS::getData() const {
        return currentData;
    }

    void BuiltInGPS::update() {
        if (!initialized) return;

        // Read NMEA data from Serial2
        while (Serial2.available()) {
            String nmea = Serial2.readStringUntil('\n');
            if (nmea.length() > 0) {
                parseNMEA(nmea);
            }
        }

        lastUpdate = millis();
    }

    void BuiltInGPS::reset() {
        memset(&currentData, 0, sizeof(currentData));
        currentData.isValid = false;
        lastUpdate = 0;
    }

    void BuiltInGPS::parseNMEA(const String& nmea) {
        // Same NMEA parsing as external GPS
        // In a real implementation, you might have device-specific parsing
        if (nmea.startsWith("$GPGGA")) {
            currentData.isValid = true;
            currentData.timestamp = millis();
        } else if (nmea.startsWith("$GPRMC")) {
            // Parse RMC sentence
        }
    }

    bool BuiltInGPS::validateChecksum(const String& nmea) {
        // Same checksum validation as external GPS
        if (nmea.length() < 4) return false;

        int asteriskPos = nmea.lastIndexOf('*');
        if (asteriskPos == -1) return false;

        return true;
    }

    // Utility functions for distance and bearing calculations
    float GPSSensorInterface::calculateDistance(float lat1, float lon1, float lat2, float lon2) const {
        // Haversine formula for calculating distance between two points
        const float R = 6371000; // Earth's radius in meters

        float lat1Rad = lat1 * PI / 180.0;
        float lat2Rad = lat2 * PI / 180.0;
        float deltaLat = (lat2 - lat1) * PI / 180.0;
        float deltaLon = (lon2 - lon1) * PI / 180.0;

        float a = sin(deltaLat/2) * sin(deltaLat/2) +
                  cos(lat1Rad) * cos(lat2Rad) *
                  sin(deltaLon/2) * sin(deltaLon/2);
        float c = 2 * atan2(sqrt(a), sqrt(1-a));

        return R * c; // Distance in meters
    }

    float GPSSensorInterface::calculateBearing(float lat1, float lon1, float lat2, float lon2) const {
        // Calculate bearing from point 1 to point 2
        float lat1Rad = lat1 * PI / 180.0;
        float lat2Rad = lat2 * PI / 180.0;
        float deltaLon = (lon2 - lon1) * PI / 180.0;

        float y = sin(deltaLon) * cos(lat2Rad);
        float x = cos(lat1Rad) * sin(lat2Rad) -
                  sin(lat1Rad) * cos(lat2Rad) * cos(deltaLon);

        float bearing = atan2(y, x) * 180.0 / PI;
        return fmod(bearing + 360.0, 360.0); // Normalize to 0-360
    }
}
