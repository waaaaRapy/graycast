#include "util.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/** 入力をグローバルに保存しておく */
char* user_input;

/**
 * エラーを報告する
 *
 * ユーザーの入力のエラー発生個所が標準エラーに出力し、プログラムを終了する。
 * @param loc エラー発生個所
 * @param fmt printfと同じ引数
 */
void error(char* loc, char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

/**
 * 文字列が特定の文字列から始まるか調べる
 *
 * @param src 対象の文字列
 * @param expect src の先頭にあることを期待する文字列
 *
 * @return src の先頭が match から始まっているかどうか
 */
bool start_with(char* src, char* expect) {
  while (*expect) {
    if (*src != *expect) {
      return false;
    }
    src++;
    expect++;
  }
  return true;
}
