#include "miniunity/unity.h"

// Declarations of test functions
void test_classifyPress();
void test_cycleIndex();
void test_formatTxMessage();

void test_control_message_format();
void test_control_message_parse_success();
void test_control_message_parse_failure();

int main(int argc, char **argv) {
	(void)argc; (void)argv;
	UNITY_BEGIN();
	RUN_TEST(test_classifyPress);
	RUN_TEST(test_cycleIndex);
	RUN_TEST(test_formatTxMessage);
	RUN_TEST(test_control_message_format);
	RUN_TEST(test_control_message_parse_success);
	RUN_TEST(test_control_message_parse_failure);
	return UNITY_END();
}