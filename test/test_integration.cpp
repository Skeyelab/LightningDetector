// Integration tests for cross-module functionality
// Tests interactions between app_logic and other components
#include <unity.h>
#include "app_logic.h"
#include <cstring>

// Integration test scenarios
struct TestScenario {
    const char* name;
    uint32_t buttonPressDuration;
    ButtonAction expectedAction;
    const char* expectedBehavior;
};

// Define test scenarios that represent real usage patterns
static const TestScenario scenarios[] = {
    {"Quick tap", 50, ButtonAction::Ignore, "Should ignore accidental touches"},
    {"Short press", 200, ButtonAction::CyclePrimary, "Should cycle primary function (SF for sender, display for receiver)"},
    {"Medium press", 1500, ButtonAction::SecondaryFunction, "Should trigger secondary function (config broadcast/network mode)"},
    {"Long press", 5000, ButtonAction::ConfigMode, "Should enter configuration mode"},
    {"Boundary short", 99, ButtonAction::Ignore, "Edge case for ignore threshold"},
    {"Boundary primary", 100, ButtonAction::CyclePrimary, "Edge case for primary function threshold"},
    {"Boundary secondary", 1000, ButtonAction::SecondaryFunction, "Edge case for secondary function threshold"},
    {"Boundary config", 3000, ButtonAction::ConfigMode, "Edge case for config mode threshold"}
};

#define NUM_SCENARIOS (sizeof(scenarios) / sizeof(scenarios[0]))

// Mock configuration for testing interactions
struct MockSystemState {
    bool isSender; // Now fixed at build time, not changeable via button
    int currentSF;
    float currentBW;
    int currentTxPower;
    uint8_t displayMode; // For receiver display cycling
    uint32_t messageSeq;
    char lastMessage[64];
    bool configBroadcastRequested;
};

static MockSystemState systemState = {true, 9, 125.0, 17, 0, 0, "", false};

// Integration helper: Simulate button press and system response
void simulateButtonPressAndResponse(uint32_t pressDuration) {
    ButtonAction action = classifyPress(pressDuration);

    switch (action) {
        case ButtonAction::Ignore:
            // No state change
            break;

        case ButtonAction::CyclePrimary:
            if (systemState.isSender) {
                // Sender: cycle through LoRa parameters (simplified for test)
                static uint8_t paramIndex = 0;
                paramIndex = (paramIndex + 1) % 3;
                if (paramIndex == 0) {
                    systemState.currentSF = cycleIndex(systemState.currentSF - 7, 6) + 7; // SF 7-12
                    snprintf(systemState.lastMessage, sizeof(systemState.lastMessage), "SF changed to %d", systemState.currentSF);
                } else if (paramIndex == 1) {
                    // Cycle BW
                    int bwIndex = (systemState.currentBW == 125.0) ? 1 : 0;
                    bwIndex = cycleIndex(bwIndex, 4);
                    switch (bwIndex) {
                        case 0: systemState.currentBW = 62.5; break;
                        case 1: systemState.currentBW = 125.0; break;
                        case 2: systemState.currentBW = 250.0; break;
                        case 3: systemState.currentBW = 500.0; break;
                    }
                    snprintf(systemState.lastMessage, sizeof(systemState.lastMessage), "BW changed to %.1f kHz", systemState.currentBW);
                } else {
                    // Cycle TX Power
                    systemState.currentTxPower = (systemState.currentTxPower == 17) ? 20 : 17;
                    snprintf(systemState.lastMessage, sizeof(systemState.lastMessage), "TX Power changed to %d dBm", systemState.currentTxPower);
                }
            } else {
                // Receiver: cycle display modes
                systemState.displayMode = (systemState.displayMode + 1) % 3;
                snprintf(systemState.lastMessage, sizeof(systemState.lastMessage), "Display mode %d", systemState.displayMode);
            }
            break;

        case ButtonAction::SecondaryFunction:
            if (systemState.isSender) {
                // Sender: broadcast configuration
                systemState.configBroadcastRequested = true;
                strcpy(systemState.lastMessage, "Config broadcast requested");
            } else {
                // Receiver: network mode change or config request
                strcpy(systemState.lastMessage, "Network mode or config request");
            }
            break;

        case ButtonAction::ConfigMode:
            strcpy(systemState.lastMessage, "Configuration mode requested");
            break;

        case ButtonAction::SleepMode:
            strcpy(systemState.lastMessage, "Sleep mode requested");
            break;
    }
}

