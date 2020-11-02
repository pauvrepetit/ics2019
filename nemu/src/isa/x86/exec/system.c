#include "cpu/exec.h"

void raise_intr(uint32_t NO, vaddr_t ret_addr);

make_EHelper(lidt) {
  assert(id_dest->width == 4);
  t0 = 0;
  interpret_rtl_lm(&t0, &id_dest->addr, 2);
  cpu.idt_size = t0;
  t0 = 0;
  id_dest->addr += 2;
  interpret_rtl_lm(&t0, &id_dest->addr, 4);
  cpu.idtr = t0;

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  // TODO();
  if (id_dest->reg == 0) {
    cpu.cr0 = id_src->val;
  } else if (id_dest->reg == 3) {
    cpu.cr3 = id_src->val;
  } else {
    // ...
    printf("not cr0 or cr3\n");
  }

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  // TODO();
  if (id_src->reg == 0) {
    id_dest->val = cpu.cr0;
  } else if (id_src->reg == 3) {
    id_dest->val = cpu.cr3;
  } else {
    // ...
    printf("not cr0 or cr3\n");
  }
  operand_write(id_dest, id_dest->val);

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

  difftest_skip_ref();
}

make_EHelper(int) {
  raise_intr(id_dest->val, *pc);

  print_asm("int %s", id_dest->str);

  difftest_skip_dut(1, 2);
}

make_EHelper(iret) {
  rtl_pop(&t0);
  rtl_pop(&cpu.cs);
  rtl_pop(&cpu.eflagsReg);
  rtl_j(t0);

  print_asm("iret");
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

make_EHelper(in) {
  // TODO();
  switch (id_dest->width) {
    case 1:
      id_dest->val = pio_read_b(id_src->val);
      break;
    case 2:
      id_dest->val = pio_read_w(id_src->val);
      break;
    case 4:
      id_dest->val = pio_read_l(id_src->val);
      break;
    default:
      break;
  }
  operand_write(id_dest, &id_dest->val);

  print_asm_template2(in);
}

make_EHelper(out) {
  // TODO();
  switch (id_src->width) {
    case 1:
      pio_write_b(id_dest->val, id_src->val);
      break;
    case 2:
      pio_write_w(id_dest->val, id_src->val);
      break;
    case 4:
      pio_write_l(id_dest->val, id_src->val);
      break;
    default:
      break;
  }

  print_asm_template2(out);
}
