#ifndef PREFERENCES_MOCKS_H
#define PREFERENCES_MOCKS_H

#ifdef ARDUINO_MOCK

#include <cstdint>
#include <cstring>
#include <map>

// Mock Preferences class
class Preferences {
public:
    Preferences() : initialized(false) {}

    bool begin(const char* name, bool readOnly) {
        (void)name;
        (void)readOnly;
        initialized = true;
        return true;
    }

    int getInt(const char* key, int defaultValue) {
        if (!initialized) return defaultValue;

        auto it = intValues.find(key);
        return (it != intValues.end()) ? it->second : defaultValue;
    }

    void putInt(const char* key, int value) {
        if (initialized) {
            intValues[key] = value;
        }
    }

    bool getBool(const char* key, bool defaultValue) {
        if (!initialized) return defaultValue;

        auto it = boolValues.find(key);
        return (it != boolValues.end()) ? it->second : defaultValue;
    }

    void putBool(const char* key, bool value) {
        if (initialized) {
            boolValues[key] = value;
        }
    }

    size_t getString(const char* key, char* value, size_t maxLen) {
        if (!initialized) return 0;

        auto it = stringValues.find(key);
        if (it != stringValues.end()) {
            size_t len = std::min(it->second.length(), maxLen - 1);
            strncpy(value, it->second.c_str(), len);
            value[len] = '\0';
            return len;
        }
        return 0;
    }

    void putString(const char* key, const char* value) {
        if (initialized) {
            stringValues[key] = value;
        }
    }

    void end() {
        initialized = false;
        intValues.clear();
        boolValues.clear();
        stringValues.clear();
    }

    bool clear() {
        intValues.clear();
        boolValues.clear();
        stringValues.clear();
        return true;
    }

    bool remove(const char* key) {
        if (!initialized) return false;

        intValues.erase(key);
        boolValues.erase(key);
        stringValues.erase(key);
        return true;
    }

private:
    bool initialized;
    std::map<std::string, int> intValues;
    std::map<std::string, bool> boolValues;
    std::map<std::string, std::string> stringValues;
};

#endif // ARDUINO_MOCK

#endif // PREFERENCES_MOCKS_H
