#include <am.h>
#include <x86.h>
#include "string.h"
#include "stdlib.h"

static _Context* (*user_handler)(_Event, _Context*) = NULL;

void __am_irq0();
void __am_vecsys();
void __am_vectrap();
void __am_vecnull();

_Context* __am_irq_handle(_Context *c) {
  // c是原进程的上下文
  __am_get_cur_as(c);
  _Context *next = c;
  printf("??? irq is %d\n", c->irq);
  if (user_handler) {
    _Event ev = {0};
    switch (c->irq) {
      case 0x32: ev.event = _EVENT_IRQ_TIMER; printf("irq_handle IRQ_TIMER\n"); break;
      case 0x80: ev.event = _EVENT_SYSCALL; printf("irq_handle SYSCALL\n"); break;
      case 0x81: ev.event = _EVENT_YIELD; printf("irq_handle YIELD\n"); break;
      default: ev.event = _EVENT_ERROR; break;
    }

    next = user_handler(ev, c);
    if (next == NULL) {
      next = c;
    }
  }
  printf("irq_handle\n");
  __am_switch(next);

  return next;
}

int _cte_init(_Context*(*handler)(_Event, _Context*)) {
  static GateDesc idt[NR_IRQ];

  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i ++) {
    idt[i] = GATE(STS_TG32, KSEL(SEG_KCODE), __am_vecnull, DPL_KERN);
  }

  // ----------------------- interrupts ----------------------------
  idt[32]   = GATE(STS_IG32, KSEL(SEG_KCODE), __am_irq0,   DPL_KERN);
  // ---------------------- system call ----------------------------
  idt[0x80] = GATE(STS_TG32, KSEL(SEG_KCODE), __am_vecsys, DPL_USER);
  idt[0x81] = GATE(STS_TG32, KSEL(SEG_KCODE), __am_vectrap, DPL_KERN);

  set_idt(idt, sizeof(idt));

  // register event handler
  user_handler = handler;

  sti();  // 开中断
  return 0;
}

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
  _Context *c = (_Context *)(stack.end) - 1;
  memset(c, 0, sizeof(_Context));
  c->cs = 8;
  c->eip = (uintptr_t)entry;
  return c;
}

void _yield() {
  asm volatile("int $0x81");
}

int _intr_read() {
  return 0;
}

void _intr_write(int enable) {
}
