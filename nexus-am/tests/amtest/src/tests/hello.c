#include <amtest.h>

static inline void putstr(const char *s) {
  for (; *s; s ++) _putc(*s);
}

typedef int bool;
#define false 0
#define true  1

unsigned long long clocks = 0;

void cpu_run(int c) {
  clocks += c;
}

unsigned long long cpu_clock() {
  return clocks;
}

void hello() {
  cpu_run(10000);
  cpu_run(5000);
  cpu_run(20000);
  bool ready = false;
  unsigned int ccl = cpu_clock();
  if (!ready) {
    if (ccl > 29658) {
      printf("???\n");
      ready = true;
    }
  }
  // if ((!ready) && (cpu_clock() > 29658)) {
  //   printf("???\n");
  //   ready = true;
  // }
  for (int i = 0; i < 10; i ++) {
    putstr("Hello, AM World @ " __ISA__ "\n");
  }
}
