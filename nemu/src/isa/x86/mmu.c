#include "nemu.h"

// Page directory and page table constants
#define PGSIZE         4096    // Bytes mapped by a page
#define PGMASK         4095    // Mask for bit ops
#define NR_PDE         1024    // # directory entries per page directory
#define NR_PTE         1024    // # PTEs per page table
#define PGSHFT         12      // log2(PGSIZE)
#define PTXSHFT        12      // Offset of PTX in a linear address
#define PDXSHFT        22      // Offset of PDX in a linear address

// Page table/directory entry flags
#define PTE_P          0x001   // Present
#define PTE_W          0x002   // Writeable
#define PTE_U          0x004   // User
#define PTE_PWT        0x008   // Write-Through
#define PTE_PCD        0x010   // Cache-Disable
#define PTE_A          0x020   // Accessed
#define PTE_D          0x040   // Dirty

typedef uint32_t PTE;
typedef uint32_t PDE;
#define PDX(va)          (((uint32_t)(va) >> PDXSHFT) & 0x3ff)
#define PTX(va)          (((uint32_t)(va) >> PTXSHFT) & 0x3ff)
#define OFF(va)          ((uint32_t)(va) & 0xfff)
#define ROUNDUP(a, sz)   ((((uintptr_t)a)+(sz)-1) & ~((sz)-1))
#define ROUNDDOWN(a, sz) ((((uintptr_t)a)) & ~((sz)-1))
#define PTE_ADDR(pte)    ((uint32_t)(pte) & ~0xfff)
#define PGADDR(d, t, o)  ((uint32_t)((d) << PDXSHFT | (t) << PTXSHFT | (o)))


paddr_t page_translate(vaddr_t vaddr) {
  // 将虚地址转换为实地址
  uint32_t* pgdir = (uint32_t*)cpu.cr3;
  printf("page_trans pgdir %d\n", pgdir);
  // uint32_t* pgtable = (uint32_t*)(pgdir[PDX(vaddr)]);
  uint32_t* pgtable = (uint32_t)paddr_read(pgdir + PDX(vaddr), 4);
  printf("page_trans pgtable %d\n", pgtable);
  if (!(((uint32_t)pgtable) & PTE_P)) {
    // 页不存在
    assert(0);
  }
  pgtable = (uint32_t*)(PTE_ADDR(pgtable));
  // uint32_t pgentry = pgtable[PTX(vaddr)];
  uint32_t pgentry = paddr_read(pgtable + PTX(vaddr), 4);
  printf("page_trans pgentry %d\n", pgentry);
  if (!(((uint32_t)pgentry) & PTE_P)) {
    assert(0);
  }
  pgentry = PTE_ADDR(pgentry);
  return pgentry + OFF(vaddr);
}

// Control Register flags
#define CR0_PE         0x00000001  // Protection Enable
#define CR0_PG         0x80000000  // Paging

uint32_t isa_vaddr_read(vaddr_t addr, int len) {
  if (!(cpu.cr0 & CR0_PG))
    return paddr_read(addr, len);
  printf("isa_vaddr_read, addr is %d, len is %d\n", addr, len);
  if ((addr & (~PAGE_MASK)) != ((addr + len) & (~PAGE_MASK))) {
    // 访问的数据段跨越了页
    assert(0);
  } else {
    paddr_t paddr = page_translate(addr);
    printf("paddr is %d\n", paddr);
    return paddr_read(paddr, len);
  }
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
  if (!(cpu.cr0 & CR0_PG))
    return paddr_write(addr, data, len);
  printf("isa_vaddr_write, addr is %d, len is %d\n", addr, len);
  if ((addr & (~PAGE_MASK)) != ((addr + len) & (~PAGE_MASK))) {
    // 访问的数据段跨越了页
    assert(0);
  } else {
    paddr_t paddr = page_translate(addr);
    return paddr_write(paddr, data, len);
  }
}
