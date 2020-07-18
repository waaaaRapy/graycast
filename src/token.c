#include "token.h"

#include <ctype.h>
#include <memory.h>
#include <stdlib.h>

#include "map.h"
#include "util.h"

/** 現在処理中のトークン */
Token* token;

char* RESERVED_TOKENS[] = {
    "+",  "-",  "*",  "/",  "(", ")",  // 四則演算
    "==", "!=", "<=", ">=", "<", ">",  // 比較演算子
    "=",  ";",                         // 代入, 文末
    NULL,                              // 番兵
};

Map* gen_keywords_map() {
  Map* map = map_new();
  map_set(map, "return", 6, (void*)TK_RETURN);
  map_set(map, "if", 2, (void*)TK_IF);
  map_set(map, "else", 4, (void*)TK_ELSE);
  map_set(map, "while", 5, (void*)TK_WHILE);
}

/**
 * 入力をトークナイズし、結果をグローバルに保存する
 *
 * @param p 入力文字列
 */
void tokenize(char* p) {
  Token head;
  head.next = NULL;
  Token* cur = &head;

  Map* kwmap = gen_keywords_map();

  while (*p) {
    /* 空白文字は無視 */
    if (isspace(*p)) {
      p++;
      continue;
    }

    /* 変数 or キーワード */
    if (isalpha(*p) || *p == '_') {
      // [a-zA-Z_][a-zA-Z0-9_]* に一致する部分を読み込む
      char* tmp = p + 1;
      while (isalnum(*tmp) || *tmp == '_') tmp++;
      int len = tmp - p;

      // 読み込んだ部分がキーワードかどうか調べる
      TokenKind kind = (TokenKind)map_get(kwmap, p, len);
      if (kind) {
        cur = new_token(kind, cur, p, len);
      } else {
        cur = new_token(TK_IDENT, cur, p, len);
      }

      p = tmp;  // 読み込んだ分だけ進める
      continue;
    }

    /* 記号 */
    char** rsv_token = RESERVED_TOKENS;
    // 一致するRESERVED_TOKENを探す
    while (*rsv_token != NULL) {
      if (start_with(p, *rsv_token)) break;
      rsv_token++;
    }
    // 見つかったら記号トークンを作成
    if (*rsv_token != NULL) {
      int len = strlen(*rsv_token);
      cur = new_token(TK_RESERVED, cur, p, len);
      p += len;
      continue;
    }

    /* 数値 */
    if (isdigit(*p)) {
      char* tmp;
      int val = strtol(p, &tmp, 10);
      cur = new_token(TK_NUM, cur, p, tmp - p);
      cur->val = val;
      p = tmp;
      continue;
    }

    error(p, "トークナイズできません");
  }

  cur = new_token(TK_EOF, cur, p, 0);
  token = head.next;
}

/**
 * 新しいトークンを作成する
 *
 * @param kind トークンの種類
 * @param prev prev->nextに作成したトークンが追加される
 * @param str トークンの文字列
 */
Token* new_token(TokenKind kind, Token* prev, char* str, int len) {
  Token* token = calloc(1, sizeof(Token));
  token->kind = kind;
  token->str = str;
  token->len = len;
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
bool consume_if(char* op) {
  if (token->kind == TK_RESERVED && strlen(op) == token->len &&
      memcmp(token->str, op, token->len) == 0) {
    token = token->next;
    return true;
  } else {
    return false;
  }
}

/**
 * トークンが期待している記号のとき、トークンを消費して次に進める。
 * それ以外の場合はエラーを報告して終了する。
 *
 * @param op 期待している記号
 */
void expect(char* op) {
  if (!consume_if(op)) {
    error(token->str, "'%s'が期待されています", op);
  }
}

/**
 * トークンが期待している種類のとき、トークンを消費して次に進める
 *
 * @param kind 期待しているトークンの種類
 * @return トークンを消費したかどうか
 */
Token* consume_if_type_is(TokenKind kind) {
  if (token->kind != kind) {
    return NULL;
  }
  Token* prev = token;
  token = token->next;
  return prev;
}

/**
 * トークンが数のとき、トークンを消費して次に進める
 *
 * @return 消費したトークンの数値へのポインタ。
 *         トークンを消費しなかった場合はNULL。
 */
int* consume_if_number() {
  if (token->kind != TK_NUM) {
    return NULL;
  }
  int* val = &token->val;
  token = token->next;
  return val;
}
