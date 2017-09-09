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
#define BAD_EXP 0x80000000

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
        
        if(rules[i].token_type!=TK_NOTYPE){


            switch (rules[i].token_type) {
                case '+': tokens[nr_token].type = '+'; break;
                case '-': tokens[nr_token].type = '-'; break;
                case '*': tokens[nr_token].type = '*'; break;
                case '/': tokens[nr_token].type = '/'; break;
                case ')': tokens[nr_token].type = ')'; break;
                case '(': tokens[nr_token].type = '('; break;
                case TK_INT: 
                          tokens[nr_token].type = TK_INT; 
                          strncpy(tokens[nr_token].str, substr_start,substr_len);
                          tokens[nr_token].str[substr_len]='\0';
                          break;
                case TK_EQ: tokens[nr_token].type = TK_EQ; break;
              default: TODO();
            }
//            printf("nr_token = %d\n",nr_token);
            nr_token++;
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

bool check_parentheses(int p, int q) {
    if (tokens[p].type!='(' || tokens[q].type!=')') {
        return false;
    }
    int cnum=0;
    int flag=0;
    for(int i=p; i<=q; i++){
       switch (tokens[i].type) {
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
//    printf("cnum=%d, flag=%d\n",cnum, flag);
    if (cnum==0 && flag==1) {
        return true;
    }
    else return false;
}

bool isOP(int index) {
    if (tokens[index].type=='+' || tokens[index].type=='-' || tokens[index].type=='*' || tokens[index].type=='/') {
        return true;
    }
    else {
        return false;
    }
}

bool inBK(int index, int p,  int q) {
    int numL=0,numR=0;
    for (int i = index+1; i <= q; ++i) {
        if (tokens[i].type=='(') {
            numL++;
        }
        else if (tokens[i].type==')') {
            numR++;
        }
    }
    if (numL!=numR) {
        return true;
    }
    numL=0;
    numR=0;
    for (int i=index-1; i>=p; --i) {
        if (tokens[i].type=='(') {
            numL++;
        } else if(tokens[i].type==')') {
            numR++;
        }
    }
    if (numL!=numR) {
        return true;
    }
    return false;
}

int priority(int pos) {
    if (tokens[pos].type=='+' || tokens[pos].type=='-') {
        return 1;
    }
    else if (tokens[pos].type=='*' || tokens[pos].type=='/') {
        return 2;
    }
    else {
        return 2;
    }
}

int findOP(int p, int q){
    int i=0;
    int roots[q-p];
    int pos=0;
//    printf("operaters: ");
    for (i = p; i <= q; ++i) {
        if (isOP(i) && !inBK(i, p, q) ) {
            roots[pos]=i;
//            printf("%d,%c\t",i,tokens[i].type);
            pos++;
        }
    }
    Log("\n");
    int min=0;
    for (i = 0; i < pos; ++i) {
        if (priority(roots[i])<=priority(roots[min])) {
//            printf("a miner op is %c\n",tokens[roots[i]].type);
            min=i;
        }
    }
    return roots[min];
}

int eval(int p, int q){
    printf("p = %d, q = %d\n",p, q);
    if (p>q) {
        /*bad expression*/
        printf("bad expression\n");
        return BAD_EXP;
    }
    else if(p==q) {
//        printf("%s\n",tokens[p].str);
        return atoi(tokens[p].str);
    }
    else if(check_parentheses(p, q)==true){
//        printf("p~q in bk\n");
        return eval(p+1,q-1);
    }
    else {
        int op = findOP(p, q);
        Log("root op is %dth", op);
        int val1 = eval(p, op-1);
        int val2 = eval(op+1, q);

        switch (tokens[op].type) {
            case '+':
                return val1 + val2;
                break;
            case '-':
                return val1 - val2;
                break;
            case '*':
                return val1*val2;
                break;
            case '/':
                return val1/val2;
                break;
            default:assert(0);
                
        }
    }
    return 0;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    printf("make_token error\n");
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
 // TODO();
 //
  return eval(0,nr_token-1);
//  return 0;
}
