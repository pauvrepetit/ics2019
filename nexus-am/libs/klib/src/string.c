#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  while (s[len++] != '\0');
  return len - 1;
}

char *strcpy(char* dst,const char* src) {
  for (size_t i = 0; src[i] != '\0'; i++) {
    dst[i] = src[i];
  }
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  size_t i;
  for(i = 0; i < n && src[i] != '\0'; i++) {
    dst[i] = src[i];
  }
  for(; i < n; i++) {
    dst[i] = '\0';
  }
  return dst;
}

char* strcat(char* dst, const char* src) {
  size_t len = strlen(dst);
  size_t i = 0;
  while (src[i] != '\0') {
    dst[len++] = src[i++];
  }
  dst[len] = '\0';
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  size_t i = 0;
  while (s1[i] != '\0' && s2[i] != '\0' && s1[i] == s2[i]) {
    i++;
  }
  return s1[i] - s2[i];
}

int strncmp(const char* s1, const char* s2, size_t n) {
  size_t i = 0;
  while (s1[i] != '\0' && s2[i] != '\0' && s1[i] == s2[i] && i < n) {
    i++;
  }
  if (i == n) {
    return 0;
  } else {
    return s1[i] - s2[i];
  }
}

void* memset(void* v,int c,size_t n) {
  for(size_t i = 0; i < n; i++) {
    ((uint8_t *)v)[i] = (uint8_t)c;
  }
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  for(size_t i = 0; i < n; i++) {
    ((uint8_t *)out)[i] = ((uint8_t *)in)[i];
  }
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n) {
  size_t i = 0;
  const uint8_t *m1 = (const uint8_t *)s1;
  const uint8_t *m2 = (const uint8_t *)s2;
  while (m1[i] == m2[i] && i < n) {
    i++;
  }
  if (i == n) {
    return 0;
  } else {
    return (int)m1[i] - (int)m2[i];
  }
  return 0;
}

#endif
