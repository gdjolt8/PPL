#include <stdio.h>
#include <stdlib.h>
#include "io.h"

char* read_file_contents(const char* filename) {
  FILE* fp = fopen(filename, "rb");
  if(!fp) {
    perror(filename);
    exit(1);
  }
  fseek(fp, 0, SEEK_END);
  long length = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char* buf = (char*)calloc(length, length);
  fread(buf, length, 1, fp);
  return buf;
}

void write_file(const char* filename, char* buffer) {
  FILE* fp = fopen(filename, "wb");
  if(!fp)
  {
    perror(filename);
  }
  fputs(buffer, fp);
  fclose(fp);
}
char* sh(const char* cmd)
{
  char* output = (char*) calloc(1, sizeof(char));
  output[0] = '\0';

  FILE *fp;
  char path[1035];

  fp = popen(cmd, "r");

  if (fp == NULL) {
    printf("Failed to run command\n" );
    exit(1);
  }

  while (fgets(path, sizeof(path), fp) != NULL) {
    output = (char*) realloc(output, (strlen(output) + strlen(path) + 1) * sizeof(char));
    strcat(output, path);
  }

  pclose(fp);

  return output;
}