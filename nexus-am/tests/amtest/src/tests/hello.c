#include <amtest.h>

static inline void putstr(const char *s) {
  for (; *s; s ++) _putc(*s);
}

typedef int bool;
#define false 0
#define true  1

unsigned long long cpu_clock() {
  return 123456;
}

void hello() {
  
  bool ready = false;
  unsigned int ccl = cpu_clock();
  if (!ready) {
    if (ccl > 29658) {
      printf("???\n");
      ready = true;
    }
  }
  // if ((!ppu.ready) && (cpu_clock() > 29658)) {
  //   printf("???\n");
  //   ppu.ready = true;
  // }
  for (int i = 0; i < 10; i ++) {
    putstr("Hello, AM World @ " __ISA__ "\n");
  }
}
