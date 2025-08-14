#include "unity_compat.h"

// Declarations of test functions
void test_classifyPress();
void test_cycleIndex();
void test_formatTxMessage();

void test_control_message_format();
void test_control_message_parse_success();
void test_control_message_parse_failure();

void test_radio_state_format();
void test_radio_controller_header_compiles();
void test_display_status_renders_lines_and_footer();

int main(int argc, char **argv) {
	(void)argc; (void)argv;
	UNITY_BEGIN();
	RUN_TEST(test_classifyPress);
	RUN_TEST(test_cycleIndex);
	RUN_TEST(test_formatTxMessage);
	RUN_TEST(test_control_message_format);
	RUN_TEST(test_control_message_parse_success);
	RUN_TEST(test_control_message_parse_failure);
	RUN_TEST(test_radio_state_format);
	RUN_TEST(test_radio_controller_header_compiles);
	RUN_TEST(test_display_status_renders_lines_and_footer);
	return UNITY_END();
}