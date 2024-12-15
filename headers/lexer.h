#ifndef LEXER_H
#define LEXER_H

typedef enum TokenType TokenType;
typedef struct Token Token;
typedef struct TokenStream TokenStream;

enum TokenType;

struct Token;
struct TokenStream;

TokenStream * new_token_stream(char * str);

void set_token_stream(TokenStream * stream);
void push_token(TokenStream * stream, Token token);
void free_token_stream(TokenStream * stream);

#endif
