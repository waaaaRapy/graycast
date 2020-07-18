#pragma once

/**
 * @file String => void* のTreeMapを実装
 */
#include <stdlib.h>

typedef struct MapNode MapNode;
struct MapNode {
  char* key;       // キー
  size_t len;      // キーの長さ
  void* val;       // 値
  MapNode* left;   // 左の子
  MapNode* right;  // 右の子
};

typedef struct Map {
  MapNode* root;
  size_t count;
} Map;

Map* map_new();
MapNode* map_node_new(char* key, size_t len, void* val);

MapNode** map_node_get(MapNode** root, char* key, size_t len);
int map_node_set(MapNode** target, char* key, size_t len, void* val);
void* map_get(Map* map, char* key, size_t len);
void map_set(Map* map, char* key, size_t len, void* val);
