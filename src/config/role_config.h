// New file: runtime role configuration
#pragma once

#include <stdint.h>

// Role configuration namespace providing runtime role determination
namespace RoleConfig {

    enum class Role : uint8_t {
        Sender = 0,
        Receiver = 1
    };

    // Initialize the role system (load from non-volatile storage when available).
    // When UNIT_TEST is defined, preferences are simulated in RAM.
    void begin();

    // Retrieve the current role.
    Role getRole();

    // Convenience helpers
    bool isSender();
    bool isReceiver();

    // Set a new role and persist it (when supported). This will take
    // effect after reboot unless the application updates its cached
    // state.
    void setRole(Role newRole);

    // Toggle between Sender and Receiver roles.
    inline void toggle() {
        setRole(isSender() ? Role::Receiver : Role::Sender);
    }

#ifdef UNIT_TEST
    // Simple in-memory stub for unit tests (no persistence)
    namespace {
        static Role _stubRole = Role::Sender;
    }

    inline void begin() {
        // Nothing to load in unit-test environment
        _stubRole = Role::Sender;
    }

    inline Role getRole() { return _stubRole; }
    inline bool isSender() { return _stubRole == Role::Sender; }
    inline bool isReceiver() { return _stubRole == Role::Receiver; }

    inline void setRole(Role newRole) { _stubRole = newRole; }
#endif
}