#pragma once

#include <cstdio>
#include <cstring>
#include <cmath>

namespace miniunity {
  static int failures = 0;
  static int tests = 0;
}

#define UNITY_BEGIN() (miniunity::failures = 0, miniunity::tests = 0)
#define UNITY_END() (miniunity::failures)
#define RUN_TEST(fn) do { miniunity::tests++; fn(); } while (0)

#define TEST_ASSERT_TRUE(cond) do { \
  if (!(cond)) { \
    std::printf("Assertion failed: %s:%d: %s\n", __FILE__, __LINE__, #cond); \
    miniunity::failures++; \
  } \
} while (0)

#define TEST_ASSERT_FALSE(cond) TEST_ASSERT_TRUE(!(cond))

#define TEST_ASSERT_EQUAL_INT(expected, actual) do { \
  long e = static_cast<long>(expected); \
  long a = static_cast<long>(actual); \
  if (e != a) { \
    std::printf("Assertion failed: %s:%d: expected %ld, got %ld\n", __FILE__, __LINE__, e, a); \
    miniunity::failures++; \
  } \
} while (0)

#define TEST_ASSERT_EQUAL_STRING(expected, actual) do { \
  const char* e = (expected); \
  const char* a = (actual); \
  if ((e == nullptr && a != nullptr) || (e != nullptr && a == nullptr) || (e && a && std::strcmp(e, a) != 0)) { \
    std::printf("Assertion failed: %s:%d: expected \"%s\", got \"%s\"\n", __FILE__, __LINE__, e ? e : "(null)", a ? a : "(null)"); \
    miniunity::failures++; \
  } \
} while (0)

#define TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual) do { \
  float d = std::fabs((float)(actual) - (float)(expected)); \
  if (d > (float)(delta)) { \
    std::printf("Assertion failed: %s:%d: |%f - %f| <= %f (diff=%f)\n", __FILE__, __LINE__, (float)(actual), (float)(expected), (float)(delta), d); \
    miniunity::failures++; \
  } \
} while (0)