#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/lexer.h"
#include "../headers/parser.h"

#include "../headers/lexer_test.h"
#include "../headers/parser_test.h"

#define VALUE_MASK               0b000000000000000000001111111
#define NUMBER_MASK              0b000000000000000000000000110
#define STRING_MASK              0b000000000000000000000011000
#define BOOLEAN_VALUE_MASK       0b000000000000000000001100000
#define BOOLEAN_OPERATOR_MASK    0b000000000000000011110000000
#define ARITHMETIC_OPERATOR_MASK 0b000000000111111100000000000
#define COMPARISON_OPERATOR_MASK 0b001111111000000000000000000
#define PARENTHESES_MASK         0b110000000000000000000000000

#define BINARY_OPERATOR_MASK     0b001111111111110001110000000
#define UNARY_OPERATOR_MASK      0b000000000000001110000000000
#define EQUALITY_OPERATOR_MASK   0b001100000000000000000000000

void add_instruction(Program * program, Instruction * instruction) {
  if (program->current_instruction >= program->instruction_capacity) {
    program->instructions = realloc(program->instructions, (program->instruction_capacity *= 2) * sizeof(Instruction *));
  }

  if (program->instructions != NULL) {
    program->instructions[program->current_instruction++] = instruction;
  }
}

void free_program(Program * program) {
  free(program->instructions);
  free_variable_dictionnary(program->variable_dictionary);
  free(program);
}

void free_variable_dictionnary(VariableDictionnary * variable_dictionnary) {
  free(variable_dictionnary->variables);
  free(variable_dictionnary);
}

int is_token_expression(Token * token){
  // La fonction ne renvoie pas seulement si le token actuel peut être considéré comme une expression,
  // mais il renvoie aussi quel type d'après ce plan selon chaque bit (La position suit le petit boutisme):
  // bit n°1 = NAME                                   ] Ensemble des valeurs
  // bit n°2 = NUMBER                                 ]
  // bit n°3 = FLOATING_NUMBER                        ]
  // bit n°4 = S_STRING       | Ensemble des strings  ]
  // bit n°5 = D_STRING       |                       ]
  // bit n°6 = TRUE           ) Ensemble des booléens ]
  // bit n°7 = FALSE          )                       ]
  // bit n°8 = AND            } Ensemble des opérateurs booléens
  // bit n°9 = OR             }
  // bit n°10 = XOR           }                                       | Opérateurs unaires
  // bit n°11 = NOT           }                                       |
  // bit n°12 = U_PLUS        ] Ensemble des opérateurs arithmétiques |
  // bit n°13 = U_MINUS       ]
  // bit n°14 = PLUS          ]
  // bit n°15 = MINUS         ]
  // bit n°16 = STAR          ]
  // bit n°17 = SLASH         ]
  // bit n°18 = PERCENT       ]
  // bit n°19 = LOWER         } Ensemble des comparaisons
  // bit n°20 = LOWER_EQUAL   }
  // bit n°21 = GREATER       }
  // bit n°22 = GREATER_EQUAL }
  // bit n°23 = UNEQUAL       }
  // bit n°24 = EQUAL         }
  // bit n°25 = LPAREN        | Ensemble des parenthèses
  // bit n°26 = RPAREN        |

  int mask = 0;

  mask =  (token->type == NAME)                  |
          (token->type == NUMBER)          << 1  |
          (token->type == FLOATING_NUMBER) << 2  |
          (token->type == S_STRING)        << 3  |
          (token->type == D_STRING)        << 4  |
          (token->type == TRUE)            << 5  |
          (token->type == FALSE)           << 6  |
          (token->type == AND)             << 7  |
          (token->type == OR)              << 8  |
          (token->type == XOR)             << 9  |
          (token->type == NOT)             << 10 |
          (token->type == U_PLUS)          << 11 |
          (token->type == U_MINUS)         << 12 |
          (token->type == PLUS)            << 13 |
          (token->type == MINUS)           << 14 |
          (token->type == STAR)            << 15 |
          (token->type == SLASH)           << 16 |
          (token->type == PERCENT)         << 17 |
          (token->type == LOWER)           << 18 |
          (token->type == LOWER_EQUAL)     << 19 |
          (token->type == GREATER)         << 20 |
          (token->type == GREATER_EQUAL)   << 21 |
          (token->type == UNEQUAL)         << 22 |
          (token->type == EQUAL)           << 23 |
          (token->type == LPAREN)          << 24 |
          (token->type == RPAREN)          << 25;

  return mask;
}

