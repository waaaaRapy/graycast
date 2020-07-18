#include "parser.h"

#include <stdlib.h>

#include "list.h"

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

/**
 * 新しいローカル変数ASTノードを作成

 * @param lvar ローカル変数情報
 */
Node* new_node_lvar(LVar* lvar) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR;
  node->lvar = lvar;
  return node;
}

/** ローカル変数を記録する変数 */
LVarStore* lvars;

/** グローバルのtokenからASTを構築 */
Node** parse() { return program(); }

/** programをパースする */
Node** program() {
  List* codes = list_new(0);
  lvars = LVarStore_new();

  while (!at_eof()) {
    list_add(codes, stmt());
  }

  return (Node**)codes->array;
}

/** stmtをパース */
Node* stmt() {
  Node* node;
  if (consume_if_type_is(TK_RETURN)) {
    node = new_node(ND_RETURN, expr(), NULL);
  } else {
    node = expr();
  }

  if (!consume_if(";")) {
    error(token->str, "';'ではありません");
  }
  return node;
}

/** exprをパース */
Node* expr() { return assign(); }

/** assignをパース */
Node* assign() {
  Node* node = equality();
  if (consume_if("=")) {
    node = new_node(ND_ASSIGN, node, assign());
  } else {
    return node;
  }
}

/** equalityをパース */
Node* equality() {
  Node* node = relational();

  for (;;) {
    if (consume_if("==")) {
      node = new_node(ND_EQ, node, relational());
    } else if (consume_if("!=")) {
      node = new_node(ND_NEQ, node, relational());
    } else {
      return node;
    }
  }
}

/** relationalをパース */
Node* relational() {
  Node* node = add();

  for (;;) {
    if (consume_if("<")) {
      node = new_node(ND_LT, node, add());
    } else if (consume_if("<=")) {
      node = new_node(ND_LE, node, add());
    } else if (consume_if(">")) {
      node = new_node(ND_LT, add(), node);
    } else if (consume_if(">=")) {
      node = new_node(ND_LE, add(), node);
    } else {
      return node;
    }
  }
}

/** addをパース */
Node* add() {
  Node* node = mul();

  for (;;) {
    if (consume_if("+")) {
      node = new_node(ND_ADD, node, mul());
    } else if (consume_if("-")) {
      node = new_node(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

/** mulをパース */
Node* mul() {
  Node* node = unary();

  for (;;) {
    if (consume_if("*")) {
      node = new_node(ND_MUL, node, unary());
    } else if (consume_if("/")) {
      node = new_node(ND_DIV, node, unary());
    } else {
      return node;
    }
  }
}

/** unaryをパース */
Node* unary() {
  if (consume_if("+")) {
    return primary();
  } else if (consume_if("-")) {
    return new_node(ND_SUB, new_node_num(0), primary());
  }
  return primary();
}

/** primaryをパース */
Node* primary() {
  // "(" expr ")" の場合
  if (consume_if("(")) {
    Node* node = expr();
    if (!consume_if(")")) {
      error(token->str, "')'ではありません");
    };
    return node;
  }

  // ident の場合
  Token* tok = consume_if_ident();
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
