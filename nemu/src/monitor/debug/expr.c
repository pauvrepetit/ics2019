#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

#include <stdlib.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NUM, TK_HEX_NUM, TK_NEQ, TK_AND, TK_REG

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
  {"==", TK_EQ},        // equal
  {"-", '-'},
  {"\\*", '*'},
  {"/", '/'},
  {"\\(", '('},
  {"\\)", ')'},
  {"0x[0-9a-fA-F]+", TK_HEX_NUM},
  {"[0-9]+", TK_NUM},
  {"!=", TK_NEQ},
  {"&&", TK_AND},
  {"\\$...", TK_REG}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

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
  int loc;
} Token;

#define TOKENS_LIMIT 64
// const static int TOKENS_LIMIT = 64;
static Token tokens[TOKENS_LIMIT] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    if (nr_token == TOKENS_LIMIT) {
      printf("Error, too many tokens in the expr\n");
      return false;
    }
    /* Try all rules one by one. */
    /* 当字符串e还没有到终止符的时候，我们逐个尝试所有的正则表达式模式，当匹配成功时，我们保存该token，将e指针移动到下一个待匹配的位置 */
    for (i = 0; i < NR_REGEX; i ++) {
      // printf("try rule %d on \"%s\"\n", i, e+position);
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
          case TK_NUM:
            tokens[nr_token].type = TK_NUM;
            memset(tokens[nr_token].str, 0, sizeof(char) * 32);
            memcpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].loc = position - substr_len;
            nr_token++;
            break;
          case TK_HEX_NUM:
            tokens[nr_token].type = TK_HEX_NUM;
            memset(tokens[nr_token].str, 0, sizeof(char) * 32);
            memcpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].loc = position - substr_len;
            nr_token++;
            break;
          case TK_NOTYPE:
            break;
          case TK_REG:
            if (substr_len != 4) {
              position -= substr_len;
              printf("no such register at position %d\n%s\n%*.s^\n", position, e, position, "");
              return false;
            } else {
              tokens[nr_token].type = TK_REG;
              memset(tokens[nr_token].str, 0, sizeof(char) * 32);
              memcpy(tokens[nr_token].str, substr_start, substr_len);
              tokens[nr_token].loc = position - substr_len;
              nr_token++;
            }
            break;
          default:
            tokens[nr_token].type = rules[i].token_type;
            memset(tokens[nr_token].str, 0, sizeof(char) * 32);
            tokens[nr_token].loc = position - substr_len;
            nr_token++;
            break;
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

typedef struct numToken {
  int type;
  int num;
  int loc;
} numToken;

uint32_t getNum(numToken *e, int len, bool *success, int *offset, char *e_str) {
  if (len == 0) {
    *success = false;
    printf("need a number at position %d\n%s\n%*.s^\n", (e-1)->loc, e_str, (e-1)->loc, "");
    return 0;
  }
  (*offset)++;
  if (e->type == TK_NUM) {
    return e->num;
  } else if (e->type == '+') {
    return getNum(e+1, len-1, success, offset, e_str);
  } else if (e->type == '-') {
    return -getNum(e+1, len-1, success, offset, e_str);
  } else if (e->type == '*') {
    uint32_t addr = getNum(e+1, len-1, success, offset, e_str);
    return *(uint32_t*)(pmem+addr);
  } else {
    *success = false;
    printf("unexcepted symbol at position %d\n%s\n%*.s^\n", e->loc, e_str, e->loc, "");
    return 0;
  }
}

uint32_t getOp(numToken *e, int len, bool *success, int *offset, char *e_str) {
  if (e->type == TK_NUM) {
    *success = false;
    printf("unexcepted number at position %d\n%s\n%*.s^\n", e->loc, e_str, e->loc, "");
    return 0;
  } else {
    *offset = 1;
    return e->type;
  }
}

