#ifndef LEXER_H
#define LEXER_H

#define OPERATOR_PRECEDENCE 8

typedef enum TokenType TokenType;
typedef struct Token Token;
typedef struct TokenStream TokenStream;

enum TokenType {
  NAME,
  FUNCTION,
  NUMBER,
  FLOATING_NUMBER,
  IF,
  ELSE,
  FOR,
  WHILE,
  CONTINUE,
  BREAK,
  AND,
  OR,
  XOR,
  TRUE,
  FALSE,
  NOT,
  S_STRING, // Guillemets simples
  D_STRING, // Guillements doubles
  SEMICOLON,
  COMMA,
  DOT,
  ASSIGN,
  EXCLAM,
  EQUAL,
  UNEQUAL,
  GREATER,
  GREATER_EQUAL,
  LOWER,
  LOWER_EQUAL,
  PLUS,
  U_PLUS, // Plus unaire
  MINUS,
  U_MINUS, // Plus unaire
  STAR,
  SLASH,
  PERCENT,
  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,
  LBRACKET,
  RBRACKET,
  END,
};

struct Token {
  TokenType type;
  char* value;
  unsigned int position;
};

struct TokenStream {
  unsigned int stream_capacity;
  unsigned int current;
  Token* tokens;
};

TokenStream * new_token_stream(char * str);

void set_token_stream(TokenStream * stream);
void push_token(TokenStream * stream, Token token);
void free_token_stream(TokenStream * stream);
void rewind_token_stream(TokenStream * stream);
void append_value(Token * token, char * value);

Token * get_token(TokenStream * stream, int position);
Token * current_token(TokenStream * stream);
Token * next_token(TokenStream * stream);

#endif
