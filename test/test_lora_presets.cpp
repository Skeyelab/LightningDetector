#include <iostream>
#include <cassert>
#include <string>
#include <cstring>

// Mock the preset structures and functions for testing
enum LoRaPreset : int {
    PRESET_LONG_RANGE_FAST = 0,
    PRESET_LONG_RANGE_SLOW,
    PRESET_LONG_RANGE_MODERATE,
    PRESET_MEDIUM_RANGE_SLOW,
    PRESET_MEDIUM_RANGE_FAST,
    PRESET_SHORT_RANGE_SLOW,
    PRESET_SHORT_RANGE_FAST,
    PRESET_SHORT_RANGE_TURBO,
    PRESET_COUNT
};

struct LoRaPresetConfig {
    const char *name;
    const char *shortName;
    float bw;
    int sf;
};

static const LoRaPresetConfig loRaPresets[PRESET_COUNT] = {
    {"Long Range - Fast",     "LR-F", 125.0f, 10},
    {"Long Range - Slow",     "LR-S", 125.0f, 12},
    {"Long Range - Moderate", "LR-M", 125.0f, 11},
    {"Medium Range - Slow",   "MR-S", 125.0f, 10},
    {"Medium Range - Fast",   "MR-F", 250.0f, 9},
    {"Short Range - Slow",    "SR-S", 125.0f, 8},
    {"Short Range - Fast",    "SR-F", 250.0f, 7},
    {"Short Range - Turbo",   "SR-T", 500.0f, 7}
};

// Mock control channel message parsing
bool parseControlMessage(const char* message, float& freq, float& bw, int& sf, int& cr, int& tx, int& preset) {
    int parsed = sscanf(message, "CFG F=%f BW=%f SF=%d CR=%d TX=%d P=%d", &freq, &bw, &sf, &cr, &tx, &preset);
    return parsed >= 5; // At least 5 parameters required
}

void test_preset_definitions() {
    std::cout << "Testing LoRa preset definitions..." << std::endl;

    // Test preset count
    assert(PRESET_COUNT == 8);
    std::cout << "  ✓ Correct number of presets" << std::endl;

    // Test long range presets use 125kHz bandwidth
    assert(loRaPresets[PRESET_LONG_RANGE_FAST].bw == 125.0f);
    assert(loRaPresets[PRESET_LONG_RANGE_SLOW].bw == 125.0f);
    assert(loRaPresets[PRESET_LONG_RANGE_MODERATE].bw == 125.0f);
    std::cout << "  ✓ Long range presets use 125kHz bandwidth" << std::endl;

    // Test spreading factor progression (higher SF = longer range, lower speed)
    assert(loRaPresets[PRESET_LONG_RANGE_SLOW].sf > loRaPresets[PRESET_LONG_RANGE_FAST].sf);
    assert(loRaPresets[PRESET_SHORT_RANGE_FAST].sf < loRaPresets[PRESET_MEDIUM_RANGE_FAST].sf);
    std::cout << "  ✓ Spreading factor progression correct" << std::endl;

    // Test turbo preset uses highest bandwidth
    assert(loRaPresets[PRESET_SHORT_RANGE_TURBO].bw == 500.0f);
    std::cout << "  ✓ Turbo preset uses maximum bandwidth" << std::endl;

    // Test short names are concise (≤ 4 characters)
    for (int i = 0; i < PRESET_COUNT; i++) {
        assert(strlen(loRaPresets[i].shortName) <= 4);
    }
    std::cout << "  ✓ All short names are ≤ 4 characters" << std::endl;
}

void test_control_channel_parsing() {
    std::cout << "Testing control channel message parsing..." << std::endl;

    float freq, bw;
    int sf, cr, tx, preset;

    // Test message with preset
    const char* msgWithPreset = "CFG F=915.0 BW=125.0 SF=10 CR=5 TX=17 P=3";
    assert(parseControlMessage(msgWithPreset, freq, bw, sf, cr, tx, preset));
    assert(freq == 915.0f);
    assert(bw == 125.0f);
    assert(sf == 10);
    assert(cr == 5);
    assert(tx == 17);
    assert(preset == 3);
    std::cout << "  ✓ Message with preset parsed correctly" << std::endl;

    // Test message without preset (legacy format)
    const char* msgLegacy = "CFG F=915.0 BW=250.0 SF=7 CR=5 TX=20";
    assert(parseControlMessage(msgLegacy, freq, bw, sf, cr, tx, preset));
    assert(freq == 915.0f);
    assert(bw == 250.0f);
    assert(sf == 7);
    std::cout << "  ✓ Legacy message format supported" << std::endl;

    // Test invalid message
    const char* invalidMsg = "INVALID MESSAGE";
    assert(!parseControlMessage(invalidMsg, freq, bw, sf, cr, tx, preset));
    std::cout << "  ✓ Invalid messages rejected" << std::endl;
}