// Integration helper: Test multi-click for sleep mode
void simulateMultiClick(uint8_t clickCount, uint32_t totalTime) {
    ButtonAction action = classifyMultipleClicks(clickCount, totalTime);
    if (action == ButtonAction::SleepMode) {
        strcpy(systemState.lastMessage, "Sleep mode requested");
    }
}

// Integration helper: Simulate message transmission
void simulateMessageTransmission() {
    if (systemState.isSender) {
        std::string message = formatTxMessage(systemState.messageSeq);
        strcpy(systemState.lastMessage, message.c_str());
        systemState.messageSeq++;
    }
}

// Reset system state for tests
void resetSystemState() {
    systemState.isSender = true; // Fixed role for sender tests
    systemState.currentSF = 9;
    systemState.currentBW = 125.0;
    systemState.currentTxPower = 17;
    systemState.displayMode = 0;
    systemState.messageSeq = 0;
    systemState.configBroadcastRequested = false;
    strcpy(systemState.lastMessage, "");
}

// Test cases
void test_button_press_scenarios() {
    for (int i = 0; i < NUM_SCENARIOS; i++) {
        const TestScenario& scenario = scenarios[i];

        // Test the classification
        ButtonAction action = classifyPress(scenario.buttonPressDuration);
        TEST_ASSERT_EQUAL_MESSAGE((int)scenario.expectedAction, (int)action, scenario.name);

        // Verify the scenario description makes sense
        TEST_ASSERT_NOT_NULL_MESSAGE(scenario.expectedBehavior, scenario.name);
        TEST_ASSERT_GREATER_THAN_MESSAGE(0, strlen(scenario.expectedBehavior), scenario.name);
    }
}

void test_multi_click_sleep_mode() {
    resetSystemState();

    // Test that 5 rapid clicks trigger sleep mode
    ButtonAction action = classifyMultipleClicks(5, 1500);
    TEST_ASSERT_EQUAL((int)ButtonAction::SleepMode, (int)action);

    // Test that fewer clicks don't trigger sleep
    action = classifyMultipleClicks(4, 1500);
    TEST_ASSERT_EQUAL((int)ButtonAction::Ignore, (int)action);

    // Test that 5 slow clicks don't trigger sleep
    action = classifyMultipleClicks(5, 3000);
    TEST_ASSERT_EQUAL((int)ButtonAction::Ignore, (int)action);
}

void test_sender_button_integration() {
    resetSystemState();
    systemState.isSender = true;

    // Test initial state
    TEST_ASSERT_TRUE(systemState.isSender);
    TEST_ASSERT_EQUAL(9, systemState.currentSF);
    TEST_ASSERT_EQUAL_FLOAT(125.0, systemState.currentBW);

    // Test primary function cycling (should cycle through SF, BW, TX Power)
    simulateButtonPressAndResponse(200); // CyclePrimary
    // First press should change SF
    TEST_ASSERT_NOT_EQUAL(9, systemState.currentSF);
    
    simulateButtonPressAndResponse(200); // CyclePrimary again
    // Second press should change BW
    TEST_ASSERT_NOT_EQUAL_FLOAT(125.0, systemState.currentBW);

    // Test secondary function (config broadcast)
    simulateButtonPressAndResponse(1500);
    TEST_ASSERT_TRUE(systemState.configBroadcastRequested);
    TEST_ASSERT_EQUAL_STRING("Config broadcast requested", systemState.lastMessage);
}

