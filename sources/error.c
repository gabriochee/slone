#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../headers/error.h"

char * program_string_content;

void set_program_content(const char * program_string){
  program_string_content = (char *)program_string;
}

void print_error(const char * error_type, const char * error_message, Token * problematic_token){
  unsigned int token_position, last_return_position, line_number;

  token_position = problematic_token->position;
  line_number = 1;

  for (unsigned int i = 0; i <= token_position; i++){
    if (program_string_content[i] == '\n') {
      line_number++;
      last_return_position = i;
    }
  }

  fprintf(stderr, "%s\nLIGNE | COLONNE : %u %u\n", error_type, line_number, token_position - last_return_position + 1);

  unsigned int current = token_position, program_part_size = 0;

  if (problematic_token->value == NULL){
  	while (!isspace(program_string_content[current]) && program_string_content[current] != ';'){
    	fputc(program_string_content[current++], stderr);
        program_part_size++;
  	}
    fputc('\n', stderr);
  } else {
	fprintf(stderr, "%s\n", problematic_token->value);
    program_part_size = strlen(problematic_token->value);
  }

  for (int i = 0; i < program_part_size; i++){
    fprintf(stderr, "%c", '^');
  }
  fputc('\n', stderr);
  fprintf(stderr, "%s\n", error_message);

}
