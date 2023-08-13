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

int uint32_optInit(uint32_opt *x);

int uint32_optSet(uint32_opt *x, uint32_t n);

int uint32_optRemove(uint32_opt *x);

bool strEq(const char *str1, const char *str2);

int clamp(int x, int min, int max);

int32_t fileGetLength(FILE *fp);
