#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * エラーを報告する
 *
 * printfと同じ引数をとる。
 */
void error(char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}
