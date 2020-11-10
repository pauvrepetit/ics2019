#include "memory.h"
#include "proc.h"

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  if (pf >= (void *)_heap.end) {
    printf("pf is %d\n", pf);
  }
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk, intptr_t increment) {
  if (current->max_brk < brk + increment) {
    int block_count = (brk + increment - current->max_brk) / PGSIZE;
    if ((brk + increment - current->max_brk) % PGSIZE) block_count++;
    for (int i = 0; i < block_count; i++) {
      _map(&current->as, current->max_brk, 0, 0);
      current->max_brk += PGSIZE;
    }
  }
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
