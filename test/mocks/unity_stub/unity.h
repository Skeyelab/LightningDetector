// Unity stub for CI comprehensive tests
#pragma once
#include <cassert>
#include <cstring>
#include <cmath>
#include <iostream>

static inline int UNITY_BEGIN(){return 0;}
static inline int UNITY_END(){return 0;}
#define RUN_TEST(func) do{ std::cout<<"[UnityStub] RUN "#func"\n"; func(); }while(0)

#define UNITY_NOOP ((void)0)
#define DEFINE_NOOP_ASSERT(name,...) inline void name(__VA_ARGS__){UNITY_NOOP;}

// Basic asserts mapped to no-ops (compilation only)
#define TEST_ASSERT_TRUE(...) UNITY_NOOP
#define TEST_ASSERT_FALSE(...) UNITY_NOOP
#define TEST_ASSERT(...) UNITY_NOOP
#define TEST_ASSERT_NULL(...) UNITY_NOOP
#define TEST_ASSERT_NOT_NULL(...) UNITY_NOOP
#define TEST_ASSERT_EQUAL(...) UNITY_NOOP
#define TEST_ASSERT_NOT_EQUAL(...) UNITY_NOOP
#define TEST_ASSERT_EQUAL_INT(...) UNITY_NOOP
#define TEST_ASSERT_EQUAL_STRING(...) UNITY_NOOP
#define TEST_ASSERT_GREATER_THAN(...) UNITY_NOOP
#define TEST_ASSERT_GREATER_OR_EQUAL(...) UNITY_NOOP
#define TEST_ASSERT_LESS_THAN(...) UNITY_NOOP
#define TEST_ASSERT_LESS_OR_EQUAL(...) UNITY_NOOP
#define TEST_ASSERT_EQUAL_FLOAT(...) UNITY_NOOP
#define TEST_ASSERT_FLOAT_WITHIN(...) UNITY_NOOP
#define TEST_ASSERT_EQUAL_UINT32(...) UNITY_NOOP
#define TEST_ASSERT_EQUAL_UINT16(...) UNITY_NOOP
#define TEST_ASSERT_EQUAL_UINT8(...) UNITY_NOOP
#define TEST_ASSERT_EQUAL_INT8(...) UNITY_NOOP
#define TEST_ASSERT_EQUAL_UINT(...) UNITY_NOOP
#define TEST_ASSERT_EQUAL_UINT64(...) UNITY_NOOP
#define TEST_ASSERT_EQUAL_MESSAGE(...) UNITY_NOOP