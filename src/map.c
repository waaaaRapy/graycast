/**
 * @file String => void* のTreeMapを実装
 */

#include "map.h"

#include <string.h>

Map* map_new() { return calloc(1, sizeof(Map)); }

MapNode* map_node_new(char* key, size_t len, void* val) {
  MapNode* node = calloc(1, sizeof(MapNode));
  node->key = key;
  node->len = len;
  node->val = val;
  return node;
}

MapNode** map_node_get(MapNode** root, char* key, size_t len) {
  MapNode** pp = root;
  MapNode* p;
  while ((p = *pp) != NULL) {
    int cmp = strncmp(p->key, key, len);
    if (cmp == 0 && len == p->len) {
      return pp;
    };
    pp = cmp <= 0 ? &p->left : &p->right;
  }
  return pp;
}

int map_node_set(MapNode** target, char* key, size_t len, void* val) {
  if (*target != NULL) {
    // MapNodeが存在していれば値を更新
    (*target)->val = val;
    return 0;
  } else {
    // 存在していなかったらノードを追加
    char* new_key = malloc(len + 1);
    memcpy(new_key, key, len);
    new_key[len] = 0;

    *target = map_node_new(new_key, len, val);
    return 1;
  }
}

void* map_get(Map* map, char* key, size_t len) {
  MapNode* p = *(map_node_get(&map->root, key, len));
  return p ? p->val : NULL;
}

void map_set(Map* map, char* key, size_t len, void* val) {
  MapNode** pp = (map_node_get(&map->root, key, len));
  map->count += map_node_set(pp, key, len, val);
}
