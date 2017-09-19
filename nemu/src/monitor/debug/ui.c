#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include "cpu/exec.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void setWP(char *args);
void showWP();
void delWP(int no);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
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
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_p(char *args);
static int cmd_x(char *args);
static int cmd_w(char *args);
static int cmd_d(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Single step execution", cmd_si},
  { "info", "Display program status, arg 'r' for regs, arg 'w' for MonitorPoint", cmd_info},
  { "p", "Expression evaluates", cmd_p},
  { "x", "Scan memery", cmd_x},
  { "w", "Set monitor point", cmd_w},
  { "d", "Delete monitor point", cmd_d},

  /* TODO: Add more commands */

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
static int cmd_si(char *args){
   char *arg = strtok(NULL, " ");
   int num = 0;
   if(NULL==arg) {
       num = 1;
   }
   else {
       num = atoi(arg);
   }
   cpu_exec(num);
   return 0;
}

static int cmd_info(char *args){
    char *arg = strtok(NULL," ");
    if(NULL==arg) {
        printf("Bad command, argument 'r' or 'w' expected\n");
    }
    else {
        char tp = arg[0];
        if(tp=='r') {
            printf("eax = %x\n", cpu.eax);
            printf("ecx = %x\n", cpu.ecx);
            printf("edx = %x\n", cpu.edx);
            printf("ebx = %x\n", cpu.ebx);
            printf("esp = %x\n", cpu.esp);
            printf("ebp = %x\n", cpu.ebp);
            printf("esi = %x\n", cpu.esi);
            printf("edi = %x\n", cpu.edi);
            printf("eip = %x\n", cpu.eip);
            printf("ZF=%d,OF=%d,CF=%d,SF=%d\n",cpu.EFLAGS.ZF,cpu.EFLAGS.OF,cpu.EFLAGS.CF,cpu.EFLAGS.SF);
        }
        else if(tp=='w'){
            showWP();
        }
        else {
           printf("Bad command, argument 'r' or 'w' expected\n");
        }
    }
    return 0;
}

static int cmd_p(char *args){
    bool success=true;
    int result=0;
    result=expr(args, &success);
    if(success && result!=0x80000000){
        printf("%d\n",result);
    }
    else {
        printf("uncorrect input!\n");
    }
    return 0;
}

static int cmd_x(char *args){
    char *arg0 = strtok(NULL, " ");
    char *arg1 = strtok(NULL, " ");
//    char *stop;
    int cnt = atoi(arg0);
    bool flag =true;
    int addr = expr(arg1, &flag);
    if (!flag) {
        printf("incorrect expression.\n");
        return 0;
    }
    uint32_t value;
    int i=0;
    printf("  addr\t  value\t\n");
    while(i<cnt) {
        value = vaddr_read(addr, 4);
        printf("0x%x: 0x%08x\n", addr, value);
        i++;
        addr+=4;
    }

    return 0;
}
static int cmd_w(char *args){
    setWP(args);
    return 0;
}
static int cmd_d(char *args){
    char *arg = strtok(NULL, " ");
    if (arg==NULL) {
        printf("Bad command, an interger argument expected.\n");
        return 0;
    }
    int no = atoi(arg);
    delWP(no);
    return 0;
}
void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
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
