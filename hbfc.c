#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static void error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

static void println(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
    fprintf(stdout, "\n");
}

static int count(void) {
    static int i = 0;
    return i++;
}

static char *cmd(char *ptr);
static char *loop(char *ptr);
static char *stmt(char *ptr);

static char *cmd(char *ptr) {
    switch (*ptr) {
        case '+': println("  add BYTE PTR [rsi], 1"); break;
        case '-': println("  sub BYTE PTR [rsi], 1"); break;
        case '>': println("  add rsi, 1"); break;
        case '<': println("  sub rsi, 1"); break;
        case '[': return loop(ptr + 1);
        case '.':
            println("  mov rax, 1");

            println("  mov rdi, 1");
            println("  mov rdx, 1");
            println("  syscall");
            break;
        case ',':
            println("  mov rax, 0");

            println("  mov rdi, 0");
            println("  mov rdx, 1");
            println("  syscall");
            break;
    }
    return ptr + 1;
}

static char *loop(char *ptr) {
    int i = count();
    println(".L.begin.%03d:", i);
    println("  movsx edi, BYTE PTR [rsi]");
    println("  cmp edi, 0");
    println("  je .L.end.%03d", i);

    ptr = stmt(ptr);
    if (*ptr++ != ']') error("error: unmatched brackets");

    println("  jmp .L.begin.%03d", i);
    println(".L.end.%03d:", i);
    return ptr;
}

static char *stmt(char *ptr) {
    while (*ptr != '\0' && *ptr != ']') ptr = cmd(ptr);
    return ptr;
}

int main(int argc, char **argv) {
    if (argc != 2) error("%s: invalid number of arguments", argv[0]);

    println("  .intel_syntax noprefix");

    println("  .global array");
    println("  .align 1");
    println("  .bss");
    println("array:");
    println("  .zero 30000");

    println("  .global main");
    println("  .text");
    println("main:");
    println("  lea rsi, array[rip]");

    char *code = argv[1];
    if (*stmt(code)) error("error: unmatched brackets");

    println("  mov rax, 0");
    println("  ret");

    return 0;
}
