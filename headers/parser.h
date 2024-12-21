#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// Un programme peut se composer de plusieurs blocs contenant eux mêmes plusieurs instructions / blocs.
// Un chunk peut être soit un bloc soit une instruction.
// Ne serait il pas mieux d'utiliser un bloc et de lui ajouter un attribut indiquant s'il possède une simple
// instruction ou plusieurs autres blocs ?

typedef struct Block Block;
typedef struct Instruction Instruction;

typedef struct Value Value;
typedef struct BinaryOperator BinaryOperator; // à voir pour le type de retour !
typedef struct UnaryOperator UnaryOperator;

typedef struct Assignement Assignement;
typedef struct ConditionalBranch ConditionalBranch;

typedef enum InstructionType InstructionType;
typedef enum BlockType BlockType;
typedef enum ExpressionType ExpressionType;
typedef enum StatementType StatementType;

typedef enum Type Type;
typedef enum BinaryOperatorType BinaryOperatorType;
typedef enum UnaryOperatorType UnaryOperatorType;

enum Type{
  INTEGER, // Type valise pour short jusqu'à unsigned long long
  FLOAT, // Type valise pour float, double, long double etc..
  BOOLEAN,
  CHAR,
  STRING,
};

enum InstructionType {
  STATEMENT,
  EXPRESSION,
};

enum ExpressionType {
  VALUE, // Compte aussi pour les variables ?
  BINARY_OPERATOR,
  //FUNCTION_CALL, Faisabilité à voir
};

enum StatementType {
  ASSIGNMENT,
  CONDITIONAL_BRANCH,
  DEFINITE_BOUCLE,
  INDEFINITE_BOUCLE,
  BOUCLE_CONTROL,
};

enum BinaryOperatorType {
  // Booléen
  LOWER_THAN,
  LOWER_THAN_EQUAL,
  EQUALS,
  GREATER_THAN,
  GREATER_THAN_EQUAL,
  // Arithmétique
  ADD,
  SUBSTRACT,
  MULTIPLY,
  DIVIDE,
};

enum UnaryOperatorType {
  // Booléen
  L_NOT
};

struct Instruction {
  InstructionType type;
};

enum BlockType {
	INSTRUCTION,
  BLOCK
};

struct Block {
  BlockType type;
	unsigned int current;
  union {
    Instruction instruction;
    Block * blocks;
  };
};

struct Value {
  Type type;
  union {
    char char_value;
    unsigned char uchar_value;

    signed short short_value;
    unsigned short ushort_value;
    signed int int_value;
    unsigned int uint_value;
    signed long long_value;
    unsigned long ulong_value;
    signed long long long_long_value;
    unsigned long long ulong_long_value;
    float float_value;
    double double_value;

    unsigned char bool_value;

    char * string_value;
  };
};

struct BinaryOperator {
  BinaryOperatorType type;
  Value left, right;
};

struct UnaryOperator {
  UnaryOperatorType type;
  Value value;
};

struct Assignement {
  char * name;
  Value value;
};

struct ConditionalBranch {
  BinaryOperator comparison;
  Block true, false;
};

/*

Block {
	Instruction;
	Block{
		Instruction;
		Instruction;
		Instruction;
		Instruction;
	},
	Block{

	},
	...
}

 */

#endif