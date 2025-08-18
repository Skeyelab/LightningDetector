#pragma once

#include <stdint.h>
#include <Arduino.h>

// GPS Sensor Interface
// Supports both external GPS modules and built-in GPS (Wireless Tracker)
namespace GPSSensor {

    // GPS data structure
    struct GPSData {
        bool isValid;
        float latitude;
        float longitude;
        float altitude;
        float speed;
        float course;
        uint8_t satellites;
        uint32_t timestamp;
        char date[11];      // YYYY-MM-DD
        char time[9];       // HH:MM:SS
    };

    // GPS configuration
    struct GPSConfig {
        uint8_t txPin;
        uint8_t rxPin;
        uint8_t ppsPin;
        uint32_t baudRate;
        uint32_t updateInterval;
        bool enablePPS;
    };

    // GPS sensor interface
    class GPSSensorInterface {
    public:
        virtual ~GPSSensorInterface() = default;

        virtual bool initialize(const GPSConfig& config) = 0;
        virtual bool isAvailable() const = 0;
        virtual GPSData getData() const = 0;
        virtual void update() = 0;
        virtual void reset() = 0;

        // Utility functions
        virtual float calculateDistance(float lat1, float lon1, float lat2, float lon2) const;
        virtual float calculateBearing(float lat1, float lon1, float lat2, float lon2) const;
    };

    // External GPS module implementation (for Heltec V3)
    class ExternalGPSModule : public GPSSensorInterface {
    public:
        ExternalGPSModule();
        ~ExternalGPSModule() override;

        bool initialize(const GPSConfig& config) override;
        bool isAvailable() const override;
        GPSData getData() const override;
        void update() override;
        void reset() override;

    private:
        GPSConfig config;
        GPSData currentData;
        bool initialized;
        uint32_t lastUpdate;

        void parseNMEA(const String& nmea);
        bool validateChecksum(const String& nmea);
    };

    // Built-in GPS implementation (for Wireless Tracker)
    class BuiltInGPS : public GPSSensorInterface {
    public:
        BuiltInGPS();
        ~BuiltInGPS() override;

        bool initialize(const GPSConfig& config) override;
        bool isAvailable() const override;
        GPSData getData() const override;
        void update() override;
        void reset() override;

    private:
        GPSConfig config;
        GPSData currentData;
        bool initialized;
        uint32_t lastUpdate;

        void parseNMEA(const String& nmea);
        bool validateChecksum(const String& nmea);
    };

    // GPS factory function
    GPSSensorInterface* createGPSSensor(bool isBuiltIn = false);

    // Global GPS instance
    extern GPSSensorInterface* gpsSensor;

    // Initialize GPS based on device capabilities
    bool initializeGPS();

    // Get current GPS data
    GPSData getGPSData();

    // Check if GPS is available
    bool isGPSAvailable();
}
