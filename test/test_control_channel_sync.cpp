#include <iostream>
#include <cassert>
#include <string>
#include <cstring>
#include <vector>

// Mock control channel message structure
struct ControlMessage {
    float freq;
    float bw;
    int sf;
    int cr;
    int tx;
    int preset;
    bool hasPreset;

    ControlMessage() : freq(0), bw(0), sf(0), cr(0), tx(0), preset(-1), hasPreset(false) {}
};

// Mock function to format control channel message
std::string formatControlMessage(float freq, float bw, int sf, int cr, int tx, int preset = -1) {
    char msg[64];
    if (preset >= 0) {
        snprintf(msg, sizeof(msg), "CFG F=%.1f BW=%.0f SF=%d CR=%d TX=%d P=%d",
                 freq, bw, sf, cr, tx, preset);
    } else {
        snprintf(msg, sizeof(msg), "CFG F=%.1f BW=%.0f SF=%d CR=%d TX=%d P=-1",
                 freq, bw, sf, cr, tx);
    }
    return std::string(msg);
}

// Mock function to parse control channel message
bool parseControlMessage(const std::string& message, ControlMessage& result) {
    int parsed = sscanf(message.c_str(), "CFG F=%f BW=%f SF=%d CR=%d TX=%d P=%d",
                       &result.freq, &result.bw, &result.sf, &result.cr, &result.tx, &result.preset);

    if (parsed >= 5) {
        result.hasPreset = (parsed == 6);
        return true;
    }
    return false;
}

// Mock device state
struct DeviceState {
    float freq;
    float bw;
    int sf;
    int cr;
    int tx;
    int preset;
    bool isReceiver;

    DeviceState(bool receiver = false) :
        freq(915.0f), bw(125.0f), sf(9), cr(5), tx(17), preset(0), isReceiver(receiver) {}

    void applyPreset(int presetIndex) {
        // Simplified preset application for testing
        preset = presetIndex;
        switch (presetIndex) {
            case 0: bw = 125.0f; sf = 10; break; // Long Range - Fast
            case 1: bw = 125.0f; sf = 12; break; // Long Range - Slow
            case 2: bw = 125.0f; sf = 11; break; // Long Range - Moderate
            case 3: bw = 125.0f; sf = 10; break; // Medium Range - Slow
            case 4: bw = 250.0f; sf = 9; break;  // Medium Range - Fast
            case 5: bw = 125.0f; sf = 8; break;  // Short Range - Slow
            case 6: bw = 250.0f; sf = 7; break;  // Short Range - Fast
            case 7: bw = 500.0f; sf = 7; break;  // Short Range - Turbo
        }
    }

    std::string broadcastConfig() {
        return formatControlMessage(freq, bw, sf, cr, tx, preset);
    }

    bool receiveConfig(const std::string& message) {
        ControlMessage msg;
        if (parseControlMessage(message, msg)) {
            freq = msg.freq;
            bw = msg.bw;
            sf = msg.sf;
            cr = msg.cr;
            tx = msg.tx;

            if (msg.hasPreset && msg.preset >= 0) {
                preset = msg.preset;
                // In real implementation, this would call applyLoRaPresetSilent
            }
            return true;
        }
        return false;
    }

    bool isInSync(const DeviceState& other) const {
        return (freq == other.freq &&
                bw == other.bw &&
                sf == other.sf &&
                cr == other.cr &&
                tx == other.tx &&
                preset == other.preset);
    }
};

void test_control_message_formatting() {
    std::cout << "Testing control message formatting..." << std::endl;

    // Test message with preset
    std::string msg = formatControlMessage(915.0f, 125.0f, 10, 5, 17, 3);
    assert(msg == "CFG F=915.0 BW=125 SF=10 CR=5 TX=17 P=3");
    std::cout << "  ✓ Message with preset formatted correctly" << std::endl;

    // Test message without preset
    msg = formatControlMessage(915.0f, 250.0f, 7, 5, 20, -1);
    assert(msg == "CFG F=915.0 BW=250 SF=7 CR=5 TX=20 P=-1");
    std::cout << "  ✓ Message without preset formatted correctly" << std::endl;
}

void test_control_message_parsing() {
    std::cout << "Testing control message parsing..." << std::endl;

    ControlMessage result;

    // Test parsing message with preset
    std::string msg = "CFG F=915.0 BW=125 SF=10 CR=5 TX=17 P=3";
    assert(parseControlMessage(msg, result));
    assert(result.freq == 915.0f);
    assert(result.bw == 125.0f);
    assert(result.sf == 10);
    assert(result.cr == 5);
    assert(result.tx == 17);
    assert(result.preset == 3);
    assert(result.hasPreset == true);
    std::cout << "  ✓ Message with preset parsed correctly" << std::endl;

    // Test parsing legacy message (no preset)
    msg = "CFG F=915.0 BW=250 SF=7 CR=5 TX=20";
    assert(parseControlMessage(msg, result));
    assert(result.hasPreset == false);
    std::cout << "  ✓ Legacy message format parsed correctly" << std::endl;

    // Test invalid message
    msg = "INVALID MESSAGE";
    assert(!parseControlMessage(msg, result));
    std::cout << "  ✓ Invalid messages rejected" << std::endl;
}

