#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char *expr;
  uint32_t exprValue;

} WP;

WP* new_wp();
void free_wp(int wp_no);

#endif
