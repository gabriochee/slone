#ifndef LEXER_H
#define LEXER_H

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
  NUMBER,
  SEMICOLON,
  COMMA,
  DOT,
  SQUOTE,
  DQUOTE,
  EQUAL,
  GREATER,
  LOWER,
  EXCLAM,
  PLUS,
  MINUS,
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

Token * next_token(TokenStream * stream);

#endif
