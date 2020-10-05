#include "nemu.h"
#include <stdlib.h>
#include <time.h>

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  uint32_t sample[8];
  uint32_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

/* 针对特定的指令集，需要使用不同的方式输出寄存器信息 */
void isa_reg_display() {
  printf("EAX\t\t%08x\t\t%10d\t\t\t\tEBX\t\t%08x\t\t%10d\n", cpu.eax, cpu.eax, cpu.ebx, cpu.ebx);
  printf("ECX\t\t%08x\t\t%10d\t\t\t\tEDX\t\t%08x\t\t%10d\n", cpu.ecx, cpu.ecx, cpu.edx, cpu.edx);
  printf("ESP\t\t%08x\t\t%10d\t\t\t\tEBP\t\t%08x\t\t%10d\n", cpu.esp, cpu.esp, cpu.ebp, cpu.ebp);
  printf("ESI\t\t%08x\t\t%10d\t\t\t\tEDI\t\t%08x\t\t%10d\n", cpu.esi, cpu.esi, cpu.edi, cpu.edi);
  printf("EIP\t\t%08x\n", cpu.pc);
  printf("CF:%d\tZF:%d\tSF:%d\tIF:%d\tOF:%d\n", cpu.eflags.CF, cpu.eflags.ZF, cpu.eflags.SF, cpu.eflags.IF, cpu.eflags.OF);
  return;
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
  *success = true;
  if (strcmp(s, "$EAX") == 0) {
    return cpu.eax;
  } else if (strcmp(s, "$EBX") == 0) {
    return cpu.ebx;
  } else if (strcmp(s, "$ECX") == 0) {
    return cpu.ecx;
  } else if (strcmp(s, "$EDX") == 0) {
    return cpu.edx;
  } else if (strcmp(s, "$ESI") == 0) {
    return cpu.esi;
  } else if (strcmp(s, "$EDI") == 0) {
    return cpu.edi;
  } else if (strcmp(s, "$ESP") == 0) {
    return cpu.esp;
  } else if (strcmp(s, "$EBP") == 0) {
    return cpu.ebp;
  } else if (strcmp(s, "$EPC") == 0) {
    return cpu.pc;
  } else {
    *success = false;
    return 0;
  }
}
