#ifndef SAFE_H
#define SAFE_H

#include <udasics.h>

int handle_malicious_store(struct ucontext_trap *regs);
void safe_init();

#endif /* SAFE_H */