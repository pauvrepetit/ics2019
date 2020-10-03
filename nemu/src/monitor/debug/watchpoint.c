#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp() {
  if (free_ == NULL) {
    assert(0);
  } else {
    WP *t = free_->next;
    free_->next = head;
    head = free_;
    free_ = t;
    return head;
  }
}

void free_wp(int wp_no) {
  WP *wp = head;
  if (head->NO == wp_no) {
    head = wp->next;
    wp->next = free_;
    free_ = wp;
  } else {
    WP *t = head;
    while(t->next->NO != wp_no && t->next != NULL) {
      t = t->next;
    }
    wp = t->next;
    if (wp == NULL) {
      return;
    } else {
      t->next = wp->next;
      wp->next = free_;
      free_ = wp;
    }
  }
}

void showWatchpoint(void) {
  WP *t = head;
  if (t == NULL) {
    printf("No watchpoint\n");
  }
  while (t != NULL) {
    printf("watchpoint %d: %s\n", t->NO, t->expr);
    t = t->next;
  }
}

WP* checkWatchpoint(void) {
  WP *t = head;
  uint32_t value;
  while (t != NULL) {
    bool success;
    value = expr(t->expr, &success);
    if (value != t->exprValue) {
      t->exprValue = value;
      return t;
    }
    t = t->next;
  }
  return NULL;
}