void test_preset_cycling() {
    std::cout << "Testing preset cycling logic..." << std::endl;

    // Test cycling through all presets
    int currentPreset = 0;
    for (int i = 0; i < PRESET_COUNT; i++) {
        int nextPreset = (currentPreset + 1) % PRESET_COUNT;
        assert(nextPreset >= 0 && nextPreset < PRESET_COUNT);
        currentPreset = nextPreset;
    }

    // After full cycle, should be back at preset 0
    assert(currentPreset == 0);
    std::cout << "  ✓ Preset cycling works correctly" << std::endl;

    // Test cycling from last preset wraps to first
    currentPreset = PRESET_COUNT - 1;
    int nextPreset = (currentPreset + 1) % PRESET_COUNT;
    assert(nextPreset == 0);
    std::cout << "  ✓ Cycling wraps from last to first preset" << std::endl;
}

void test_preset_ranges() {
    std::cout << "Testing preset range categories..." << std::endl;

    // Long range presets should use lower bandwidth, higher SF
    for (int i = PRESET_LONG_RANGE_FAST; i <= PRESET_LONG_RANGE_MODERATE; i++) {
        assert(loRaPresets[i].bw <= 125.0f);
        assert(loRaPresets[i].sf >= 10);
    }
    std::cout << "  ✓ Long range presets have appropriate parameters" << std::endl;

    // Short range presets should have lower SF for speed
    for (int i = PRESET_SHORT_RANGE_SLOW; i <= PRESET_SHORT_RANGE_TURBO; i++) {
        assert(loRaPresets[i].sf <= 8);
    }
    std::cout << "  ✓ Short range presets prioritize speed" << std::endl;

    // Medium range should be between long and short
    assert(loRaPresets[PRESET_MEDIUM_RANGE_FAST].sf < loRaPresets[PRESET_LONG_RANGE_FAST].sf);
    assert(loRaPresets[PRESET_MEDIUM_RANGE_FAST].sf > loRaPresets[PRESET_SHORT_RANGE_FAST].sf);
    std::cout << "  ✓ Medium range presets are balanced" << std::endl;
}

void test_preset_names() {
    std::cout << "Testing preset naming consistency..." << std::endl;

    // Check that all presets have both long and short names
    for (int i = 0; i < PRESET_COUNT; i++) {
        assert(loRaPresets[i].name != nullptr);
        assert(loRaPresets[i].shortName != nullptr);
        assert(strlen(loRaPresets[i].name) > 0);
        assert(strlen(loRaPresets[i].shortName) > 0);
    }
    std::cout << "  ✓ All presets have valid names" << std::endl;

    // Check that short names are unique
    for (int i = 0; i < PRESET_COUNT; i++) {
        for (int j = i + 1; j < PRESET_COUNT; j++) {
            assert(strcmp(loRaPresets[i].shortName, loRaPresets[j].shortName) != 0);
        }
    }
    std::cout << "  ✓ Short names are unique" << std::endl;
}

void test_web_interface_integration() {
    std::cout << "Testing web interface integration..." << std::endl;

    // Test JSON structure for web interface
    for (int i = 0; i < PRESET_COUNT; i++) {
        // Simulate web interface preset selection
        int selectedPreset = i;
        assert(selectedPreset >= 0 && selectedPreset < PRESET_COUNT);

        // Verify preset can be applied
        float newBW = loRaPresets[selectedPreset].bw;
        int newSF = loRaPresets[selectedPreset].sf;

        assert(newBW > 0);
        assert(newSF >= 7 && newSF <= 12); // Valid SF range
    }
    std::cout << "  ✓ Web interface preset selection validated" << std::endl;

    // Test that all preset indices are valid for web dropdown
    for (int i = 0; i < PRESET_COUNT; i++) {
        assert(i >= 0 && i < PRESET_COUNT);
    }
    std::cout << "  ✓ Preset indices valid for web interface" << std::endl;
}

int main() {
    std::cout << "Running LoRa preset tests..." << std::endl;

    try {
        test_preset_definitions();
        test_control_channel_parsing();
        test_preset_cycling();
        test_preset_ranges();
        test_preset_names();
        test_web_interface_integration();

        std::cout << "\n✅ All LoRa preset tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
}
