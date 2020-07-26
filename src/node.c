#include "node.h"

/**
 * 新しいASTノードを生成
 */
Node* new_node(NodeKind kind) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

/**
 * 新しい2項ASTノードを生成
 */
Node* new_node_opd(NodeKind kind, Node* lhs, Node* rhs) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->OP.lhs = lhs;
  node->OP.rhs = rhs;
  return node;
}

/**
 * 新しい数値ASTノードを作成
 */
Node* new_node_num(int val) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->NUM.val = val;
  return node;
}

/**
 * 新しいローカル変数ASTノードを作成

 * @param lvar ローカル変数情報
 */
Node* new_node_lvar(LVar* lvar) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR;
  node->LVAR.data = lvar;
  return node;
}

/**
 * 新しいブロックASTノードを生成
 */
Node* new_node_block() {
  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_BLOCK;
  node->BLOCK.stmts = list_new(0);
  return node;
}

/**
 * 新しい関数呼び出しASTノードを生成
 */
Node* new_node_call(char* name, size_t len) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_CALL;
  node->CALL.name = name;
  node->CALL.len = len;
  node->CALL.args = list_new(0);
  return node;
}
