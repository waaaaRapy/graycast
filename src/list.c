/**
 * @file ArrayListを実装
 */
#include "list.h"

/**
 * コンストラクタ
 *
 * @param capacity はじめに領域を確保しておく要素数
 *                 0を指定した場合デフォルトの31が使われる
 */
List* list_new(size_t capacity) {
  if (capacity == 0) capacity = 31;
  List* list = calloc(1, sizeof(List));
  list->capacity = capacity;
  list->array = calloc(capacity + 1, sizeof(void*));
  list->len = 0;
  return list;
}

/**
 * リストに要素を追加する
 *
 * @param list 操作対象のリスト
 * @param ptr  追加する要素(ポインタ)
 */
void list_add(List* list, void* ptr) {
  // 確保済みの領域が足りない場合は拡張
  if (list->len >= list->capacity) {
    size_t capacity = list->capacity * 2 + 1;
    list->capacity = capacity;
    list->array = realloc(list->array, sizeof(void*) * (capacity + 1));
  }

  list->array[list->len++] = ptr;
}