int is_token_statement(Token * token) {
  return token->type == NAME || token->type == ASSIGN ||
         token->type == IF   || token->type == ELSE   ||
         token->type == FOR  || token->type == WHILE  ||
         token->type == CONTINUE || token->type == BREAK ||
         token->type == LBRACE;
}

short operator_precedence(Token * token) {
  short mask = 0;
  mask =  (token->type == LPAREN)                                                            |
          (token->type == OR)                                                           << 1 |
          (token->type == AND)                                                          << 2 |
          (token->type == XOR)                                                          << 3 |
          (token->type == AND)                                                          << 4 |
          ((token->type == UNEQUAL) || (token->type == EQUAL))                          << 5 |
          ((token->type == GREATER) || (token->type == LOWER)
          || (token->type == GREATER_EQUAL) || (token->type == LOWER_EQUAL))            << 6 |
          ((token->type == PLUS) || (token->type == MINUS))                             << 7 |
          ((token->type == STAR) || (token->type == PERCENT) || (token->type == SLASH)) << 8 |
          ((token->type == U_PLUS) || (token->type == U_MINUS))                         << 9 ;

  return mask;
}

Program * new_program() {
  Program * program = malloc(sizeof(Program));

  if (program != NULL) {
    if ((program->instructions = malloc(sizeof(Instruction*))) != NULL && (program->variable_dictionary = new_variable_dictionary()) != NULL){
      program->instruction_capacity = 1;
      program->current_instruction = 0;
      return program;
    }
    free_program(program);
  }

  return NULL;
}

TokenStack * new_token_stack() {
  TokenStack * stack = malloc(sizeof(TokenStack));

  if (stack != NULL) {
    stack->is_empty = 1;
    stack->current= 0;
    stack->token_capacity = 1;
    stack->tokens = malloc(sizeof(Token *));
  }

  return stack;
}

VariableDictionnary * new_variable_dictionary() {
  VariableDictionnary * dictionary = malloc(sizeof(VariableDictionnary));

  if (dictionary != NULL) {
    if ((dictionary->variables = malloc(sizeof(Variable *))) != NULL) {
      dictionary->capacity = 1;
      dictionary->current = 0;
      return dictionary;
    }

    free(dictionary);
  }

  return NULL;
}

Token * pop_token_stack(TokenStack * token_stack) {
  if (token_stack->current > 1) {
    return token_stack->tokens[--token_stack->current];
  }

  if (!token_stack->is_empty) {
    token_stack->is_empty = 1;
    return token_stack->tokens[--token_stack->current];
  }

  return NULL;
}

Token * top_token_stack(TokenStack * token_stack) {
  if (token_stack->current > 0) {
    return token_stack->tokens[token_stack->current-1];
  }

  return token_stack->tokens[token_stack->current];
}

void push_token_stack(TokenStack * token_stack, Token * token) {
  if (token_stack->current >= token_stack->token_capacity) {
    token_stack->tokens = realloc(token_stack->tokens, (token_stack->token_capacity *= 2) * sizeof(Token *));
  }

  if (token_stack->tokens != NULL) {
    token_stack->is_empty = 0;
    token_stack->tokens[token_stack->current++] = token;
  }
}

void free_token_stack(TokenStack * token_stack) {
  if (token_stack->tokens != NULL) {
    free(token_stack->tokens);
  }

  free(token_stack);
}

