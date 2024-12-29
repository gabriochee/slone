#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// Un programme peut se composer de plusieurs blocs contenant eux mêmes plusieurs instructions / blocs.
// Un chunk peut être soit un bloc soit une instruction.
// Ne serait il pas mieux d'utiliser un bloc et de lui ajouter un attribut indiquant s'il possède une simple
// Afin de préciser la syntaxe de notre langage de programmation, nous utiliserons la notation Backus Naur.
//
// 1. Types élémentaires
//      <symbol>   ::= ensemble des caractères de la table ASCII.
//
//      <letter>   ::= ensemble des caractères de la table ASCII chiffres, espaces et operateurs exclus.
//
//      <digit>    ::= 0|1|2|3|4|5|6|7|8|9|
//
//      <number>   ::= <digit> | <digit><number> | <number>.<number>
//
//      <bool>     ::= true | false
//
//      <char>     ::= '<symbol>'
//
//      <word>     ::= <letter> | <letter><word>
//
//      <string>   ::= "<symbol>" | "<word>" | '<word>'
//
//      <variable> ::= <word> | <number><word> | <word><number>
//
// 2. Structure de programme
//      <value>           ::= <number> | <bool> | <char> | <string> | <variable>
//
//      <binary_operator> ::= +|-|*|/|%|<|>|<=|>=|==|!=|and|or|xor
//
//      <unary_operator>  ::= not
//
//      <expression>      ::= <value> | <expression><binary_operator><expression> | <unary_operator><expression> | (<expression>)
//
//      <if>              ::= if (<expression>) {<program>}
//
//      <else>            ::= <if> else {<program>} | <if> else <if>
//
//      <for>             ::= for (<statement>,<expression>,<statement>){<program>}
//
//      <while>           ::= while (<expression>){<program>}
//
//      <continue>        ::= continue
//
//      <break>           ::= break
//
//      <assignement>     ::= <variable>=<expression>
//      
//      <statement>       ::= <assignment>|<if>|<else>|<for>|<while>|<continue>|<break>|{<program>}
//
//      <instruction>     ::= <expression>; | <statement>
//
//      <program>         ::= <instruction> | <instruction><program>
//
// La notation Backus-Naur permet d'expliciter la syntaxe du langage de programmation et ainsi nous guider sur la construction des différents objets pouvant constituer notre arbre de syntaxe.
//
// Notre parser utilise la méthode de consrtuction d'arbre syntaxique par descente récursive.

typedef struct Value Value;
typedef struct Variable Variable;
typedef struct Expression Expression;
typedef struct Instruction Instruction;
typedef struct Program Program;

typedef enum Type Type;
typedef enum ExpressionType ExpressionType;
typedef enum InstructionType InstructionType;

typedef struct TokenStack TokenStack;

enum Type{
  INTEGER,
  FLOAT,
  BOOL,
  CHAR,
  STRING,
};

enum ExpressionType{
  VALUE,
  EXPR,
  BINARY_OPERATION,
  UNARY_OPERATION,
  END_OF_EXPRESSION,
};

enum InstructionType{
  EXPRESSION,
  STATEMENT,
};

struct Value{
  Type type;
  union {
    unsigned long long unsigned_integer_value;
    signed long long integer_value;
    long double float_value;
    unsigned char bool_value;
    char char_value;
    char * string_value;
  };
};

struct Variable{
  Type type;
  char * name;
};

struct Expression{
  ExpressionType type;
  union{
    Value * value;
  };
};

struct Instruction{
  InstructionType type;
  union {
    Expression * expression;
  };
};

struct Program{
  unsigned int instruction_capacity;
  unsigned int current_instruction;
  Instruction ** instructions;
};

struct TokenStack{
  unsigned char is_empty;
  unsigned int current;
  unsigned int token_capacity;
  Token ** tokens;
};

Program * parse(TokenStream * stream);

Expression * parse_expression(TokenStream * stream);

TokenStack * infix_to_postfix(TokenStream * stream);

Token * pop_token_stack(TokenStack * token_stack);
Token * top_token_stack(TokenStack * token_stack);

TokenStack * new_token_stack();

void set_program(Program * program);
void add_instruction(Program * program, Instruction * instruction);

void push_token_stack(TokenStack * token_stack, Token * token);
void free_token_stack(TokenStack * token_stack);

int is_token_expression(Token * token);
short operator_precendence(Token * token);

#endif
