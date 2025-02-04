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
//      <letter>   ::= ensemble des caractères de la table ASCII. Chiffres, espaces et operateurs exclus.
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
//      <unary_operator>  ::= not|-|+
//
//      <expression>      ::= <value> | <expression><binary_operator><expression> | <unary_operator><expression> | (<expression>)
//
//      <if>              ::= if <expression> {<program>}
//
//      <else>            ::= <if> else {<program>} | <if> else <if>
//
//      <for>             ::= for <statement>,<expression>,<statement> {<program>}
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
// Notre parser utilise la méthode de construction d'arbre syntaxique par descente récursive.

typedef struct Value Value;
typedef struct Variable Variable;
typedef struct BinaryOperator BinaryOperator;
typedef struct UnaryOperator UnaryOperator;
typedef struct Expression Expression;

typedef struct Statement Statement;
typedef struct Assignment Assignment;
typedef struct ConditionalBranch ConditionalBranch;
typedef struct ForLoop ForLoop;
typedef struct WhileLoop WhileLoop;

typedef struct Instruction Instruction;
typedef struct Program Program;

typedef struct TokenStack TokenStack;

typedef struct VariableDictionnary VariableDictionnary;

typedef enum Type Type;
typedef enum BinaryOperatorType BinaryOperatorType;
typedef enum UnaryOperatorType UnaryOperatorType;
typedef enum ExpressionType ExpressionType;
typedef enum StatementType StatementType;
typedef enum InstructionType InstructionType;
typedef enum LoopInstruction LoopInstruction;

enum Type{
  UNSIGNED_INTEGER,
  INTEGER,
  FLOAT,
  BOOL,
  CHAR,
  STRING,
};

enum ExpressionType{
  VALUE,
  VARIABLE,
  EXPR,
  BINARY_OPERATION,
  UNARY_OPERATION,
  EMPTY_EXPRESSION,
};

enum InstructionType{
  EXPRESSION,
  STATEMENT,
};

enum BinaryOperatorType {
  LOGIC_AND,
  LOGIC_OR,
  LOGIC_XOR,
  ADD,
  SUB,
  MULT,
  DIV,
  MOD,
  GREATER_THAN,
  GREATER_EQUAL_THAN,
  LOWER_THAN,
  LOWER_EQUAL_THAN,
  EQUAL_TO,
  NOT_EQUAL_TO,
};

enum UnaryOperatorType {
  LOGIC_NOT,
  UNARY_MINUS,
  UNARY_PLUS,
};

enum StatementType {
  ASSIGNMENT,
  CONDITIONAL_BRANCH,
  FOR_LOOP,
  WHILE_LOOP,
  LOOP_INSTRUCTION,
  PROGRAM,
  EMPTY_STATEMENT,
};

enum LoopInstruction {
  CONTINUE_LOOP,
  BREAK_LOOP,
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

struct BinaryOperator {
  BinaryOperatorType type;
  Expression * left_expression,  * right_expression;
};

struct UnaryOperator {
  UnaryOperatorType type;
  Expression * expression;
};

struct Expression{
  ExpressionType type;
  union{
    Value * value;
    BinaryOperator * binary_operator;
    UnaryOperator * unary_operator;
    Variable * variable;
  };
};

struct Assignment {
  Variable * variable;
  Expression * value;
};

struct ConditionalBranch {
  Expression * expression;
  Program * true_branch, * false_branch;
};

struct ForLoop {
  Statement * initial_statement;
  Expression * condition;
  Statement * modifier;
  Program * program;
};

struct WhileLoop {
  Expression * condition;
  Program * program;
};

struct Statement {
  StatementType type;
  union {
    Assignment * assignment;
    ConditionalBranch * conditional_branch;
    ForLoop * for_loop;
    WhileLoop * while_loop;
    LoopInstruction * loop_instruction;
    Program * program;
  };
};

struct Instruction{
  InstructionType type;
  union {
    Expression * expression;
    Statement * statement;
  };
};

struct Program{
  unsigned int instruction_capacity;
  unsigned int current_instruction;
  VariableDictionnary * variable_dictionary;
  Instruction ** instructions;
};

struct TokenStack{
  unsigned char is_empty;
  unsigned int current;
  unsigned int token_capacity;
  Token ** tokens;
};

struct VariableDictionnary {
  unsigned int current;
  unsigned int capacity;
  Value ** values;
  Variable ** variables;
};

Program * new_program();
Program * parse(TokenStream * stream);

Expression * parse_expression(TokenStack * postfix_expression);

Statement * parse_statement(TokenStream * stream);

Token * pop_token_stack(TokenStack * token_stack);
Token * top_token_stack(TokenStack * token_stack);

TokenStack * new_token_stack();
TokenStack * infix_to_postfix(TokenStream * stream);

Variable * get_variable(VariableDictionnary * variable_dictionary, char * name);

VariableDictionnary * new_variable_dictionary();

void add_instruction(Program * program, Instruction * instruction);
void free_instruction(Instruction * instruction);

void push_token_stack(TokenStack * token_stack, Token * token);
void free_token_stack(TokenStack * token_stack);

void free_variable_dictionnary(VariableDictionnary * variable_dictionnary);
void add_to_variable_dictionnary(VariableDictionnary * dictionary, Variable * variable);

int is_token_expression(Token * token);
int is_token_statement(Token * token);
short operator_precedence(Token * token);

int is_expression_boolean(Expression * expression);
int is_expression_numeric(Expression * expression);

int is_unary_valid(UnaryOperator * unary_operator);
int is_binary_valid(BinaryOperator * binary_operator);

void free_value(Value * value);
void free_variable(Variable * variable);
void free_binary_operator(BinaryOperator * binary_operator);
void free_unary_operator(UnaryOperator * unary_operator);
void free_expression(Expression * expression);

void free_statement(Statement * statement);
void free_assignment(Assignment * assignment);
void free_conditional_branch(ConditionalBranch * conditional_branch);
void free_for_loop(ForLoop * for_loop);
void free_while_loop(WhileLoop * while_loop);

void free_program(Program * program);

#endif
