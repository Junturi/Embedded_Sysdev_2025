#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "TimeParser.h"

// Time format: HHMMSS (6 characters)
int time_parse(char *time) {

	// How many seconds, default returns error
	int seconds = TIME_LEN_ERROR;

	// Check that string is not null
	if (!time) {
		return TIME_ARRAY_ERROR;
	}

	// Check that string is 6 characters long
	if (strlen(time) != 6) {
		return TIME_ARRAY_ERROR;
	}

	// Check that the string contains only numerical values
	// Will also automatically fail any negative values
	for (int i = 0; i < 6; i++) {
		if (!isdigit((unsigned char)time[i])) {
			return TIME_VALUE_ERROR;
		}
	}

	// Parse values from time string
	// For example: 124033 -> 12hour 40min 33sec
    int values[3];
	values[2] = atoi(time+4); // seconds
	time[4] = 0;
	values[1] = atoi(time+2); // minutes
	time[2] = 0;
	values[0] = atoi(time); // hours
	// Now you have:
	// values[0] hour
	// values[1] minute
	// values[2] second

	// Check that parsed times are valid: above limit not allowed
	// Limits are 59 for minutes, 23 for hours, etc
	if (values[2] > 59) {
		return TIME_VALUE_ERROR;
	}

	if (values[1] > 59) {
		return TIME_VALUE_ERROR;
	}

	if (values[0] > 23){
		return TIME_VALUE_ERROR;
	}

	// Calculate return value from the parsed minutes and seconds
	seconds = values[0]*60*60 + values[1]*60 + values[2];

	return seconds;
}
