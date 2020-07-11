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

/**
 * 新しいローカル変数ASTノードを作成

 * @param name 変数名
 */
Node* new_node_lvar(char name) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR;
  node->offset = (name - 'a' + 1) * 8;
  return node;
}

/** グローバルのtokenからASTを構築 */
Node** parse() { return program(); }

/** stmtノードを保存する配列 */
Node* codes[100];

/** programをパースして結果をcode[]に保存する */
Node** program() {
  int i = 0;
  while (!at_eof()) {
    codes[i++] = stmt();
    if (i >= 100) {
      error(NULL, "コードが長すぎます。");
      exit(1);
    }
  }
  codes[i] = NULL;
  return codes;
}

/** stmtをパース */
Node* stmt() {
  Node* node = expr();
  if (consume_if(";")) {
    return node;
  } else {
    error(token->str, "';'ではありません");
  }
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
  char* name = consume_if_ident();
  if (name != NULL) {
    return new_node_lvar(*name);
  }

  // num の場合
  int* val = consume_if_number();
  if (val != NULL) {
    return new_node_num(*val);
  }

  // その他の場合
  error(token->str, "変数名または数値が期待されています");
}
