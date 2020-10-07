#include "cpu/exec.h"

// decode operand helper
#define make_DopHelper(name) void concat(decode_op_, name) (vaddr_t *pc, Operand *op, bool load_val)

/* Refer to Appendix A in i386 manual for the explanations of these abbreviations */

/* Ib, Iv */
static inline make_DopHelper(I) {
  /* pc here is pointing to the immediate */
  op->type = OP_TYPE_IMM;
  op->imm = instr_fetch(pc, op->width);
  rtl_li(&op->val, op->imm);

  print_Dop(op->str, OP_STR_SIZE, "$0x%x", op->imm);
}

/* I386 manual does not contain this abbreviation, but it is different from
 * the one above from the view of implementation. So we use another helper
 * function to decode it.
 */
/* sign immediate */
static inline make_DopHelper(SI) {
  assert(op->width == 1 || op->width == 4);

  op->type = OP_TYPE_IMM;

  /* TODO: Use instr_fetch() to read `op->width' bytes of memory
   * pointed by 'pc'. Interpret the result as a signed immediate,
   * and assign it to op->simm.
   *
   op->simm = ???
   */
  // TODO();
  op->simm = instr_fetch(pc, op->width);
  if (op->width == 1 && (op->simm >= (1 << 7))) {
      op->simm |= 0xffffff00;
  } else if (op->width == 2 && op->simm >= (1 << 15)) {
    op->simm |= 0xffff0000;
  }

  rtl_li(&op->val, op->simm);

  print_Dop(op->str, OP_STR_SIZE, "$0x%x", op->simm);
}

/* I386 manual does not contain this abbreviation.
 * It is convenient to merge them into a single helper function.
 */
/* AL/eAX */
static inline make_DopHelper(a) {
  op->type = OP_TYPE_REG;
  op->reg = R_EAX;
  if (load_val) {
    rtl_lr(&op->val, R_EAX, op->width);
  }

  print_Dop(op->str, OP_STR_SIZE, "%%%s", reg_name(R_EAX, op->width));
}

/* This helper function is use to decode register encoded in the opcode. */
/* XX: AL, AH, BL, BH, CL, CH, DL, DH
 * eXX: eAX, eCX, eDX, eBX, eSP, eBP, eSI, eDI
 */
static inline make_DopHelper(r) {
  op->type = OP_TYPE_REG;
  op->reg = decinfo.opcode & 0x7;
  if (load_val) {
    rtl_lr(&op->val, op->reg, op->width);
  }

  print_Dop(op->str, OP_STR_SIZE, "%%%s", reg_name(op->reg, op->width));
}

/* I386 manual does not contain this abbreviation.
 * We decode everything of modR/M byte by one time.
 */
/* Eb, Ew, Ev
 * Gb, Gv
 * Cd,
 * M
 * Rd
 * Sw
 */
static inline void decode_op_rm(vaddr_t *pc, Operand *rm, bool load_rm_val, Operand *reg, bool load_reg_val) {
  read_ModR_M(pc, rm, load_rm_val, reg, load_reg_val);
}

/* Ob, Ov */
static inline make_DopHelper(O) {
  op->type = OP_TYPE_MEM;
  rtl_li(&op->addr, instr_fetch(pc, 4));
  if (load_val) {
    rtl_lm(&op->val, &op->addr, op->width);
  }

  print_Dop(op->str, OP_STR_SIZE, "0x%x", op->addr);
}

/* Eb <- Gb
 * Ev <- Gv
 * r/m, r
 */
make_DHelper(G2E) {
  decode_op_rm(pc, id_dest, true, id_src, true);
}

make_DHelper(mov_G2E) {
  decode_op_rm(pc, id_dest, false, id_src, true);
}

/* Gb <- Eb
 * Gv <- Ev
 * r, r/m
 */
make_DHelper(E2G) {
  decode_op_rm(pc, id_src, true, id_dest, true);
}

make_DHelper(mov_E2G) {
  decode_op_rm(pc, id_src, true, id_dest, false);
}

make_DHelper(lea_M2G) {
  decode_op_rm(pc, id_src, false, id_dest, false);
}

/* AL <- Ib
 * eAX <- Iv
 */
make_DHelper(I2a) {
  decode_op_a(pc, id_dest, true);
  decode_op_I(pc, id_src, true);
}

