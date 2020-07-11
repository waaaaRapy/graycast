/**
 * @file ArrayListを実装
 */

#include <stdlib.h>

/**
 * Listの実体
 */
typedef struct List {
  void** array;     // ポインタの配列
  size_t len;       // 要素数
  size_t capacity;  // 確保済み領域
} List;

List* list_new(size_t capacity);
void list_add(List* list, void* ptr);