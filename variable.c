#include "mycc.h"

LVar *find_lvar(Token *tok, LVar *head) {
  LVar *var;
  for (var = head; var != NULL; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}
