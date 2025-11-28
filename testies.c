#include "bighead.h"
#include <string.h>

void sb_test(void) {
  StringBuilder *sb = sb_new();
  sb_append(sb, "Bruce");
  log_msg(INFO, "%s cap: %zu size: %zu\n", sb->buffer, sb->capacity, sb->size);
  sb_append(sb, " Wayne");
  log_msg(INFO, "%s cap: %zu size: %zu\n", sb->buffer, sb->capacity, sb->size);
  sb_append(sb, " is Batman");
  log_msg(INFO, "%s cap: %zu size: %zu\n", sb->buffer, sb->capacity, sb->size);
  String *s = sb_to_string(sb);
  log_msg(INFO, "String: %s size: %zu\n", s->string, s->size);
}

void arena_test(void) {
  Arena *a = arena_new(sizeof(char) * 10, false);
  char *str1 = arena_alloc(a, sizeof(char) * 5);
  strcpy(str1, "abcdefghi");
  log_msg(INFO, str1);
  char *str2 = arena_alloc(a, sizeof(char) * 5);
  strcpy(str2, "FGHI");
  log_msg(INFO, str2);
  log_msg(INFO, str1);
}

int main(void) {
  sb_test();
  arena_test();
}
