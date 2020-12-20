#include "mycc.h"

LVar *find_lvar(Token *tok) {
  LVar *var;
  for (var = locals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}
