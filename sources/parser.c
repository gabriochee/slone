#include <stdio.h>
#include <stdlib.h>

#include "../headers/lexer.h"
#include "../headers/parser.h"

#include "../headers/lexer_test.h"

#define VALUE_MASK               0b00000000000000000011111111
#define NUMBER_MASK              0b00000000000000000000000110
#define STRING_MASK              0b00000000000000000001100000
#define BOOLEAN_VALUE_MASK       0b00000000000000000110000000
#define BOOLEAN_OPERATOR_MASK    0b00000000000001111000000000
#define ARITHMETIC_OPERATOR_MASK 0b00000011111110000000000000
#define COMPARISON_OPERATOR_MASK 0b00111100000000000000000000
#define PARENTHESES_MASK         0b11000000000000000000000000

#define BINARY_OPERATOR_MASK     0b00111111111000111000000000
#define UNARY_OPERATOR_MASK      0b00000000000111000000000000

void set_program(Program * program){
  program->instruction_capacity = 1;
  program->current_instruction = 0;
  program->instructions = malloc(sizeof(Instruction *));
}

void add_instruction(Program * program, Instruction * instruction) {
  if (program->current_instruction >= program->instruction_capacity) {
    program->instructions = realloc(program->instructions, (program->instruction_capacity *= 2) * sizeof(Instruction *));
  }

  if (program->instructions != NULL) {
    program->instructions[program->current_instruction++] = instruction;
  }
}

int is_token_expression(Token * token){
  // La fonction ne renvoie pas seulement si le token actuel peut être considéré comme une expression,
  // mais il renvoie aussi quel type d'après ce plan selon chaque bit (La position suit le petit boutisme):
  // bit n°1 = NAME                             ] Ensemble des valeurs
  // bit n°2 = NUMBER                           ]
  // bit n°3 = DOT                              ]
  // bit n°4 = SQUOTE                           ]
  // bit n°5 = DQUOTE                           ]
  // bit n°6 = S_STRING | Ensemble des strings  ]
  // bit n°7 = D_STRING |                       ]
  // bit n°8 = TRUE     ) Ensemble des booléens ]
  // bit n°9 = FALSE    )                       ]
  // bit n°10 = AND     } Ensemble des opérateurs booléens
  // bit n°11 = OR      }
  // bit n°12 = XOR     }                                       | Opérateurs unaires
  // bit n°13 = NOT     }                                       |
  // bit n°14 = U_PLUS  ] Ensemble des opérateurs arithmétiques |
  // bit n°15 = U_MINUS ]
  // bit n°16 = PLUS    ]
  // bit n°17 = MINUS   ]
  // bit n°18 = STAR    ]
  // bit n°19 = SLASH   ]
  // bit n°20 = PERCENT ]
  // bit n°21 = LOWER   } Ensemble des comparaisons (égal est exclu pour le cas spécifique de l'assignation.)
  // bit n°22 = GREATER }
  // bit n°23 = EXCLAM  }
  // bit n°24 = EQUAL   }
  // bit n°25 = LPAREN  | Ensemble des parenthèses
  // bit n°26 = RPAREN  |

  int mask = 0;

  mask =  (token->type == NAME)           |
          (token->type == NUMBER)   << 1  |
          (token->type == DOT)      << 2  |
          (token->type == SQUOTE)   << 3  |
          (token->type == DQUOTE)   << 4  |
          (token->type == S_STRING) << 5  |
          (token->type == D_STRING) << 6  |
          (token->type == TRUE)     << 7  |
          (token->type == FALSE)    << 8  |
          (token->type == AND)      << 9  |
          (token->type == OR)       << 10 |
          (token->type == XOR)      << 11 |
          (token->type == NOT)      << 12 |
          (token->type == U_PLUS)   << 13 |
          (token->type == U_MINUS)  << 14 |
          (token->type == PLUS)     << 15 |
          (token->type == MINUS)    << 16 |
          (token->type == STAR)     << 17 |
          (token->type == SLASH)    << 18 |
          (token->type == PERCENT)  << 19 |
          (token->type == LOWER)    << 20 |
          (token->type == GREATER)  << 21 |
          (token->type == EXCLAM)   << 23 |
          (token->type == LPAREN)   << 24 |
          (token->type == RPAREN)   << 25;

  return mask;
}

