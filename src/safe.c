#include <udasics.h>
#include <utrap.h>
#include <setjmp.h>

extern char __RODATA_BEGIN__[];
extern char __RODATA_END__[];

// int handle_DasicsULoadFault(struct ucontext_trap * regs)
// {
//     int idx = 0;
//     uint16_t c_ld;
//     uint32_t _ld;

// //     umain_got_t * _got_entry = _get_area(regs->uepc);
// // #ifdef DASICS_DEBUG
// //     if (_got_entry)
// //         dasics_printf("[ufault info]: hit read ufault uepc: 0x%lx, address: 0x%lx \n", regs->uepc, regs->utval);
// //     // debug_print_umain_map(_got_entry);
// // #endif
// //     if (_got_entry == NULL)
// //     {
// //     #ifdef DASICS_DEBUG
// //         dasics_printf("[error]: failed to find the _got_entry\n");
// //     #endif
// //         if (_umain_got_table == NULL) goto dasics_static_load;
// //         exit(1);
// //     }
//     dasics_printf("[DASICS_EXCEPTION]: Load fault: 0x%lx, pc: 0x%lx\n", regs->utval, regs->uepc);

// dasics_static_load:

//     #define LD_INCMASK 0x00000003

//     // Jump load instruction for future excution
//     c_ld = *((uint16_t *)regs->uepc);
//     _ld = *((uint32_t *)regs->uepc);

//     if ((_ld & LD_INCMASK) == LD_INCMASK)
//         regs->uepc += 4;
//     else
//         regs->uepc += 2;

//     if (idx == -1)
//     {
//         dasics_printf("[error]: no more libbounds!!\n");
//         exit(1);
//     }
//     return 0;
// }

jmp_buf safe_handle_point;

int handle_malicious_store(struct ucontext_trap *regs)
{
    uint32_t _sd;

    printf("[DASICS EXCEPTION]: Store fault: 0x%lx pc: 0x%lx\n", regs->utval, regs->uepc);
    printf("May caused by malicious requests\n");
    printf("Trying to respond with an error to the request\n");

#define SD_INCMASK 0x00000003

    _sd = *((uint32_t *)regs->uepc);

    if ((_sd & SD_INCMASK) == SD_INCMASK)
        regs->uepc += 4;
    else
        regs->uepc += 2;

    longjmp(safe_handle_point, 1);

    return 0;
}

void safe_init() {
    register_udasics(0);
    resgister_ustore_fault_handler(handle_malicious_store);
    dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_W | DASICS_LIBCFG_R, (uint64_t)__RODATA_BEGIN__, (uint64_t)__RODATA_END__);
}