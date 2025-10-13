#ifndef UARTPARSER_H
#define UARTPARSER_H

// Error codes
#define STRING_FORMAT_ERROR         -1
#define UNKNOWN_CHAR_ERROR     -2
#define TIME_VALUE_ERROR            -3

struct data_t {
        void *fifo_reserved;
        char color[20];
        uint32_t time_ms;
};

data_t uart_parse(char* input);


#endif
