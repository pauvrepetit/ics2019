#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  char buf[1024];
  va_list args;
  va_start(args, fmt);
  vsprintf(buf, fmt, args);
  va_end(args);
  size_t i = 0;
  while (buf[i] != '\0') {
    _putc(buf[i++]);
  }
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  size_t i = 0;
  size_t o = 0;
  for(i = 0; fmt[i] != '\0'; i++) {
    if (fmt[i] != '%') {
      out[o++] = fmt[i];
    } else {
      if (fmt[i+1] == '\0') {
        return -1;  // error
      } else {
        if (fmt[i+1] == '%') {
          out[o++] = fmt[i++];
        } else if (fmt[i+1] == 'd') {
          // %d 十进制输出数字
          char numStr[20];
          int num = va_arg(ap, int);
          char *ns = itoa(num, numStr, 20);
          strcpy(out+o, ns);
          o += strlen(ns);
          i ++;
        } else if (fmt[i+1] == 's') {
          // %s 输出字符串
          char *s = va_arg(ap, char *);
          strcpy(out+o, s);
          o += strlen(s);
          i ++;
        } else if (fmt[i+1] == '0') {
          // %0 数字 d
          i++;
          if (fmt[i+1] > '0' && fmt[i+1] <= '9') {
            int numLen = atoi(fmt+1); // 这个数字表示输出的长度
            while (fmt[i] > '9' || fmt[i] < '0') {
              i++;
            }
            // 跳过上面处理过的数字
            if (fmt[i] == 'd') {
              char numStr[20];
              int num = va_arg(ap, int);
              char *ns = itoa(num, numStr, 20);
              int numRealLen = strlen(ns);
              for (int j = 0; j < numLen - numRealLen; j++) {
                out[o++] = '0';
              }
              strcpy(out+o, ns);
              o += numRealLen;
            } else {
              return -1;  // 对于其他的类型，暂时不做处理
            }
          }
        } else {
          return -1;  // error
        }
      }
    }
  }
  out[o] = '\0';
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsprintf(out, fmt, ap);
  va_end(ap);
  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
