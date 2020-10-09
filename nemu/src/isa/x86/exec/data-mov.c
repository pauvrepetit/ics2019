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
  if (decinfo.isa.is_operand_size_16) {
    assert(1 == 0);
  } else {
    rtl_lr(&t1, R_ESP, 4);
    rtl_lr(&t0, R_EAX, 4);
    rtl_push(&t0);
    rtl_lr(&t0, R_ECX, 4);
    rtl_push(&t0);
    rtl_lr(&t0, R_EDX, 4);
    rtl_push(&t0);
    rtl_lr(&t0, R_EBX, 4);
    rtl_push(&t0);
    rtl_push(&t1);
    rtl_lr(&t0, R_EBP, 4);
    rtl_push(&t0);
    rtl_lr(&t0, R_ESI, 4);
    rtl_push(&t0);
    rtl_lr(&t0, R_EDI, 4);
    rtl_push(&t0);
  }

  print_asm("pusha");
}

make_EHelper(popa) {
  if (decinfo.isa.is_operand_size_16) {
    assert(1 == 0);
  } else {
    rtl_pop(&t0);
    rtl_sr(R_EDI, &t0, 4);
    rtl_pop(&t0);
    rtl_sr(R_ESI, &t0, 4);
    rtl_pop(&t0);
    rtl_sr(R_EBP, &t0, 4);
    rtl_pop(&t0);
    rtl_pop(&t0);
    rtl_sr(R_EBX, &t0, 4);
    rtl_pop(&t0);
    rtl_sr(R_EDX, &t0, 4);
    rtl_pop(&t0);
    rtl_sr(R_ECX, &t0, 4);
    rtl_pop(&t0);
    rtl_sr(R_EAX, &t0, 4);
  }

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
    rtl_lr(&t0, R_EAX, 2);
    t1 = (t0 & 0x8000) != 0 ? 0xffff : 0;
    rtl_sr(R_EDX, &t1, 2);
  }
  else {
    rtl_lr(&t0, R_EAX, 4);
    t1 = (t0 & 0x80000000) != 0 ? 0xffffffff : 0;
    rtl_sr(R_EDX, &t1, 4);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
    rtl_lr(&t0, R_EAX, 1);
    if ((t0 & 0x80) != 0) {
      t0 |= 0xffffff00;
    } else {
      t0 &= 0xff;
    }
    rtl_sr(R_EAX, &t0, 2);
  } else {
    rtl_lr(&t0, R_EAX, 2);
    if ((t0 & 0x8000) != 0) {
      t0 |= 0xffff0000;
    } else {
      t0 &= 0xffff;
    }
    rtl_sr(R_EAX, &t0, 4);
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
