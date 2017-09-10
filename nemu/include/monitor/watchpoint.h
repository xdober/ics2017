#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

#define EXP_LENG 100

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  
  /* TODO: Add more members if necessary */
  char exp[EXP_LENG];
  int value;
} WP;

#endif
