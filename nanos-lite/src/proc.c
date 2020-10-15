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

void init_proc() {
  context_kload(&pcb[0], (void *)hello_fun);
  printf("create kernel context finished\n");
  pcb_boot = pcb[0];
  printf("pcb_boot.eip is %d\n", pcb_boot.cp->eip);
  switch_boot_pcb();
  printf("hahha\n");

  _yield();

  // switch_boot_pcb();

  // Log("Initializing processes...");

  // load program here
  // naive_uload(NULL, "/bin/init");
}

_Context* schedule(_Context *prev) {
  // current->cp = prev; // ??
  current = pcb;  // current指向第0个pcb进程
  return current->cp;
}
