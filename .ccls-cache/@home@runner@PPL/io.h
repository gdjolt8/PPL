#ifndef PPL_INPUTOUTPUT_H
#define PPL_INPUTOUTPUT_H

char* read_file_contents(const char* filename);
void write_file(const char* filename, char* buffer);
char* sh(const char* cmd);

#endif