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

void gen(Node *node) {
  int label_count;
  Node *current;
  switch (node->kind) {
    case ND_NUM:
      printf("  push %d\n", node->val);
      return;
    case ND_LVAR:
      gen_lvar(node);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;
    case ND_ASSIGN:
      gen_lvar(node->child);
      gen(node->child->brother);
      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      return;
    case ND_RETURN:
      gen(node->child);
      printf("  pop rax\n");
      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
      return;
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
        printf("  je .Lend%d\n", label_count);
        gen_block(current);
        printf(".Lend%d:\n", label_count);
      }
      return;
  }

  gen(node->child);
  gen(node->child->brother);

  printf("  pop rdi\n");
  printf("  pop rax\n");

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
}
