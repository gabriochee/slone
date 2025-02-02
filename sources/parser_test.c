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
          printf("UNSIGNED INTEGER : %llu\n", root->value->unsigned_integer_value);
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
  } else if (root->type == VARIABLE){
    printf("VARIABLE : %s\n", root->variable->name);
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
      case EQUAL_TO:
        printf("EQUAL_TO\n");
        break;
      case NOT_EQUAL_TO:
        printf("NOT_EQUAL_TO\n");
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
  } else if (root->type == UNARY_OPERATION){
    switch(root->unary_operator->type){
      case LOGIC_NOT:
        printf("LOGIC_NOT\n");
        break;
      case UNARY_PLUS:
        printf("UNARY_PLUS\n");
        break;
      case UNARY_MINUS:
        printf("UNARY_MINUS\n");
        break;
      default:
        break;
    }
  } else if (root->type == EMPTY_EXPRESSION){
    printf("EMPTY_EXPRESSION\n");
  } else if (root->type == FUNCTION_CALL){
    printf("FUNCTION_CALL\n");
  }

  tabs++;

  if (root->type == BINARY_OPERATION){
  	print_tree(root->binary_operator->left_expression);
  	print_tree(root->binary_operator->right_expression);
  } else if (root->type == UNARY_OPERATION){
    print_tree(root->unary_operator->expression);
  }
}

void print_statement(Statement * root){
  switch (root->type){
  case ASSIGNMENT:
    printf("ASSIGNMENT\n");
    break;
  case CONDITIONAL_BRANCH:
    printf("IF ");
    print_tree(root->conditional_branch->expression);
    break;
  case FOR_LOOP:
    printf("FOR_LOOP\n");
    break;
  case WHILE_LOOP:
    printf("WHILE_LOOP\n");
    break;
  case LOOP_INSTRUCTION:
    printf("LOOP_INSTRUCTION\n");
    break;
  case PROGRAM:
    printf("PROGRAM\n");
    break;
  case EMPTY_STATEMENT:
    printf("EMPTY_STATEMENT\n");
    break;
  default:
    printf("UNKNOWN STATEMENT\n");
    break;
  }
}