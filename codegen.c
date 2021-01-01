#include "mycc.h"

int internal_label_count = 0;

int get_label_count() {
  return internal_label_count++;
}

void gen_lvar(Node *node) {
  if (node->kind != ND_LVAR)
    error("左辺値が変数ではありません");

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen_block(Node *node) {
  if (node->kind != ND_BLOCK)
    error("ブロックではありません");

  Node *current = node->child;

  while (current != NULL) {
    gen(current);
    printf("  pop rax\n");
    current = current->brother;
  }
}

NodeKind gen(Node *node) {
  int label_count;
  Node *current;

  switch (node->kind) {
    case ND_NUM:
      printf("  push %d\n", node->val);
      return ND_NUM;
    case ND_LVAR:
      gen_lvar(node);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return ND_LVAR;
    case ND_ASSIGN:
      gen_lvar(node->child);
      gen(node->child->brother);
      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      return ND_ASSIGN;
    case ND_RETURN:
      gen(node->child);
      printf("  pop rax\n");
      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
      return ND_RETURN;
    case ND_IF:
      label_count = get_label_count();
      current = node->child;
      // 条件式
      gen(current);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      // 条件が真の時の処理
      current = current->brother;
      if (current->brother != NULL ) {
        printf("  je .Lelse%d\n", label_count);
        gen_block(current);
        printf(".Lelse%d:\n", label_count);
        // 条件が偽の時の処理
        gen_block(current->brother);
      } else {
        printf("  je .Lendif%d\n", label_count);
        gen_block(current);
        printf(".Lendif%d:\n", label_count);
      }
      return ND_IF;
    case ND_WHILE:
      label_count = get_label_count();
      printf(".Lbeginwhile:\n");
      // 条件式
      gen(node->child);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lendwhile\n");
      // ループ内の処理
      gen_block(node->child->brother);
      printf("  jmp .Lbeginwhile\n");
      printf(".Lendwhile:\n");
      return ND_WHILE;
    case ND_FOR:
      label_count = get_label_count();
      Node *current = node->child;
      // 初期化式
      gen(current);
      printf(".Lbeginfor:\n");
      current = current->brother;
      // 条件式
      gen(current);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lendfor\n");
      current = current->brother;
      // ループ本体
      gen_block(current->brother);
      // 変化式
      gen(current);
      printf("  jmp .Lbeginfor\n");
      printf(".Lendfor:\n");
      return ND_FOR;
  }

  gen(node->child);
  gen(node->child->brother);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  // 二項演算
  switch (node->kind) {
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
    case ND_EQ:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_NE:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LE:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
  }

  printf("  push rax\n");
  return node->kind;
}
