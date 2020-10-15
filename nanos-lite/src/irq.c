#include "common.h"

_Context* do_syscall(_Context *c);
_Context* schedule(_Context *prev);

static _Context* do_event(_Event e, _Context* c) {
  switch (e.event) {
    case _EVENT_SYSCALL: do_syscall(c); break;
    case _EVENT_YIELD:
      // 在接受到_EVENT_YIELD事件的时候，调用schedule进行进程调度
      // 返回新的上下文
      return schedule(c);
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
