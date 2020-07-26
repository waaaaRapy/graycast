#include "parser.h"

#include <stdlib.h>

#include "list.h"
#include "node.h"

/** Programのコンストラクタ */
Program* new_program() {
  Program* prog = calloc(1, sizeof(Program));
  prog->funcs = list_new(0);
  return prog;
}

/** Funcのコンストラクタ */
Func* new_func(char* name, size_t len) {
  Func* func = calloc(1, sizeof(Func));
  func->name = name;
  func->len = len;
  func->args = list_new(0);
  func->body = new_node_block();
  func->lvars = LVarStore_new();
  return func;
}

/** グローバルのtokenからASTを構築 */
Program* parse() { return program(); }

/**
 *  program := func*
 */
Program* program() {
  Program* prog = new_program();
  while (!at_eof()) {
    list_add(prog->funcs, func());
  }

  return prog;
}

LVarStore* lvars;

/*
 * func := `ident` "(" (`ident` ("," `ident`)*)? ")" "{" stmt* "}"
 */
Func* func() {
  // 関数名
  Token* tok = consume_if_type_is(TK_IDENT);
  Func* func = new_func(tok->str, tok->len);
  lvars = func->lvars;

  // 引数
  expect("(");
  if (!consume_if(")")) {
    // 引数ありの場合
    do {
      Token* tok = consume_if_type_is(TK_IDENT);
      if (tok == NULL) {
        error(token->str, "識別子ではありません");
      }
      LVar* lvar = LVarStore_load(func->lvars, tok->str, tok->len);
      list_add(func->args, lvar);
    } while (consume_if(","));
    expect(")");
  }

  // 本体
  expect("{");
  while (!consume_if("}")) {
    list_add(func->body->BLOCK.stmts, stmt());
  }
  return func;
}

/**
 *  stmt := exper ";"
 *         | "{" stmt* "}"
 *         | "return" expr ";"
 *         | "if" "(" expr ")" stmt ("else" stmt)?
 *         | "while" "(" expr ")" stmt
 *         | "for" "(" expr ";" expr ";" expr ")" stmt
 */
Node* stmt() {
  // ブロックのとき
  if (consume_if("{")) {
    Node* node = new_node_block();
    while (!consume_if("}")) {
      list_add(node->BLOCK.stmts, stmt());
    }
    return node;
  }

  // return文のとき
  if (consume_if_type_is(TK_RETURN)) {
    Node* node = new_node_opd(ND_RETURN, expr(), NULL);
    expect(";");
    return node;
  }

  // if文のとき
  if (consume_if_type_is(TK_IF)) {
    Node* node = new_node(ND_IFELSE);
    expect("(");
    node->IF.cond = expr();
    expect(")");
    node->IF.if_body = stmt();
    if (consume_if_type_is(TK_ELSE)) {
      node->IF.else_body = stmt();
    }
    return node;
  }

  // while文のとき
  if (consume_if_type_is(TK_WHILE)) {
    Node* node = new_node(ND_WHILE);
    expect("(");
    node->WHILE.cond = expr();
    expect(")");
    node->WHILE.body = stmt();
    return node;
  }

  // for文のとき
  if (consume_if_type_is(TK_FOR)) {
    // for(prepare;cond;next) body として読み込む
    expect("(");
    Node* prepare = expr();
    expect(";");
    Node* cond = expr();
    expect(";");
    Node* next = expr();
    expect(")");
    Node* body = stmt();

    // { prepare; while(cond) {body; next;} } として出力する
    Node* node = new_node_block();
    list_add(node->BLOCK.stmts, prepare);
    Node* w = new_node(ND_WHILE);
    list_add(node->BLOCK.stmts, w);
    w->WHILE.cond = cond;
    w->WHILE.body = new_node_block();
    list_add(w->WHILE.body->BLOCK.stmts, body);
    list_add(w->WHILE.body->BLOCK.stmts, next);
    return node;
  }

  // exprのとき
  Node* node = expr();
  expect(";");
  return node;
}

/**
 *  expr := assign
 */
Node* expr() { return assign(); }

/**
 * assign := equality (("=" | "+=" | "-=" | "*=" | "/=") assign)?
 */
