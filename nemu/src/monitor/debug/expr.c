#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_INT

  /* TODO: Add more token types */
  
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},         // equal
  {"\\-", '-'},         //sub
  {"\\*", '*'},         //mul
  {"\\/", '/'},         //div
  {"\\(", '('},         //
  {"\\)", ')'},         //
  {"[1-9]+[0-9]*|0", TK_INT}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
            case TK_NOTYPE: ;break;
            case '+': tokens[i].type = '+'; break;
            case '-': tokens[i].type = '+'; break;
            case '*': tokens[i].type = '*'; break;
            case '/': tokens[i].type = '/'; break;
            case ')': tokens[i].type = ')'; break;
            case '(': tokens[i].type = '('; break;
            case TK_INT: 
                      tokens[i].type = TK_INT; 
                      strcpy(tokens[i].str, rules[i].regex);
                      break;
            case TK_EQ: tokens[i].type = TK_EQ; break;
          default: TODO();
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

static bool check_parentheses(int p, int q) {
    int cnum=0;
    int flag=0;
    for(int i=p; i<=q; i++){
       switch (tokens[p].type) {
           case '('/* variable case */:
               cnum++;
               flag=1;
               break;
           case ')':
               cnum--;
               break;
           default: ;
       }
       if (cnum<=0 && i!=q) {
           return false;
       }
    }
    if (cnum==0 && flag==1) {
        return true;
    }
    else return false;
}
int eval(int p, int q){
    if (p>q) {
        /*bad expression*/
    }
    else if(p==q) {
        return atoi(tokens[p].str);
    }
    else if(check_parentheses(p, q)==true){
        return eval(p+1,q-1);
    }
    else {
        
    }
}
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
