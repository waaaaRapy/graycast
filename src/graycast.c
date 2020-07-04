#include <stdio.h>
#include <stdlib.h>

#include "tokenize.h"
#include "util.h"

/** 現在着目しているトークン */
Token* token;

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
    error("数ではありません");
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  // トークナイズする
  token = tokenize(argv[1]);

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  // 最初の数値を処理
  printf("  mov rax, %d\n", consume_number());

  // (+|-)数値 が続く限り処理
  while (!at_eof()) {
    if (consume_if('+')) {
      printf("  add rax, %d\n", consume_number());
      continue;
    }

    if (consume_if('-')) {
      printf("  sub rax, %d\n", consume_number());
      continue;
    }

    error("期待されていない記号です");
  }

  printf("  ret\n");
  return 0;
}