void test_device_preset_application() {
    std::cout << "Testing device preset application..." << std::endl;

    DeviceState device;

    // Test applying different presets
    device.applyPreset(0); // Long Range - Fast
    assert(device.bw == 125.0f && device.sf == 10);

    device.applyPreset(7); // Short Range - Turbo
    assert(device.bw == 500.0f && device.sf == 7);

    device.applyPreset(4); // Medium Range - Fast
    assert(device.bw == 250.0f && device.sf == 9);

    std::cout << "  ✓ Preset application works correctly" << std::endl;
}

void test_device_synchronization() {
    std::cout << "Testing device synchronization..." << std::endl;

    DeviceState receiver(true);
    DeviceState transmitter(false);

    // Initially different settings
    receiver.applyPreset(0);
    transmitter.applyPreset(7);
    assert(!receiver.isInSync(transmitter));
    std::cout << "  ✓ Devices start out of sync" << std::endl;

    // Receiver changes preset and broadcasts
    receiver.applyPreset(3);
    std::string broadcast = receiver.broadcastConfig();

    // Transmitter receives and applies
    assert(transmitter.receiveConfig(broadcast));
    assert(receiver.isInSync(transmitter));
    std::cout << "  ✓ Devices sync after broadcast" << std::endl;
}

void test_preset_broadcast_reception() {
    std::cout << "Testing preset broadcast and reception..." << std::endl;

    DeviceState receiver(true);
    DeviceState tx1(false);
    DeviceState tx2(false);

    // Set different initial states
    receiver.applyPreset(0);
    tx1.applyPreset(3);
    tx2.applyPreset(7);

    // Receiver changes to preset 5 and broadcasts
    receiver.applyPreset(5);
    std::string broadcast = receiver.broadcastConfig();

    // Both transmitters should receive and sync
    assert(tx1.receiveConfig(broadcast));
    assert(tx2.receiveConfig(broadcast));

    assert(receiver.isInSync(tx1));
    assert(receiver.isInSync(tx2));
    assert(tx1.isInSync(tx2));

    std::cout << "  ✓ Multiple devices sync from single broadcast" << std::endl;
}

void test_bidirectional_sync() {
    std::cout << "Testing bidirectional synchronization..." << std::endl;

    DeviceState receiver(true);
    DeviceState transmitter(false);

    // Transmitter changes preset and broadcasts
    transmitter.applyPreset(6);
    std::string txBroadcast = transmitter.broadcastConfig();

    // Receiver should receive and sync
    assert(receiver.receiveConfig(txBroadcast));
    assert(transmitter.isInSync(receiver));
    std::cout << "  ✓ Receiver syncs to transmitter changes" << std::endl;

    // Receiver changes preset and broadcasts back
    receiver.applyPreset(2);
    std::string rxBroadcast = receiver.broadcastConfig();

    // Transmitter should receive and sync
    assert(transmitter.receiveConfig(rxBroadcast));
    assert(receiver.isInSync(transmitter));
    std::cout << "  ✓ Transmitter syncs to receiver changes" << std::endl;
}

void test_legacy_compatibility() {
    std::cout << "Testing legacy message compatibility..." << std::endl;

    DeviceState modernDevice;

    // Test receiving legacy message (no preset)
    std::string legacyMsg = "CFG F=915.0 BW=250.0 SF=8 CR=5 TX=15";
    assert(modernDevice.receiveConfig(legacyMsg));

    // Parameters should be applied, preset should remain unchanged
    assert(modernDevice.freq == 915.0f);
    assert(modernDevice.bw == 250.0f);
    assert(modernDevice.sf == 8);
    assert(modernDevice.cr == 5);
    assert(modernDevice.tx == 15);

    std::cout << "  ✓ Legacy messages handled correctly" << std::endl;
}

void test_malformed_message_handling() {
    std::cout << "Testing malformed message handling..." << std::endl;

    DeviceState device;
    float originalFreq = device.freq;

    std::vector<std::string> malformedMessages = {
        "CFG F=INVALID BW=125 SF=9 CR=5 TX=17 P=0",
        "CFG F=915.0 BW=125 SF=INVALID CR=5 TX=17 P=0",
        "TOTALLY INVALID MESSAGE",
        "CFG F=915.0", // Incomplete
        "CFG F=915.0 BW=125 SF=9 CR=5", // Missing TX
    };

    for (const auto& msg : malformedMessages) {
        assert(!device.receiveConfig(msg));
        assert(device.freq == originalFreq); // State should be unchanged
    }

    std::cout << "  ✓ Malformed messages rejected without state change" << std::endl;
}

void test_preset_boundary_conditions() {
    std::cout << "Testing preset boundary conditions..." << std::endl;

    DeviceState device;

    // Test preset at boundaries
    device.applyPreset(0); // First preset
    assert(device.preset == 0);

    device.applyPreset(7); // Last preset (PRESET_COUNT - 1)
    assert(device.preset == 7);

    // Test broadcast and reception of boundary presets
    std::string broadcast = device.broadcastConfig();
    DeviceState otherDevice;
    assert(otherDevice.receiveConfig(broadcast));
    assert(otherDevice.preset == 7);

    std::cout << "  ✓ Preset boundary conditions handled correctly" << std::endl;
}

int main() {
    std::cout << "Running control channel synchronization tests..." << std::endl;

    try {
        test_control_message_formatting();
        test_control_message_parsing();
        test_device_preset_application();
        test_device_synchronization();
        test_preset_broadcast_reception();
        test_bidirectional_sync();
        test_legacy_compatibility();
        test_malformed_message_handling();
        test_preset_boundary_conditions();

        std::cout << "\n✅ All control channel synchronization tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
}
