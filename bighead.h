#include <stddef.h>

typedef enum { INFO, WARN, ERROR } LogLevel;

void log_msg(LogLevel level, const char *format, ...);

typedef struct {
  size_t size;
  size_t capacity;
  void **data;
} DynamicArray;

DynamicArray *da_new(size_t initial_capacity);
int da_push(DynamicArray *list, void *item);
void *da_get(DynamicArray *list, size_t index);
int da_remove_at(DynamicArray *list, size_t index);
void da_free(DynamicArray *list);
void da_deep_free(DynamicArray *list);

typedef struct {
  char *key;
  void *value;
} HashEntry;

typedef struct {
  size_t num_buckets;
  size_t size;
  DynamicArray **buckets;
  unsigned long (*hash_func)(const char *);
} HashMap;

HashMap *hm_new(size_t num_buckets);
void hm_put(HashMap *map, char *key, void *value);
void *hm_get(HashMap *map, const char *key);
int hm_remove(HashMap *map, const char *key);
void hm_resize(HashMap *map, size_t new_num_buckets);
void hm_free(HashMap *map);
// TODO: check if there is a way to figure out if a pointer is heap or stack alocated.
// If its heap allocated hm_deep_free makes sense. Else delete it.
// If all memory is arena allocated cleaning it should be easy, and this function wouldn't
// be needed, as we can simply just free the arena.
void hm_deep_free(HashMap *map);

typedef struct {
  size_t size;
  char *string;
} String;

typedef struct {
  size_t size;
  size_t capacity;
  char *buffer;
} StringBuilder;

StringBuilder *sb_new();
StringBuilder *sb_append(StringBuilder *sb, char *s);
String *sb_to_string(StringBuilder *sb);
void sb_free(StringBuilder *sb);

typedef struct Arena {
  size_t size;
  size_t offset;
  size_t commited;
  void *mem;
} Arena;

Arena *arena_new(size_t size);
void *arena_alloc(Arena *arena, size_t size);
void arena_free(Arena *arena);
void arena_clear(Arena *arena);
