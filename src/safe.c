#include <udasics.h>
#include <usyscall.h>
#include <utrap.h>
#include <setjmp.h>
#include <syscall.h>

#include "safe.h"

extern char __RODATA_BEGIN__[];
extern char __RODATA_END__[];

jmp_buf safe_handle_point;
int safe_handle_point_initialized = 0;

int handle_malicious_load(struct ucontext_trap * regs)
{
    uint32_t _ld;

    printf("[DASICS EXCEPTION]: Load fault: 0x%lx pc: 0x%lx\n", regs->utval, regs->uepc);

#define LD_INCMASK 0x00000003

    _ld = *((uint32_t *)regs->uepc);

    if ((_ld & LD_INCMASK) == LD_INCMASK)
        regs->uepc += 4;
    else
        regs->uepc += 2;

    if (safe_handle_point_initialized)
        longjmp(safe_handle_point, 1);
    return 0;
}

int handle_malicious_store(struct ucontext_trap *regs)
{
    uint32_t _sd;

    printf("[DASICS EXCEPTION]: Store fault: 0x%lx pc: 0x%lx\n", regs->utval, regs->uepc);
    printf("May caused by malicious requests\n");
    printf("Trying to respond with an error to the request\n");
    fflush(stdout);

#define SD_INCMASK 0x00000003

    _sd = *((uint32_t *)regs->uepc);

    if ((_sd & SD_INCMASK) == SD_INCMASK)
        regs->uepc += 4;
    else
        regs->uepc += 2;

    if (safe_handle_point_initialized)
        longjmp(safe_handle_point, 1);

    return 0;
}

int write_syscall_check(unsigned long a0, unsigned long a1, unsigned long a2, unsigned long a3, unsigned long a4, unsigned long a5, unsigned long a6, unsigned long a7)
{
    printf("Sys_write in untrusted code\n");
}

int read_syscall_check(unsigned long a0, unsigned long a1, unsigned long a2, unsigned long a3, unsigned long a4, unsigned long a5, unsigned long a6, unsigned long a7)
{
    printf("Sys_read in untrusted code\n");
}

int default_check()
{
    printf("Default check handler called.\n");
    return 0;
}

void safe_init() {
    register_udasics(0);
    resgister_ustore_fault_handler(handle_malicious_store);
    register_syscall_check(SYS_write, write_syscall_check, default_check);
    register_syscall_check(SYS_read, read_syscall_check, default_check);
    dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R, (uint64_t)__RODATA_BEGIN__, (uint64_t)__RODATA_END__);
}