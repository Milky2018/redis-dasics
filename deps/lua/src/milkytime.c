#include "milkytime.h"

time_t milky_time(time_t *t) {
    struct timespec ts;
    syscall(SYS_clock_gettime, CLOCK_REALTIME, &ts);
    if (t) {
        *t = ts.tv_sec;
    }
    return ts.tv_sec;
}

int milky_clock_gettime(clockid_t clk_id, struct timespec *tp) {
    return syscall(SYS_clock_gettime, clk_id, tp);
}

int milky_gettimeofday(struct timeval *tv, struct timezone *tz) {
    return syscall(SYS_gettimeofday, tv, tz);
}