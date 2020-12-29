#include "mycc.h"

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

Node *expr();
Node *assign();
Node *stmt();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

LVar tail = { NULL, NULL, 0, 0 };
LVar *locals = &tail;

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->child = lhs;
  lhs->brother = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Node *new_node_ident(Token *tok) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR;
  node->offset = (tok->str[0] - 'a' + 1) * 8;
  return node;
}

void program() {
  int i = 0;
  while (!at_eof())
    code[i++] = stmt();
  code[i] = NULL;
}

Node *stmt() {
  Node *node;

  if (consume_by_kind(TK_RETURN)) {
    node = calloc(1, sizeof(node));
    node->kind = ND_RETURN;
    node->child = expr();

    expect(";");
    return node;
  }

  if (consume_by_kind(TK_IF)) {
    node = calloc(1, sizeof(node));
    node->kind = ND_IF;
    Node *block = calloc(1, sizeof(node));
    block->kind = ND_BLOCK;
    expect("(");
    node->child = expr();
    node->child->brother = block;
    expect(")");
    expect("{");

    Node **current = &(block->child);
    while (!consume("}")) {
      if (at_eof())
        error("'}'がありません");
      *current = stmt();
      current = &((*current)->brother);
    }
    return node;
  }

  node = expr();
  expect(";");
  return node;
}

Node *expr() {
  Node *node = assign();
  return node;
}

Node *assign() {
  Node *node = equality();
  if (consume("="))
    node = new_node(ND_ASSIGN, node, assign());
  return node;
}

Node *equality() {
  Node *node = relational();
  for(;;) {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node(ND_NE, node, relational());
    return node;
  }
}

Node *relational() {
  Node *node = add();
  for(;;) {

    /* >及び>=については抽象構文木の左右を入れ替えて<または<=とする */
    if (consume("<"))
      node = new_node(ND_LT, node, add());
    else if (consume("<="))
      node = new_node(ND_LE, node, add());
    else if (consume(">"))
      node = new_node(ND_LT, add(), node);
    else if (consume(">="))
      node = new_node(ND_LE, add(), node);
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();
  for (;;) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary() {
  if (consume("+"))
    return primary();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), primary());
  return primary();
}

Node *primary() {
  // 次のトークンが"("なら, "(" expr ")"のはず
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_ident();
  if (tok != NULL) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(tok);
    if (lvar) {
      node->offset = lvar->offset;
    } else {
      lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->len = tok->len;
      lvar->offset = locals->offset + 8;
      node->offset = lvar->offset;
      return new_node_ident(tok);
    }
  }

  // そうでなければ数値のはず
  return new_node_num(expect_number());
}
