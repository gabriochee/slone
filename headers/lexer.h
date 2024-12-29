#ifndef LEXER_H
#define LEXER_H

#define OPERATOR_PRECEDENCE 8

typedef enum TokenType TokenType;
typedef struct Token Token;
typedef struct TokenStream TokenStream;

enum TokenType {
  NAME,
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
  NUMBER,
  S_STRING, // Guillemets simples
  D_STRING, // Guillements doubles
  SEMICOLON,
  COMMA,
  DOT,
  SQUOTE,
  DQUOTE,
  EQUAL,
  GREATER,
  LOWER,
  PLUS,
  U_PLUS, // Plus unaire
  MINUS,
  U_MINUS, // Plus unaire
  STAR,
  SLASH,
  PERCENT,
  EXCLAM,
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

Token * get_token(TokenStream * stream, int position);
Token * current_token(TokenStream * stream);
Token * next_token(TokenStream * stream);

#endif
