#include "rtl/rtl.h"

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  // extern GateDesc *idt;
  // uint64_t data = vaddr_read(((void *)idt) + NO * 8, 4) | (vaddr_read(((void *)idt) + NO * 8 + 4, 4) << 32);
  // GateDesc *idt_entry = &data;

  // rtl_j((idt_entry->off_31_16 << 16) | idt_entry->off_15_0);
  uint32_t idt = cpu.idtr;
  uint32_t lo = vaddr_read(idt+NO*8, 4);
  uint32_t hi = vaddr_read(idt+NO*8+4, 4);
  uint32_t target = (hi & 0xffff0000) | (lo & 0xffff);
  t0 = cpu.eflagsReg;
  rtl_push(&t0);
  t0 = cpu.cs;
  rtl_push(&t0);
  t0 = ret_addr;
  rtl_push(&t0);
  rtl_j(target);
}

bool isa_query_intr(void) {
  return false;
}
