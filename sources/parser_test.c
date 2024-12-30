#include <stdio.h>

#include "../headers/parser_test.h"
#include "../headers/parser.h"
#include "../headers/lexer.h"

int tabs = 0;

void print_tree(Expression * root){

  for (int i = 0; i < tabs; i++){
    printf("  ");
  }

  if (root->type == VALUE){
      switch (root->value->type) {
        case FLOAT:
          printf("FLOAT : %Lf\n", root->value->float_value);
          break;
        case INTEGER:
          printf("INTEGER %lld\n", root->value->integer_value);
          break;
        case UNSIGNED_INTEGER:
          printf("UNSIGNED INTEGER%llu\n", root->value->unsigned_integer_value);
          break;
        case BOOL:
          printf("BOOL : %s\n", root->value->bool_value ? "true" : "false");
          break;
        case STRING:
          printf("STRING : %s\n", root->value->string_value);
          break;
        case CHAR:
          printf("CHAR : %c\n", root->value->char_value);
          break;
        default:
          printf("UNKNOWN TYPE\n");
          break;
      }
      return;
  } else if (root->type == BINARY_OPERATION){
    switch(root->binary_operator->type){
      case ADD:
        printf("ADD\n");
        break;
      case SUB:
        printf("SUB\n");
        break;
      case MULT:
        printf("MULT\n");
        break;
      case DIV:
        printf("DIV\n");
        break;
      case MOD:
        printf("MOD\n");
        break;
      case GREATER_THAN:
        printf("GREATER_THAN\n");
        break;
      case GREATER_EQUAL_THAN:
        printf("GREATER_EQUAL_THAN\n");
        break;
      case LOWER_THAN:
        printf("LOWER_THAN\n");
        break;
      case LOWER_EQUAL_THAN:
        printf("LOWER_EQUAL_THAN\n");
        break;
      case EQUALS:
        printf("EQUALS\n");
        break;
      case NOT_EQUALS:
        printf("NOT_EQUALS\n");
        break;
      case LOGIC_AND:
        printf("AND\n");
        break;
      case LOGIC_OR:
        printf("OR\n");
        break;
      case LOGIC_XOR:
        printf("XOR\n");
        break;
    }
  }

  tabs++;

  print_tree(root->binary_operator->left_expression);
  print_tree(root->binary_operator->right_expression);
}