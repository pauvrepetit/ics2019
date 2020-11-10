#include <am.h>
#include <x86.h>
#include <nemu.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN = {};
static PTE kptabs[(PMEM_SIZE + MMIO_SIZE) / PGSIZE] PG_ALIGN = {};
static void* (*pgalloc_usr)(size_t) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static _Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE},
  {.start = (void*)MMIO_BASE,  .end = (void*)(MMIO_BASE + MMIO_SIZE)}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

int _vme_init(void* (*pgalloc_f)(size_t), void (*pgfree_f)(void*)) {
  // printf("start vme_init\n");
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  printf("1\n");
  set_cr0(get_cr0() | CR0_PG);
  vme_enable = 1;

  return 0;
}

int _protect(_AddressSpace *as) {
  PDE *updir = (PDE*)(pgalloc_usr(1));
  as->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  return 0;
}

void _unprotect(_AddressSpace *as) {
}

static _AddressSpace *cur_as = NULL;
void __am_get_cur_as(_Context *c) {
  c->as = cur_as;
}

void __am_switch(_Context *c) {
  if (vme_enable) {
    set_cr3(c->as->ptr);
    cur_as = c->as;
  }
}

int _map(_AddressSpace *as, void *va, void *pa, int prot) {
  if (pa == NULL) {
    pa = pgalloc_usr(1);
  }
  // 将虚地址va映射到实地址pa处，实际上就是填写页表
  // 页表地址为as->ptr
  uint32_t *pte = ((uint32_t *)(as->ptr))[PDX(va)];
  if (!(((uint32_t)pte) & PTE_P)) {
    // 页不存在 申请一页 填入一级页表中
    uint32_t *new_page = pgalloc_usr(1);
    // memset(new_page, 0, PGSIZE);
    ((uint32_t *)(as->ptr))[PDX(va)] = ((uint32_t)new_page) | PTE_P;
  }
  uint32_t *pgentry = ((uint32_t *)(PTE_ADDR(pte)))[PTX(va)];
  if (!(((uint32_t)pgentry) & PTE_P)) {
    // 已经有映射了???
    return -1;
  }
  ((uint32_t *)(PTE_ADDR(pte)))[PTX(va)] = ((uint32_t)pa) | PTE_P;
  printf("map finish, va is %d, pa is %d\n", va, pa);
  return pa;
}

#include "string.h"

_Context *_ucontext(_AddressSpace *as, _Area ustack, _Area kstack, void *entry, void *args) {
  _Context *c = ((_Context *)ustack.end) - 1;
  memset(c, 0, sizeof(_Context));
  c->cs = 8;
  c->eip = (uintptr_t)entry;
  c->as = as;
  return c;
}
