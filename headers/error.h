#include <stdio.h>

#include "lexer.h"

#ifndef ERROR_H
#define ERROR_H

char * file_content = NULL;

void set_file_content(const char * source);
void print_error(const char * error_message, Token * problematic_token);

#endif //ERROR_H
