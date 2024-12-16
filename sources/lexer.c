#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../headers/lexer.h"

void set_token_stream(TokenStream * stream) {
  if (stream != NULL) {
    stream->tokens = malloc(sizeof(Token));
    stream->token_count = 1;
    stream->current = 0;
  }
}

void push_token(TokenStream * stream, Token token) {
  if (stream->current == stream->token_count) {
    stream->tokens = realloc(stream->tokens, (stream->token_count *= 2) * sizeof(Token));
  }

  if (stream->tokens != NULL) {
    stream->tokens[stream->current++] = token;
  }
}

void free_token_stream(TokenStream * stream) {
  free(stream->tokens);
}

TokenStream * new_token_stream(char * str) {
  size_t cursor1 = -1;//, cursor2 = 0;
  size_t len = strlen(str);

  TokenStream * stream = NULL;

  if ((stream = malloc(sizeof(TokenStream))) == NULL) {
    fprintf(stderr, "ERREUR::LEXEUR::ALLOCATION_1\n\nL'allocation mémoire du flux de tokens a échoué.\n");
    return NULL;
  };

  set_token_stream(stream);

  main_boucle:
  while (++cursor1 < len) {

    while (isspace(str[cursor1])) {
      cursor2 = cursor1 + 1;
      goto main_boucle;
    }

    if (stream->tokens == NULL) {
      fprintf(stderr, "ERREUR::LEXEUR::ALLOCATION_2\n\nL'allocation mémoire du flux de tokens a échoué.\n");
      return NULL;
    }

    switch (str[cursor1]) {
      case ';':
        push_token(stream, (Token){SEMICOLON, ";"});
        goto main_boucle;
      case ',':
        push_token(stream, (Token){COMMA, ","});
        goto main_boucle;
      case '.':
        push_token(stream, (Token){DOT, "."});
        goto main_boucle;
      case '=':
        push_token(stream, (Token){EQUAL, "="});
        goto main_boucle;
      case '!':
        push_token(stream, (Token){EXCLAM, "!"});
        goto main_boucle;
      case '(':
        push_token(stream, (Token){LPAREN, "("});
        goto main_boucle;
      case ')':
        push_token(stream, (Token){RPAREN, ")"});
        goto main_boucle;
      case '{':
        push_token(stream, (Token){LBRACE, "{"});
        goto main_boucle;
      case '}':
        push_token(stream, (Token){RBRACE, "}"});
        goto main_boucle;
      case '[':
        push_token(stream, (Token){LBRACKET, "["});
        goto main_boucle;
      case ']':
        push_token(stream, (Token){RBRACKET, "]"});
        goto main_boucle;
      default:
        break;
    }



  }

  push_token(stream, (Token){END, NULL});

  return stream;
}
