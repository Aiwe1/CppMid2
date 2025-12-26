#include <cstdint>
#include <gtest/gtest.h>

#include "scan.hpp"

TEST(ScanTest, SimpleTest) {
    auto result = stdx::scan<std::string>("number", "{}");
    ASSERT_FALSE(!result);
}

TEST(ScanTest, Int) {
    auto result = stdx::scan<int>("3", "{%d}");
    ASSERT_TRUE(std::get<0>(result.value().values()) == 3);
}

TEST(ScanTest, uint8) {
    auto result = stdx::scan<int>("3", "{%u}");
    ASSERT_TRUE(std::get<0>(result.value().values()) == 3);
}

TEST(ScanTest, uint8_error) {
    auto result = stdx::scan<uint8_t>("-3", "{%u}");
    ASSERT_TRUE(result.error().message == "invalid_argument");
}

TEST(ScanTest, string) {
    auto result = stdx::scan<std::string>("alalala", "{%s}");
    ASSERT_TRUE(std::get<0>(result.value().values()) == "alalala");
}

TEST(ScanTest, string_view) {
    auto result = stdx::scan<std::string_view>("alalala", "{%s}");
    ASSERT_TRUE(std::get<0>(result.value().values()) == "alalala");
}

TEST(ScanTest, Base) {
    auto result = stdx::scan<int, float>("I want to sum 42 and 3.14 numbers.", "I want to sum {} and {%f} numbers.");
    ASSERT_TRUE(std::get<0>(result.value().values()) == 42);
    ASSERT_TRUE(std::get<1>(result.value().values()) == 3.14f);
}

TEST(ScanTest, double1) {
    auto result =
        stdx::scan<int, double>("I want to sum 42 and 3.14e+10 numbers.", "I want to sum {} and {%f} numbers.");
    ASSERT_TRUE(std::get<0>(result.value().values()) == 42);
    ASSERT_TRUE(std::get<1>(result.value().values()) == 31400000000.0);
}

TEST(ScanTest, not_double1) {
    auto result =
        stdx::scan<int, float>("I want to sum 42 and 3.14e+100 numbers.", "I want to sum {} and {%f} numbers.");
    ASSERT_TRUE(!result);
    ASSERT_TRUE(result.error().message == "Result out of range");
}
