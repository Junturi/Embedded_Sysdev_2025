#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include "UartParser.h"

data_t uart_parse(char* input){
    data_t *buf = (data_t *)malloc(sizeof(data_t));
    if (buf == NULL) {
        // handle allocation failure
        data_t dummy = {"", 0};
        return dummy;
    }

    char *comma = strchr(input, ',');
    if (comma == NULL) {
        char* error_msg = "STRING_FORMAT_ERROR";
        size_t len = strlen(error_msg);
        strncpy(buf->color, error_msg, len);
        buf->color[len] = '\0';   
        buf->time_ms = STRING_FORMAT_ERROR;
        return *buf;
    }

    size_t len = comma - input;
    if (len >= sizeof(buf->color)) {
        len = sizeof(buf->color) - 1;
    }

    for (int i = 0; i < len; i++) {
        if (toupper((unsigned char)input[i]) != 'G' && 
            toupper((unsigned char)input[i]) != 'R' && 
            toupper((unsigned char)input[i]) != 'Y') {

            char* error_msg = "UNKNOWN_CHAR_ERROR";
            size_t len = strlen(error_msg);
            strncpy(buf->color, error_msg, len);
            buf->color[len] = '\0';   
            buf->time_ms = UNKNOWN_CHAR_ERROR;
            return *buf;
            }
    }

    strncpy(buf->color, input, len);
    buf->color[len] = '\0';

    int parsed_time = atoi(comma + 1);
    if (parsed_time < 0) {
        char* error_msg = "TIME_VALUE_ERROR";
        size_t len = strlen(error_msg);
        strncpy(buf->color, error_msg, len);
        buf->color[len] = '\0';   
        buf->time_ms = TIME_VALUE_ERROR;
        return *buf;
    }

    buf->time_ms = (uint32_t)parsed_time;

    return *buf;
}