void test_receiver_button_integration() {
    resetSystemState();
    systemState.isSender = false;

    // Test primary function (display mode cycling)
    uint8_t initialDisplayMode = systemState.displayMode;
    simulateButtonPressAndResponse(200); // CyclePrimary
    TEST_ASSERT_NOT_EQUAL(initialDisplayMode, systemState.displayMode);
    TEST_ASSERT_LESS_THAN(3, systemState.displayMode); // Should be 0, 1, or 2

    // Test secondary function
    simulateButtonPressAndResponse(1500);
    TEST_ASSERT_EQUAL_STRING("Network mode or config request", systemState.lastMessage);
}

void test_parameter_cycling_integration() {
    resetSystemState();

    // Test that parameters cycle within valid ranges
    for (int i = 0; i < 10; i++) {
        simulateButtonPressAndResponse(200); // CyclePrimary
        
        TEST_ASSERT_GREATER_OR_EQUAL(7, systemState.currentSF);
        TEST_ASSERT_LESS_OR_EQUAL(12, systemState.currentSF);

        bool validBW = (systemState.currentBW == 62.5 ||
                        systemState.currentBW == 125.0 ||
                        systemState.currentBW == 250.0 ||
                        systemState.currentBW == 500.0);
        TEST_ASSERT_TRUE(validBW);
    }
}

void test_message_transmission_integration() {
    resetSystemState();

    // Test message formatting and sequence increment
    simulateMessageTransmission();
    TEST_ASSERT_EQUAL_STRING("PING seq=0", systemState.lastMessage);
    TEST_ASSERT_EQUAL(1, systemState.messageSeq);

    simulateMessageTransmission();
    TEST_ASSERT_EQUAL_STRING("PING seq=1", systemState.lastMessage);
    TEST_ASSERT_EQUAL(2, systemState.messageSeq);

    // Switch to receiver mode and verify no transmission
    systemState.isSender = false;
    strcpy(systemState.lastMessage, ""); // Clear message
    simulateMessageTransmission();
    TEST_ASSERT_EQUAL_STRING("", systemState.lastMessage); // Should remain empty
}

void test_ignore_action_integration() {
    resetSystemState();

    // Store initial state
    bool initialMode = systemState.isSender;
    int initialSF = systemState.currentSF;
    float initialBW = systemState.currentBW;
    uint32_t initialSeq = systemState.messageSeq;

    // Simulate ignored button press
    simulateButtonPressAndResponse(50); // Ignore

    // Verify no state changes
    TEST_ASSERT_EQUAL(initialMode, systemState.isSender);
    TEST_ASSERT_EQUAL(initialSF, systemState.currentSF);
    TEST_ASSERT_EQUAL_FLOAT(initialBW, systemState.currentBW);
    TEST_ASSERT_EQUAL(initialSeq, systemState.messageSeq);
}

void test_boundary_conditions_integration() {
    resetSystemState();

    // Test boundary values that should trigger different actions
    simulateButtonPressAndResponse(99);   // Should ignore
    simulateButtonPressAndResponse(100);  // Should cycle primary
    simulateButtonPressAndResponse(999);  // Should cycle primary
    simulateButtonPressAndResponse(1000); // Should trigger secondary
    simulateButtonPressAndResponse(2999); // Should trigger secondary
    simulateButtonPressAndResponse(3000); // Should enter config mode

    // Verify system state remains consistent after all operations
    TEST_ASSERT_GREATER_OR_EQUAL(7, systemState.currentSF);
    TEST_ASSERT_LESS_OR_EQUAL(12, systemState.currentSF);
}

void test_config_mode_integration() {
    resetSystemState();

    // Test config mode activation
    simulateButtonPressAndResponse(5000); // ConfigMode
    TEST_ASSERT_EQUAL_STRING("Configuration mode requested", systemState.lastMessage);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Scenario-based tests
    RUN_TEST(test_button_press_scenarios);
    RUN_TEST(test_multi_click_sleep_mode);

    // State integration tests
    RUN_TEST(test_sender_button_integration);
    RUN_TEST(test_receiver_button_integration);
    RUN_TEST(test_parameter_cycling_integration);
    RUN_TEST(test_message_transmission_integration);
    RUN_TEST(test_ignore_action_integration);

    // Edge case tests
    RUN_TEST(test_boundary_conditions_integration);
    RUN_TEST(test_config_mode_integration);

    return UNITY_END();
}