uint32_t basic_cal_expr(numToken *e, int len, bool *success, char *e_str) {
  numToken *tokenStack = (numToken *)malloc(sizeof(numToken) * len);
  int top = 0;

  // int negFlag = 1;
  // int refCount = 0;
  // bool calFlag = true;

  *success = true;

  bool odd = true;
  while(len != 0) {
    int offset = 0;
    if (odd) {
      tokenStack[top].num = getNum(e, len, success, &offset, e_str);
      tokenStack[top].type = TK_NUM;
      tokenStack[top].loc = e->loc;
      top++;
      // 当进来一个数字的时候，我们将乘除法处理掉
      if (top > 1) {
        if (tokenStack[top-2].type == '*') {
          tokenStack[top-3].num = tokenStack[top-3].num * tokenStack[top-1].num;
          top -= 2;
        } else if (tokenStack[top-2].type == '/') {
          tokenStack[top-3].num = tokenStack[top-3].num / tokenStack[top-1].num;
          top -= 2;
        }
      }
    } else {
      tokenStack[top].type = getOp(e, len, success, &offset, e_str);
      tokenStack[top].loc = e->loc;
      top++;
    }
    if (!*success) {
      free(tokenStack);
      return 0;
    }
    e += offset;
    len -= offset;
    odd = !odd;
  }

  if (odd) {
    *success = false;
    printf("unexcepted operator at position %d\n%s\n%*.s^\n", (e-1)->loc, e_str, (e-1)->loc, "");
    free(tokenStack);
    return 0;
  }

  // 至此，乘除法计算完成，下面计算加减法
  int newTop = 0;
  int result = tokenStack[0].num;
  for(int i = 1; i < top; i += 2) {
    numToken t = tokenStack[i];
    if (t.type == '+') {
      result += tokenStack[i+1].num;
    } else if (t.type == '-') {
      result -= tokenStack[i+1].num;
    } else {
      // 不是加减法，==/!=/&&
      tokenStack[newTop].num = result;
      tokenStack[newTop].type = TK_NUM;
      tokenStack[newTop].loc = t.loc-1;
      newTop++;
      tokenStack[newTop].type = t.type;
      tokenStack[newTop].loc = t.loc;
      newTop++;
      result = tokenStack[i+1].num;
    }
  }
  tokenStack[newTop].num = result;
  tokenStack[newTop].type = TK_NUM;
  tokenStack[newTop].loc = tokenStack[top-1].loc;
  newTop++;
  // 至此，加减法计算完成，余下部分都是逻辑运算 或者 就是计算结果
  result = tokenStack[0].num;
  for(int i = 1; i < newTop; i += 2) {
    numToken t = tokenStack[i];
    switch (t.type) {
    case TK_EQ:
      result = result == tokenStack[i+1].num;
      break;
    case TK_NEQ:
      result = result != tokenStack[i+1].num;
      break;
    case TK_AND:
      result = result && tokenStack[i+1].num;
      break;
    default:
      printf("unexcepted num at position %d\n%s\n%*.s^\n", t.loc, e_str, t.loc, "");
      break;
    }
  }
  
  free(tokenStack);
  return result;





  // for(int i = 0; i < len; i++) {
  //   numToken t = e[i];
  //   if (t.type == TK_NUM) {
  //     if (calFlag == false) {
  //       *success = false;
  //       printf("need a math operator at position %d\n%s\n%*.s^\n", t.loc, e_str, t.loc, "");
  //       break;
  //     } else if (refCount != 0) {
  //       // 对这个数字，其前面存在refCount次解引用操作，我们执行这些操作得到最终的操作数

  //     }
  //     tokenStack[top].type = TK_NUM;
  //     tokenStack[top].num = negFlag * t.num;
  //     tokenStack[top].loc = t.loc;
  //     top++;
  //     calFlag = false;
  //     negFlag = 1;
  //     // 这里加入了一个数到栈中，我们考虑进行一些操作，如果说这个数的前一个位置是一个乘号/除号，那么就在这进行一次计算，其他的运算的话就不管
  //     // 最终处理完成以后，栈中就是加减法/==/!=/&&
  //     if (top == 1) {
  //       // 该数是栈中的第一个元素，不处理
  //       continue;
  //     } else {
  //       if (tokenStack[top-2].type == '*') {
  //         tokenStack[top-3].num = tokenStack[top-3].num * tokenStack[top-1].num;
  //         top -= 2;
  //       } else if (tokenStack[top-2].type == '/') {
  //         tokenStack[top-3].num = tokenStack[top-3].num / tokenStack[top-1].num;
  //         top -= 2;
  //       } else {
  //         continue;
  //       }
  //     }
  //   } else if (t.type == '+') {
  //     if (calFlag) {
  //       continue;
  //     } else {
  //       tokenStack[top].type = '+';
  //       tokenStack[top++].loc = t.loc;
  //       calFlag = true;
  //     }
  //   } else if (t.type == '-') {
  //     if (calFlag) {
  //       negFlag = -negFlag;
  //       continue;
  //     } else {
  //       tokenStack[top].type = '-';
  //       tokenStack[top++].loc = t.loc;
  //       calFlag = true;
  //     }
  //   } else if (t.type == '*') {
  //     if (calFlag) {
  //       // * 作为解引用运算符
  //       refCount++;
  //       continue;
  //     } else {
  //       // * 作为乘法运算符
  //       tokenStack[top].type = t.type;
  //       tokenStack[top++].loc = t.loc;
  //       calFlag = true;
  //     }
  //   } else if (t.type == '/') {
  //     if (calFlag) {
  //       *success = false;
  //       printf("unexpected %c at position %d\n%s\n%*.s^\n", t.type, t.loc, e_str, t.loc, "");
  //       break;
  //     } else {
  //       tokenStack[top].type = t.type;
  //       tokenStack[top++].loc = t.loc;
  //       calFlag = true;
  //     }
  //   } else {
  //     // todo: 此处后续可以增加更多的对type的处理
  //   }
  // }

  // if (top == 0) {
  //   *success = false;
  //   printf("unexpected %c at position %d\n%s\n%*.s^\n", e[0].type, e[0].loc, e_str, e[0].loc, "");
  // }

  // if (!*success) {
  //   free(tokenStack);
  //   return 0;
  // }

  // int result = tokenStack[0].num;
  // for(int i = 1; i < top; i += 2) {
  //   numToken t = tokenStack[i];
  //   if (t.type == '+') {
  //     result += tokenStack[i+1].num;
  //   } else if (t.type == '-') {
  //     result -= tokenStack[i+1].num;
  //   }
  // }
  // free(tokenStack);
  // return result;
}


