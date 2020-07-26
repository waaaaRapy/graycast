#include <stdio.h>
#include <stdlib.h>

#include "generator.h"
#include "parser.h"
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
  // 抽象構文木を生成
  Program* prog = parse();
  // コードを生成
  generate_program(prog);

  return 0;
}
