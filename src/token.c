#include "token.h"

#include <ctype.h>
#include <stdlib.h>

#include "util.h"

/** 現在処理中のトークン */
Token* token;

/**
 * 入力をトークナイズし、結果をグローバルに保存する
 *
 * @param p 入力文字列
 */
void tokenize(char* p) {
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
  token = head.next;
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

/** 入力の終了かどうか判定 */
bool at_eof() { return token->kind == TK_EOF; }

/**
 * トークンが期待している記号のとき、トークンを消費して次に進める
 *
 * @param op 期待している記号
 * @return トークンを消費したかどうか
 */
bool consume_if(char op) {
  if (token->kind == TK_RESERVED && *token->str == op) {
    token = token->next;
    return true;
  } else {
    return false;
  }
}

/**
 * トークンが数のとき、トークンを消費して次に進める
 * そうでなければエラーを報告して終了する。
 *
 * @return トークンの数値
 */
int consume_number() {
  if (token->kind == TK_NUM) {
    int val = token->val;
    token = token->next;
    return val;
  } else {
    error(token->str, "数ではありません");
  }
}