uint32_t cal_expr(int l, int r, bool *success, char *e) {
  numToken tokenStack[TOKENS_LIMIT];
  int top = 0;
  for(int i = l; i < r; i++) {
    Token newToken = tokens[i];
    if (newToken.type == '(') {
      int start = i + 1;
      int end = start;
      int countLeft = 0;
      while (true) {
        if (end == r) {
          // 括号不匹配
          *success = false;
          printf("no matching right bracket at position %d\n%s\n%*.s^\n", newToken.loc, e, newToken.loc, "");
          return 0;
        }
        // 找到匹配的右括号
        if (tokens[end].type == '(') {
          countLeft++;
        } else if (tokens[end].type == ')') {
          if (countLeft == 0) {
            break;
          } else {
            countLeft--;
          }
        }
        end++;
      }

      if (start == end) {
        // 不允许空括号
        *success = false;
        printf("NULL brackets is not allowed at position %d\n%s\n%*.s^\n", tokens[end-1].loc, e, tokens[end-1].loc, "");
        return 0;
      }
      bool s;
      int result = cal_expr(start, end, &s, e);
      if (!s) {
        *success = false;
        return 0;
      } else {
        tokenStack[top].type = TK_NUM;
        tokenStack[top].num = result;
        tokenStack[top].loc = tokens[start-1].loc;
        top++;
        i = end;
      }

    } else if(newToken.type == ')') {
      *success = false;
      printf("unexpected right bracket at position %d\n%s\n%*.s^\n", newToken.loc, e, newToken.loc, "");
      return 0;
    } else {
      tokenStack[top].type = newToken.type;
      tokenStack[top].loc = newToken.loc;
      if (newToken.type == TK_NUM) {
        tokenStack[top].num = atoi(newToken.str);
      } else if (newToken.type == TK_HEX_NUM) {
        tokenStack[top].type = TK_NUM;
        tokenStack[top].num = strtol(newToken.str, NULL, 16);
      } else if (newToken.type == TK_REG) {
        // 在这里读出寄存器的内容，将TK_REG直接转换成TK_NUM
        tokenStack[top].type = TK_NUM;
        if (strcmp(newToken.str, "$EAX") == 0) {
          tokenStack[top].num = cpu.eax;
        } else if (strcmp(newToken.str, "$EBX") == 0) {
          tokenStack[top].num = cpu.ebx;
        } else if (strcmp(newToken.str, "$ECX") == 0) {
          tokenStack[top].num = cpu.ecx;
        } else if (strcmp(newToken.str, "$EDX") == 0) {
          tokenStack[top].num = cpu.edx;
        } else if (strcmp(newToken.str, "$ESI") == 0) {
          tokenStack[top].num = cpu.esi;
        } else if (strcmp(newToken.str, "$EDI") == 0) {
          tokenStack[top].num = cpu.edi;
        } else if (strcmp(newToken.str, "$ESP") == 0) {
          tokenStack[top].num = cpu.esp;
        } else if (strcmp(newToken.str, "$EBP") == 0) {
          tokenStack[top].num = cpu.ebp;
        } else if (strcmp(newToken.str, "$EPC") == 0) {
          tokenStack[top].num = cpu.pc;
        } else {
          *success = false;
          printf("unknown register at position %d\n%s\n%*.s^\n", newToken.loc, e, newToken.loc, "");
          return 0;
        }
      }
      top++;
    }
  }

  // 至此，tokenStack中仅包含加减乘除和数字，还有==/!=/&&，去除了所有的括号
  return basic_cal_expr(tokenStack, top, success, e);
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  /* 在tokens数组中保存了表达式中所有的token，下面通过处理这些token进行计算 */
  // TODO();
  return cal_expr(0, nr_token, success, e);
}
