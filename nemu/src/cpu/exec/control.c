#include "cpu/exec.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
  decoding.is_jmp = 1;

  print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  decoding.is_jmp = t2;

  print_asm("j%s %x", get_cc_name(subcode), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
  decoding.jmp_eip = id_dest->val;
  decoding.is_jmp = 1;

  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  // the target address is calculated at the decode stage
//  TODO();
  decoding.jmp_eip = decoding.seq_eip+id_dest->val;
  rtl_push(&decoding.seq_eip);
//  printf("call: %x\npushed: %x\n", decoding.jmp_eip, decoding.seq_eip);
  decoding.is_jmp = 1;
  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
//  TODO();
  rtl_pop(&decoding.jmp_eip);
//  printf("ret: %x\n", decoding.jmp_eip);
  decoding.is_jmp=1;
  
  print_asm("ret");
}

make_EHelper(call_rm) {
//  TODO();
  t2=vaddr_read(id_dest->val, 4);
  printf("t2:%x\n", t2);
  decoding.jmp_eip = decoding.seq_eip+t2;
  rtl_push(&decoding.seq_eip);

  decoding.is_jmp = 1;
  print_asm("call *%s", id_dest->str);
}
