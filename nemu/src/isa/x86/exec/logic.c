#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test) {
  // TODO();
  rtl_and(&t0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t0, id_dest->width);
  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);

  print_asm_template2(test);
}

make_EHelper(and) {
  // TODO();
  rtl_and(&t0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);

  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&t0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);

  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&t0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);

  print_asm_template2(or);
}

make_EHelper(sar) {
  // unnecessary to update CF and OF in NEMU
  rtl_sar(&t0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);

  print_asm_template2(sar);
}

make_EHelper(shl) {
  // unnecessary to update CF and OF in NEMU
  rtl_shl(&t0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);

  print_asm_template2(shl);
}

make_EHelper(shr) {
  // unnecessary to update CF and OF in NEMU
  rtl_shr(&t0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  rtl_not(&id_dest->val, &id_dest->val);
  operand_write(id_dest, &id_dest->val);

  print_asm_template1(not);
}

make_EHelper(rol) {
  t0 = id_src->val;
  t1 = id_dest->val;
  switch (id_dest->width) {
    case 1:
      t0 %= 8;
      t1 = (t1 >> (8 - t0)) | (t1 << t0);
      break;
    case 2:
      t0 %= 16;
      t1 = (t1 >> (16 - t0)) | (t1 << t0);
      break;
    case 4:
      t0 %= 32;
      t1 = (t1 >> (32 - t0)) | (t1 << t0);
      break;
    default:
      break;
  }
  operand_write(id_dest, &t1);

  print_asm_template2(rol);
}