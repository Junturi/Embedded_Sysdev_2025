#include <gtest/gtest.h>
#include "../UartParser.h"

// Test with correct uart-message
TEST(UartParserTest, TestUartCorrectMsg) {
    data_t result = uart_parse("RYG,1000");
    EXPECT_STREQ(result.color, "RYG");
    EXPECT_EQ(result.time_ms, 1000);
}

// Test with incorrect uart-message format
TEST(UartParserTest, TestUartNoComma) {
    data_t result = uart_parse("RYG,1000");
    EXPECT_STREQ(result.color, "RYG");
    EXPECT_EQ(result.time_ms, 1000);

    data_t no_comma = uart_parse("RYG1000");
    EXPECT_STREQ(no_comma.color, "STRING_FORMAT_ERROR");
    EXPECT_EQ(no_comma.time_ms, STRING_FORMAT_ERROR);
}

// Test with incorrect character in uart-message
TEST(UartParserTest, TestUartIncorrectChar) {
    data_t result = uart_parse("RYG,1000");
    EXPECT_STREQ(result.color, "RYG");
    EXPECT_EQ(result.time_ms, 1000);

    data_t incorrect_char = uart_parse("RYGB,1000");
    EXPECT_STREQ(incorrect_char.color, "UNKNOWN_CHAR_ERROR");
    EXPECT_EQ(incorrect_char.time_ms, UNKNOWN_CHAR_ERROR);
}

// Test with out of bounds time
// Time is out of bounds if it's less than 0
TEST(UartParserTest, TestUartIncorrectTime) {
    data_t result = uart_parse("RYG,1000");
    EXPECT_STREQ(result.color, "RYG");
    EXPECT_EQ(result.time_ms, 1000);
    
    data_t incorrect_time = uart_parse("RYG,-1");
    EXPECT_STREQ(incorrect_time.color, "TIME_VALUE_ERROR");
    EXPECT_EQ(incorrect_time.time_ms, TIME_VALUE_ERROR);
}