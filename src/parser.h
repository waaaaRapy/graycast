#pragma once

/**
 * BNF:
 *  expr    := mul ("+" mul | "-" mul)*
 *  mul     := unary ("*" unary | "/" unary)*
 *  unary   := ("+" | "-")? primary
 *  primary := num | "(" expr ")"
 */

#include "token.h"
#include "util.h"

/** ASTのノードの種類 */
typedef enum NodeKind {
  ND_ADD,  // +
  ND_SUB,  // -
  ND_MUL,  // *
  ND_DIV,  // /
  ND_NUM,  // 整数
} NodeKind;

/** ASTのノード */
typedef struct Node Node;
struct Node {
  NodeKind kind;  // ノードの種類
  Node* lhs;      // 左辺
  Node* rhs;      // 右辺
  int val;        // king = ND_NUM のときの数値
};

Node* new_node(NodeKind, Node* lhs, Node* rhs);
Node* new_node_num(int val);

Node* parse();

Node* expr();
Node* mul();
Node* unary();
Node* primary();
