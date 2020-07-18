#include "generator.h"

#include "parser.h"

/** ASTからコードを生成する */
void generate_main(Node** ast) {
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  generate_function("main", ast);
}

/** 関数のコードを生成する */
void generate_function(char* name, Node** codes) {
  printf("%s:\n", name);
  // プロローグ
  printf("  push rbp\n");      // 開始時点のRBPを退避
  printf("  mov rbp, rsp\n");  // 関数内でのRBPを設定
  printf("  sub rsp, %d\n",
         lvars->total_offset);  // ローカル変数の領域を確保

  // 本体のコード生成
  for (Node** code = codes; *code != NULL; code++) {
    generate(*code);
    printf("  pop rax\n");  // 式の結果をpopしておく
  }

  // エピローグ
  printf("  mov rsp, rbp\n");  // ローカル変数分のスタックを戻す
  printf("  pop rbp\n");       // RBPを復元
  printf("  ret\n");           // RAXに残っている値を返す
}

/** 左辺値のコードを生成 */
void generate_leftval(Node* node) {
  if (node->kind != ND_LVAR) {
    error(NULL, "代入できない左辺値です");
  }

  // 左辺値のアドレス(RBP - node->offset)をスタックにpushする
  printf("  lea rax, [rbp - %d]\n", ((LVar*)node->data)->offset);
  printf("  push rax\n");
}

/** AST nodeを評価してスタックに積むコードを生成 */
void generate(Node* node) {
  switch (node->kind) {
    case ND_NUM:
      // 数値ノードの場合、スタックに値をpushするだけ
      printf("  push %ld\n", (size_t)node->data);
      return;
    case ND_LVAR:
      // ローカル変数の場合、
      // 1.左辺値のアドレスをpushする
      generate_leftval(node);
      // 2.アドレスから値を読みだしてpushする(右辺値に変換)
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;
    case ND_ASSIGN:
      // 代入の場合
      // 1. 左辺値のアドレスをpushする
      generate_leftval(node->lhs);
      // 2. 右辺値の値をpushする
      generate(node->rhs);
      // 3. 左辺値のアドレスに右辺値の値を保存
      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      // 4. 代入した値をpush
      printf("  push rdi\n");
      return;

    case ND_RETURN:
      // return の場合
      generate(node->lhs);
      printf("  pop rax\n");       // 式の結果をpopしておく
      printf("  mov rsp, rbp\n");  // ローカル変数分のスタックを戻す
      printf("  pop rbp\n");       // RBPを復元
      printf("  ret\n");           // RAXに残っている値を返す
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

    case ND_EQ:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_NEQ:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LE:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
  }

  // 結果をスタックにpush
  printf("  push rax\n");
}