/* Gv <- EvIb
 * Gv <- EvIv
 * use for imul */
make_DHelper(I_E2G) {
  decode_op_rm(pc, id_src2, true, id_dest, false);
  decode_op_I(pc, id_src, true);
}

/* Eb <- Ib
 * Ev <- Iv
 */
make_DHelper(I2E) {
  decode_op_rm(pc, id_dest, true, NULL, false);
  decode_op_I(pc, id_src, true);
}

make_DHelper(mov_I2E) {
  decode_op_rm(pc, id_dest, false, NULL, false);
  decode_op_I(pc, id_src, true);
}

/* XX <- Ib
 * eXX <- Iv
 */
make_DHelper(I2r) {
  decode_op_r(pc, id_dest, true);
  decode_op_I(pc, id_src, true);
}

make_DHelper(mov_I2r) {
  decode_op_r(pc, id_dest, false);
  decode_op_I(pc, id_src, true);
}

/* used by unary operations */
make_DHelper(I) {
  decode_op_I(pc, id_dest, true);
}

make_DHelper(r) {
  decode_op_r(pc, id_dest, true);
}

make_DHelper(E) {
  decode_op_rm(pc, id_dest, true, NULL, false);
}

make_DHelper(setcc) {
  decode_op_rm(pc, id_dest, false, NULL, false);
}

make_DHelper(gp7_E) {
  decode_op_rm(pc, id_dest, false, NULL, false);
}

/* used by test in group3 */
make_DHelper(test_I) {
  decode_op_I(pc, id_src, true);
}

make_DHelper(SI2E) {
  assert(id_dest->width == 2 || id_dest->width == 4);
  decode_op_rm(pc, id_dest, true, NULL, false);
  id_src->width = 1;
  decode_op_SI(pc, id_src, true);
  if (id_dest->width == 2) {
    id_src->val &= 0xffff;
  }
}

make_DHelper(SI_E2G) {
  assert(id_dest->width == 2 || id_dest->width == 4);
  decode_op_rm(pc, id_src2, true, id_dest, false);
  id_src->width = 1;
  decode_op_SI(pc, id_src, true);
  if (id_dest->width == 2) {
    id_src->val &= 0xffff;
  }
}

make_DHelper(gp2_1_E) {
  decode_op_rm(pc, id_dest, true, NULL, false);
  id_src->type = OP_TYPE_IMM;
  id_src->imm = 1;
  rtl_li(&id_src->val, 1);

  print_Dop(id_src->str, OP_STR_SIZE, "$1");
}

make_DHelper(gp2_cl2E) {
  decode_op_rm(pc, id_dest, true, NULL, false);
  id_src->type = OP_TYPE_REG;
  id_src->reg = R_CL;
  rtl_lr(&id_src->val, R_CL, 1);

  print_Dop(id_src->str, OP_STR_SIZE, "%%cl");
}

make_DHelper(gp2_Ib2E) {
  decode_op_rm(pc, id_dest, true, NULL, false);
  id_src->width = 1;
  decode_op_I(pc, id_src, true);
}

/* Ev <- GvIb
 * use for shld/shrd */
make_DHelper(Ib_G2E) {
  decode_op_rm(pc, id_dest, true, id_src2, true);
  id_src->width = 1;
  decode_op_I(pc, id_src, true);
}

/* Ev <- GvCL
 * use for shld/shrd */
make_DHelper(cl_G2E) {
  decode_op_rm(pc, id_dest, true, id_src2, true);
  id_src->type = OP_TYPE_REG;
  id_src->reg = R_CL;
  rtl_lr(&id_src->val, R_CL, 1);

  print_Dop(id_src->str, OP_STR_SIZE, "%%cl");
}

make_DHelper(O2a) {
  decode_op_O(pc, id_src, true);
  decode_op_a(pc, id_dest, false);
}

make_DHelper(a2O) {
  decode_op_a(pc, id_src, true);
  decode_op_O(pc, id_dest, false);
}

make_DHelper(J) {
  decode_op_SI(pc, id_dest, false);
  // the target address can be computed in the decode stage
  decinfo.jmp_pc = id_dest->simm + *pc;
}

make_DHelper(push_SI) {
  decode_op_SI(pc, id_dest, true);
}

