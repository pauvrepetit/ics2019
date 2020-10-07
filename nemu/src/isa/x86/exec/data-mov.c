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
  // TODO();
  rtl_pop(&id_dest->val);
  operand_write(id_dest, &id_dest->val);

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
  rtl_lr(&t1, R_EBP, 4);
  rtl_sr(R_ESP, &t1, 4);
  rtl_pop(&t1);
  rtl_sr(R_EBP, &t1, 4);

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decinfo.isa.is_operand_size_16) {
    // TODO();
    rtl_lr(&t0, R_EAX, 2);
    t1 = (t0 & 0x8000) != 0 ? 0xffff : 0;
    rtl_sr(R_EDX, &t1, 2);
  }
  else {
    // TODO();
    rtl_lr(&t0, R_EAX, 4);
    t1 = (t0 & 0x80000000) != 0 ? 0xffffffff : 0;
    rtl_sr(R_EDX, &t1, 4);
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
  printf("movsx, id_src is %08x\nid_src is %s\n", id_src->val, id_src->str);
  rtl_sext(&s0, &id_src->val, id_src->width);
  printf("res is %08x\n", s0);
  operand_write(id_dest, &s0);
  printf("movsx, id_dest is %s\n", id_dest->str);
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
