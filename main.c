#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers/lexer.h"

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

  printf("TokenStream {\n  current : %u\n  token_count : %u\n}\n", stream->current, stream->token_count);

  for (int i = 0; i < stream->current; i++) {
    char car[20] = {0};
    switch (stream->tokens[i].type) {
  case NAME:
    strcpy(car, "NAME");
    break;
  case IF:
    strcpy(car, "IF");
    break;
  case ELSE:
    strcpy(car, "ELSE");
    break;
  case FOR:
    strcpy(car, "FOR");
    break;
  case WHILE:
    strcpy(car, "WHILE");
    break;
  case INTEGER:
    strcpy(car, "INTEGER");
    break;
  case FLOAT:
    strcpy(car, "FLOAT");
    break;
  case CHAR:
    strcpy(car, "CHAR");
    break;
  case STRING:
    strcpy(car, "STRING");
    break;
  case SEMICOLON:
    strcpy(car, "SEMICOLON");
    break;
  case EQUAL:
    strcpy(car, "EQUAL");
    break;
  case EXCLAM:
    strcpy(car, "EXCLAM");
    break;
  case COMMA:
    strcpy(car, "COMMA");
    break;
  case DOT:
    strcpy(car, "DOT");
    break;
  case LPAREN:
    strcpy(car, "LPAREN");
    break;
  case RPAREN:
    strcpy(car, "RPAREN");
    break;
  case LBRACE:
    strcpy(car, "LBRACE");
    break;
  case RBRACE:
    strcpy(car, "RBRACE");
    break;
  case LBRACKET:
    strcpy(car, "LBRACKET");
    break;
  case RBRACKET:
    strcpy(car, "RBRACKET");
    break;
  case END:
    strcpy(car, "END");
    break;
      default:
        strcpy(car, "UNKNOWN");
        break;
    }

    printf("%s\n", car);
  }

  fclose(file);
  free_token_stream(stream);
  free(file_content);
  free(stream);
  return 0;
}
