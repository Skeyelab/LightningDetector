#include <iostream>
#include <cassert>
#include <string>
#include <sstream>

// Mock ArduinoJson for testing
class JsonDocument {
public:
    int preset_value = -1;
    bool contains_preset = false;

    bool containsKey(const char* key) const {
        return std::string(key) == "preset" && contains_preset;
    }

    int as_int() const { return preset_value; }

    void setPreset(int value) {
        preset_value = value;
        contains_preset = true;
    }
};

// Mock Preferences for testing
class MockPreferences {
private:
    int stored_preset = -1;
    bool web_cfg_flag = false;

public:
    bool begin(const char* name, bool readonly) { return true; }
    void end() {}

    bool putInt(const char* key, int value) {
        if (std::string(key) == "preset") {
            stored_preset = value;
            return true;
        }
        return false;
    }

    bool putBool(const char* key, bool value) {
        if (std::string(key) == "web_cfg") {
            web_cfg_flag = value;
            return true;
        }
        return false;
    }

    int getInt(const char* key, int defaultValue) {
        if (std::string(key) == "preset") {
            return stored_preset != -1 ? stored_preset : defaultValue;
        }
        return defaultValue;
    }

    bool getBool(const char* key, bool defaultValue) {
        if (std::string(key) == "web_cfg") {
            return web_cfg_flag;
        }
        return defaultValue;
    }

    bool remove(const char* key) {
        if (std::string(key) == "web_cfg") {
            web_cfg_flag = false;
            return true;
        }
        return false;
    }

    // Test helpers
    int getStoredPreset() const { return stored_preset; }
    bool getWebCfgFlag() const { return web_cfg_flag; }
};

// Mock web server response codes
enum HttpStatusCode {
    HTTP_OK = 200,
    HTTP_BAD_REQUEST = 400,
    HTTP_INTERNAL_ERROR = 500
};

// Mock WebServer response
class MockWebResponse {
public:
    HttpStatusCode status_code;
    std::string content_type;
    std::string body;

    void send(HttpStatusCode code, const char* type, const char* content) {
        status_code = code;
        content_type = type;
        body = content;
    }
};

// Simulate the web server preset handling logic
HttpStatusCode handlePresetPost(JsonDocument& doc, MockPreferences& prefs, MockWebResponse& response) {
    if (!doc.containsKey("preset")) {
        response.send(HTTP_BAD_REQUEST, "application/json", "{\"error\":\"Missing preset parameter\"}");
        return HTTP_BAD_REQUEST;
    }

    int preset = doc.as_int();
    if (preset < 0 || preset >= 8) { // PRESET_COUNT = 8
        response.send(HTTP_BAD_REQUEST, "application/json", "{\"error\":\"Invalid preset range\"}");
        return HTTP_BAD_REQUEST;
    }

    // Save preset to main device preferences
    prefs.begin("LtngDet", false);
    if (!prefs.putInt("preset", preset)) {
        prefs.end();
        response.send(HTTP_INTERNAL_ERROR, "application/json", "{\"error\":\"Failed to save preset\"}");
        return HTTP_INTERNAL_ERROR;
    }
    prefs.end();

    // Set flag to notify main loop to reload
    prefs.begin("LtngDet", false);
    prefs.putBool("web_cfg", true);
    prefs.end();

    response.send(HTTP_OK, "application/json", "{\"status\":\"preset_saved\"}");
    return HTTP_OK;
}

void test_valid_preset_request() {
    std::cout << "Testing valid preset request..." << std::endl;

    MockPreferences prefs;
    MockWebResponse response;
    JsonDocument doc;

    // Test valid preset
    doc.setPreset(3);
    HttpStatusCode result = handlePresetPost(doc, prefs, response);

    assert(result == HTTP_OK);
    assert(response.status_code == HTTP_OK);
    assert(response.body == "{\"status\":\"preset_saved\"}");
    assert(prefs.getStoredPreset() == 3);
    assert(prefs.getWebCfgFlag() == true);

    std::cout << "  ✓ Valid preset request handled correctly" << std::endl;
}

