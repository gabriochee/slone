#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers/lexer.h"
#include "headers/lexer_test.h"
#include "headers/parser.h"
#include "headers/parser_test.h"

int main(const int argc, const char * argv[]){

  if (argc != 2) {
    const char * error_message = "ERREUR::ENTREE\n\nUn nombre (%d) d'arguments inadaptés a été entré.\n"
      "L'interpréteur Slone nécessite un seul argument, le chemin vers le fichier source.\n";

    fprintf(stderr, error_message, argc - 1);
    return -1;
  }

  FILE * file = NULL;
  char * file_content = NULL;
  unsigned int file_size = 0;

  if ((file = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "ERREUR::ENTREE\n\nImpossible d'ouvrir le fichier '%s'.\n", argv[1]);
    return -1;
  }

  while (fgetc(file) != EOF) {
    file_size++;
  }

  if ((file_content = calloc(file_size + 1, sizeof(char))) == NULL) {
    fprintf(stderr, "ERREUR::INTERPRETEUR::ALLOCATION_1\n\nL'allocation du contenu du fichier a échoué\n");
    fclose(file);
    return -1;
  }

  rewind(file);
  fread(file_content, sizeof(char), file_size, file);

  TokenStream * stream = new_token_stream(file_content);

  parse_statement(stream);

  fclose(file);
  free_token_stream(stream);
  free(stream);
  free(file_content);
  return 0;
}