short operator_precedence(Token * token) {
  short mask = 0;
  mask =  (token->type == LPAREN)                                                            |
          (token->type == OR)                                                           << 1 |
          (token->type == AND)                                                          << 2 |
          (token->type == XOR)                                                          << 3 |
          (token->type == AND)                                                          << 4 |
          ((token->type == EXCLAM) || (token->type == EQUAL))                           << 5 |
          ((token->type == GREATER) || (token->type == LOWER))                          << 6 |
          ((token->type == PLUS) || (token->type == MINUS))                             << 7 |
          ((token->type == STAR) || (token->type == PERCENT) || (token->type == SLASH)) << 8 |
          ((token->type == U_PLUS) || (token->type == U_MINUS))                         << 9 ;

  return mask;
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

Token * pop_token_stack(TokenStack * token_stack) {
  if (token_stack->current > 0) {
    return token_stack->tokens[--token_stack->current];
  }

  token_stack->is_empty = 1;
  return token_stack->tokens[token_stack->current];
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
//  Finalement, si notre opérateur est un parenthèse fermante, alors on dépile toutes les valeurs de la pile des
//  opérateurs vers la pile de sortie jusqu'a que l'on rencontre une parenthèse ouvrante dans la pile des opérateurs.
//
//  9 * (4 + 5) * 15 = 81 * 15 = 1215
// SORTIE : 9 4 5 + * 15 *
// OPERAS :
//
//  9 * 4 + 5 * 15 = 111
// SORTIE : 9 4 * 5 15 * +
// OPERAS :
//
//  1 + 4 * -5
// SORTIE : 1 4 5 - * +
// OPERAS :
//  9 * 5 / 10;
//
// SORTIE : 9 5 * 10 /
// OPERAS :
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
    fprintf(stderr, "ERREUR::PARSER::PARSE_EXPRESSION::INFIX_TO_POSTFIX::ALLOCATION_2\n\nErreur d'allocation de la pile des opérateurs.\n");
    return NULL;
  }

  int mask;

  while ((mask = is_token_expression(token = next_token(stream)))) {
    if (mask & VALUE_MASK) {
      push_token_stack(output_stack, token);
    } else if (token->type == LPAREN){
      push_token_stack(operator_stack, token);
    } else if (token->type == RPAREN) {
      while (!operator_stack->is_empty && top_token_stack(operator_stack)->type != LPAREN) {
        push_token_stack(output_stack, pop_token_stack(operator_stack));
      }
      if (top_token_stack(operator_stack)->type != LPAREN) {
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

Expression * parse_expression(TokenStream * stream){
  TokenStack * postfix_expression = NULL;
  Token * top_token = NULL;

  if ((postfix_expression = infix_to_postfix(stream)) == NULL) {
    return NULL;
  }

  while (!postfix_expression->is_empty) {
    top_token = pop_token_stack(postfix_expression);
    if (top_token->type & VALUE_MASK) {

    } else if (top_token->type & BINARY_OPERATOR_MASK) {

    } else if (top_token->type & UNARY_OPERATOR_MASK) {

    }
  }

  return NULL;
}

Program * parse(TokenStream * stream){
  Program * program = NULL;

  if ((program = malloc(sizeof(Program))) == NULL) {
    fprintf(stderr, "ERREUR::PARSER::ALLOCATION_1\n\nL'allocation mémoire de l'arbre de syntaxe a échouée.\n");
    return NULL;
  }

  set_program(program);

  if (program->instructions == NULL) {
    fprintf(stderr, "ERREUR::PARSER::ALLOCATION_2\n\nL'allocation mémoire de l'arbre de syntaxe a échouée.\n");
    return NULL;
  };

  printf("%d\n", is_token_expression(current_token(stream)));

  if (is_token_expression(current_token(stream))){
    Instruction * instruction = malloc(sizeof(Instruction));
    instruction->type = EXPRESSION;
    instruction->expression = parse_expression(stream);
    add_instruction(program, instruction);
  }

  return NULL;
}
