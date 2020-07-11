#pragma once

#include <stdbool.h>

extern char* user_input;

void error(char* loc, char* fmt, ...);
bool start_with(char* src, char* expect);
