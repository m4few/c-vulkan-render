#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

typedef struct {
  bool exists;
  uint32_t value;
} uint32_opt;
uint32_opt uint32_opt_DEFAULT = {false, 0};

int uint32_optInit(uint32_opt *x) {
  *x = uint32_opt_DEFAULT;
  return EXIT_SUCCESS;
}

int uint32_optSet(uint32_opt *x, uint32_t n) {
  x->value = n;
  x->exists = true;
  return EXIT_SUCCESS;
}

int uint32_optRemove(uint32_opt *x) {
  x->exists = false;
  return EXIT_SUCCESS;
}

bool strEq(const char *str1, const char *str2) {
  bool equal = true;
  uint8_t count = 0;
  while (true) {
    if (str1[count] != str2[count]) {
      equal = false;
      break;
    }
    if (str1[count] == '\0') {
      break;
    }

    count++;
  }

  return equal;
}

int clamp(int x, int min, int max) {
  const int t = x < min ? min : x;
  return t > max ? max : t;
}

int32_t fileGetLength(FILE *fp) {
  fseek(fp, 0L, SEEK_END);
  int32_t size = ftell(fp);
  rewind(fp);

  return size;
}
