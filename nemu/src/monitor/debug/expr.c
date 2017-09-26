#include "nemu.h"
#include <stdlib.h>
#include <ctype.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

uint32_t eval(int p, int q);
int dominant_operator(int p, int q);
bool check_parentheses(int p, int q);
uint32_t get_reg_val(char* reg);
enum {
  NOTYPE = 256, EQ, NUM, HNUM, REGISTER,AND,OR,DEREF,NEG,NEQ,LE,BE
      //less or equal ; big or equal
  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
  int pri;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {"0[xX][0-9]+", HNUM, 0},
  {"[0-9]+", NUM, 0},
  {"\\$[A-Za-z]+", REGISTER, 0},
  {" +", NOTYPE, 0},    // spaces
  {"\\+", '+', 4},         // plus
  {"-", '-', 4},
  {"\\*", '*', 3},//pri of mul=3 pri of deref=2
  {"\\/", '/', 3},
  {"\\(", '(', 1},
  {"\\)", ')', 1},
  {"==", EQ, 7},         // equal
  {"!=", NEQ, 7},
  {"!", '!', 2},
  {"&&", AND, 8},
  {"\\|\\|", OR, 9},
  {">=", BE,6},
  {"<=", LE, 6},
  {"<", '<', 6},
  {">", '>',6}
};//'+'mean one or more so need to transfer

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
  int pri;
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

        //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
         //   i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

    switch (rules[i].token_type) {
  	    case NOTYPE:{break;}
        case REGISTER:{
           
           //transfer register into value and set type to number
           char tok[20];
           strncpy(tok, substr_start+1, substr_len-1);
           tok[substr_len-1]='\0';
         
	   //upper character to lower
	  int j = 0; 
	 //printf("reg before:%s\n", tok);
          while(tok[j])
	 {
	    tok[j] = tolower(tok[j]);
	    j++;
	 }
         //printf("reg after: %s\n", tok);
   
	   uint32_t value = get_reg_val(tok);
           
           tokens[nr_token].type =NUM;
           tokens[nr_token].pri = 0;
           sprintf(tokens[nr_token].str, "%d", value);
           //printf("value: %d, string:%s\n", value, tokens[nr_token].str);
           nr_token++;
           break;
        }
  	    default: {
    		tokens[nr_token].type =rules[i].token_type;
    		strncpy(tokens[nr_token].str, substr_start, substr_len);
    		tokens[nr_token].str[substr_len]='\0';
                tokens[nr_token].pri = rules[i].pri;
    		nr_token++;
            break;
	 	      }
        }
        break;
      }
    } 
    if  (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }

} 

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  // TODO();
  *success = true;

  //to distinct mul and deref
  int i;
  for(i=0; i<nr_token; i++)
  {
    if(tokens[i].type=='*' && (i==0||tokens[i-1].type=='('||tokens[i-1].type=='+'
        ||tokens[i-1].type=='-'||tokens[i-1].type=='*'||tokens[i-1].type=='\\'))
        {
            tokens[i].type = DEREF;
            tokens[i].pri = 2;
         }
    else if(tokens[i].type=='*' && (i==0||tokens[i-1].type=='('||tokens[i-1].type=='+'
		||tokens[i-1].type=='-'))
    { 
        tokens[i].type = NEG;
        tokens[i].pri = 2;
    }
    } 
    return eval(0, nr_token-1);
 // return 0;
}

uint32_t eval(int p, int q)
{
    //printf("nr_token = %d, p = %d, q= %d\n", nr_token, p, q);
    if(p>q)
    {
	printf("p>q\n");
	assert(0);
    } 
    else if(p == q){
//	printf("str:%s\n", tokens[p].str);
	if(tokens[p].type==NUM){
//	    printf("num:%d\n", atoi(tokens[p].str));
	    return atoi(tokens[p].str);
	    
	}
	else if(tokens[p].type ==HNUM)
	{
	    int n;
	   // printf("%s\n", tokens[p].str+2);
	    sscanf(tokens[p].str+2, "%x", &n);
//	    printf("hnum: %d\n",n);
	    return n;
	}
    }
    else if(check_parentheses(p, q)==true){
	return eval(p+1, q-1);
    }
    else{
	int op = dominant_operator(p, q);
   // printf("dominant_operator:%d %c\n",op, tokens[op].type);
	uint32_t val1 = (op == p)?0:eval(p, op-1);//eg.!0 op is the first operator
	uint32_t val2 = eval(op+1, q);
   // printf("dominant_operator:%c, val1:%d, val2:%d\n", tokens[op].type,val1,val2);
	
	switch(tokens[op].type){
	    case '+': return val1+val2;
	    case '-': return val1-val2;
	    case '*': return val1*val2;
	    case '/': return val1/val2;
            case AND: return val1 && val2;
	    case OR: return val1||val2;
	    case EQ: return val1==val2;
	    case NEQ: return val1!=val2;
	    case DEREF: return vaddr_read(val2,4);
	    case '!': return !val2;
	    case NEG: return -val2;
	    case '>': return val1>val2;
	    case '<': return val1 <val2;
	    case LE: return val1<=val2;
	    case BE: return val1>=val2;
	    default:return -1;//assert(0);
	}
    }
    return -1;
}

bool check_parentheses(int p, int q)
{
    if(tokens[p].type == '(' && tokens[q].type==')'){
	int lc = 0, rc = 0;
	int i;
	for(i = p+1; i<q; i++)
	{
	    if(tokens[i].type =='(')	lc++;
	    if(tokens[i].type == ')')	rc++;
	    if(rc>lc)	return false;
	}
	if(rc == lc)	return true;
    }
    return false;
}

int dominant_operator(int p, int q)
{
  int i = 0;
  int lc = 0;
  int min_pri = 0;
  int op = p;
  for(i = p; i<q; i++)
  {
      int t = tokens[i].type;
      if(t == NUM || t == HNUM || t == REGISTER)
        continue;
      if(t == '(' || t == ')')
      {
        if(t == '(')  lc++;
        if(t == ')')  lc--;
      }
      //dominant operator priority be minist
      else if(lc == 0 && tokens[i].pri>min_pri)
      {
        op = i;
        min_pri = tokens[i].pri;
      }
  }
  return op;
}
uint32_t get_reg_val(char* reg)
{
  int i = 0;
   for(i = 0; i<8;i++)
  {
    if(strcmp(reg, regsl[i]) == 0)
      return reg_l(i);
    else if(strcmp(reg, regsw[i]) == 0)
      return reg_w(i); 
    else if(strcmp(reg, regsb[i]) == 0)
      return reg_b(i);
    else if(strcmp(reg, "eip") == 0)
      return cpu.eip;
  }
  return 0;
}

