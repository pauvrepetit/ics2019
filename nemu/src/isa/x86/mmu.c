#include "nemu.h"

paddr_t page_translate(vaddr_t vaddr) {
  // 将虚地址转换为实地址
  uint32_t* pgdir = cpu.cr3;
  uint32_t* pgtable = pgdir[PDX(vaddr)];
  // if (pgtable & )
  uint32_t pgentry = pgtable[PTX(vaddr)];
  return pgentry + OFF(vaddr);
}

uint32_t isa_vaddr_read(vaddr_t addr, int len) {
  // return paddr_read(addr, len);
  if ((addr & (~PAGE_MASK)) != ((addr + len) & (~PAGE_MASK))) {
    // 访问的数据段跨越了页
    assert(0);
  } else {
    paddr_t paddr = page_translate(addr);
    return paddr_read(paddr, len);
  }
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
  // paddr_write(addr, data, len);
  if ((addr & (~PAGE_MASK)) != ((addr + len) & (~PAGE_MASK))) {
    // 访问的数据段跨越了页
    assert(0);
  } else {
    paddr_t paddr = page_translate(addr);
    return paddr_write(paddr, data, len);
  }
}
