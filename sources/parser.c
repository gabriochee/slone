#include <stdio.h>
#include <stdlib.h>

#include "../headers/lexer.h"
#include "../headers/parser.h"

#define VALUE_MASK               0b00000000000001111111
#define NUMBER_MASK              0b00000000000000000011
#define STRING_MASK              0b00000000000000110000
#define BOOLEAN_VALUE_MASK       0b00000000000011000000
#define BOOLEAN_OPERATOR_MASK    0b00000000111100000000
#define ARITHMETIC_OPERATOR_MASK 0b00011111000000000000
#define COMPARISON_OPERATOR_MASK 0b11100000000000000000

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
  // UNSIGNED SHORT = 0000 0000 0000 0000
  // bit n°1 = NUMBER                           ] Ensemble des valeurs
  // bit n°2 = DOT                              ]
  // bit n°3 = SQUOTE                           ]
  // bit n°4 = DQUOTE                           ]
  // bit n°5 = S_STRING | Ensemble des strings  ]
  // bit n°6 = D_STRING |                       ]
  // bit n°7 = TRUE     ) Ensemble des booléens ]
  // bit n°8 = FALSE    )                       ]
  // bit n°9 = AND      } Ensemble des opérateurs booléens
  // bit n°10 = OR       }
  // bit n°11 = XOR      }
  // bit n°12 = NOT      }
  // bit n°13 = PLUS    ] Ensemble des opérateurs arithmétiques
  // bit n°14 = MINUS   ]
  // bit n°15 = STAR    ]
  // bit n°16 = SLASH   ]
  // bit n°17 = PERCENT ]
  // bit n°18 = LOWER   } Ensemble des comparaisons (égal est exclu pour le cas spécifique de l'assignation.)
  // bit n°19 = GREATER }
  // bit n°20 = EXCLAM  }
  // bit n°21 = LPAREN

  int mask = 0;

  mask |= (token->type == NUMBER)         |
          (token->type == DOT)      << 1  |
          (token->type == SQUOTE)   << 2  |
          (token->type == DQUOTE)   << 3  |
          (token->type == S_STRING) << 4  |
          (token->type == D_STRING) << 5  |
          (token->type == TRUE)     << 6  |
          (token->type == FALSE)    << 7  |
          (token->type == AND)      << 8  |
          (token->type == OR)       << 9  |
          (token->type == XOR)      << 10 |
          (token->type == NOT)      << 11 |
          (token->type == PLUS)     << 12 |
          (token->type == MINUS)    << 13 |
          (token->type == STAR)     << 14 |
          (token->type == SLASH)    << 15 |
          (token->type == PERCENT)  << 16 |
          (token->type == LOWER)    << 17 |
          (token->type == GREATER)  << 18 |
          (token->type == EXCLAM)   << 20 |
          (token->type == LPAREN)   << 21;

  return mask;
}

Expression * parse_expression(TokenStream * stream){
  Token * token = next_token(stream);
  Expression * next = NULL;
  Expression * current = malloc(sizeof(Expression));

  if (token->type == SEMICOLON || token->type == RPAREN){
    current->type = END_OF_EXPRESSION;
    return current;
  } else if (token->type == SQUOTE || token->type == DQUOTE){
    return parse_expression(stream);
  }
  
  next = parse_expression(stream);

  if (next->type == END_OF_EXPRESSION){
    int expression_mask = is_token_expression(token);

    if (expression_mask & VALUE_MASK){

      current->type = VALUE;
      current->value = malloc(sizeof(Value));

      if (expression_mask & STRING_MASK){
        if (token->value[1] == '\0'){
          current->value->char_value = *(token->value);
          current->value->type = CHAR;
        } else {
          current->value->string_value = token->value;
          current->value->type = STRING;
        }
      } else if (expression_mask & BOOLEAN_VALUE_MASK){
          current->value->bool_value = (token->type == TRUE);
          current->value->type = BOOL;
      } else if (expression_mask & NUMBER_MASK){
          current->value->unsigned_integer_value = strtoull(token->value, NULL, 10);
          current->value->type = INTEGER;
      }

      return current;
    } else if (next->type & (ARITHMETIC_OPERATOR_MASK | BOOLEAN_OPERATOR_MASK | COMPARISON_OPERATOR_MASK)){
      // GERER ERREUR !
    }

  } else if (next->type == VALUE){
    if (token->type == DOT && next->value->type == INTEGER){
      current->type = VALUE;
      current->value = malloc(sizeof(Value));
      current->value->type = FLOAT;
      current->value->float_value = strtold(token->value, NULL);
      while (current->value->float_value > 1.0){
        current->value->float_value /= 10.0;
      }

      return current;
    }
  }

  return current;
}

Program * parse(TokenStream * stream){
  Program * program = NULL;

  if ((program = malloc(sizeof(Program))) == NULL) {
    fprintf(stderr, "ERREUR::PARSER::ALLOCATION_1\n\nL'allocation mémoire de l'arbre de syntaxe a échouée.\n");
    return NULL;
  };

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

    if (program->instructions[0]->type == EXPRESSION){
      switch(program->instructions[0]->expression->value->type){
        case INTEGER:
          printf("INTEGER : %llu\n", program->instructions[0]->expression->value->unsigned_integer_value);
          break;
        case FLOAT:
          printf("FLOAT : %Lf\n", program->instructions[0]->expression->value->float_value);
          break;
        case BOOL:
          printf("%s\n", program->instructions[0]->expression->value->bool_value ? "TRUE" : "FALSE");
          break;
        case CHAR:
          printf("CHAR : %c\n", program->instructions[0]->expression->value->char_value);
          break;
        case STRING:
          printf("STRING : %s\n", program->instructions[0]->expression->value->string_value);
          break;
        default:
          break;
      }
    }
  }


  return NULL;
}
