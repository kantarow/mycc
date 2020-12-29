#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子
  TK_NUM,	   // 整数トークン
  TK_RETURN,   // return
  TK_IF,       // if文
  TK_SOB,      // ブロックの開始
  TK_EOB,      // ブロックの終わり
  TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token {
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合、その数値
  char *str;      // トークン文字列
  int len;        // トークンの長さ
};

// 抽象構文木のノードの種類
typedef enum {
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // /
  ND_ASSIGN, // =
  ND_LVAR,   // local variable
  ND_EQ,     // ==
  ND_NE,     // !=
  ND_LT,     // <
  ND_LE,     // <=
  ND_NUM,    // 整数
  ND_RETURN, // return
  ND_IF,
  ND_BLOCK,
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind; // ノードの型
  Node *child;   // 子ノードの先頭
  Node *brother; // 弟妹ノード
  int val;       // kindがND_NUMの場合のみ使う
  int offset;    // kindがND_LVARの場合のみ使う
};

// local variable
typedef struct LVar LVar;

struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
};

extern LVar *locals;

LVar *find_lvar(Token *tok);

// codegen
void gen(Node *node);

// parse
void program();

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
bool consume_by_kind(TokenKind kind);
Token *consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();
bool startswith(char *p, const char *q);
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize();

// 入力されたプログラム
extern char *user_input;

// 現在着目しているトークン
extern Token *token;

extern Node *code[100];
