#ifndef SAFE_H
#define SAFE_H

#include <udasics.h>
#include <setjmp.h>

int handle_malicious_store(struct ucontext_trap *regs);
void safe_init();

extern jmp_buf safe_handle_point;
extern int safe_handle_point_initialized;

#endif /* SAFE_H */