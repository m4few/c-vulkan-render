#pragma once
#include <stdbool.h>
#include <stdint.h>


typedef struct {
  bool exists;
  uint32_t value;
} uint32_opt;
uint32_opt uint32_opt_default = {false,0};


bool strEq(const char* str1, const char* str2){
  bool equal = true;
  uint8_t count = 0;
  while(true){
    if(str1[count] != str2[count]){
      equal = false;
      break;
    }
    if(str1[count] == '\0'){
      break;
    }

    count++;
  }

  return equal;
}
