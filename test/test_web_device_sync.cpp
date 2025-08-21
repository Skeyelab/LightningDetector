#include <iostream>
#include <cassert>
#include <string>

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
    
    int getInt(const char* key, int defaultValue) {
        if (std::string(key) == "preset") {
            return stored_preset != -1 ? stored_preset : defaultValue;
        }
        return defaultValue;
    }
    
    // Test helpers
    void setStoredPreset(int preset) { stored_preset = preset; }
    int getStoredPreset() const { return stored_preset; }
};

// Mock JsonDocument for testing
class MockJsonDocument {
private:
    int lora_preset_value = 0;
    
public:
    bool containsKey(const char* key) const {
        return std::string(key) == "lora_preset";
    }
    
    int get(const char* key) const {
        if (std::string(key) == "lora_preset") {
            return lora_preset_value;
        }
        return 0;
    }
    
    void set(const char* key, int value) {
        if (std::string(key) == "lora_preset") {
            lora_preset_value = value;
        }
    }
    
    bool operator!=(int other) const {
        return lora_preset_value != other;
    }
    
    // Test helpers
    int getPreset() const { return lora_preset_value; }
    void setPreset(int preset) { lora_preset_value = preset; }
};

// Mock ConfigManager for testing
class MockConfigManager {
private:
    MockJsonDocument configDoc_;
    MockPreferences webPrefs_;
    MockPreferences mainPrefs_; // Shared main preferences instance
    
public:
    MockConfigManager() {
        mainPrefs_.begin("LtngDet", false);
    }
    
    bool load() {
        // Simulate loading from web config
        int webPreset = webPrefs_.getInt("web_preset", 0);
        configDoc_.setPreset(webPreset);
        std::cout << "    [DEBUG] Loaded web preset: " << webPreset << std::endl;
        
        // Sync with main device preferences - read current preset
        int currentPreset = mainPrefs_.getInt("preset", -1);
        std::cout << "    [DEBUG] Device preset: " << currentPreset << std::endl;
        
        if (currentPreset >= 0 && currentPreset < 8) { // Valid preset range
            if (configDoc_.getPreset() != currentPreset) {
                std::cout << "  [CONFIG] Syncing web config with device preset: " << currentPreset << std::endl;
                configDoc_.setPreset(currentPreset);
                // Save the updated web config
                save();
            }
        } else {
            // If device preset is invalid, keep web preset unchanged
            std::cout << "    [DEBUG] Device preset invalid, keeping web preset: " << configDoc_.getPreset() << std::endl;
        }
        
        std::cout << "    [DEBUG] Final web preset: " << configDoc_.getPreset() << std::endl;
        return true;
    }
    
    bool save() {
        // Simulate saving to web config
        webPrefs_.putInt("web_preset", configDoc_.getPreset());
        return true;
    }
    
    void toJson(MockJsonDocument& doc) {
        doc.setPreset(configDoc_.getPreset());
    }
    
    // Test helpers
    void setWebPreset(int preset) { 
        configDoc_.setPreset(preset);
        webPrefs_.putInt("web_preset", preset); // Also save to web prefs
    }
    int getWebPreset() const { return configDoc_.getPreset(); }
    void setDevicePreset(int preset) { 
        mainPrefs_.putInt("preset", preset);
    }
};

void test_web_config_sync_on_load() {
    std::cout << "Testing web config sync on load..." << std::endl;
    
    MockConfigManager configManager;
    
    // Set initial web config to preset 0
    configManager.setWebPreset(0);
    
    // Set device preset to 5
    configManager.setDevicePreset(5);
    
    // Load should sync web config with device
    configManager.load();
    
    // Web config should now match device preset
    assert(configManager.getWebPreset() == 5);
    std::cout << "  ✓ Web config synced with device preset on load" << std::endl;
}

