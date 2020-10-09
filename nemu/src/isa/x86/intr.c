#include "rtl/rtl.h"

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  // extern GateDesc *idt;
  // uint64_t data = vaddr_read(((void *)idt) + NO * 8, 4) | (vaddr_read(((void *)idt) + NO * 8 + 4, 4) << 32);
  // GateDesc *idt_entry = &data;

  // rtl_j((idt_entry->off_31_16 << 16) | idt_entry->off_15_0);
}

bool isa_query_intr(void) {
  return false;
}
