// Unit tests for LoRa Preset functionality
// These tests focus on testable logic and minimize hardware dependencies
#include <unity.h>
#include <cstring>
#include <cstdlib>

// Mock definitions for Arduino dependencies
class MockSerial {
public:
    static void printf(const char* format, ...) {}
    static void println(const char* str) {}
    static void print(const char* str) {}
};

class MockPreferences {
public:
    int storedPreset = 0;
    int storedBW = 125;
    int storedSF = 10;

    void begin(const char* name, bool readOnly) {}
    int getInt(const char* key, int defaultValue) {
        if (strcmp(key, "preset") == 0) return storedPreset;
        if (strcmp(key, "currentBW") == 0) return storedBW;
        if (strcmp(key, "currentSF") == 0) return storedSF;
        return defaultValue;
    }
    void putInt(const char* key, int value) {
        if (strcmp(key, "preset") == 0) storedPreset = value;
        if (strcmp(key, "currentBW") == 0) storedBW = value;
        if (strcmp(key, "currentSF") == 0) storedSF = value;
    }
    void end() {}
};

#define Serial MockSerial

// Unity test setup and teardown functions
void setUp(void) {
    // Initialize before each test
}

void tearDown(void) {
    // Clean up after each test
}

// LoRa Preset definitions (extracted from main.cpp)
enum class LoRaPreset {
  LONG_RANGE_FAST = 0,
  LONG_RANGE_SLOW = 1,
  LONG_RANGE_MODERATE = 2,
  MEDIUM_RANGE_SLOW = 3,
  MEDIUM_RANGE_FAST = 4,
  SHORT_RANGE_SLOW = 5,
  SHORT_RANGE_FAST = 6,
  SHORT_RANGE_TURBO = 7
};

struct LoRaPresetConfig {
  const char* name;
  const char* shortName;
  int bandwidth;
  int spreadingFactor;
};

const LoRaPresetConfig loRaPresets[] = {
  {"Long Range - Fast", "LRF", 125, 10},
  {"Long Range - Slow", "LRS", 125, 12},
  {"Long Range - Moderate", "LRM", 125, 11},
  {"Medium Range - Slow", "MRS", 125, 10},
  {"Medium Range - Fast", "MRF", 250, 9},
  {"Short Range - Slow", "SRS", 125, 8},
  {"Short Range - Fast", "SRF", 250, 7},
  {"Short Range - Turbo", "SRT", 500, 7}
};

const int NUM_LORA_PRESETS = sizeof(loRaPresets) / sizeof(loRaPresets[0]);

// Helper functions to test
bool isValidPresetIndex(int index) {
    return index >= 0 && index < NUM_LORA_PRESETS;
}

int getPresetBandwidth(int presetIndex) {
    if (!isValidPresetIndex(presetIndex)) return -1;
    return loRaPresets[presetIndex].bandwidth;
}

int getPresetSpreadingFactor(int presetIndex) {
    if (!isValidPresetIndex(presetIndex)) return -1;
    return loRaPresets[presetIndex].spreadingFactor;
}

const char* getPresetName(int presetIndex) {
    if (!isValidPresetIndex(presetIndex)) return nullptr;
    return loRaPresets[presetIndex].name;
}

const char* getPresetShortName(int presetIndex) {
    if (!isValidPresetIndex(presetIndex)) return nullptr;
    return loRaPresets[presetIndex].shortName;
}

int cyclePreset(int currentPreset) {
    return (currentPreset + 1) % NUM_LORA_PRESETS;
}

// Control channel message parsing
bool parseControlMessage(const char* message, int* preset) {
    if (!message || !preset) return false;

    // Look for preset parameter "P=n"
    const char* presetPos = strstr(message, "P=");
    if (presetPos) {
        int parsedPreset = atoi(presetPos + 2);
        if (isValidPresetIndex(parsedPreset)) {
            *preset = parsedPreset;
            return true;
        }
    }
    return false;
}

// Test cases
void test_preset_count() {
    TEST_ASSERT_EQUAL(8, NUM_LORA_PRESETS);
}

void test_preset_definitions() {
    // Test that all presets have valid parameters
    for (int i = 0; i < NUM_LORA_PRESETS; i++) {
        TEST_ASSERT_NOT_NULL(loRaPresets[i].name);
        TEST_ASSERT_NOT_NULL(loRaPresets[i].shortName);
        TEST_ASSERT_GREATER_THAN(0, loRaPresets[i].bandwidth);
        TEST_ASSERT_GREATER_THAN(0, loRaPresets[i].spreadingFactor);

        // Valid bandwidth values
        bool validBW = (loRaPresets[i].bandwidth == 125 ||
                       loRaPresets[i].bandwidth == 250 ||
                       loRaPresets[i].bandwidth == 500);
        TEST_ASSERT_TRUE(validBW);

        // Valid spreading factor range
        TEST_ASSERT_GREATER_OR_EQUAL(7, loRaPresets[i].spreadingFactor);
        TEST_ASSERT_LESS_OR_EQUAL(12, loRaPresets[i].spreadingFactor);

        // Short names should be <= 4 characters for OLED display
        TEST_ASSERT_LESS_OR_EQUAL(4, strlen(loRaPresets[i].shortName));
    }
}

