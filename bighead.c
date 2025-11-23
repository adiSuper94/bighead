#include "bighead.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void log_msg(LogLevel level, const char *format, ...) {
  const char *level_str;
  switch (level) {
  case INFO:
    level_str = "INFO";
    break;
  case WARN:
    level_str = "WARN";
    break;
  case ERROR:
    level_str = "ERROR";
    break;
  default:
    level_str = "UNKNOWN";
    break;
  }
  fprintf(stderr, "[%s] ", level_str);
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fprintf(stderr, "\n");
}

DynamicArray *da_new(size_t initial_capacity) {
  DynamicArray *list = malloc(sizeof(DynamicArray));
  if (!list)
    return NULL;
  list->data = malloc(initial_capacity * sizeof(void *));
  if (!list->data) {
    free(list);
    return NULL;
  }
  list->size = 0;
  list->capacity = initial_capacity;
  return list;
}

int da_push(DynamicArray *list, void *item) {
  if (list->size >= list->capacity) {
    size_t new_capacity = list->capacity * 2;
    void **new_data = realloc(list->data, new_capacity * sizeof(void *));
    if (!new_data)
      return -1;
    list->data = new_data;
    list->capacity = new_capacity;
  }
  list->data[list->size++] = item;
  return list->size;
}

void *da_get(DynamicArray *list, size_t index) {
  if (index >= list->size)
    return NULL;
  return list->data[index];
}

int da_remove_at(DynamicArray *list, size_t index) {
  if (index >= list->size)
    return -1;
  memmove(&list->data[index], &list->data[index + 1], (list->size - index - 1) * sizeof(void *));
  list->size--;
  return 0;
}

void da_free(DynamicArray *list) {
  free(list->data);
  free(list);
}

unsigned long hash_string(const char *str) {
  unsigned long hash = 5381;
  int c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}

HashMap *hm_new(size_t num_buckets) {
  HashMap *map = malloc(sizeof(HashMap));
  if (!map)
    return NULL;
  map->buckets = malloc(num_buckets * sizeof(DynamicArray *));
  if (!map->buckets) {
    free(map);
    return NULL;
  }
  for (size_t i = 0; i < num_buckets; i++) {
    map->buckets[i] = da_new(4);
    if (!map->buckets[i]) {
      for (size_t j = 0; j < i; j++) {
        da_free(map->buckets[j]);
      }
      free(map->buckets);
      free(map);
      return NULL;
    }
  }
  map->num_buckets = num_buckets;
  map->size = 0;
  map->hash_func = hash_string;
  return map;
}

void hm_put(HashMap *map, char *key, void *value) {
  unsigned long hash = map->hash_func(key) % map->num_buckets;
  DynamicArray *bucket = map->buckets[hash];
  for (size_t i = 0; i < bucket->size; i++) {
    HashEntry *entry = (HashEntry *)da_get(bucket, i);
    if (strcmp(entry->key, key) == 0) {
      entry->value = value;
      return;
    }
  }
  HashEntry *new_entry = malloc(sizeof(HashEntry));
  new_entry->key = strdup(key);
  new_entry->value = value;
  da_push(bucket, new_entry);
  map->size++;
  if (map->size > map->num_buckets) {
    hm_resize(map, map->num_buckets * 2);
  }
}

void *hm_get(HashMap *map, const char *key) {
  unsigned long hash = map->hash_func(key) % map->num_buckets;
  DynamicArray *bucket = map->buckets[hash];
  for (size_t i = 0; i < bucket->size; i++) {
    HashEntry *entry = (HashEntry *)da_get(bucket, i);
    if (strcmp(entry->key, key) == 0) {
      return entry->value;
    }
  }
  return NULL;
}

