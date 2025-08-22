// Unit tests for RoleConfig runtime role system
#include <iostream>
#include <cassert>
#include "config/role_config.h"

void test_default_role() {
    std::cout << "Testing default role..." << std::endl;
    // After begin() without persistent storage, default should remain Sender
    RoleConfig::begin();
    assert(RoleConfig::isSender());
    std::cout << "  ✓ Default role is Sender" << std::endl;
}

void test_set_get_role() {
    std::cout << "Testing set/get role..." << std::endl;
    RoleConfig::setRole(RoleConfig::Role::Receiver);
    assert(RoleConfig::isReceiver());
    RoleConfig::setRole(RoleConfig::Role::Sender);
    assert(RoleConfig::isSender());
    std::cout << "  ✓ Set/Get role works" << std::endl;
}

int main() {
    std::cout << "Running RoleConfig tests..." << std::endl;
    test_default_role();
    test_set_get_role();
    std::cout << "\n✅ All RoleConfig tests passed!" << std::endl;
    return 0;
}