#include "cpu/exec.h"

make_EHelper(test) {
  TODO();

  print_asm_template2(test);
}

make_EHelper(and) {
//  TODO();
  rtl_and(&t2, &id_src->val, &id_dest->val);
  operand_write(id_dest, &t2);
  printf("src:%x,%d\n", id_src->val,id_src->width);

  cpu.EFLAGS.CF=0;
  cpu.EFLAGS.OF=0;
  rtl_update_ZFSF(&t2, id_dest->width);

  print_asm_template2(and);
}

make_EHelper(xor) {
//  TODO();2
  rtl_xor(&t2, &id_src->val, &id_dest->val);
  operand_write(id_dest, &t2);

  cpu.EFLAGS.CF=0;
  cpu.EFLAGS.OF=0;
  rtl_update_ZFSF(&t2, id_dest->width);


  print_asm_template2(xor);
}

make_EHelper(or) {
  TODO();

  print_asm_template2(or);
}

make_EHelper(sar) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  TODO();

  print_asm_template1(not);
}