// parse_expression cas :
// A UTILISER : Notation Polonaise + Shunting Yard Algorithm
// Explication Notation Polonaise = n1 n2 opérateur
// Exemple : 1 + 2 * 3 = 1 2 3 * +
//
// Shunting Yard Algorithm :
// 2 piles : Sortie / opérateurs
// Toutes les valeurs (variables, nombres, values etc..) vont directement dans la sortie
//
// Pour les opérateurs :
//   Si la pile est vide ou que l'opérateur au sommet de la pile a une précédence inférieure à notre opérateur ou
//   que c'est une parenthèse ouverte, alors on empile notre opérateur sur la pile des opérateurs.
//
//   Cependant, si notre opérateur à une précédence inférieure ou égale à l'opérateur au sommet de la pile, alors on
//   dépile l'opérateur du sommet de la pile des opérateurs vers la pile de sortie et on empile notre opérateur sur
//   la pile des opérateurs.
//
//  Finalement, si notre opérateur est une parenthèse fermante, alors on dépile toutes les valeurs de la pile des
//  opérateurs vers la pile de sortie jusqu'à ce que l'on rencontre une parenthèse ouvrante dans la pile des opérateurs.
//
// Pour faire une structure en arbre, on lit de droite à gauche

TokenStack * infix_to_postfix(TokenStream * stream) {
  Token * token = NULL;
  TokenStack * operator_stack = NULL;
  TokenStack * output_stack = NULL;

  if ((operator_stack = new_token_stack()) == NULL) {
    fprintf(stderr, "ERREUR::PARSER::PARSE_EXPRESSION::INFIX_TO_POSTFIX::ALLOCATION_1\n\nErreur d'allocation de la pile des opérateurs.\n");
    return NULL;
  }

  if ((output_stack = new_token_stack()) == NULL) {
    free_token_stack(operator_stack);
    fprintf(stderr, "ERREUR::PARSER::PARSE_EXPRESSION::INFIX_TO_POSTFIX::ALLOCATION_2\n\nErreur d'allocation de la pile de sortie.\n");
    return NULL;
  }

  int mask;

  while ((mask = is_token_expression(token = next_token(stream)))) {
    if (mask & VALUE_MASK) {
      push_token_stack(output_stack, token);
    } else if (token->type == LPAREN) {
      push_token_stack(operator_stack, token);
    } else if (token->type == RPAREN) {
      while (!operator_stack->is_empty && top_token_stack(operator_stack)->type != LPAREN) {
        push_token_stack(output_stack, pop_token_stack(operator_stack));
      }
      if (top_token_stack(operator_stack)->type != LPAREN) {
        // Insérer message d'erreur parenthèse fermante en trop.
        free_token_stack(operator_stack);
        free_token_stack(output_stack);
        return NULL;
      }
      pop_token_stack(operator_stack);
    } else if (operator_stack->is_empty) {
      push_token_stack(operator_stack, token);
    } else if (operator_precedence(token) > operator_precedence(top_token_stack(operator_stack))) {
      push_token_stack(operator_stack, token);
    } else {
      push_token_stack(output_stack, pop_token_stack(operator_stack));
      push_token_stack(operator_stack, token);
    }
  }

  while (!operator_stack->is_empty) {
    push_token_stack(output_stack, pop_token_stack(operator_stack));
  }

  return output_stack;
}