void test_long_range_presets() {
    // Long range presets (0, 1, 2) should use 125kHz
    TEST_ASSERT_EQUAL(125, getPresetBandwidth(0)); // Long Range - Fast
    TEST_ASSERT_EQUAL(125, getPresetBandwidth(1)); // Long Range - Slow
    TEST_ASSERT_EQUAL(125, getPresetBandwidth(2)); // Long Range - Moderate

    // Should have appropriate spreading factors
    TEST_ASSERT_EQUAL(10, getPresetSpreadingFactor(0));
    TEST_ASSERT_EQUAL(12, getPresetSpreadingFactor(1));
    TEST_ASSERT_EQUAL(11, getPresetSpreadingFactor(2));
}

void test_short_range_presets() {
    // Short range turbo should use maximum bandwidth
    TEST_ASSERT_EQUAL(500, getPresetBandwidth(7)); // Short Range - Turbo
    TEST_ASSERT_EQUAL(7, getPresetSpreadingFactor(7)); // Minimum SF for speed
}

void test_preset_validation() {
    // Valid indices
    TEST_ASSERT_TRUE(isValidPresetIndex(0));
    TEST_ASSERT_TRUE(isValidPresetIndex(7));

    // Invalid indices
    TEST_ASSERT_FALSE(isValidPresetIndex(-1));
    TEST_ASSERT_FALSE(isValidPresetIndex(8));
    TEST_ASSERT_FALSE(isValidPresetIndex(999));
}

void test_preset_accessors() {
    // Test valid preset access
    TEST_ASSERT_EQUAL_STRING("Long Range - Fast", getPresetName(0));
    TEST_ASSERT_EQUAL_STRING("LRF", getPresetShortName(0));
    TEST_ASSERT_EQUAL(125, getPresetBandwidth(0));
    TEST_ASSERT_EQUAL(10, getPresetSpreadingFactor(0));

    // Test invalid preset access
    TEST_ASSERT_NULL(getPresetName(-1));
    TEST_ASSERT_NULL(getPresetShortName(8));
    TEST_ASSERT_EQUAL(-1, getPresetBandwidth(999));
    TEST_ASSERT_EQUAL(-1, getPresetSpreadingFactor(-5));
}

void test_preset_cycling() {
    // Test normal cycling
    TEST_ASSERT_EQUAL(1, cyclePreset(0));
    TEST_ASSERT_EQUAL(2, cyclePreset(1));
    TEST_ASSERT_EQUAL(7, cyclePreset(6));

    // Test wrap-around
    TEST_ASSERT_EQUAL(0, cyclePreset(7));
}

void test_short_name_uniqueness() {
    // All short names should be unique
    for (int i = 0; i < NUM_LORA_PRESETS; i++) {
        for (int j = i + 1; j < NUM_LORA_PRESETS; j++) {
            TEST_ASSERT_NOT_EQUAL(0, strcmp(loRaPresets[i].shortName, loRaPresets[j].shortName));
        }
    }
}

void test_control_message_parsing() {
    int preset;

    // Valid messages
    TEST_ASSERT_TRUE(parseControlMessage("BW=125,SF=10,CR=5,TX=14,F=915,P=3", &preset));
    TEST_ASSERT_EQUAL(3, preset);

    TEST_ASSERT_TRUE(parseControlMessage("P=0", &preset));
    TEST_ASSERT_EQUAL(0, preset);

    TEST_ASSERT_TRUE(parseControlMessage("P=7", &preset));
    TEST_ASSERT_EQUAL(7, preset);

    // Invalid messages
    TEST_ASSERT_FALSE(parseControlMessage("P=8", &preset)); // Invalid preset
    TEST_ASSERT_FALSE(parseControlMessage("P=-1", &preset)); // Invalid preset
    TEST_ASSERT_FALSE(parseControlMessage("BW=125,SF=10", &preset)); // No preset
    TEST_ASSERT_FALSE(parseControlMessage("", &preset)); // Empty message
    TEST_ASSERT_FALSE(parseControlMessage(nullptr, &preset)); // Null message
}

void test_preset_range_categories() {
    // Long range category (0-2): Should prioritize range over speed
    for (int i = 0; i <= 2; i++) {
        TEST_ASSERT_EQUAL(125, getPresetBandwidth(i)); // Lower BW for range
        TEST_ASSERT_GREATER_OR_EQUAL(10, getPresetSpreadingFactor(i)); // Higher SF
    }

    // Short range category (5-7): Should prioritize speed
    for (int i = 5; i <= 7; i++) {
        TEST_ASSERT_GREATER_OR_EQUAL(125, getPresetBandwidth(i)); // Higher BW allowed
        TEST_ASSERT_LESS_OR_EQUAL(8, getPresetSpreadingFactor(i)); // Lower SF for speed
    }
}

void test_preferences_integration() {
    MockPreferences prefs;

    // Test storing and retrieving preset
    prefs.begin("LtngDet", false);
    prefs.putInt("preset", 5);
    TEST_ASSERT_EQUAL(5, prefs.getInt("preset", -1));

    // Test default value when key doesn't exist
    TEST_ASSERT_EQUAL(-1, prefs.getInt("nonexistent", -1));

    prefs.end();
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Basic preset tests
    RUN_TEST(test_preset_count);
    RUN_TEST(test_preset_definitions);
    RUN_TEST(test_preset_validation);
    RUN_TEST(test_preset_accessors);

    // Preset category tests
    RUN_TEST(test_long_range_presets);
    RUN_TEST(test_short_range_presets);
    RUN_TEST(test_preset_range_categories);

    // Preset logic tests
    RUN_TEST(test_preset_cycling);
    RUN_TEST(test_short_name_uniqueness);

    // Communication tests
    RUN_TEST(test_control_message_parsing);

    // Integration tests
    RUN_TEST(test_preferences_integration);

    return UNITY_END();
}
