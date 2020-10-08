#include <amtest.h>

static inline void putstr(const char *s) {
  for (; *s; s ++) _putc(*s);
}

void hello() {
  for (int i = 0; i < 10; i ++) {
    putstr("Hello, AM World @ " __ISA__ "\n");
    printf("Hello World from printf, %d, %d, %d, %s, %s\n", i, i+1, i+2, "Hu", "Ao");
  }
}
