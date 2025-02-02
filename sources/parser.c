#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/lexer.h"
#include "../headers/lexer_test.h"
#include "../headers/parser.h"
#include "../headers/parser_test.h"
#include "../headers/error.h"

#define VALUE_MASK               0b0000000000000000000011111111
#define NUMBER_MASK              0b0000000000000000000000001100
#define STRING_MASK              0b0000000000000000000000110000
#define BOOLEAN_VALUE_MASK       0b0000000000000000000011000000
#define BOOLEAN_OPERATOR_MASK    0b0000000000000000111100000000
#define ARITHMETIC_OPERATOR_MASK 0b0000000001111111000000000000
#define COMPARISON_OPERATOR_MASK 0b0011111110000000000000000000
#define PARENTHESES_MASK         0b1100000000000000000000000000

#define BINARY_OPERATOR_MASK     0b0011111111111100011100000000
#define UNARY_OPERATOR_MASK      0b0000000000000011100000000000
#define EQUALITY_OPERATOR_MASK   0b0011000000000000000000000000

void add_instruction(Program * program, Instruction * instruction) {
  if (program->current_instruction >= program->instruction_capacity) {
    program->instructions = realloc(program->instructions, (program->instruction_capacity *= 2) * sizeof(Instruction *));
  }

  if (program->instructions != NULL) {
    program->instructions[program->current_instruction++] = instruction;
  }
}

void free_variable_dictionnary(VariableDictionnary * variable_dictionnary) {
  for (int i = 0; i < variable_dictionnary->current; i++) {
    free_variable(variable_dictionnary->variables[i]);
    free_value(variable_dictionnary->values[i]);
  }
  free(variable_dictionnary->variables);
  free(variable_dictionnary);
}

