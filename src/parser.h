#pragma once

/**
 * BNF:
 *  program    := stmt*
 *  stmt       := exper ";"
 *  expr       := assign
 *  assign     := equality ("=" assign)?
 *  equality   := relational ("==" relational | "!=" relational)*
 *  relational := add ("<" add | "<=" add | ">" add | ">=" add)*
 *  add        := mul ("+" mul | "-" mul)*
 *  mul        := unary ("*" unary | "/" unary)*
 *  unary      := ("+" | "-")? primary
 *  primary    := num | ident | "(" expr ")"
 */

#include "token.h"
#include "util.h"

/** ASTのノードの種類 */
typedef enum NodeKind {
  ND_ADD,     // 加算("+")
  ND_SUB,     // 減算("-")
  ND_MUL,     // 乗算("*")
  ND_DIV,     // 除算("/")
  ND_LT,      // 比較("<")
  ND_LE,      // 比較("<=")
  ND_EQ,      // 比較("==")
  ND_NEQ,     // 比較("!=")
  ND_ASSIGN,  // 代入("=")
  ND_LVAR,    // ローカル変数
  ND_NUM,     // 整数
} NodeKind;

/** ASTのノード */
typedef struct Node Node;
struct Node {
  NodeKind kind;  // ノードの種類
  Node* lhs;      // 左辺
  Node* rhs;      // 右辺
  int val;        // kind = ND_NUM のときの数値
  int offset;     // kind = ND_LVAR のときのRBPからのオフセット
};

Node* new_node(NodeKind, Node* lhs, Node* rhs);
Node* new_node_num(int val);

Node** parse();

Node** program();
Node* stmt();
Node* expr();
Node* assign();
Node* expr();
Node* equality();
Node* relational();
Node* add();
Node* mul();
Node* unary();
Node* primary();
