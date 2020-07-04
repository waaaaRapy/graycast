#pragma once

#include <stdbool.h>

/** トークンの種類 */
typedef enum TokenKind {
  TK_RESERVED,  // 記号
  TK_NUM,       // 整数トークン
  TK_EOF,       // 入力の終わり
} TokenKind;

typedef struct Token Token;

/** トークン型 */
struct Token {
  TokenKind kind;  // トークンの種類
  Token* next;     // 次のトークン(Link)
  int val;         // 整数トークンの場合、その数値
  char* str;       // トークン文字列
};

Token* tokenize(char* p);
Token* new_token(TokenKind, Token* prev, char* str);
