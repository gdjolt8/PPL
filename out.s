  SYS_INIT_MODULE  equ 0x80
  SYS_EXIT         equ 0x01
  SYS_WRITE        equ 0x04

section .data
  message:     db  "Hello, world!", 0xa
  message_len: equ $-message

section .text
  global _start

_start:
  mov eax, SYS_WRITE; // sys_write(unsigned int fd, const char __user *buffer, size_t count)
  mov ebx, 0; // file descriptor
  mov ecx, message; // char
  mov edx, message_len; // size
  int SYS_INIT_MODULE; // invoke kernel
  mov eax, SYS_EXIT; // sys_exit(int error_code)
  mov ebx, 0; // error code
  int SYS_INIT_MODULE; // invoke kernel