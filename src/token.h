#pragma once

#include <stdbool.h>

/** トークンの種類 */
typedef enum TokenKind {
  TK_EOF,       // 入力の終わり
  TK_RESERVED,  // 記号
  TK_IDENT,     // 識別子
  TK_NUM,       // 整数トークン

  TK_RETURN,  // return
} TokenKind;

typedef struct Token Token;

/** トークン型 */
struct Token {
  TokenKind kind;  // トークンの種類
  Token* next;     // 次のトークン(Link)
  int val;         // 整数トークンの場合、その数値
  char* str;       // トークン文字列
  int len;         // トークンの長さ
};

void tokenize(char* p);
Token* new_token(TokenKind, Token* prev, char* str, int len);

extern Token* token;

bool at_eof();
bool consume_if(char* op);
Token* consume_if_type_is(TokenKind kind);
int* consume_if_number();