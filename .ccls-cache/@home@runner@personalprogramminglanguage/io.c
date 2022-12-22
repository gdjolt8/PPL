#include <stdio.h>
#include <stdlib.h>
#include "io.h"

char* read_file_contents(const char* filename) {
  FILE* fp = fopen(filename, "rb");
  if(!fp) {
    perror(filename);
  }
  fseek(fp, 0, SEEK_END);
  long length = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char* buf = (char*)calloc(length, length);
  fread(buf, length, 1, fp);
  return buf;
}