#pragma once

/**
 * BNF:
 *  program    := func*
 *  func       := `ident` "(" (`ident` ("," `ident`)*)? ")" "{" stmt* "}"
 *  stmt       := exper ";"
 *               | "{" stmt* "}"
 *               | "return" expr ";"
 *               | "if" "(" expr ")" stmt ("else" stmt)?
 *               | "while" "(" expr ")" stmt
 *               | "for" "(" expr ";" expr ";" expr ")" stmt
 *  expr       := assign
 *  assign     := logic_or (("=" | "+=" | "-=" | "*=" | "/=") assign)?
 *  logic_or   := logic_and ("||" logic_and)*
 *  logic_and  := bit_or ("&&" bit_or)*
 *  bit_or     := bit_xor ("|" bit_xor)*
 *  bit_xor    := bit_and ("^" bit_and)*
 *  bit_and    := equality ("&" equality)*
 *  equality   := relational ("==" relational | "!=" relational)*
 *  relational := add ("<" add | "<=" add | ">" add | ">=" add)*
 *  add        := mul ("+" mul | "-" mul)*
 *  mul        := unary_r ("*" unary_r | "/" unary_r | "%" unary_r)*
 *  unary_r    := ("+" | "-" | "++" | "--")? unary_l
 *  unary_l    := primary ("++" | "--")?
 *  primary    := `num`
 *              | `ident`
 *              | `ident` "(" (expr ("," expr)*)? ")"
 *              | "(" expr ")"
 */

#include "node.h"

/** プログラム全体を表す構造体 */
typedef struct Program {
  List* funcs;  // 関数のリスト
} Program;

/** 関数定義を表す構造体 */
typedef struct Func {
  char* name;        // 関数名
  int len;           // nameの長さ
  List* args;        // 引数のリスト(List<LVar*>*)
  Node* body;        // 本文(NodeBlock*)
  LVarStore* lvars;  // ローカル変数
} Func;

Program* parse();

Program* program();
Func* func();
Node* stmt();
Node* expr();
Node* assign();
Node* logic_or();
Node* logic_and();
Node* bit_or();
Node* bit_xor();
Node* bit_and();
Node* equality();
Node* relational();
Node* add();
Node* mul();
Node* unary_r();
Node* unary_l();
Node* primary();
