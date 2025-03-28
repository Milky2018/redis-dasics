#include <time.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

time_t milky_time(time_t *t);
int milky_clock_gettime(clockid_t clk_id, struct timespec *tp);
int milky_gettimeofday(struct timeval *tv, struct timezone *tz);

int32_t dasics_libcfg_alloc(uint64_t cfg, uint64_t lo, uint64_t hi);
int32_t dasics_libcfg_free(int32_t idx);

extern void lib_call(void *func_name, ...);

#define DASICS_LIBCFG_V 0x8UL
#define DASICS_LIBCFG_R 0x2UL
#define DASICS_LIBCFG_W 0x1UL

#define LIBCFG_ALLOC_RW(base, len) (dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_R | DASICS_LIBCFG_W, ((uint64_t)(base)), ((uint64_t)(base)) + ((uint64_t)(len))));

#define LIBCFG_ALLOC_CHAR_ARRAY(arr) LIBCFG_ALLOC_RW(arr, sizeof(arr) - 1)
#define LIBCFG_ALLOC_CHAR_POINTER(arr) LIBCFG_ALLOC_RW(arr, strlen(arr))

#define LIBCFG_FREE(idx) (dasics_libcfg_free(idx))

#define LIB_CALL(func, ...)                                                                          \
    {                                                                                                \
        uint64_t sp;                                                                                 \
        asm volatile("mv %0, sp" : "=r"(sp));                                                        \
        int stack_handler = dasics_libcfg_alloc(DASICS_LIBCFG_V | DASICS_LIBCFG_W | DASICS_LIBCFG_R, \
                                                sp - 0x1000,                                         \
                                                sp);                                                 \
        lib_call(func, ##__VA_ARGS__);                                                               \
        dasics_libcfg_free(stack_handler);                                                           \
    }

#define LIBCFG_WITH_CHAR_ARRAY(arr, body)           \
    {                                               \
        int handler = LIBCFG_ALLOC_CHAR_ARRAY(arr); \
        body;                                       \
        LIBCFG_FREE(handler);                       \
    }

#define LIBCFG_WITH_CHAR_POINTER(arr, body)           \
    {                                                 \
        int handler = LIBCFG_ALLOC_CHAR_POINTER(arr); \
        body;                                         \
        LIBCFG_FREE(handler);                         \
    }
