#include "unity_compat.h"
#include "config.h"

void test_control_message_format() {
	RadioConfig cfg{915.0f, 125.0f, 9, 5, 17};
	auto msg = cfg.toControlMessage();
	TEST_ASSERT_EQUAL_STRING("CFG F=915.0 BW=125 SF=9 CR=5 TX=17", msg.c_str());
}

void test_control_message_parse_success() {
	RadioConfig cfg{};
	bool ok = RadioConfig::parseControlMessage("CFG F=915.0 BW=125 SF=9 CR=5 TX=17", cfg);
	TEST_ASSERT_TRUE(ok);
	TEST_ASSERT_FLOAT_WITHIN(0.05f, 915.0f, cfg.frequencyMHz);
	TEST_ASSERT_FLOAT_WITHIN(0.05f, 125.0f, cfg.bandwidthKHz);
	TEST_ASSERT_EQUAL_INT(9, cfg.spreadingFactor);
	TEST_ASSERT_EQUAL_INT(5, cfg.codingRate);
	TEST_ASSERT_EQUAL_INT(17, cfg.txPowerDbm);
}

void test_control_message_parse_failure() {
	RadioConfig cfg{};
	bool ok = RadioConfig::parseControlMessage("HELLO WORLD", cfg);
	TEST_ASSERT_FALSE(ok);
}