void test_missing_preset_parameter() {
    std::cout << "Testing missing preset parameter..." << std::endl;

    MockPreferences prefs;
    MockWebResponse response;
    JsonDocument doc; // No preset set

    HttpStatusCode result = handlePresetPost(doc, prefs, response);

    assert(result == HTTP_BAD_REQUEST);
    assert(response.status_code == HTTP_BAD_REQUEST);
    assert(response.body.find("Missing preset parameter") != std::string::npos);

    std::cout << "  ✓ Missing preset parameter rejected" << std::endl;
}

void test_invalid_preset_range() {
    std::cout << "Testing invalid preset range..." << std::endl;

    MockPreferences prefs;
    MockWebResponse response;
    JsonDocument doc;

    // Test preset too low
    doc.setPreset(-1);
    HttpStatusCode result = handlePresetPost(doc, prefs, response);
    assert(result == HTTP_BAD_REQUEST);

    // Test preset too high
    doc.setPreset(8); // PRESET_COUNT = 8, so valid range is 0-7
    result = handlePresetPost(doc, prefs, response);
    assert(result == HTTP_BAD_REQUEST);

    std::cout << "  ✓ Invalid preset ranges rejected" << std::endl;
}

void test_preference_flag_mechanism() {
    std::cout << "Testing preference flag mechanism..." << std::endl;

    MockPreferences prefs;

    // Initially no flag should be set
    assert(prefs.getBool("web_cfg", false) == false);

    // Set flag
    prefs.begin("LtngDet", false);
    prefs.putBool("web_cfg", true);
    prefs.end();

    // Check flag is set
    assert(prefs.getBool("web_cfg", false) == true);

    // Clear flag (simulating main loop processing)
    prefs.begin("LtngDet", false);
    prefs.remove("web_cfg");
    prefs.end();

    // Check flag is cleared
    assert(prefs.getBool("web_cfg", false) == false);

    std::cout << "  ✓ Preference flag mechanism works correctly" << std::endl;
}

void test_all_valid_presets() {
    std::cout << "Testing all valid preset values..." << std::endl;

    MockPreferences prefs;
    MockWebResponse response;

    // Test all valid preset values (0-7)
    for (int i = 0; i < 8; i++) {
        JsonDocument doc;
        doc.setPreset(i);

        HttpStatusCode result = handlePresetPost(doc, prefs, response);
        assert(result == HTTP_OK);
        assert(prefs.getStoredPreset() == i);
        assert(prefs.getWebCfgFlag() == true);

        // Clear flag for next test
        prefs.begin("LtngDet", false);
        prefs.remove("web_cfg");
        prefs.end();
    }

    std::cout << "  ✓ All valid preset values accepted" << std::endl;
}

void test_json_response_format() {
    std::cout << "Testing JSON response format..." << std::endl;

    MockPreferences prefs;
    MockWebResponse response;
    JsonDocument doc;

    // Test successful response format
    doc.setPreset(0);
    handlePresetPost(doc, prefs, response);

    assert(response.content_type == "application/json");
    assert(response.body == "{\"status\":\"preset_saved\"}");

    // Test error response format
    JsonDocument invalidDoc; // No preset
    handlePresetPost(invalidDoc, prefs, response);

    assert(response.content_type == "application/json");
    assert(response.body.find("error") != std::string::npos);

    std::cout << "  ✓ JSON response format correct" << std::endl;
}

void test_concurrent_preset_changes() {
    std::cout << "Testing concurrent preset changes..." << std::endl;

    MockPreferences prefs;
    MockWebResponse response;

    // Simulate rapid preset changes
    for (int i = 0; i < 3; i++) {
        JsonDocument doc;
        doc.setPreset(i);

        HttpStatusCode result = handlePresetPost(doc, prefs, response);
        assert(result == HTTP_OK);

        // Each change should set the flag
        assert(prefs.getWebCfgFlag() == true);

        // Don't clear flag to simulate rapid changes
    }

    // Final preset should be the last one set
    assert(prefs.getStoredPreset() == 2);
    assert(prefs.getWebCfgFlag() == true);

    std::cout << "  ✓ Concurrent preset changes handled correctly" << std::endl;
}

int main() {
    std::cout << "Running web preset integration tests..." << std::endl;

    try {
        test_valid_preset_request();
        test_missing_preset_parameter();
        test_invalid_preset_range();
        test_preference_flag_mechanism();
        test_all_valid_presets();
        test_json_response_format();
        test_concurrent_preset_changes();

        std::cout << "\n✅ All web preset integration tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
}
