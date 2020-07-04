#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  char* p = argv[1];
  printf("  mov rax, %ld\n", strtol(p, &p, 10));

  while (*p) {
    switch (*(p++)) {
      case '+':
        printf("  add rax, %ld\n", strtol(p, &p, 10));
        continue;

      case '-':
        printf("  sub rax, %ld\n", strtol(p, &p, 10));
        continue;

      default:
        fprintf(stderr, "予期しない文字です: '%c'\n", *(p - 1));
        return 1;
    }
  }

  printf("  ret\n");
  return 0;
}