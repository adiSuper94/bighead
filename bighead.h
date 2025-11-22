#include <stddef.h>

typedef enum { INFO, WARN, ERROR } LogLevel;

typedef struct {
  void **data;
  size_t size;
  size_t capacity;
} DynamicArray;

typedef struct {
  char *key;
  void *value;
} HashEntry;

typedef struct {
  DynamicArray **buckets;
  size_t num_buckets;
  size_t size;
  unsigned long (*hash_func)(const char *);
} HashMap;

void log_msg(LogLevel level, const char *format, ...);

HashMap *hm_new(size_t num_buckets);
void hm_put(HashMap *map, char *key, void *value);
void *hm_get(HashMap *map, const char *key);
int hm_remove(HashMap *map, const char *key);
void hm_resize(HashMap *map, size_t new_num_buckets);
void hm_free(HashMap *map);
void hm_deep_free(HashMap *map);

DynamicArray *da_new(size_t initial_capacity);
int da_push(DynamicArray *list, void *item);
void *da_get(DynamicArray *list, size_t index);
int da_remove_at(DynamicArray *list, size_t index);
void da_free(DynamicArray *list);
void da_deep_free(DynamicArray *list);
