#include "generator.h"

/** ASTからコードを生成する */
void generate_main(Node* ast) {
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");
  generate(ast);
  printf("  pop rax\n");
  printf("  ret\n");
}

/** AST nodeを評価してスタックに積むコードを生成 */
void generate(Node* node) {
  // 数値ノードの場合、スタックに値をpushするだけ
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  // 左辺・右辺を評価してスタックに積む
  generate(node->lhs);
  generate(node->rhs);

  // スタックからpop
  printf("  pop rdi\n");
  printf("  pop rax\n");

  // 演算を実行
  switch (node->kind) {
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      // cqo: [RDX:RAX] <- RAX (128bitに符号拡張)
      printf("  cqo\n");
      // idiv op1:
      //   RAX <- [RDX:RAX] / op1
      //   RDX <- [RDX:RAX] % op1
      printf("  idiv rdi\n");
      break;
  }

  // 結果をスタックにpush
  printf("  push rax\n");
}