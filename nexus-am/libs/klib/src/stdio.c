#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  size_t i = 0;
  size_t o = 0;
  for(i = 0; fmt[i] != '\0'; i++) {
    if (fmt[i] != '%') {
      out[o++] = fmt[i];
    } else {
      if (fmt[i+1] == '\0') {
        va_end(args);
        return -1;  // error
      } else {
        if (fmt[i+1] == '%') {
          out[o++] = fmt[i++];
        } else if (fmt[i+1] == 'd') {
          // number
          char numStr[20];
          int num = va_arg(args, int);
          char *ns = itoa(num, numStr, 20);
          strcat(out+o, ns);
          // o += (numStr + 19 - ns);
          o += strlen(ns);
          i ++;

        } else if (fmt[i+1] == 's') {
          // string
          char *s = va_arg(args, char *);
          strcat(out+o, s);
          o += strlen(s);
          i ++;

        } else {
          va_end(args);
          return -1;  // error
        }
      }
    }
  }
  va_end(args);
  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
