#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// Un programme peut se composer de plusieurs blocs contenant eux mêmes plusieurs instructions / blocs.
// Un chunk peut être soit un bloc soit une instruction.

typedef struct Program Program;
typedef struct Chunk Chunk;
typedef struct Block Block;
typedef struct Instruction Instruction;

typedef enum InstructionType InstructionType;
typedef enum ChunkType ChunkType;

enum InstructionType {
  STATEMENT,
  EXPRESSION,
};

enum ChunkType {
  BLOCK,
  INSTRUCTION,
};

struct Block {
	unsigned int current;
	Chunk * chunks;
};

struct Instruction {
  InstructionType type;
};

struct Chunk {
  ChunkType type;
  union {
    Block block;
    Instruction instruction;
  };
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