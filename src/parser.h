#pragma once

/**
 * BNF:
 *  program    := stmt*
 *  stmt       := exper ";"
 *               | "return" expr ";"
 *               | "if" "(" expr ")" stmt ("else" stmt)?
 *  expr       := assign
 *  assign     := equality ("=" assign)?
 *  equality   := relational ("==" relational | "!=" relational)*
 *  relational := add ("<" add | "<=" add | ">" add | ">=" add)*
 *  add        := mul ("+" mul | "-" mul)*
 *  mul        := unary ("*" unary | "/" unary)*
 *  unary      := ("+" | "-")? primary
 *  primary    := num | ident | "(" expr ")"
 */

#include "local_var.h"
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

  ND_RETURN,  // return
  ND_IFELSE,  // if-else
} NodeKind;

/** ASTのノード */
typedef union Node Node;

union Node {
  NodeKind kind;  // ノードの種類

  struct Node_Operands {
    NodeKind kind;  // ノードの種類が2項演算子のとき
    Node* lhs;      // 左辺
    Node* rhs;      // 右辺
  } OP;

  struct Node_Num {
    NodeKind kind;  // ノードの種類がND_NUMのとき
    int val;        // 数値
  } NUM;

  struct Node_Lvar {
    NodeKind kind;  // ノードの種類がND_LVARのとき
    LVar* data;     // ローカル変数情報
  } LVAR;

  struct Node_If {
    NodeKind kind;    // ノードの種類がND_IFのとき
    Node* cond;       // 分岐条件
    Node* if_body;    // condのときに実行する文
    Node* else_body;  // !condのときに実行する文
  } IF;
};

Node* new_node_opd(NodeKind, Node* lhs, Node* rhs);
Node* new_node_num(int val);
Node* new_node_lvar(LVar* lvar);

Node** parse();

extern LVarStore* lvars;

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
