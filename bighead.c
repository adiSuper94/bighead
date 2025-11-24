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
  case DEBUG:
    level_str = "DEBUG";
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
  DynamicArray *list;
  MALLOC_OR_RETURN(list, sizeof(DynamicArray), "Failed to allocate memory for DynamicArray", NULL);

  MALLOC_OR_CLEANUP(list->data, initial_capacity * sizeof(void *),
                    "Failed to allocate memory for DynamicArray data", { free(list); });
  list->size = 0;
  list->capacity = initial_capacity;
  return list;
}

int da_push(DynamicArray *list, void *item) {
  if (list->size >= list->capacity) {
    size_t new_capacity = list->capacity * 2;
    REALLOC_OR_RETURN(list->data, new_capacity * sizeof(void *),
                      "Failed to realloc DynamicArray data", -1);
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
  HashMap *map;
  MALLOC_OR_RETURN(map, sizeof(HashMap), "Failed to allocate memory for HashMap", NULL);

  MALLOC_OR_CLEANUP(map->buckets, num_buckets * sizeof(DynamicArray *),
                    "Failed to allocate memory for HashMap buckets", { free(map); });

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

bool hm_put(HashMap *map, char *key, void *value) {
  unsigned long hash = map->hash_func(key) % map->num_buckets;
  DynamicArray *bucket = map->buckets[hash];
  for (size_t i = 0; i < bucket->size; i++) {
    HashEntry *entry = (HashEntry *)da_get(bucket, i);
    if (strcmp(entry->key, key) == 0) {
      entry->value = value;
      return false;
    }
  }
  HashEntry *new_entry;
  MALLOC_OR_RETURN(new_entry, sizeof(HashEntry), "Failed to allocate memory for HashEntry", false);
  new_entry->key = strdup(key);
  new_entry->value = value;
  int result = da_push(bucket, new_entry);
  if (result == -1) {
    free(new_entry->key);
    free(new_entry);
    return false;
  }
  map->size++;
  if (map->size > map->num_buckets) {
    hm_resize(map, map->num_buckets * 2);
  }
  return true;
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
  DynamicArray **new_buckets;
  MALLOC_OR_RETURN(new_buckets, new_num_buckets * sizeof(DynamicArray *),
                   "Failed to allocate memory for new buckets",
                   ;);
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
      // TODO: handle da_push failure
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
  StringBuilder *sb;
  MALLOC_OR_RETURN(sb, sizeof(StringBuilder), "Failed to allocate memory for StringBuilder", NULL);

  sb->size = 0;
  MALLOC_OR_CLEANUP(sb->buffer, sizeof(char) * init_size,
                    "Failed to allocate memory for StringBuilder buffer", { free(sb); });
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
    REALLOC_OR_RETURN(sb->buffer, new_buff_len, "Failed to realloc StringBuilder buffer", NULL);
    sb->capacity = new_buff_len;
  }
  memmove(sb->buffer + sb->size, s, s_len);
  sb->size += s_len;
  return sb;
}

String *sb_to_string(StringBuilder *sb) {
  String *s;
  MALLOC_OR_RETURN(s, sizeof(String), "Failed to allocate memory for String", NULL);
  s->size = sb->size;
  MALLOC_OR_CLEANUP(s->string, sizeof(char) * (s->size + 1),
                    "Failed to allocate memory for String string", { free(s); });
  strncpy(s->string, sb->buffer, s->size);
  s->string[s->size] = '\0';
  return s;
}

void sb_free(StringBuilder *sb) {
  free(sb->buffer);
  free(sb);
}

#define ALIGNMENT (sizeof(void *))

size_t aligned_size(size_t size) {
  if (size % ALIGNMENT == 0) {
    return size;
  }
  return (((size + ALIGNMENT) / ALIGNMENT) * ALIGNMENT);
}

Arena *arena_new(size_t size, bool fixed) {
  Arena *a;
  MALLOC_OR_RETURN(a, sizeof(Arena), "Failed to allocate memory for Arena", NULL);

  size = aligned_size(size);
  a->size = size;
  MALLOC_OR_CLEANUP(a->mem, size, "Failed to allocate memory for Arena buffer", { free(a); });

  a->offset = 0;
  a->fixed = fixed;
  a->next = NULL;
  return a;
}

void *unaligned_arena_alloc(Arena *arena, size_t size) {
  if (arena->offset + size > arena->size) {
    if (arena->fixed == true) {
      log_msg(ERROR, "Arena out of memory!");
      return NULL;
    }
    if (arena->next == NULL) {
      arena->next = arena_new(arena->size > size ? arena->size * 2 : size * 2, false);
    }
    return unaligned_arena_alloc(arena->next, size);
  }
  arena->offset += size;
  // Casting to char* to do pointer arithmetic without warning
  return ((char *)arena->mem) + arena->offset - size;
}

void *arena_alloc(Arena *arena, size_t size) {
  size = aligned_size(size);
  return unaligned_arena_alloc(arena, size);
}

void arena_free(Arena *arena) {
  int arena_count = 1;
  Arena *current = arena;
  while (current->next != NULL) {
    arena_count++;
    current = current->next;
  }
  current = arena;
  Arena **arenas;
  MALLOC_OR_RETURN(
      arenas, sizeof(Arena *) * arena_count, "Failed to allocate memory for arenas array", ;);
  for (int i = 0; i < arena_count; i++) {
    arenas[i] = current;
    current = current->next;
  }
  current = arena;
  for (int i = 0; i < arena_count; i++) {
    current = arenas[arena_count - i - 1];
    free(current->mem);
    free(current);
  }
  free(arenas);
}

void arena_clear(Arena *arena) {
  Arena *current = arena;
  do {
    current->offset = 0;
    current = current->next;
  } while (current != NULL);
}
