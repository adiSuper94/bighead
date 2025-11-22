# BigHead

BigHead is a small and useless C library for everyone but me (maybe even me) that provides dynamic arrays, hash maps, and logging utilities. Just copy-pasta into your codebase and boom – instant utility power or everything crashes!

## Features

- **Dynamic Arrays**: Grow your arrays dynamically – infinitely scalable!
- **Hash Maps**: Store key-value pairs efficiently.
- **Logging**: Log messages with levels INFO, WARN, ERROR – because debugging can be full of surprises.

## Installation

Copy `bighead.h` and `bighead.c` into your project. Include `bighead.h` and compile `bighead.c` with your code.

No dependencies, no npm install, no VC funding required.

## Usage

### Dynamic Array

```c
DynamicArray *arr = da_new(10);
da_push(arr, "Hello");
da_push(arr, "World");
printf("%s\n", (char*)da_get(arr, 0)); // Hello
da_free(arr);
```

### Hash Map

```c
HashMap *map = hm_new(16);
hm_put(map, "key", "value");
printf("%s\n", (char*)hm_get(map, "key")); // value
hm_free(map);
```

### Logging

```c
log_msg(INFO, "This is an info message");
log_msg(ERROR, "Something went wrong: %s", error_msg);
```
