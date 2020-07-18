#include "local_var.h"

LVarStore* LVarStore_new() {
  LVarStore* store = calloc(1, sizeof(LVarStore));
  store->root = NULL;
  return store;
}

LVar* LVar_new(char* name, size_t len, int offset) {
  LVar* lvar = malloc(sizeof(LVar));
  lvar->name = name;
  lvar->len = len;
  lvar->offset = offset;
  return lvar;
}

LVar* LVarStore_load(LVarStore* store, char* name, size_t len) {
  MapNode** pp = map_node_get(&store->root, name, len);
  if (*pp != NULL) {
    return (*pp)->val;
  } else {
    LVar* lvar = LVar_new(name, len, (store->total_offset += 8));
    map_node_set(pp, name, len, lvar);
    return lvar;
  }
}