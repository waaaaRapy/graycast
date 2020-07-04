#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "util.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  // 入力をグローバルに保存しておく
  user_input = argv[1];

  // トークナイズする
  tokenize(argv[1]);

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

    error(token->str, "期待されていない記号です");
  }

  printf("  ret\n");
  return 0;
}
