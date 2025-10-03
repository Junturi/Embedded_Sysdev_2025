#include <gtest/gtest.h>
#include "../TimeParser.h"

// Test with correct time
TEST(TimeParserTest, TestCaseCorrectTime) {
    char time_test[] = "141205";
    int expectedSeconds = 14*60*60 + 12*60 + 5;
    ASSERT_EQ(time_parse(time_test),expectedSeconds);
}

// Test for null pointer
TEST(TimeParserTest, TestCaseNullPointer) {
    ASSERT_EQ(time_parse(nullptr),TIME_ARRAY_ERROR);
}

// Test for time length
TEST(TimeParserTest, TestCaseTimeLength) {
    char time_test[] = "11022";
    ASSERT_EQ(time_parse(time_test), TIME_ARRAY_ERROR);
}

TEST(TimeParserTest, TestCaseIsDigit) {
    char time_test[] = "12A503";
    ASSERT_EQ(time_parse(time_test), TIME_VALUE_ERROR);
}

// Test with incorrect time
TEST(TimeParserTest, TestCaseIncorrectSeconds) {
    char time_test[] = "141265";
    ASSERT_EQ(time_parse(time_test),TIME_VALUE_ERROR);
}

TEST(TimeParserTest, TestCaseIncorrectMinutes) {
    char time_test[] = "148005";
    ASSERT_EQ(time_parse(time_test),TIME_VALUE_ERROR);
}

TEST(TimeParserTest, TestCaseIncorrectHours) {
    char time_test[] = "301205";
    ASSERT_EQ(time_parse(time_test),TIME_VALUE_ERROR);
}
