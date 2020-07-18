#include "parser.h"

#include <stdlib.h>

#include "list.h"

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

/** ローカル変数を記録する変数 */
LVarStore* lvars;

/** グローバルのtokenからASTを構築 */
Node* parse() { return program(); }

/**
 *  program := stmt*
 */
Node* program() {
  Node* node = new_node_block();
  lvars = LVarStore_new();

  while (!at_eof()) {
    list_add(node->BLOCK.stmts, stmt());
  }

  return node;
}

/**
 *  stmt := exper ";"
 *         | "{" stmt* "}"
 *         | "return" expr ";"
 *         | "if" "(" expr ")" stmt ("else" stmt)?
 *         | "while" "(" expr ")" stmt
 */
Node* stmt() {
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
 *  assign := equality ("=" assign)?
 */
Node* assign() {
  Node* node = equality();
  if (consume_if("=")) {
    node = new_node_opd(ND_ASSIGN, node, assign());
  } else {
    return node;
  }
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
 *  unary := ("+" | "-")? primary
 */
Node* unary() {
  if (consume_if("+")) {
    return primary();
  } else if (consume_if("-")) {
    return new_node_opd(ND_SUB, new_node_num(0), primary());
  }
  return primary();
}

/**
 *  primary := `num` | `ident` | "(" expr ")"
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
    LVar* lvar = LVarStore_load(lvars, tok->str, tok->len);
    return new_node_lvar(lvar);
  }

  // num の場合
  int* val = consume_if_number();
  if (val != NULL) {
    return new_node_num(*val);
  }

  // その他の場合
  error(token->str, "変数名または数値が期待されています");
}