int is_token_expression(Token * token){
  // La fonction ne renvoie pas seulement si le token actuel peut être considéré comme une expression,
  // mais il renvoie aussi quel type d'après ce plan selon chaque bit (La position suit le petit boutisme):
  // bit n°1 = NAME                                   ] Ensemble des valeurs
  // bit n°2 = FUNCTION                               ]
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
          (token->type == FUNCTION)        << 1  |
          (token->type == NUMBER)          << 2  |
          (token->type == FLOATING_NUMBER) << 3  |
          (token->type == S_STRING)        << 4  |
          (token->type == D_STRING)        << 5  |
          (token->type == TRUE)            << 6  |
          (token->type == FALSE)           << 7  |
          (token->type == AND)             << 8  |
          (token->type == OR)              << 9  |
          (token->type == XOR)             << 10 |
          (token->type == NOT)             << 11 |
          (token->type == U_PLUS)          << 12 |
          (token->type == U_MINUS)         << 13 |
          (token->type == PLUS)            << 14 |
          (token->type == MINUS)           << 15 |
          (token->type == STAR)            << 16 |
          (token->type == SLASH)           << 17 |
          (token->type == PERCENT)         << 18 |
          (token->type == LOWER)           << 19 |
          (token->type == LOWER_EQUAL)     << 20 |
          (token->type == GREATER)         << 21 |
          (token->type == GREATER_EQUAL)   << 22 |
          (token->type == UNEQUAL)         << 23 |
          (token->type == EQUAL)           << 24 |
          (token->type == LPAREN)          << 25 |
          (token->type == RPAREN)          << 26;

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
    if ((dictionary->variables = malloc(sizeof(Variable *))) != NULL && (dictionary->values = malloc(sizeof(Value *))) != NULL) {
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

FunctionCall * new_function_call() {
  FunctionCall * function = malloc(sizeof(FunctionCall));
  if (function != NULL) {
    function->parameters = malloc(sizeof(Expression*));
    if (function->parameters != NULL) {
      function->current_parameter = 0;
      function->parameters_count = 1;

      return function;
    }
  }

  return NULL;
}

FunctionCall * parse_function_call(TokenStream * stream) {
  FunctionCall * function_call = new_function_call();

  next_token(stream);

  if (function_call != NULL) {
    function_call->name = current_token(stream)->value;
    TokenStack * token_stack = infix_to_postfix(stream);
    print_token(current_token(stream));

    if (token_stack != NULL) {
      Expression *arg = parse_expression(token_stack);

      if (arg != NULL) {
        add_argument(function_call, arg);
      }

    }
  }

  return NULL;
}

void add_argument(FunctionCall * function_call, Expression * expression) {
  if (function_call->current_parameter >= function_call->parameters_count) {
    function_call->parameters = realloc(function_call->parameters, (function_call->parameters_count *= 2) * sizeof(Expression *));
  }

  if (function_call->parameters != NULL) {
    function_call->parameters[function_call->current_parameter++] = expression;
  }
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

void free_value(Value * value) {
  if (value == NULL) {
    return;
  }

  if (value->type == STRING) {
    free(value->string_value);
  }

  free(value);
}

void free_variable(Variable * variable) {
  if (variable == NULL) {
    return;
  }

  free(variable->name);
  free(variable);
}

void free_function(FunctionCall * function_call) {
  if (function_call == NULL) {
    return;
  }

  for (unsigned int i = 0; i < function_call->parameters_count; i++) {
    free_expression(function_call->parameters[i]);
  }

  free(function_call->parameters);
  free(function_call);
}

void free_binary_operator(BinaryOperator * binary_operator) {
  if (binary_operator == NULL) {
    return;
  }

  free_expression(binary_operator->left_expression);
  free_expression(binary_operator->right_expression);
  free(binary_operator);
}

void free_unary_operator(UnaryOperator * unary_operator) {
  if (unary_operator == NULL) {
    return;
  }

  free_expression(unary_operator->expression);
  free(unary_operator);
}

void free_expression(Expression * expression) {
  if (expression == NULL) {
    return;
  }

  switch (expression->type) {
    case VALUE:
      free_value(expression->value);
      break;
    case VARIABLE:
      free_variable(expression->variable);
      break;
    case BINARY_OPERATION:
      free_binary_operator(expression->binary_operator);
      break;
    case UNARY_OPERATION:
      free_unary_operator(expression->unary_operator);
      break;
    case EXPR:
      free_expression(expression);
      break;
    default:
      break;
  }

  free(expression);
}

void free_assignment(Assignment * assignment) {
  if (assignment == NULL) {
    return;
  }

  free_variable(assignment->variable);
  free_expression(assignment->value);
  free(assignment);
}

void free_conditional_branch(ConditionalBranch * conditional_branch) {
  if (conditional_branch == NULL) {
    return;
  }

  free_expression(conditional_branch->expression);
  free_program(conditional_branch->true_branch);
  free_program(conditional_branch->false_branch);
  free(conditional_branch);
}

void free_while_loop(WhileLoop * while_loop) {
  if (while_loop == NULL) {
    return;
  }

  free_expression(while_loop->condition);
  free_program(while_loop->program);
  free(while_loop);
}

void free_for_loop(ForLoop * for_loop) {
  if (for_loop == NULL) {
    return;
  }

  free_statement(for_loop->initial_statement);
  free_expression(for_loop->condition);
  free_statement(for_loop->modifier);
  free_program(for_loop->program);
  free(for_loop);
}

void free_statement(Statement * statement) {
  if (statement == NULL) {
    return;
  }

  switch (statement->type) {
    case PROGRAM:
      free_program(statement->program);
      break;
    case ASSIGNMENT:
      free_assignment(statement->assignment);
      break;
    case CONDITIONAL_BRANCH:
      free_conditional_branch(statement->conditional_branch);
      break;
    case FOR_LOOP:
      free_for_loop(statement->for_loop);
      break;
    case WHILE_LOOP:
      free_while_loop(statement->while_loop);
      break;
    case LOOP_INSTRUCTION:
      free(statement->loop_instruction);
      break;
    default:
      break;
  }

  free(statement);
}

void free_instruction(Instruction * instruction) {
  if (instruction == NULL) {
    return;
  }

  switch (instruction->type) {
    case STATEMENT:
      free_statement(instruction->statement);
      break;
    case EXPRESSION:
      free_expression(instruction->expression);
      break;
  }

  free(instruction);
}

void free_program(Program * program) {
  if (program == NULL) {
    return;
  }

  for (int i = 0; i < program->current_instruction; i++) {
    free_instruction(program->instructions[i]);
  }

  free(program->instructions);
  free_variable_dictionnary(program->variable_dictionary);
  free(program);
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

  while ((mask = is_token_expression(token = current_token(stream)))) {
    if (mask & VALUE_MASK) {
      push_token_stack(output_stack, token);
    } else if (token->type == LPAREN) {
      push_token_stack(operator_stack, token);
    } else if (token->type == RPAREN) {
      while (!operator_stack->is_empty && top_token_stack(operator_stack)->type != LPAREN) {
        push_token_stack(output_stack, pop_token_stack(operator_stack));
      }
      if (top_token_stack(operator_stack)->type != LPAREN || operator_stack->is_empty) {
        print_error("ERREUR DE SYNTAXE", "Le nombre de parenthèses ouvertes et fermées ne correspond pas.", token);
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

    next_token(stream);
  }

  while (!operator_stack->is_empty) {
    if (top_token_stack(operator_stack)->type == LPAREN) {
        print_error("ERREUR DE SYNTAXE", "Le nombre de parenthèses ouvertes et fermées ne correspond pas.", top_token_stack(operator_stack));
        free_token_stack(operator_stack);
        free_token_stack(output_stack);
        return NULL;
    }
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

  expression->type = EMPTY_EXPRESSION;

  if (!postfix_expression->is_empty) {
    top_token = pop_token_stack(postfix_expression);
    int token_mask = is_token_expression(top_token);

    if (token_mask & VALUE_MASK) {
      Value * value = NULL;
      expression->type = VALUE;

      if ((value = malloc(sizeof(Value))) == NULL) {
        fprintf(stderr, "ERREUR::PARSER::PARSE_EXPRESSION::ALLOCATION_2\n\nErreur d'allocation de la valeur.\n");
        free_expression(expression);
        return NULL;
      }

      expression->value = value;

      if (token_mask & NUMBER_MASK) {
        if (top_token->type == FLOATING_NUMBER) {
          value->type = FLOAT;
          value->float_value = strtold(top_token->value, NULL);

          if (errno == ERANGE) {
            print_error("ERREUR DE CAPACITE", "Le nombre entrée est trop grand.", top_token);
            free_value(value);
            free_expression(expression);
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
            print_error("ERREUR DE CAPACITE", "Le nombre entrée est trop grand.", top_token);
            free_value(value);
            free_expression(expression);
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

        if ((variable = malloc(sizeof(Variable))) == NULL || (variable->name = malloc(sizeof(char) * (strlen(top_token->value) + 1))) == NULL) {
          fprintf(stderr, "ERREUR::PARSER::PARSE_EXPRESSION::ALLOCATION_3\n\nErreur d'allocation de la variable.\n");
          free_expression(expression);
          return NULL;
        }

        strcpy(variable->name, top_token->value);

        expression->type = VARIABLE;
        expression->variable = variable;

      }
    } else if (token_mask & BINARY_OPERATOR_MASK) {
      BinaryOperator * binary_operator = NULL;

      expression->type = BINARY_OPERATION;

      if ((binary_operator = malloc(sizeof(BinaryOperator))) == NULL) {
        fprintf(stderr, "ERREUR::PARSER::PARSE_EXPRESSION::ALLOCATION_3\n\nErreur d'allocation de l'opérateur binaire.\n");
        free_expression(expression);
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
      } else {
        free_expression(left_expression);
        free_expression(right_expression);
        free_binary_operator(binary_operator);
        return NULL;
      }
    } else if (token_mask & UNARY_OPERATOR_MASK) {
      UnaryOperator *unary_operator = NULL;

      if ((unary_operator = malloc(sizeof(UnaryOperator))) == NULL) {
        fprintf(stderr, "ERREUR::PARSER::PARSE_EXPRESSION::ALLOCATION_3\n\nErreur d'allocation de l'opérateur binaire.\n");
        free_expression(expression);
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
      } else {
        free_unary_operator(unary_operator);
        free_expression(expression);
        return NULL;
      }
    }
  }

  return expression;
}

Statement * parse_statement(TokenStream * stream) {
  Statement * statement = NULL;

  if ((statement = malloc(sizeof(Statement))) == NULL) {
    fprintf(stderr, "ERREUR::PARSER::PARSE_STATEMENT::ALLOCATION_1\n\nErreur d'allocation de l'instruction.\n");
    return NULL;
  }

  statement->type = EMPTY_STATEMENT;

  while (current_token(stream)->type != END && is_token_statement(current_token(stream))) {

    if (current_token(stream)->type == ASSIGN) {
      Variable * variable = NULL;

      if (stream->current < 1) {
        print_error("ERREUR DE SYNTAXE", "L'assignation n'a pas de valeur.", current_token(stream));
        free_statement(statement);
        return NULL;
      }

      if (get_token(stream, -1)->type != NAME) {
        print_error("ERREUR DE SYNTAXE", "Il est possible d'assigner des valeurs uniquement à des variables.",
                    get_token(stream, -1));
        free_statement(statement);
        return NULL;
      }

      if ((variable = malloc(sizeof(Variable))) == NULL || (variable->name = malloc(sizeof(char) * (strlen(get_token(stream, -1)->value) + 1))) == NULL) {
        fprintf(stderr, "ERREUR::PARSER::PARSE_STATEMENT::ALLOCATION_2\n\nErreur d'allocation de la variable.\n");
        free_statement(statement);
        return NULL;
      }

      strcpy(variable->name, get_token(stream, -1)->value);
      next_token(stream);

      TokenStack * token_stack = infix_to_postfix(stream);
      if (token_stack == NULL) {
        free_statement(statement);
        return NULL;
      }

      Expression * value = parse_expression(token_stack);

      if (value == NULL) {
        free_token_stack(token_stack);
        free_statement(statement);
        return NULL;
      }

      if (value->type == EMPTY_EXPRESSION) {
        Token* token_to_print = current_token(stream);
        if (token_to_print->type == END) {
          print_error("ERREUR DE SYNTAXE", "L'assignation n'a pas de valeur.", get_token(stream, -1));
        } else {
          print_error("ERREUR DE SYNTAXE", "L'assignation n'a pas de valeur.", token_to_print);
        }
        free_token_stack(token_stack);
        free_statement(statement);
        return NULL;
      }

      if ((statement->assignment = malloc(sizeof(Assignment))) == NULL) {
        fprintf(stderr, "ERREUR::PARSER::PARSE_STATEMENT::ALLOCATION_3\n\nErreur d'allocation de l'assignation.\n");
        free_token_stack(token_stack);
        free_statement(statement);
        return NULL;
      }

      statement->assignment->value = value;
      statement->assignment->variable = variable;
      statement->type = ASSIGNMENT;
      break;

    }

    if (current_token(stream)->type == IF) {
      next_token(stream);

      TokenStack * token_stack = infix_to_postfix(stream);
      if (token_stack == NULL) {
        free_statement(statement);
        return NULL;
      }

      Expression * condition = parse_expression(token_stack);

      if (condition->type == EMPTY_EXPRESSION) {
        print_error("ERREUR DE SYNTAXE", "Une instruction if nécessite une condition.", current_token(stream));
        free_token_stack(token_stack);
        free_expression(condition);
        free_statement(statement);
        return NULL;
      }

      if (current_token(stream)->type != LBRACE) {
        print_error("ERREUR DE SYNTAXE", "Caractère inattendue trouvé, accolade ouverte attendue.", current_token(stream));
        free_token_stack(token_stack);
        free_expression(condition);
        free_statement(statement);
        return NULL;
      }

      next_token(stream);

      Program * true_branch = parse(stream);

      if (true_branch == NULL) {
        free_token_stack(token_stack);
        free_statement(statement);
        return NULL;
      }

      if ((statement->conditional_branch = malloc(sizeof(ConditionalBranch))) == NULL) {
        fprintf(stderr, "ERREUR::PARSER::PARSE_STATEMENT::ALLOCATION_4\n\nErreur d'allocation de la branche conditionnelle.\n");
        free_program(true_branch);
        free_token_stack(token_stack);
        free_statement(statement);
        return NULL;
      }

      statement->conditional_branch->expression = condition;
      statement->conditional_branch->true_branch = true_branch;
      statement->type = CONDITIONAL_BRANCH;

      if (next_token(stream)->type == ELSE) {
        next_token(stream);
        if (current_token(stream)->type == LBRACE || current_token(stream)->type == IF) {
          if ((statement->conditional_branch->false_branch = parse(stream)) == NULL) {
            free_program(true_branch);
            free_token_stack(token_stack);
            free(statement);
            return NULL;
          }
        } else {
          print_error("ERREUR DE SYNTAXE", "Caractères inattendus après la branche else.", current_token(stream));
          free_program(true_branch);
          free_token_stack(token_stack);
          free(statement);
          return NULL;
        }
      }

      break;
    }

    if (current_token(stream)->type == FOR) {
      next_token(stream);

      if ((statement->for_loop = malloc(sizeof(ForLoop))) == NULL) {
        fprintf(stderr, "ERREUR::PARSER::PARSE_STATEMENT::ALLOCATION_5\n\nErreur d'allocation de la boucle définie.\n");
        free_statement(statement);
        return NULL;
      }

      statement->for_loop->program = NULL;
      statement->for_loop->condition = NULL;
      statement->for_loop->modifier = NULL;

      statement->for_loop->initial_statement = parse_statement(stream);

      if (statement->for_loop->initial_statement == NULL) {
        free_for_loop(statement->for_loop);
        free_statement(statement);
        return NULL;
      }

      if (statement->for_loop->initial_statement->type != EMPTY_STATEMENT && current_token(stream)->type != COMMA) {
        print_error("ERREUR DE SYNTAXE", "L'initialisation d'une boucle for doit être suivie d'une virgule." , current_token(stream));
        free_for_loop(statement->for_loop);
        free_statement(statement);
        return NULL;
      }

      if (statement->for_loop->initial_statement->type == EMPTY_STATEMENT && current_token(stream)->type == COMMA) {
        print_error("ERREUR DE SYNTAXE", "Une virgule est présente; Or aucune initialisation n'a été déclarée pour la boucle for.\nRetirer la virgule pour ne plus rencontrer cette erreur." , current_token(stream));
        free_for_loop(statement->for_loop);
        free_statement(statement);
        return NULL;
      }

      if (statement->for_loop->initial_statement->type != EMPTY_STATEMENT) {
        next_token(stream);
      }

      TokenStack * token_stack = infix_to_postfix(stream);

      if (token_stack == NULL) {
        free_for_loop(statement->for_loop);
        free_statement(statement);
        return NULL;
      }

      statement->for_loop->condition = parse_expression(token_stack);

      if (statement->for_loop->condition == NULL) {
        free_token_stack(token_stack);
        free_for_loop(statement->for_loop);
        free_statement(statement);
        return NULL;
      }

      if (statement->for_loop->condition->type == EMPTY_EXPRESSION) {
        print_error("ERREUR DE SYNTAXE", "L'expression attendue pour la boucle for est vide.", current_token(stream));
        free_token_stack(token_stack);
        free_for_loop(statement->for_loop);
        free_statement(statement);
        return NULL;
      }

      if (current_token(stream)->type != COMMA) {
        print_error("ERREUR DE SYNTAXE", "Une virgule attendue pour séparer la condition du modificateur de la boucle for n'est pas présente.", current_token(stream));
        free_for_loop(statement->for_loop);
        free_statement(statement);
        return NULL;
      }

      next_token(stream);

      statement->for_loop->modifier = parse_statement(stream);

      if (statement->for_loop->modifier == NULL) {
        free_token_stack(token_stack);
        free_for_loop(statement->for_loop);
        free_statement(statement);
        return NULL;
      }

      if (statement->for_loop->modifier->type == EMPTY_STATEMENT || statement->for_loop->modifier->type == PROGRAM) {
        print_error("ERREUR DE SYNTAXE", "Une instruction de modification est attendue pour la boucle for.", current_token(stream));
        free_token_stack(token_stack);
        free_for_loop(statement->for_loop);
        free_statement(statement);
        return NULL;
      }

      next_token(stream);

      Program * for_body = parse(stream);

      if (for_body == NULL) {
        free_token_stack(token_stack);
        free_for_loop(statement->for_loop);
        free_statement(statement);
        return NULL;
      }

      statement->type = FOR_LOOP;
      statement->for_loop->program = for_body;
      break;

    }

    if (current_token(stream)->type == WHILE) {
      next_token(stream);

      if ((statement->while_loop = malloc(sizeof(WhileLoop))) == NULL) {
        fprintf(stderr, "ERREUR::PARSER::PARSE_STATEMENT::ALLOCATION_6\n\nErreur d'allocation de la boucle indefinie.\n");
        free_statement(statement);
        return NULL;
      }

      statement->while_loop->program = NULL;

      TokenStack * token_stack = infix_to_postfix(stream);

      if (token_stack == NULL) {
        free_while_loop(statement->while_loop);
        free_statement(statement);
        return NULL;
      }

      statement->while_loop->condition = parse_expression(token_stack);

      if (statement->while_loop->condition == NULL) {
        free_token_stack(token_stack);
        free_while_loop(statement->while_loop);
        free_statement(statement);
        return NULL;
      }

      if (statement->while_loop->condition->type == EMPTY_EXPRESSION) {
        print_error("ERREUR DE SYNTAXE", "Une condition est attendue pour la boucle while, mais il n'y en a pas.", current_token(stream));
        free_token_stack(token_stack);
        free_while_loop(statement->while_loop);
        free_statement(statement);
        return NULL;
      }

      if (current_token(stream)->type != LBRACE) {
        print_error("ERREUR DE SYNTAXE", "Une accolade ouvrant le corps de la boucle while est attendue.", current_token(stream));
        free_token_stack(token_stack);
        free_while_loop(statement->while_loop);
        free_statement(statement);
        return NULL;
      }

      next_token(stream);

      Program * while_body = parse(stream);

      if (while_body == NULL) {
        free_token_stack(token_stack);
        free_while_loop(statement->while_loop);
        free_statement(statement);
        return NULL;
      }

      statement->type = WHILE_LOOP;
      statement->while_loop->program = while_body;
      break;

    }

    if (current_token(stream)->type == BREAK || current_token(stream)->type == CONTINUE) {

      if ((statement->loop_instruction = malloc(sizeof(LoopInstruction))) == NULL) {
        fprintf(stderr, "ERREUR::PARSER::PARSE_STATEMENT::ALLOCATION_7\n\nErreur d'allocation de la boucle indefinie.\n");
        free_statement(statement);
        return NULL;
      }

      statement->type = LOOP_INSTRUCTION;
      *(statement->loop_instruction) = (current_token(stream)->type == BREAK) ? BREAK_LOOP : CONTINUE_LOOP;
      break;
    }

    next_token(stream);
  }

  return statement;
}

Program * parse(TokenStream * stream){
  Program * program = NULL;

  if ((program = new_program()) == NULL) {
    fprintf(stderr, "ERREUR::PARSER::ALLOCATION_1\n\nL'allocation mémoire de l'arbre de syntaxe a échouée.\n");
    return NULL;
  }

  char in_brace = get_token(stream, -1)->type == LBRACE;

  while (current_token(stream)->type != END && current_token(stream)->type != RBRACE) {
    Instruction * instruction = NULL;

    if (is_token_statement(current_token(stream))) {
      instruction = malloc(sizeof(Instruction));

      if (instruction == NULL) {
        fprintf(stderr, "ERREUR::PARSER::ALLOCATION_2\n\nL'allocation mémoire de l'instruction a échouée.\n");
        free_program(program);
        return NULL;
      }

      instruction->type = STATEMENT;

      if (current_token(stream)->type == LBRACE) {
        Statement * statement = NULL;

        if ((statement = malloc(sizeof(Statement))) == NULL) {
          fprintf(stderr, "ERREUR::PARSER::PARSE::ALLOCATION_3\n\nErreur d'allocation de l'instruction.\n");
          return NULL;
        }

        next_token(stream);
        instruction->statement = statement;
        statement->program = parse(stream);

        if (statement->program == NULL) {
          free_program(program);
          free_statement(statement);
          free_instruction(instruction);
          return NULL;
        }

        if (current_token(stream)->type != RBRACE) {
          print_error("ERREUR DE SYNTAXE", "Une ou plusieurs accolades ouvrantes n'ont pas été fermées.", get_token(stream, -1));
          print_statement(statement);
          free_instruction(instruction);
          return NULL;
        }
      } else {
        instruction->statement = parse_statement(stream);
      }

      if (instruction->statement == NULL) {
        free(instruction);
        free(program->instructions);
        free(program);
        return NULL;
      }

      if (current_token(stream)->type != SEMICOLON && instruction->statement->type == LOOP_INSTRUCTION) {
        print_error("ERREUR DE SYNTAXE", "Un point virgule doit obligatoirement suivre cette instruction.", current_token(stream));
        free_program(program);
        free_instruction(instruction);
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

      if (current_token(stream)->type == FUNCTION) {
        parse_function_call(stream);
      }

      if ((stack = infix_to_postfix(stream)) == NULL) {
        free_program(program);
        free_instruction(instruction);
        return NULL;
      }

      instruction->type = EXPRESSION;

      instruction->expression = parse_expression(stack);

      if (instruction->expression == NULL) {
        free_token_stack(stack);
        free_instruction(instruction);
        free_program(program);
        return NULL;
      }

      add_instruction(program, instruction);
    }

    next_token(stream);
  }

  if (current_token(stream)->type == RBRACE && !in_brace) {
    print_error("ERREUR DE SYNTAXE", "Une ou plusieurs accolades fermantes ne correspondent à aucun bloc de code.", current_token(stream));
    free_program(program);
    return NULL;
  }

  return program;
}
