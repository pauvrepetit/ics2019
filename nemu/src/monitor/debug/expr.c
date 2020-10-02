#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

#include <stdlib.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NUM

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
  {"[0-9]+", TK_NUM}
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

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    if (nr_token == 32) {
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
            if (substr_len > 32) {
              printf("number is too large\n");
              printf("%s\n", e);
              for(int i = 0; i < position - substr_len; i++) {
                putchar(' ');
              }
              putchar('^');
              putchar('\n');

              for(int i = 0; i < position - substr_len; i++) {
                putchar(' ');
              }
              putchar('|');
              putchar('\n');
              return false;
            }
            tokens[nr_token].type = TK_NUM;
            memcpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].loc = position - substr_len;
            nr_token++;
            break;
          case TK_NOTYPE:
            break;
          // default: TODO();
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

uint32_t basic_cal_expr(numToken *e, int len, bool *success, char *e_str) {
  numToken *tokenStack = (numToken *)malloc(sizeof(numToken) * len);
  int top = 0;

  int negFlag = 1;
  bool calFlag = true;

  *success = true;

  for(int i = 0; i < len; i++) {
    numToken t = e[i];
    if (t.type == TK_NUM) {
      if (calFlag == false) {
        *success = false;
        break;
      }
      tokenStack[top].type = TK_NUM;
      tokenStack[top].num = negFlag * t.num;
      top++;
      calFlag = false;
      negFlag = 1;
      // 这里加入了一个数到栈中，我们考虑进行一些操作，如果说这个数的前一个位置是一个乘号/除号，那么就在这进行一次计算，加减的话就不管
      // 最终处理完成以后，栈中就全部是加减法了
      if (top == 1) {
        // 该数是栈中的第一个元素，不处理
        continue;
      } else {
        if (tokenStack[top-2].type == '*') {
          tokenStack[top-3].num = tokenStack[top-3].num * tokenStack[top-1].num;
          top -= 2;
        } else if (tokenStack[top-2].type == '/') {
          tokenStack[top-3].num = tokenStack[top-3].num / tokenStack[top-1].num;
          top -= 2;
        } else {
          continue;
        }
      }
    } else if (t.type == '+') {
      if (calFlag) {
        continue;
      } else {
        tokenStack[top].type = '+';
        tokenStack[top++].loc = t.loc;
        calFlag = true;
      }
    } else if (t.type == '-') {
      if (calFlag) {
        negFlag = -negFlag;
        continue;
      } else {
        tokenStack[top].type = '-';
        tokenStack[top++].loc = t.loc;
        calFlag = true;
      }
    } else if (t.type == '*' || t.type == '/') {
      if (calFlag) {
        *success = false;
        printf("unexpected %c at position %d\n%s\n%*.s^\n", t.type, t.loc, e_str, t.loc, "");
        break;
      } else {
        tokenStack[top].type = t.type;
        tokenStack[top++].loc = t.loc;
        calFlag = true;
      }
    } else {
      // todo: 此处后续可以增加更多的对type的处理
    }
  }

  if (!*success) {
    free(tokenStack);
    return 0;
  }

  int result = tokenStack[0].num;
  for(int i = 1; i < top; i += 2) {
    numToken t = tokenStack[i];
    if (t.type == '+') {
      result += tokenStack[i+1].num;
    } else if (t.type == '-') {
      result -= tokenStack[i+1].num;
    }
  }
  free(tokenStack);
  return result;
}


uint32_t cal_expr(int l, int r, bool *success, char *e) {
  numToken tokenStack[32];
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

      bool s;
      int result = cal_expr(start, end, &s, e);
      if (!s) {
        *success = false;
        return 0;
      } else {
        tokenStack[top].type = TK_NUM;
        tokenStack[top].num = result;
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
      }
      top++;
    }
  }

  // 至此，tokenStack中仅包含加减乘除和数字 去除了所有的括号
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
