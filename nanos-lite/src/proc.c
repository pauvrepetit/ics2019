#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void context_kload(PCB *pcb, void *entry);
void context_uload(PCB *pcb, const char *filename);

void init_proc() {
  // context_kload(&pcb[0], (void *)hello_fun);
  // context_uload(&pcb[0], "/bin/hello");
  context_uload(&pcb[0], "/bin/init");
  switch_boot_pcb();

  // _yield();

  // switch_boot_pcb();

  // Log("Initializing processes...");

  // load program here
  // naive_uload(NULL, "/bin/init");
}

void context_uload_sys(const char *filename) {
  context_uload(&pcb[1], filename);
}

_Context* schedule(_Context *prev) {
  current->cp = prev; // 这个操作很重要，prev是当前进程的上下文，他在栈里面一个不确定的位置，因此在进行上下文切换的时候我们需要保存当前进程上下文的位置，保存到该进程的PCB中，也就是current中，current只是一个指针，下面把这个指针指向另一个PCB就可以执行另一个进程了
  // 而下次进程调度如果调度到当前进程的话，我们就可以在栈中找到其被中断时的上下文
  // current = pcb;  // current指向第0个pcb进程
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  return current->cp;
}
