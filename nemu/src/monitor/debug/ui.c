#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
// 这里需要添加函数声明来避免编译过程中出现的warning
vaddr_t exec_once(void);
void isa_reg_display();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

/* 单步执行，参数中可能存在一个数字，表示执行的步数 */
static int cmd_si(char *args) {
  int steps;
  if (args == NULL) {
    exec_once();
  } else {
    steps = atoi(args);
    for (int i = 0; i < steps; i++) {
      exec_once();
    }
  }
  bool s;
  printf("$EPC: %08x\n", isa_reg_str2val("$EPC", &s));
  return 0;
}

/* 打印程序信息，参数中必定存在一个字母，r or w，分别表示寄存器信息和监视点信息 */
static int cmd_info(char *args) {
  if(args == NULL) {
    printf("Error: info need an argument, r or w\n");
  } else if (args[0] == 'r') {
    // 打印寄存器信息
    isa_reg_display();
  } else if (args[0] == 'w') {
    // 打印监视点信息
    showWatchpoint();
  } else {
    printf("Error: info's argument can only be r or w\n");
  }
  return 0;
}

static int cmd_x(char *args) {
  int n, addr;
  sscanf(args, "%d%x", &n, &addr);
  for(int i = 0; i < n; i++) {
    if (i % 4 == 0) {
      printf("0x%08x:\t", addr+4*i);
    }
    if (i % 4 != 3) {
      printf("%02x%02x%02x%02x\t", pmem[addr+4*i], pmem[addr+1+4*i], pmem[addr+2+4*i], pmem[addr+3+4*i]);
    } else {
      printf("%02x%02x%02x%02x\n", pmem[addr+4*i], pmem[addr+1+4*i], pmem[addr+2+4*i], pmem[addr+3+4*i]);
    }
  }
  putchar('\n');
  return 0;
}

static int cmd_p(char *args) {
  bool success;
  int res = expr(args, &success);
  if (success) {
    printf("%d\n", res);
  } else {
    printf("expr error\n");
  }
  return 0;
}

static int cmd_w(char *args) {
  bool success;
  uint32_t res = expr(args, &success);
  if (success) {
    WP *w = new_wp();
    // w->expr = args;
    strcpy(w->expr, args);
    w->exprValue = res;
    printf("watchpoint %d: %s\n", w->NO, args);
  } else {
    printf("expr error, watchpoint not added\n");
  }
  return 0;
}

static int cmd_d(char *args) {
  int wp_no = atoi(args);
  if (args[0] != '0' && wp_no == '0') {
    printf("argument error\n");
  } else {
    free_wp(wp_no);
  }
  return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "Step N instructions, default one instruction", cmd_si },
  { "info", "Print program status. 'r' for register, 'w' for watchpoint", cmd_info },
  { "x", "Scan memory. Use 'x N EXPR' to print 4*N bytes from addr EXPR", cmd_x },
  { "p", "Calculate expr.", cmd_p},
  { "w", "Add a watchpoint.", cmd_w},
  { "d", "Delete a watchpoint.", cmd_d},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
