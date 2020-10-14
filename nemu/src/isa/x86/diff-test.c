#include "nemu.h"
#include "monitor/diff-test.h"

void isa_reg_display_diff(CPU_state cpu);

#define DIFFTEST_REG_CHECK_SIZE (sizeof(uint32_t) * 9)

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  // 不检查ELFAGS，DIFFTEST_REG_SIZE被我们调整为8个GPRs+EIP+EFLAGS，所以这里做一些调整
  if (memcmp((void *)ref_r, (void *)&cpu, DIFFTEST_REG_CHECK_SIZE) != 0) {
    printf("qemu regs:\n");
    isa_reg_display_diff(*ref_r);
    return false;
  }
  return true;
}

void isa_difftest_attach(void) {
  ref_difftest_memcpy_from_dut(0, 0, 0x7c00);

  CPU_state ref_r;
  ref_difftest_getregs(&ref_r);
  uint16_t idt_data[3];
  idt_data[0] = cpu.idt_size;
  idt_data[1] = cpu.idtr;
  idt_data[2] = ((uint32_t)cpu.idtr) >> 16;

  // uint32_t lidt = 
  uint32_t pc = ref_r.pc;
}

void isa_reg_display_diff(CPU_state cpu) {
  printf("EAX\t\t%08x\t\t%10d\t\t\t\tEBX\t\t%08x\t\t%10d\n", cpu.eax, cpu.eax, cpu.ebx, cpu.ebx);
  printf("ECX\t\t%08x\t\t%10d\t\t\t\tEDX\t\t%08x\t\t%10d\n", cpu.ecx, cpu.ecx, cpu.edx, cpu.edx);
  printf("ESP\t\t%08x\t\t%10d\t\t\t\tEBP\t\t%08x\t\t%10d\n", cpu.esp, cpu.esp, cpu.ebp, cpu.ebp);
  printf("ESI\t\t%08x\t\t%10d\t\t\t\tEDI\t\t%08x\t\t%10d\n", cpu.esi, cpu.esi, cpu.edi, cpu.edi);
  printf("EIP\t\t%08x\n", cpu.pc);
  return;
}