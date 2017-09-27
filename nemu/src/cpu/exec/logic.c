#include "cpu/exec.h"

make_EHelper(test) {
//  TODO();
//  printf("src=0x%x,dest=0x%x\n", id_src->val, id_dest->val);
  rtl_and(&t2, &id_src->val, &id_dest->val);
  cpu.EFLAGS.CF=0;
  cpu.EFLAGS.OF=0;
  rtl_update_ZFSF(&t2, id_dest->width);
  print_asm_template2(test);
}

make_EHelper(and) {
//  TODO();
  rtl_and(&t2, &id_src->val, &id_dest->val);
  operand_write(id_dest, &t2);
//  printf("src:%x,%d,%d\n", id_src->val,id_src->val,id_src->width);
//  printf("dest:%x,%d,%d\n", id_dest->val,id_dest->val,id_dest->width);

  cpu.EFLAGS.CF=0;
  cpu.EFLAGS.OF=0;
  rtl_update_ZFSF(&t2, id_dest->width);

  print_asm_template2(and);
}

make_EHelper(xor) {
//  TODO();
  rtl_xor(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);

  cpu.EFLAGS.CF=0;
  cpu.EFLAGS.OF=0;
  rtl_update_ZFSF(&t2, id_dest->width);


  print_asm_template2(xor);
}

make_EHelper(or) {
//  TODO();
  rtl_or(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  cpu.EFLAGS.CF=0;
  cpu.EFLAGS.OF=0;
  rtl_update_ZFSF(&t2, id_dest->width);

  print_asm_template2(or);
}

make_EHelper(sar) {
//  TODO();
//  t2=id_dest->val;
//  t1=id_src->val;
//  printf("src:%x, dest:%d, dest-width:%d\n", id_src->val, id_dest->val, id_dest->width);
/*  while(t1!=0){
      t2=t2>>1;
      t2=t2|(id_dest->val & 0x80000000);
      t1--;
  }
  */
  if(id_dest->width==2 && ((id_dest->val)&0x8000)){
      id_dest->val |= 0xffff00;
  }
  rtl_sar(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  rtl_update_ZFSF(&t2, id_dest->val);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
//  TODO();
  
/*  t2=id_dest->val;
  t1=id_src->val;
  while(t1!=0){
      t2=t2*2;
      t1--;
  }
 */
  rtl_shl(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  rtl_update_ZFSF(&t2, id_dest->val);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
//  TODO();
  // unnecessary to update CF and OF in NEMU

/*  t2=id_dest->val;
  t1=id_src->val;
  while(t1!=0){
      t2=t2/2;
      t1--;
  }
  */
  rtl_shr(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  rtl_update_ZFSF(&t2, id_dest->val);
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
//  TODO();
  t2=id_dest->val;
  rtl_not(&t2);
  operand_write(id_dest, &t2);

  print_asm_template1(not);
}

