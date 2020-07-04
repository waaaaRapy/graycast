#include "tokenize.h"

#include <ctype.h>
#include <stdlib.h>

#include "util.h"

/**
 * トークナイズする
 *
 * @param p 入力文字列
 * @return 先頭のトークン
 */
Token* tokenize(char* p) {
  Token head;
  head.next = NULL;
  Token* cur = &head;

  while (*p) {
    // 空白文字は無視
    if (isspace(*p)) {
      p++;
      continue;
    }

    // 記号
    if (*p == '+' || *p == '-') {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    // 数値
    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error(p, "トークナイズできません");
  }

  cur = new_token(TK_EOF, cur, p);
  return head.next;
}

/**
 * 新しいトークンを作成する
 *
 * @param kind トークンの種類
 * @param prev prev->nextに作成したトークンが追加される
 * @param str トークンの文字列
 */
Token* new_token(TokenKind kind, Token* prev, char* str) {
  Token* token = calloc(1, sizeof(Token));
  token->kind = kind;
  token->str = str;
  prev->next = token;
  return token;
}