make_DHelper(in_I2a) {
  id_src->width = 1;
  decode_op_I(pc, id_src, true);
  decode_op_a(pc, id_dest, false);
}

make_DHelper(in_dx2a) {
  id_src->type = OP_TYPE_REG;
  id_src->reg = R_DX;
  rtl_lr(&id_src->val, R_DX, 2);

  print_Dop(id_src->str, OP_STR_SIZE, "(%%dx)");

  decode_op_a(pc, id_dest, false);
}

make_DHelper(out_a2I) {
  decode_op_a(pc, id_src, true);
  id_dest->width = 1;
  decode_op_I(pc, id_dest, true);
}

make_DHelper(out_a2dx) {
  decode_op_a(pc, id_src, true);

  id_dest->type = OP_TYPE_REG;
  id_dest->reg = R_DX;
  rtl_lr(&id_dest->val, R_DX, 2);

  print_Dop(id_dest->str, OP_STR_SIZE, "(%%dx)");
}

void operand_write(Operand *op, rtlreg_t* src) {
  if (op->type == OP_TYPE_REG) { rtl_sr(op->reg, src, op->width); }
  else if (op->type == OP_TYPE_MEM) { rtl_sm(&op->addr, src, op->width); }
  else { assert(0); }
}


make_DHelper(call_rel32) {
  id_src->width = 4;
  decode_op_I(pc, id_src, true);
  decinfo_set_jmp(true);
  decinfo.jmp_pc = id_src->imm + *pc;
}

make_DHelper(push_r32) {
  // decode_op_r(pc, id_src, true);
  // rtl_lr(&id_dest->val, R_ESP, 4);
  decode_op_r(pc, id_dest, true);
}

make_DHelper(push_imm32) {
  // id_src->width = 4;
  // decode_op_I(pc, id_src, true);
  // rtl_lr(&id_dest->val, R_ESP, 4);
  id_dest->width = 4;
  decode_op_I(pc, id_dest, true);
}

make_DHelper(sub) {
  // printf("hit here sub\n");
}

make_DHelper(xor) {
  // printf("hit here xor\n");
  decode_op_rm(pc, id_dest, true, id_src, true);
  // decode_op_rm(pc, id_src, true, NULL, false);
  // decode_op_r(pc, id_dest, true);
  // decode_op_r(pc, id_src, true);
}

make_DHelper(ret) {
  // printf("hit here ret\n");
}

make_DHelper(lea) {
  decode_op_rm(pc, id_src, true, id_dest, false);
}

make_DHelper(and) {
  // printf("hit here and\n");
}

make_DHelper(push_m) {
  // printf("hit here push_m\n");
}

make_DHelper(add_01) {
  decode_op_rm(pc, id_dest, true, id_src, true);
}

make_DHelper(cmp_3b) {
  decode_op_rm(pc, id_src, true, id_dest, true);
}

make_DHelper(movzb) {
  id_src->width = 1;
  decode_op_rm(pc, id_dest, false, id_src, true);
}

make_DHelper(jcc) {
  // id_dest->width = 1;
  decode_op_SI(pc, id_dest, true);
  id_dest->val += *pc;
  decinfo.jmp_pc = id_dest->val;
}

make_DHelper(leave) {
  // printf("hit here leave\n");
}

make_DHelper(add) {
  // printf("hit here add\n");
}

make_DHelper(inc) {
  decode_op_r(pc, id_dest, true);
}

make_DHelper(cmp_83) {
  // printf("hit here cmp_83\n");
}

make_DHelper(push_imm8) {
  id_dest->width = 1;
  decode_op_SI(pc, id_dest, true);
}

make_DHelper(inc_rm) {
  // printf("hit here inc_rm\n");
}

make_DHelper(pop) {
  decode_op_r(pc, id_dest, true);
}

make_DHelper(nop) {
}

make_DHelper(add_03) {
  decode_op_rm(pc, id_src, true, id_dest, true);
}

make_DHelper(adc_13) {
  decode_op_rm(pc, id_src, true, id_dest, true);
}

make_DHelper(jmp_imm8) {
  id_dest->width = 1;
  decode_op_I(pc, id_dest, true);
  id_dest->val += *pc;
  decinfo.jmp_pc = id_dest->val;
  decinfo_set_jmp(true);
}