int hm_remove(HashMap *map, const char *key) {
  unsigned long hash = map->hash_func(key) % map->num_buckets;
  DynamicArray *bucket = map->buckets[hash];
  for (size_t i = 0; i < bucket->size; i++) {
    HashEntry *entry = (HashEntry *)da_get(bucket, i);
    if (strcmp(entry->key, key) == 0) {
      free(entry->key);
      free(entry);
      da_remove_at(bucket, i);
      map->size--;
      return 0;
    }
  }
  return -1;
}

void hm_resize(HashMap *map, size_t new_num_buckets) {
  DynamicArray **new_buckets = malloc(new_num_buckets * sizeof(DynamicArray *));
  if (!new_buckets)
    return;
  for (size_t i = 0; i < new_num_buckets; i++) {
    new_buckets[i] = da_new(4);
    if (!new_buckets[i]) {
      for (size_t j = 0; j < i; j++) {
        da_free(new_buckets[j]);
      }
      free(new_buckets);
      return;
    }
  }
  for (size_t i = 0; i < map->num_buckets; i++) {
    DynamicArray *bucket = map->buckets[i];
    for (size_t j = 0; j < bucket->size; j++) {
      HashEntry *entry = (HashEntry *)da_get(bucket, j);
      unsigned long new_hash = map->hash_func(entry->key) % new_num_buckets;
      da_push(new_buckets[new_hash], entry);
    }
    da_free(bucket);
  }
  free(map->buckets);
  map->buckets = new_buckets;
  map->num_buckets = new_num_buckets;
}

void hm_free(HashMap *map) {
  for (size_t i = 0; i < map->num_buckets; i++) {
    DynamicArray *bucket = map->buckets[i];
    for (size_t j = 0; j < bucket->size; j++) {
      HashEntry *entry = (HashEntry *)da_get(bucket, j);
      free(entry->key);
      free(entry);
    }
    da_free(bucket);
  }
  free(map->buckets);
  free(map);
}

StringBuilder *sb_new(void) {
  const size_t init_size = 64;
  StringBuilder *sb = malloc(sizeof(StringBuilder));
  sb->size = 0;
  sb->buffer = malloc(sizeof(char *) * init_size);
  sb->capacity = init_size;
  return sb;
}

StringBuilder *sb_append(StringBuilder *sb, char *s) {
  size_t s_len = strlen(s);
  if (sb->capacity - sb->size <= s_len) {
    size_t new_buff_len = sb->capacity * 2;
    while (new_buff_len - sb->size <= s_len) {
      new_buff_len *= 2;
    }
    sb->buffer = realloc(sb->buffer, new_buff_len);
    sb->capacity = new_buff_len;
  }
  char *str_copy = malloc(sizeof(char *) * s_len + 1);
  strcpy(str_copy, s);
  memmove(sb->buffer + sb->size, str_copy, s_len);
  sb->size += s_len;
  return sb;
}
String *sb_to_string(StringBuilder *sb) {
  String *s = malloc(sizeof(String));
  s->size = sb->size;
  s->string = malloc(sizeof(char *) * s->size);
  strcpy(s->string, sb->buffer);
  return s;
}

void sb_free(StringBuilder *sb) {
  free(sb->buffer);
  free(sb);
}

#define ALIGNMENT (sizeof(void *))

Arena *arena_new(size_t size) {
  Arena *a = malloc(sizeof(Arena));
  a->size = size;
  a->commited = 0;
  a->mem = malloc(size);
  a->offset = 0;
  return a;
}
void *arena_alloc(Arena *arena, size_t size) {
  if (arena->offset % ALIGNMENT != 0) {
    arena->offset = (((arena->offset + ALIGNMENT) / ALIGNMENT) * ALIGNMENT);
  }
  arena->commited += size;
  arena->offset += size;
  // Casting to char* to do pointer arithmetic without warning
  return ((char *)arena->mem) + arena->offset - size;
}

void arena_free(Arena *arena) {
  free(arena->mem);
  free(arena);
}

void arena_clear(Arena *arena) {
  arena->size = 0;
  arena->offset = 0;
  arena->commited = 0;
}
