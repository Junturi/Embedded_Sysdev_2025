#include <gtest/gtest.h>
#include "../TimeParser.h"

// Test with correct time
TEST(TimeParserTest, TestCaseCorrectTime) {
    char test_correct[] = "141205";
    int expectedSeconds = 14*60*60 + 12*60 + 5;
    ASSERT_EQ(time_parse(test_correct),expectedSeconds);
}

// Test for null pointer
TEST(TimeParserTest, TestCaseNullPointer) {
    char test_correct[] = "141205";
    int expectedSeconds = 14*60*60 + 12*60 + 5;
    ASSERT_EQ(time_parse(test_correct),expectedSeconds);

    ASSERT_EQ(time_parse(nullptr),TIME_ARRAY_ERROR);
}

// Test for time length
TEST(TimeParserTest, TestCaseTimeLength) {
    char test_correct[] = "141205";
    int expectedSeconds = 14*60*60 + 12*60 + 5;
    ASSERT_EQ(time_parse(test_correct),expectedSeconds);

    char test_length[] = "11022";
    ASSERT_EQ(time_parse(test_length), TIME_ARRAY_ERROR);
}

// Test that time includes only digits
// Tests for negative numbers as well
TEST(TimeParserTest, TestCaseIsDigit) {
    char test_correct[] = "141205";
    int expectedSeconds = 14*60*60 + 12*60 + 5;
    ASSERT_EQ(time_parse(test_correct),expectedSeconds);

    char test_digit[] = "12A503";
    ASSERT_EQ(time_parse(test_digit), TIME_VALUE_ERROR);
}

// Test with incorrect time
TEST(TimeParserTest, TestCaseIncorrectTime) {
    char test_correct[] = "141205";
    int expectedSeconds = 14*60*60 + 12*60 + 5;
    ASSERT_EQ(time_parse(test_correct),expectedSeconds);

    // Test seconds
    char test_seconds[] = "141265";
    ASSERT_EQ(time_parse(test_seconds),TIME_VALUE_ERROR);

    // Test minutes
    char test_minutes[] = "148005";
    ASSERT_EQ(time_parse(test_minutes),TIME_VALUE_ERROR);

    // Test hours
    char test_hours[] = "301205";
    ASSERT_EQ(time_parse(test_hours),TIME_VALUE_ERROR);
}

// Test for return value of 0
TEST(TimeParserTest, TestCaseNotZero) {
    char test_correct[] = "141205";
    int expectedSeconds = 14*60*60 + 12*60 + 5;
    ASSERT_EQ(time_parse(test_correct),expectedSeconds);

    char test_zero[] = "000000";
    ASSERT_EQ(time_parse(test_zero), TIME_VALUE_ERROR);
}