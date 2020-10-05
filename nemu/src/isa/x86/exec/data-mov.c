#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  // TODO();
  rtl_push(&id_dest->val);

  print_asm_template1(push);
}

make_EHelper(pop) {
  TODO();

  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  // TODO();
  // rtl_pop(&t0);
  printf("now EBP is %x\n", cpu.ebp);
  rtl_lr(&t0, R_EBP, 4);
  printf("now t0 is %x\n", t0);
  rtl_sr(R_ESP, &t0, 4);
  printf("now t0 is %x\n", t0);
  printf("now ESP is %x\n", cpu.esp);
  rtl_pop(&t0);
  printf("now t0 is %x\n", t0);
  printf("now ESP is %x\n", cpu.esp);
  rtl_sr(R_EBP, &t0, 4);
  printf("now t0 is %x\n", t0);
  printf("now EBP is %x\n", cpu.ebp);
  printf("now ESP is %x\n", cpu.esp);

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decinfo.isa.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
