#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  // return false;
  ref_r->eflagsReg &= 0b101011000001;
  if (memcmp((void *)ref_r, (void *)&cpu, sizeof(CPU_state)) != 0) {
    printf("qemu regs:\n");
    isa_reg_display_diff(*ref_r);
    return false;
  }
  return true;
}

void isa_difftest_attach(void) {
}

void isa_reg_display_diff(CPU_state cpu) {
  printf("EAX\t\t%08x\t\t%10d\t\t\t\tEBX\t\t%08x\t\t%10d\n", cpu.eax, cpu.eax, cpu.ebx, cpu.ebx);
  printf("ECX\t\t%08x\t\t%10d\t\t\t\tEDX\t\t%08x\t\t%10d\n", cpu.ecx, cpu.ecx, cpu.edx, cpu.edx);
  printf("ESP\t\t%08x\t\t%10d\t\t\t\tEBP\t\t%08x\t\t%10d\n", cpu.esp, cpu.esp, cpu.ebp, cpu.ebp);
  printf("ESI\t\t%08x\t\t%10d\t\t\t\tEDI\t\t%08x\t\t%10d\n", cpu.esi, cpu.esi, cpu.edi, cpu.edi);
  printf("EIP\t\t%08x\n", cpu.pc);
  printf("CF:%d\tZF:%d\tSF:%d\tIF:%d\tOF:%d\n", cpu.eflags.CF, cpu.eflags.ZF, cpu.eflags.SF, cpu.eflags.IF, cpu.eflags.OF);
  return;
}