#include "generator.h"

#include "parser.h"

/** ASTからコードを生成する */
void generate_program(Program* prog) {
  printf(".intel_syntax noprefix\n");
  Func** funcs = (Func**)prog->funcs->array;
  for (int i = 0; i < prog->funcs->len; i++) {
    generate_function(funcs[i]);
  }
}

/** ブロックのコードを生成 */
void generate_block(Node* node) {
  printf("# == BLOCK == \n");
  Node** codes = (Node**)node->BLOCK.stmts->array;
  // 本体のコード生成
  for (Node** code = codes; *code != NULL; code++) {
    generate(*code);
    switch ((*code)->kind) {
      case ND_IFELSE:
      case ND_WHILE:
      case ND_BLOCK:
        continue;  // expr 以外のときは pop しない
    }
    printf("  pop rax  # end of stmt\n");  // 式の結果をpopしておく
  }
  printf("# == END BLOCK == \n");
}

char* FCALL_REGS[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

/** 関数のコードを生成する */
void generate_function(Func* func) {
  printf("\n");
  printf(".type %.*s, @function\n", func->len, func->name);
  printf(".global %.*s\n", func->len, func->name);
  printf("%.*s:\n", func->len, func->name);
  // プロローグ
  printf("  push rbp\n");      // 開始時点のRBPを退避
  printf("  mov rbp, rsp\n");  // 関数内でのRBPを設定
  // 引数をスタックに保存
  for (int i = 0; i < func->args->len; i++) {
    printf("  mov [rbp - 0x%02x], %s\n", (i + 1) * 8, FCALL_REGS[i]);
  }

  printf("  sub rsp, %d\n",
         func->lvars->total_offset);  // ローカル変数の領域を確保

  generate_block(func->body);

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
  printf("  lea rax, [rbp - 0x%02x]\n", node->LVAR.data->offset);
  printf("  push rax\n");
}

/** AST nodeを評価してスタックに積むコードを生成 */
void generate(Node* node) {
  static int label_count = 0;
  int label;

  switch (node->kind) {
    case ND_NUM:
      // 数値ノードの場合、スタックに値をpushするだけ
      printf("  push %d\n", node->NUM.val);
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
      generate_leftval(node->OP.lhs);
      // 2. 右辺値の値をpushする
      generate(node->OP.rhs);
      // 3. 左辺値のアドレスに右辺値の値を保存
      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      // 4. 代入した値をpush
      printf("  push rdi\n");
      return;

    case ND_RETURN:
      // return の場合
      generate(node->OP.lhs);
      printf("  pop rax\n");       // 式の結果をpopしておく
      printf("  mov rsp, rbp\n");  // ローカル変数分のスタックを戻す
      printf("  pop rbp\n");       // RBPを復元
      printf("  ret\n");           // RAXに残っている値を返す
      return;

    case ND_CALL:
      // 関数呼び出しの場合
      // 引数を評価
      for (int i = node->CALL.args->len - 1; i >= 0; i--) {
        // 後ろの引数からスタックに積んでいく
        Node* arg = (Node*)node->CALL.args->array[i];
        generate(arg);
      }
      // 引数をレジスタに格納
      // http://herumi.in.coocan.jp/prog/x64.html
      for (int i = 0; i < node->CALL.args->len; i++) {
        printf("  pop %s\n", FCALL_REGS[i]);
      }

      // TODO: スタック(RSP)のアライメント

      // 関数を呼び出す
      printf("  call %.*s\n", node->CALL.len, node->CALL.name);
      printf("  push rax\n");
      return;

    case ND_IFELSE:
      // if文の場合
      label = label_count++;
      generate(node->IF.cond);   // condを評価
      printf("  pop rax\n");     // 結果をpop
      printf("  cmp rax, 0\n");  // フラグを設定
      if (node->IF.else_body == NULL) {
        // else節がないとき
        printf("  je  .Lend%03d\n", label);  // cond==0 なら終わりにジャンプ
        generate(node->IF.if_body);          // if節を実行
      } else {
        // else節があるとき
        printf("  je  .Lelse%03d\n", label);  // cond==0 ならelse節にジャンプ
        generate(node->IF.if_body);           // if節を実行
        printf("  jmp .Lend%03d\n", label);  // if文の終わりにジャンプ
        printf(".Lelse%03d:\n", label);      // else節の始まりのラベル
        generate(node->IF.else_body);        // else節を実行
      }
      printf(".Lend%03d:\n", label);  // if文の終わりのラベル
      return;

    case ND_WHILE:
      // while文の場合
      label = label_count++;
      printf(".Lbegin%03d:\n", label);  // while文の始まりのラベル
      generate(node->WHILE.cond);       // condを評価
      printf("  pop rax\n");            // 結果をpop
      printf("  cmp rax, 0\n");         // フラグを設定
      printf("  je  .Lend%03d\n", label);  // cond==0 なら終わりにジャンプ
      generate(node->WHILE.body);          // bodyを実行
      printf("  jmp .Lbegin%03d\n", label);  // while文の始まりにジャンプ
      printf(".Lend%03d:\n", label);         // while文の終わりのラベル
      return;

    case ND_BLOCK:
      generate_block(node);
      return;
  }

  // 左辺・右辺を評価してスタックに積む
  generate(node->OP.lhs);
  generate(node->OP.rhs);

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