Expression * parse_expression(TokenStack * postfix_expression){
  Token * top_token = NULL;
  Expression * expression = NULL;

  if ((expression = malloc(sizeof(Expression))) == NULL) {
    fprintf(stderr, "ERREUR::PARSER::PARSE_EXPRESSION::ALLOCATION_1\n\nErreur d'allocation de l'expression.\n");
    return NULL;
  }

  expression->type = EMPTY;

  if (!postfix_expression->is_empty) {
    top_token = pop_token_stack(postfix_expression);
    int token_mask = is_token_expression(top_token);

    if (token_mask & VALUE_MASK) {
      Value * value = NULL;
      expression->type = VALUE;

      if ((value = malloc(sizeof(Value))) == NULL) {
        fprintf(stderr, "ERREUR::PARSER::PARSE_EXPRESSION::ALLOCATION_2\n\nErreur d'allocation de la valeur.\n");
        return NULL;
      }

      expression->value = value;

      if (token_mask & NUMBER_MASK) {
        if (top_token->type == FLOATING_NUMBER) {
          value->type = FLOAT;
          value->float_value = strtold(top_token->value, NULL);
          if (errno == ERANGE) {
            // Message d'erreur : fonction pretty_error
            fprintf(stderr, "Trop grand !\n");
            return NULL;
          }
        } else {
          value->type = INTEGER;
          value->integer_value = strtoll(top_token->value, NULL, 10);
          if (errno == ERANGE) {
            errno = 0;
            value->type = UNSIGNED_INTEGER;
            value->unsigned_integer_value = strtoull(top_token->value, NULL, 10);
          }
          if (errno == ERANGE) {
            // Message d'erreur : fonction pretty_error
            fprintf(stderr, "Trop grand !\n");
            return NULL;
          }
        }
      } else if (token_mask & BOOLEAN_VALUE_MASK) {
        value->type = BOOL;
        value->bool_value = (top_token->type == TRUE);
      } else if (token_mask & STRING_MASK) {
        if (strlen(top_token->value) == 1) {
          value->type = CHAR;
          value->char_value = top_token->value[0];
        } else {
          value->type = STRING;
          value->string_value = top_token->value;
        }
      } else if (top_token->type == NAME) {
        Variable * variable = NULL;

        if ((variable = malloc(sizeof(Variable))) == NULL) {
          fprintf(stderr, "ERREUR::PARSER::PARSE_EXPRESSION::ALLOCATION_3\n\nErreur d'allocation de la variable.\n");
          return NULL;
        }

        variable->name = top_token->value;

        expression->type = VARIABLE;
        expression->variable = variable;

      }
    } else if (token_mask & BINARY_OPERATOR_MASK) {
      BinaryOperator * binary_operator = NULL;

      expression->type = BINARY_OPERATION;

      if ((binary_operator = malloc(sizeof(BinaryOperator))) == NULL) {
        fprintf(stderr, "ERREUR::PARSER::PARSE_EXPRESSION::ALLOCATION_3\n\nErreur d'allocation de l'opérateur binaire.\n");
        return NULL;
      }

      expression->binary_operator = binary_operator;

      Expression * left_expression = NULL;
      Expression * right_expression = NULL;

      right_expression = parse_expression(postfix_expression);
      left_expression = parse_expression(postfix_expression);

      if (right_expression != NULL && left_expression != NULL) {
        expression->type = BINARY_OPERATION;
        switch (top_token->type) {
          case AND:
            expression->binary_operator->type = LOGIC_AND;
          break;
          case OR:
            expression->binary_operator->type = LOGIC_OR;
          break;
          case XOR:
            expression->binary_operator->type = LOGIC_XOR;
          break;
          case EQUAL:
            expression->binary_operator->type = EQUAL_TO;
          break;
          case UNEQUAL:
            expression->binary_operator->type = NOT_EQUAL_TO;
          break;
          case GREATER:
            expression->binary_operator->type = GREATER_THAN;
          break;
          case GREATER_EQUAL:
            expression->binary_operator->type = GREATER_EQUAL_THAN;
          break;
          case LOWER:
            expression->binary_operator->type = LOWER_THAN;
          break;
          case LOWER_EQUAL:
            expression->binary_operator->type = LOWER_EQUAL_THAN;
          break;
          case PLUS:
            expression->binary_operator->type = ADD;
          break;
          case MINUS:
            expression->binary_operator->type = SUB;
          break;
          case STAR:
            expression->binary_operator->type = MULT;
          break;
          case SLASH:
            expression->binary_operator->type = DIV;
          break;
          case PERCENT:
            expression->binary_operator->type = MOD;
          break;
          default:
            break;
        }

        binary_operator->left_expression = left_expression;
        binary_operator->right_expression = right_expression;
      }
    } else if (token_mask & UNARY_OPERATOR_MASK) {
      UnaryOperator *unary_operator = NULL;

      if ((unary_operator = malloc(sizeof(UnaryOperator))) == NULL) {
        fprintf(stderr, "ERREUR::PARSER::PARSE_EXPRESSION::ALLOCATION_3\n\nErreur d'allocation de l'opérateur binaire.\n");
        return NULL;
      }

      Expression *next_expression = parse_expression(postfix_expression);

      if (next_expression != NULL) {
        switch (top_token->type) {
          case NOT:
            unary_operator->type = LOGIC_NOT;
            break;
          case U_PLUS:
            unary_operator->type = UNARY_PLUS;
            break;
          case U_MINUS:
            unary_operator->type = UNARY_MINUS;
            break;
          default:
            break;
        }
        unary_operator->expression = next_expression;

        expression->unary_operator = unary_operator;
        expression->type = UNARY_OPERATION;
      }
    }
  }

  return expression;
}

