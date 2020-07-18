#pragma once

#include "map.h"

typedef struct LVarStore {
  MapNode* root;
  int total_offset;
} LVarStore;

typedef struct LVar {
  char* name;
  size_t len;
  int offset;
} LVar;

LVarStore* LVarStore_new();

LVar* LVar_new(char* name, size_t len, int offset);

LVar* LVarStore_load(LVarStore* store, char* name, size_t len);