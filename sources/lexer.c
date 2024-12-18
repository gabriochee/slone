#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../headers/lexer.h"

void set_token_stream(TokenStream * stream) {
  // Permet d'initialiser un flux de tokens.
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
  for (int i = 0; i < stream->current; i++) {
    if (stream->tokens[i].value != NULL) {
      free(stream->tokens[i].value);
    }
  }

  free(stream->tokens);
}

TokenStream * new_token_stream(char * str) {
  const char keyword_set[] = "if else for while continue break and or xor true false";
  const char reserved_char_set[] = "!\"%'()*+,-./;<=>[]{}";

  size_t cursor1 = -1, cursor2;
  size_t len = strlen(str);

  TokenStream * stream = NULL;

  if ((stream = malloc(sizeof(TokenStream))) == NULL) {
    fprintf(stderr, "ERREUR::LEXEUR::ALLOCATION_1\n\nL'allocation mémoire du flux de tokens a échouée.\n");
    return NULL;
  };

  set_token_stream(stream);

    // Plan de décomposition :
    // ^	Caractère à caractère :
    // |        Est-ce un espace ?			  On l'ignore	|
    // |------- Est-ce un caractère reservé ? On l'ajoute	|
    // |    Portion de chaine de caractère :				|
    // |------- Est-ce un mot clé reservé ?   On l'ajoute	|
    // |------- Est-ce un nombre ?			  On l'ajoute	|
    // |------- C'est un nom				  On l'ajoute   v
    //
    // Note : Quand on ajoute on revient au début de la boucle.
    // 		  Quand on ignore on continue.

  while (++cursor1 < len) {
    cursor2 = cursor1;

    // ----------- Analyse simple (caractère à caractère)

    if (isspace(str[cursor1])) {
      continue;
    }

    if (stream->tokens == NULL) {
      fprintf(stderr, "ERREUR::LEXEUR::ALLOCATION_2\n\nL'allocation mémoire du flux de tokens a échouée.\n");
      return NULL;
    }

    switch (str[cursor1]) {
      case ';':
        push_token(stream, (Token){SEMICOLON, NULL, cursor1});
        continue;
      case ',':
        push_token(stream, (Token){COMMA, NULL, cursor1});
        continue;
      case '.':
        push_token(stream, (Token){DOT, NULL, cursor1});
        continue;
      case '!':
        push_token(stream, (Token){EXCLAM, NULL, cursor1});
        continue;
      case '\'':
        push_token(stream, (Token){SQUOTE, NULL, cursor1});
        continue;
      case '"':
        push_token(stream, (Token){DQUOTE, NULL, cursor1});
        continue;
      case '=':
        push_token(stream, (Token){EQUAL, NULL, cursor1});
        continue;
      case '>':
        push_token(stream, (Token){GREATER, NULL, cursor1});
        continue;
      case '<':
    	push_token(stream, (Token){LOWER, NULL, cursor1});
        continue;
      case '+':
    	push_token(stream, (Token){PLUS, NULL, cursor1});
        continue;
      case '-':
    	push_token(stream, (Token){MINUS, NULL, cursor1});
        continue;
      case '*':
    	push_token(stream, (Token){STAR, NULL, cursor1});
        continue;
      case '/':
    	push_token(stream, (Token){SLASH, NULL, cursor1});
        continue;
      case '%':
    	push_token(stream, (Token){PERCENT, NULL, cursor1});
        continue;
      case '(':
        push_token(stream, (Token){LPAREN, NULL, cursor1});
        continue;
      case ')':
        push_token(stream, (Token){RPAREN, NULL, cursor1});
        continue;
      case '{':
        push_token(stream, (Token){LBRACE, NULL, cursor1});
        continue;
      case '}':
        push_token(stream, (Token){RBRACE, NULL, cursor1});
        continue;
      case '[':
        push_token(stream, (Token){LBRACKET, NULL, cursor1});
        continue;
      case ']':
        push_token(stream, (Token){RBRACKET, NULL, cursor1});
        continue;
      default:
        break;
    }

    // ----------- Analyse complexe (portions de chaines de caractères)

	while (cursor2 < len && !isspace(str[cursor2]) && strchr(reserved_char_set, str[cursor2]) == NULL) {
  		cursor2++;
    }
    cursor2--;

    char * cpy = NULL;
    if ((cpy = calloc((cursor2 - cursor1 + 1), sizeof(char))) == NULL) {
      fprintf(stderr, "ERREUR::LEXEUR::ALLOCATION_3\n\nL'allocation mémoire du flux de tokens a échouée.\n");
      return NULL;
    }
    strncpy(cpy, str + cursor1, cursor2 - cursor1 + 1);

    if (strstr(keyword_set, cpy) != NULL) {
      if (strcmp(cpy, "if") == 0) {
      	push_token(stream, (Token){IF, NULL, cursor1});
      } else if (strcmp(cpy, "else") == 0) {
      	push_token(stream, (Token){ELSE, NULL, cursor1});
      } else if (strcmp(cpy, "for") == 0) {
      	push_token(stream, (Token){FOR, NULL, cursor1});
      } else if (strcmp(cpy, "while") == 0) {
      	push_token(stream, (Token){WHILE, NULL, cursor1});
      } else if (strcmp(cpy, "continue") == 0) {
      	push_token(stream, (Token){CONTINUE, NULL, cursor1});
      } else if (strcmp(cpy, "break") == 0) {
      	push_token(stream, (Token){BREAK, NULL, cursor1});
      } else if (strcmp(cpy, "and") == 0) {
      	push_token(stream, (Token){AND, NULL, cursor1});
      } else if (strcmp(cpy, "or") == 0) {
      	push_token(stream, (Token){OR, NULL, cursor1});
      } else if (strcmp(cpy, "xor") == 0) {
      	push_token(stream, (Token){XOR, NULL, cursor1});
      } else if (strcmp(cpy, "true") == 0) {
      	push_token(stream, (Token){TRUE, NULL, cursor1});
      } else if (strcmp(cpy, "false") == 0) {
      	push_token(stream, (Token){FALSE, NULL, cursor1});
      }

      cursor1 = cursor2;
      free(cpy);
      continue;
    }

    unsigned int pos = 0;

    while (isdigit(cpy[pos]) && pos++ < (cursor2 - cursor1 + 1));
    if (cursor2 - cursor1 + 1 == pos){
       push_token(stream, (Token){NUMBER, cpy, cursor1});
       cursor1 = cursor2;
       continue;
    }

    push_token(stream, (Token){NAME, cpy, cursor1});

    cursor1 = cursor2;
  }

  push_token(stream, (Token){END, NULL});

  return stream;
}