Statement * parse_statement(TokenStream * stream) {
  Token * token = NULL;
  Statement * statement = NULL;

  if ((statement = malloc(sizeof(Statement))) == NULL) {
    fprintf(stderr, "ERREUR::PARSER::PARSE_STATEMENT::ALLOCATION_1\n\nErreur d'allocation de l'instruction.\n");
    return NULL;
  }

  while ((token = next_token(stream))->type != END) {
    if (token->type == ASSIGN) {
      if (get_token(stream, -1)->type != NAME) {
        // Gérer l'erreur
        printf("ASSIGNATION IMPOSSIBLE !\n");
        return NULL;
      }
    } else if (token->type == IF) {
      TokenStack * token_stack = infix_to_postfix(stream);
      if (token_stack == NULL) {
        // Gérer l'erreur
        return NULL;
      }

      Expression * condition = parse_expression(token_stack);

      if (condition->type == EMPTY) {
        // Gérer l'erreur
        printf("PAS DE CONDITION !\n");
        free_token_stack(token_stack);
        return NULL;
      }

      if (get_token(stream, -1)->type != LBRACE) {
        // Gérer l'erreur
        printf("PAS D'ACCOLADE !\n");
        free_token_stack(token_stack);
        return NULL;
      }

      Program * true_branch = parse(stream);


    }
  }

  return NULL;
}

Program * parse(TokenStream * stream){
  Program * program = NULL;

  if ((program = new_program()) == NULL) {
    fprintf(stderr, "ERREUR::PARSER::ALLOCATION_1\n\nL'allocation mémoire de l'arbre de syntaxe a échouée.\n");
    return NULL;
  }

  while (current_token(stream)->type != END && current_token(stream)->type != RBRACE) {
    Instruction * instruction = NULL;

    if (is_token_statement(current_token(stream))) {
      instruction = malloc(sizeof(Instruction));

      if (instruction == NULL) {
        fprintf(stderr, "ERREUR::PARSER::ALLOCATION_2\n\nL'allocation mémoire de l'instruction a échouée.\n");
        free(program->instructions);
        free(program);
        return NULL;
      }

      instruction->type = STATEMENT;
      instruction->statement = parse_statement(stream);

      if (instruction->expression == NULL) {
        free(instruction);
        free(program->instructions);
        free(program);
        return NULL;
      }

      add_instruction(program, instruction);
    }

    if (is_token_expression(current_token(stream))) {
      instruction = malloc(sizeof(Instruction));
      TokenStack * stack = NULL;

      if (instruction == NULL) {
        fprintf(stderr, "ERREUR::PARSER::ALLOCATION_3\n\nL'allocation mémoire de l'instruction a échouée.\n");
        free(program->instructions);
        free(program);
        return NULL;
      }

      if ((stack = infix_to_postfix(stream)) == NULL) {
        free(instruction);
        free(program->instructions);
        free(program);
        return NULL;
      }

      instruction->type = EXPRESSION;
      instruction->expression = parse_expression(stack);

      if (instruction->expression == NULL) {
        free(stack);
        free(instruction);
        free(program->instructions);
        free(program);
        return NULL;
      }

      add_instruction(program, instruction);
    }
  }

  return program;
}
