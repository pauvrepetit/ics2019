static unsigned long int next = 1;

int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

char *itoa(int num, char *s, int len) {
  // todo
  int neg = 0;
  if (num < 0) {
    neg = 1;
    num = -num;
  }
  s[--len] = '\0';
  if (num == 0) {
    s[--len] = '0';
  } else {
    while (num != 0) {
      s[--len] = num % 10 + '0';
      num /= 10;
    }
  }
  if (neg) s[--len] = '-';
  return s+len;
}

int atoi(const char* nptr) {
  int neg = 1;
  int loc = 0;
  int num = 0;
  if (nptr[loc] == '-') {
    neg = -1;
    loc++;
  }

  while (nptr[loc] >= '0' && nptr[loc] <= '9') {
    num *= 10;
    num += nptr[loc++] - '0';
  }
  return num * neg;
}