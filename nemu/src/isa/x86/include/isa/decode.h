#ifndef __X86_DECODE_H__
#define __X86_DECODE_H__

#include "common.h"
#include "cpu/decode.h"

struct ISADecodeInfo {
  bool is_operand_size_16;
  uint8_t ext_opcode;
};

#define suffix_char(width) ((width) == 4 ? 'l' : ((width) == 1 ? 'b' : ((width) == 2 ? 'w' : '?')))

typedef union {
  struct {
    uint8_t R_M		:3;
    uint8_t reg		:3;
    uint8_t mod		:2;
  };
  struct {
    uint8_t dont_care	:3;
    uint8_t opcode		:3;
  };
  uint8_t val;
} ModR_M;

typedef union {
  struct {
    uint8_t base	:3;
    uint8_t index	:3;
    uint8_t ss		:2;
  };
  uint8_t val;
} SIB;

void load_addr(vaddr_t *, ModR_M *, Operand *);
void read_ModR_M(vaddr_t *, Operand *, bool, Operand *, bool);

make_DHelper(I2E);
make_DHelper(I2a);
make_DHelper(I2r);
make_DHelper(SI2E);
make_DHelper(SI_E2G);
make_DHelper(I_E2G);
make_DHelper(I_G2E);
make_DHelper(I);
make_DHelper(r);
make_DHelper(E);
make_DHelper(setcc_E);
make_DHelper(gp7_E);
make_DHelper(test_I);
make_DHelper(SI);
make_DHelper(G2E);
make_DHelper(E2G);

make_DHelper(mov_I2r);
make_DHelper(mov_I2E);
make_DHelper(mov_G2E);
make_DHelper(mov_E2G);
make_DHelper(lea_M2G);

make_DHelper(gp2_1_E);
make_DHelper(gp2_cl2E);
make_DHelper(gp2_Ib2E);

make_DHelper(Ib_G2E);
make_DHelper(cl_G2E);

make_DHelper(O2a);
make_DHelper(a2O);

make_DHelper(J);

make_DHelper(push_SI);

make_DHelper(in_I2a);
make_DHelper(in_dx2a);
make_DHelper(out_a2I);
make_DHelper(out_a2dx);

make_DHelper(call_rel32);
make_DHelper(push_r32);
make_DHelper(push_imm32);
make_DHelper(sub);
make_DHelper(xor);
make_DHelper(ret);
// make_DHelper(lea);
make_DHelper(and);
make_DHelper(push_m);
make_DHelper(add_01);
make_DHelper(cmp_3b);
// make_DHelper(sete);
make_DHelper(movzb);
make_DHelper(jcc);
make_DHelper(leave);
make_DHelper(add);
make_DHelper(inc);
make_DHelper(cmp_83);
make_DHelper(push_imm8);
make_DHelper(inc_rm);
make_DHelper(pop);
make_DHelper(nop);
make_DHelper(add_03);
make_DHelper(adc_13);
make_DHelper(or_09);
#endif
