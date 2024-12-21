#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../headers/lexer.h"

void set_token_stream(TokenStream * stream) {
  // Permet d'initialiser un flux de tokens.
  if (stream != NULL) {
    stream->tokens = malloc(sizeof(Token));
    stream->stream_capacity = 1;
    stream->current = 0;
  }
}

void push_token(TokenStream * stream, Token token) {
  if (stream->current == stream->stream_capacity) {
    stream->tokens = realloc(stream->tokens, (stream->stream_capacity *= 2) * sizeof(Token));
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

void rewind_token_stream(TokenStream * stream) {
  stream->current = 0;
}

Token * next_token(TokenStream * stream) {
  Token * next = stream->tokens + stream->current;
  if (stream->current < stream->stream_capacity && stream->tokens[stream->current].type != END) {
    stream->current++;
  }
  return next;
}

TokenStream * new_token_stream(char * str) {
  const char reserved_char_set[] = "!\"%'()*+,-./;<=>[]{}";

  size_t cursor1 = -1, cursor2;
  size_t len = strlen(str);

  unsigned char in_string = 0;

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
        if (in_string){
          in_string = 0;
          continue;
        }
        in_string = '\'';
        break;
      case '"':
        push_token(stream, (Token){DQUOTE, NULL, cursor1});
        if (in_string){
          in_string = 0;
          continue;
        }
        in_string = '"';
        break;
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

    if (in_string){
      while (cursor2 < len && str[cursor2 + 1] != in_string) {
        cursor2++;
      }
      cursor1++;
    } else {
        while (cursor2 < len && !isspace(str[cursor2 + 1]) && strchr(reserved_char_set, str[cursor2 + 1]) == NULL) {
            cursor2++;
        }
    }

    char * cpy = NULL;
    if ((cpy = calloc((cursor2 - cursor1 + 1), sizeof(char))) == NULL) {
      fprintf(stderr, "ERREUR::LEXEUR::ALLOCATION_3\n\nL'allocation mémoire du flux de tokens a échouée.\n");
      return NULL;
    }
    strncpy(cpy, str + cursor1, cursor2 - cursor1 + 1);

    if (in_string){
      push_token(stream, (Token){ (in_string == '"') ? D_STRING : S_STRING, cpy, cursor1});
      cursor1 = cursor2;
      continue;
    }

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
    } else if (strcmp(cpy, "not") == 0) {
      push_token(stream, (Token){NOT, NULL, cursor1});
    } else {
      goto skip;
    }

    cursor1 = cursor2;
    free(cpy);
    continue;

    skip:

    unsigned int pos = 0;

    while (isdigit(cpy[pos]) && ++pos < (cursor2 - cursor1 + 1));
    if (cursor2 - cursor1 + 1 == pos){
       push_token(stream, (Token){NUMBER, cpy, cursor1});
       cursor1 = cursor2;
       continue;
    }

    push_token(stream, (Token){NAME, cpy, cursor1});

    cursor1 = cursor2;
  }

  push_token(stream, (Token){END, NULL});

  rewind_token_stream(stream);

  return stream;
}
