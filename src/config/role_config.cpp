// New file: runtime role configuration implementation
#include "role_config.h"

#ifndef UNIT_TEST

#ifdef ARDUINO
#include <Arduino.h>
#include <Preferences.h>
#endif

namespace RoleConfig {

    // Current role cached in RAM
    static Role currentRole = Role::Sender;

#ifdef ARDUINO
    static Preferences prefs;
#endif

    void begin() {
#ifdef ARDUINO
        prefs.begin("LtngDet", /* readOnly = */ true);
        if (prefs.isKey("role")) {
            uint8_t saved = prefs.getUChar("role", static_cast<uint8_t>(currentRole));
            if (saved <= static_cast<uint8_t>(Role::Receiver)) {
                currentRole = static_cast<Role>(saved);
            }
        }
        prefs.end();
#else
        // In unit-test environment there is no non-volatile storage. Keep default role.
#endif
    }

    Role getRole() { return currentRole; }

    bool isSender() { return currentRole == Role::Sender; }
    bool isReceiver() { return currentRole == Role::Receiver; }

    void setRole(Role newRole) {
        currentRole = newRole;
#ifdef ARDUINO
        prefs.begin("LtngDet", /* readOnly = */ false);
        prefs.putUChar("role", static_cast<uint8_t>(currentRole));
        prefs.end();
#endif
    }

} // namespace RoleConfig

#endif // UNIT_TEST