void test_web_config_no_sync_when_same() {
    std::cout << "Testing web config no sync when same..." << std::endl;
    
    MockConfigManager configManager;
    
    // Set both web and device to same preset
    configManager.setWebPreset(3);
    configManager.setDevicePreset(3);
    
    // Load should not change anything
    int beforePreset = configManager.getWebPreset();
    configManager.load();
    int afterPreset = configManager.getWebPreset();
    
    assert(beforePreset == afterPreset);
    std::cout << "  ✓ Web config unchanged when already in sync" << std::endl;
}

void test_web_config_sync_multiple_changes() {
    std::cout << "Testing web config sync with multiple changes..." << std::endl;
    
    MockConfigManager configManager;
    
    // Start with web at 0, device at 0
    configManager.setWebPreset(0);
    configManager.setDevicePreset(0);
    configManager.load();
    assert(configManager.getWebPreset() == 0);
    
    // Change device to preset 2
    configManager.setDevicePreset(2);
    configManager.load();
    assert(configManager.getWebPreset() == 2);
    
    // Change device to preset 7
    configManager.setDevicePreset(7);
    configManager.load();
    assert(configManager.getWebPreset() == 7);
    
    // Change device back to preset 1
    configManager.setDevicePreset(1);
    configManager.load();
    assert(configManager.getWebPreset() == 1);
    
    std::cout << "  ✓ Web config synced through multiple device changes" << std::endl;
}

void test_web_config_sync_invalid_preset() {
    std::cout << "Testing web config sync with invalid preset..." << std::endl;
    
    MockConfigManager configManager;
    
    // Set web config to preset 3
    configManager.setWebPreset(3);
    
    // Set device to invalid preset (-1)
    configManager.setDevicePreset(-1);
    
    // Load should not change web config for invalid preset
    int beforePreset = configManager.getWebPreset();
    configManager.load();
    int afterPreset = configManager.getWebPreset();
    
    assert(beforePreset == afterPreset);
    std::cout << "  ✓ Web config unchanged for invalid device preset" << std::endl;
    
    // Set device to out-of-range preset (8)
    configManager.setDevicePreset(8);
    beforePreset = configManager.getWebPreset();
    configManager.load();
    afterPreset = configManager.getWebPreset();
    
    assert(beforePreset == afterPreset);
    std::cout << "  ✓ Web config unchanged for out-of-range device preset" << std::endl;
}

void test_web_config_sync_persistence() {
    std::cout << "Testing web config sync persistence..." << std::endl;
    
    MockConfigManager configManager;
    
    // Set initial state
    configManager.setWebPreset(0);
    configManager.setDevicePreset(4);
    
    // Load should sync
    configManager.load();
    assert(configManager.getWebPreset() == 4);
    
    // Simulate restart by creating new config manager
    MockConfigManager newConfigManager;
    newConfigManager.setWebPreset(0); // Reset to default
    
    // Load should sync again
    newConfigManager.load();
    assert(newConfigManager.getWebPreset() == 4);
    
    std::cout << "  ✓ Web config sync persists across restarts" << std::endl;
}

void test_web_config_sync_edge_cases() {
    std::cout << "Testing web config sync edge cases..." << std::endl;
    
    MockConfigManager configManager;
    
    // Test boundary presets
    configManager.setWebPreset(0);
    configManager.setDevicePreset(7); // Last valid preset
    configManager.load();
    assert(configManager.getWebPreset() == 7);
    
    configManager.setWebPreset(7);
    configManager.setDevicePreset(0); // First valid preset
    configManager.load();
    assert(configManager.getWebPreset() == 0);
    
    std::cout << "  ✓ Web config sync works for boundary presets" << std::endl;
}

int main() {
    std::cout << "Running web device sync tests..." << std::endl;
    
    try {
        test_web_config_sync_on_load();
        test_web_config_no_sync_when_same();
        test_web_config_sync_multiple_changes();
        test_web_config_sync_invalid_preset();
        test_web_config_sync_persistence();
        test_web_config_sync_edge_cases();
        
        std::cout << "\n✅ All web device sync tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
}
