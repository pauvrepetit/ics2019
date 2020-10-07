#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  // return false;
  ref_r->eflagsReg &= 0b101011000001;
  if (memcmp((void *)ref_r, (void *)&cpu, sizeof(CPU_state)) != 0) {
    return false;
  }
  return true;
}

void isa_difftest_attach(void) {
}
