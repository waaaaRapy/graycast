#include "parser.h"

#include <stdlib.h>

/**
 * 新しいASTノードを生成
 */
Node* new_node(NodeKind kind, Node* lhs, Node* rhs) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

/**
 * 新しい数値ASTノードを作成
 */
Node* new_node_num(int val) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

/** グローバルのtokenからASTを構築 */
Node* parse() { return expr(); }

/** experをパース */
Node* expr() {
  Node* node = mul();

  for (;;) {
    if (consume_if('+')) {
      node = new_node(ND_ADD, node, mul());
    } else if (consume_if('-')) {
      node = new_node(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

/** mulをパース */
Node* mul() {
  Node* node = primary();

  for (;;) {
    if (consume_if('*')) {
      node = new_node(ND_MUL, node, primary());
    } else if (consume_if('/')) {
      node = new_node(ND_DIV, node, primary());
    } else {
      return node;
    }
  }
}

/** primaryをパース */
Node* primary() {
  if (consume_if('(')) {
    // "(" expr ")" の場合
    Node* node = expr();
    if (!consume_if(')')) {
      error(token->str, "')'ではありません");
    };
    return node;
  } else {
    // num
    int val = consume_number();
    return new_node_num(val);
  }
}
