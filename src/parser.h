#pragma once

/**
 * BNF:
 *  program    := stmt*
 *  stmt       := exper ";"
 *               | "{" stmt* "}"
 *               | "return" expr ";"
 *               | "if" "(" expr ")" stmt ("else" stmt)?
 *               | "while" "(" expr ")" stmt
 *               | "for" "(" expr ";" expr ";" expr ")" stmt
 *  expr       := assign
 *  assign     := equality ("=" assign | ("+=" | "-=" | "*=" | "/=") assign)?
 *  equality   := relational ("==" relational | "!=" relational)*
 *  relational := add ("<" add | "<=" add | ">" add | ">=" add)*
 *  add        := mul ("+" mul | "-" mul)*
 *  mul        := unary ("*" unary | "/" unary)*
 *  unary      := ("+" | "-" | "++" | "--")? primary
 *  primary    := `num`
 *              | `ident`
 *              | `ident` "(" (expr ("," expr)*)? ")"
 *              | "(" expr ")"
 */

#include "list.h"
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

  ND_BLOCK,   // ブロック
  ND_RETURN,  // return
  ND_IFELSE,  // if-else
  ND_WHILE,   // while
  ND_CALL,    // 関数呼び出し
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

  struct Node_Block {
    NodeKind kind;  // ノードの種類がND_LVARのとき
    List* stmts;    // 文のリスト
  } BLOCK;

  struct Node_If {
    NodeKind kind;    // ノードの種類がND_IFのとき
    Node* cond;       // 分岐条件
    Node* if_body;    // condのときに実行する文
    Node* else_body;  // !condのときに実行する文
  } IF;

  struct Node_While {
    NodeKind kind;  // ノードの種類がND_IFのとき
    Node* cond;     // ループ条件
    Node* body;     // condの間くりかえし実行する文
  } WHILE;

  struct Node_For {
    NodeKind kind;  // ノードの種類がND_IFのとき
    Node* prepare;  // 初期化部分
    Node* cond;     // ループ条件
    Node* next;     //
    Node* body;     // 繰り返し文
  } FOR;

  struct Node_Call {
    NodeKind kind;  // ノードの種類がND_CALLのとき
    char* name;     // 呼び出す関数名
    size_t len;     // nameの長さ
    List* args;     // 引数ノードのリスト
  } CALL;
};

Node* new_node_opd(NodeKind, Node* lhs, Node* rhs);
Node* new_node_num(int val);
Node* new_node_lvar(LVar* lvar);
Node* new_node_block();
Node* new_node_call(char* name, size_t len);

Node* parse();

extern LVarStore* lvars;

Node* program();
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