Node* assign() {
  Node* node = equality();
  if (consume_if("=")) {
    node = new_node_opd(ND_ASSIGN, node, assign());
  } else if (consume_if("+=")) {
    Node* rightval = new_node_opd(ND_ADD, node, assign());
    node = new_node_opd(ND_ASSIGN, node, rightval);
  } else if (consume_if("-=")) {
    Node* rightval = new_node_opd(ND_SUB, node, assign());
    node = new_node_opd(ND_ASSIGN, node, rightval);
  } else if (consume_if("*=")) {
    Node* rightval = new_node_opd(ND_MUL, node, assign());
    node = new_node_opd(ND_ASSIGN, node, rightval);
  } else if (consume_if("/=")) {
    Node* rightval = new_node_opd(ND_DIV, node, assign());
    node = new_node_opd(ND_ASSIGN, node, rightval);
  }

  return node;
}

/**
 *  equality := relational ("==" relational | "!=" relational)*
 */
Node* equality() {
  Node* node = relational();

  for (;;) {
    if (consume_if("==")) {
      node = new_node_opd(ND_EQ, node, relational());
    } else if (consume_if("!=")) {
      node = new_node_opd(ND_NEQ, node, relational());
    } else {
      return node;
    }
  }
}

/**
 *  relational := add ("<" add | "<=" add | ">" add | ">=" add)*
 */
Node* relational() {
  Node* node = add();

  for (;;) {
    if (consume_if("<")) {
      node = new_node_opd(ND_LT, node, add());
    } else if (consume_if("<=")) {
      node = new_node_opd(ND_LE, node, add());
    } else if (consume_if(">")) {
      node = new_node_opd(ND_LT, add(), node);
    } else if (consume_if(">=")) {
      node = new_node_opd(ND_LE, add(), node);
    } else {
      return node;
    }
  }
}

/**
 *  add := mul ("+" mul | "-" mul)*
 */
Node* add() {
  Node* node = mul();

  for (;;) {
    if (consume_if("+")) {
      node = new_node_opd(ND_ADD, node, mul());
    } else if (consume_if("-")) {
      node = new_node_opd(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

/**
 *  mul := unary ("*" unary | "/" unary)*
 */
Node* mul() {
  Node* node = unary();

  for (;;) {
    if (consume_if("*")) {
      node = new_node_opd(ND_MUL, node, unary());
    } else if (consume_if("/")) {
      node = new_node_opd(ND_DIV, node, unary());
    } else {
      return node;
    }
  }
}

/**
 *  unary := ("+" | "-" | "++" | "--")? primary
 */
Node* unary() {
  if (consume_if("+")) {
    return primary();
  } else if (consume_if("-")) {
    return new_node_opd(ND_SUB, new_node_num(0), primary());
  } else if (consume_if("++")) {
    Node* operand = primary();
    Node* rightval = new_node_opd(ND_ADD, operand, new_node_num(1));
    return new_node_opd(ND_ASSIGN, operand, rightval);
  } else if (consume_if("--")) {
    Node* operand = primary();
    Node* rightval = new_node_opd(ND_SUB, operand, new_node_num(1));
    return new_node_opd(ND_ASSIGN, operand, rightval);
  }

  return primary();
}

/**
 * primary := `num`
 *          | `ident`
 *          | `ident` "(" (expr ("," expr)*)? ")"
 *          | "(" expr ")"
 */
Node* primary() {
  // "(" expr ")" の場合
  if (consume_if("(")) {
    Node* node = expr();
    expect(")");
    return node;
  }

  // ident の場合
  Token* tok = consume_if_type_is(TK_IDENT);
  if (tok != NULL) {
    if (consume_if("(")) {
      // 後ろに "(" が続く場合は関数呼び出し
      Node* node = new_node_call(tok->str, tok->len);
      if (consume_if(")")) {
        // 引数なしの場合
        return node;
      } else {
        // 引数ありの場合
        do {
          list_add(node->CALL.args, expr());
        } while (consume_if(","));
        expect(")");
        return node;
      }
    } else {
      // そうでなければ変数
      LVar* lvar = LVarStore_load(lvars, tok->str, tok->len);
      return new_node_lvar(lvar);
    }
  }

  // num の場合
  int* val = consume_if_number();
  if (val != NULL) {
    return new_node_num(*val);
  }

  // その他の場合
  error(token->str, "変数名,数値または関数呼び出しが期待されています");
}
