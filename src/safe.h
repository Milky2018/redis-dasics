#ifndef SAFE_H
#define SAFE_H

#include <udasics.h>

void set_uret_point(reg_t point);
int handle_malicious_store(struct ucontext_trap *regs);

#endif /* SAFE_H */