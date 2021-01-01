#include "mycc.h"

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

Node *expr();
Node *assign();
Node *stmt();
Node *block();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

LVar *head = NULL;

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

void program() {
  int i = 0;
  while (!at_eof())
    code[i++] = stmt();
  code[i] = NULL;
}

Node *stmt() {
  Node *node;

  if (consume_by_kind(TK_RETURN)) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->child = expr();

    expect(";");
    return node;
  }

  if (consume_by_kind(TK_IF)) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_IF;
    expect("(");
    node->child = expr();
    expect(")");

    Node *if_block = block();
    node->child->brother = if_block;

    if (consume_by_kind(TK_ELSE))
      if_block->brother = block();

    return node;
  }

  if (consume_by_kind(TK_WHILE)) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_WHILE;
    expect("(");
    node->child = expr();
    expect(")");
    node->child->brother = block();

    return node;
  }

  if (consume_by_kind(TK_FOR)) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_FOR;

    Node *current;

    expect("(");
    // 初期化式
    node->child = expr();
    current = node->child;
    expect(";");
    // 条件式
    current->brother = expr();
    expect(";");
    current = current->brother;
    // 変化式
    current->brother = expr();
    expect(")");
    current = current->brother;
    current->brother = block();

    return node;
  }

  node = expr();
  expect(";");
  return node;
}

Node *block() {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_BLOCK;
  Node **current = &(node->child);
  expect("{");

  while(!consume("}")) {
    if (at_eof())
      error("'}'がありません");
    *current = stmt();
    current = &((*current)->brother);
  }

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

    LVar *lvar = find_lvar(tok, head);
    if (lvar != NULL) {
      node->offset = lvar->offset;
    } else {
      lvar = (LVar *)calloc(1, sizeof(LVar));
      lvar->len = tok->len;
      lvar->name = tok->str;
      if (head == NULL) {
        lvar->offset = 0;
        head = lvar;
      } else {
        lvar->offset = head->offset + 8;
        lvar->next = head;
        head = lvar;
      }
      node->offset = lvar->offset;
    }
    return node;
  }

  // そうでなければ数値のはず
  return new_node_num(expect_number());
}
