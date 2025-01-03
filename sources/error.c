#include <stdio.h>

#include "../headers/error.h"

void set_file_content(const char * source){
  file_content = (char *)source;
}

void print_error(const char * error_message, Token * problematic_token){
  unsigned int token_position, last_return_position, line_number;

  token_position = problematic_token->position;
  line_number = 1;

  for (unsigned int i = 0; i <= token_position; i++){
    if (file_content[i] == '\n') {
      line_number++;
      last_return_position = i;
    }
  }

  fprintf(stderr, "ERREUR DE SYNTAXE.\n LIGNE | COLONNE : %u %u\n", line_number, token_position - last_return_position);

  // TODO : Implémenter une visualisation sympa de la zone de code aux alentours et souligner le token problèmatique.

}
