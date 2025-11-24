# BigHead

BigHead is a small and useless C library for everyone but me (maybe even me) that provides dynamic arrays, hash maps, and logging utilities. Just copy-pasta into your codebase and boom – instant utility power or everything crashes!

## Features

- **Dynamic Arrays**: Grow your arrays dynamically – infinitely scalable!
- **Hash Maps**: Store key-value pairs efficiently.
- **Logging**: Log messages with levels INFO, WARN, ERROR – because debugging can be full of surprises.
- **String Builder**: Efficient string concatenation without manual memory management.
- **Arena Allocator**: Fast memory pool allocation with bulk cleanup.

## Installation

Copy `bighead.h` and `bighead.c` into your project. Include `bighead.h` and compile `bighead.c` with your code.

No dependencies, no npm install, no VC funding required.

## Usage

### Dynamic Array

```c
DynamicArray *arr = da_new(10);
da_push(arr, "Hello");
da_push(arr, "World");
log_msg(INFO, "%s\n", (char*)da_get(arr, 0)); // Hello
da_free(arr);
```

### Hash Map

```c
HashMap *map = hm_new(16);
hm_put(map, "key", "value");
log_msg(INFO, "%s\n", (char*)hm_get(map, "key")); // value
hm_free(map);
```

### Logging

```c
log_msg(INFO, "This is an info message");
log_msg(ERROR, "Something went wrong: %s", error_msg);
```

### String Builder

```c
StringBuilder *sb = sb_new();
sb_append(sb, "Hello");
sb_append(sb, " World");
String *result = sb_to_string(sb);
log_msg(INFO, "%s\n", result->string); // Hello World
sb_free(sb);
```

### Arena Allocator

```c
Arena *arena = arena_new(1024, false);
char *str = arena_alloc(arena, 100);
strcpy(str, "Allocated string");
// Use memory...
arena_clear(arena); // Reset without freeing
arena_free(arena);  // Complete cleanup
```
