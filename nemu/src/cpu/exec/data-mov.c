#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
//  operand_write(id_dest, &id_src->val);
  rtl_push(&id_dest->val);
//  printf("id_srcval: 0x%x\n", id_src->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&t2);
  operand_write(id_dest, &t2);

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
//  TODO();
  cpu.esp=cpu.ebp;
  rtl_pop(&cpu.ebp);
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
//    TODO();
    if (cpu.eax & 0x8000) {
        cpu.edx = cpu.edx | 0xffff;
    }
    else {
        cpu.edx = cpu.edx & 0xffff0000;
    }
  }
  else {
//    TODO();
    if(cpu.eax & 0x80000000){
        cpu.edx=0xffffffff;
    }
    else {
        cpu.edx=0;
    }
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
//    TODO();
    if (cpu.eax & 0x80) {
        cpu.eax |= 0xff00;
    }
    else {
        cpu.eax &= 0xffff00ff;
    }
  }
  else {
//    TODO();
    if(cpu.eax & 0x8000) {
        cpu.eax |= 0xffff0000;
    }
    else {
        cpu.eax &= 0xffff;
    }
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
//  printf("src=0x%x,dest=0x%x,t2=0x%x\n", id_src->val, id_dest->val,t2);
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
//  printf("src=0x%x,dest=0x%x,t2=0x%x\n", id_src->val, id_dest->val,t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
