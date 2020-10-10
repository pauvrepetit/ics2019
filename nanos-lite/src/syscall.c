#include "common.h"
#include "syscall.h"

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_exit: _halt(a[1]); break;
    case SYS_yield: _yield(); c->GPRx = 0; break;
    case SYS_write: {
      Log("call SYS_write %s\n", (char *)a[2]);
      if (a[1] != 1 && a[1] != 2) {
        c->GPRx = -1;
        break;
      }
      int count = 0;
      for (count = 0; count < a[3]; count++) {
        _putc(((char *)a[2])[count]);
      }
      c->GPRx = count;
      break;
    }
    case SYS_brk: c->GPRx = -1; break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
