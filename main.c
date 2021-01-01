#include "mycc.h"

char *user_input;
Token *token;
Node *code[100];

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の数が正しくありません\n");
    return 1;
  }

  // トークナイズしてパースする
  user_input = argv[1];
  token = tokenize();
  program();


  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // プロローグ
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  for (int i=0; code[i]; i++) {
    switch(gen(code[i])) {
      case ND_IF:
      case ND_WHILE:
      case ND_FOR:
      case ND_ASSIGN:
      case ND_RETURN:
        continue;
    }

    printf("  pop rax\n");
  }

  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}
