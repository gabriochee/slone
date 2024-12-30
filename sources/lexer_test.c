#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

#include "../headers/lexer_test.h"
#include "../headers/lexer.h"

void print_token(Token * token) {
    switch(token->type){
      case NAME:
        printf("NAME : %s\n", token->value);
        break;
      case IF:
        printf("IF\n");
        break;
      case ELSE:
        printf("ELSE\n");
        break;
      case FOR:
        printf("FOR\n");
        break;
      case WHILE:
        printf("WHILE\n");
        break;
      case CONTINUE:
        printf("CONTINUE\n");
        break;
      case BREAK:
        printf("BREAK\n");
        break;
      case AND:
        printf("AND\n");
        break;
      case OR:
        printf("OR\n");
        break;
      case XOR:
        printf("XOR\n");
        break;
      case TRUE:
        printf("TRUE\n");
        break;
      case FALSE:
        printf("FALSE\n");
        break;
      case NOT:
        printf("NOT\n");
        break;
      case NUMBER:
        printf("NUMBER : %s\n", token->value);
        break;
      case FLOATING_NUMBER:
        printf("FLOATING_NUMBER : %s\n", token->value);
        break;
      case S_STRING:
        printf("S_STRING : %s\n", token->value);
        break;
      case D_STRING:
        printf("D_STRING : %s\n", token->value);
        break;
      case SEMICOLON:
        printf("SEMICOLON\n");
        break;
      case COMMA:
        printf("COMMA\n");
        break;
      case DOT:
        printf("DOT\n");
        break;
      case ASSIGN:
        printf("ASSIGN\n");
        break;
      case EQUAL:
        printf("EQUAL\n");
        break;
      case UNEQUAL:
        printf("UNEQUAL\n");
        break;
      case GREATER:
        printf("GREATER\n");
        break;
      case GREATER_EQUAL:
        printf("GREATER_EQUAL\n");
        break;
      case LOWER:
        printf("LOWER\n");
        break;
      case LOWER_EQUAL:
        printf("LOWER_EQUAL\n");
        break;
      case PLUS:
        printf("PLUS\n");
        break;
      case U_PLUS:
        printf("U_PLUS\n");
        break;
      case MINUS:
        printf("MINUS\n");
        break;
      case U_MINUS:
        printf("U_MINUS\n");
        break;
      case STAR:
        printf("STAR\n");
        break;
      case SLASH:
        printf("SLASH\n");
        break;
      case PERCENT:
        printf("PERCENT\n");
        break;
      case EXCLAM:
        printf("EXCLAM\n");
        break;
      case LPAREN:
        printf("LPAREN\n");
        break;
      case RPAREN:
        printf("RPAREN\n");
        break;
      case LBRACE:
        printf("LBRACE\n");
        break;
      case RBRACE:
        printf("RBRACE\n");
        break;
      case LBRACKET:
        printf("LBRACKET\n");
        break;
      case RBRACKET:
        printf("RBRACKET\n");
        break;
      case END:
        printf("END\n");
        break;
      default:
        printf("UNKNOWN\n");
        break;
    }
}

void test_lexer(char * file_content){
  TokenStream * stream = NULL;
  Token * token = NULL;

  stream = new_token_stream(file_content);

  while ((token = next_token(stream))->type != END){
    print_token(token);
  }
}
