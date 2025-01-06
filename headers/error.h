#ifndef ERROR_H
#define ERROR_H

#include "lexer.h"

extern char * program_string_content; // extern évite que le compilateur pense qu'on déclare 2 fois la même variable.

void set_program_content(const char * program_string);
void print_error(const char* error_type, const char * error_message, Token * problematic_token);

#endif
