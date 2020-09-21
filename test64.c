// With two u64 args, perform 10 million iterations of each operation and
// report averge time for each. Otherwise, loop on reading two u64's from stdin
// and print the result of all operations.
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define die(...) fprintf(stderr, __VA_ARGS__),exit(1)
#define u64 unsigned long long int
#define s64 long long int
#define barrier() asm volatile("": : :"memory")

// monotonic nanoseconds
u64 now(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return ((u64)t.tv_sec * 1000000000ULL) + (u64)t.tv_nsec;
}

// functions defined by arith64.c should also be in libgcc
s64 __absvdi2(s64 a);
int __clzdi2(u64 a);
int __ctzdi2(u64 a);
int __ffsdi2(u64 a);
u64 __udivdi3(u64 a, u64 b);
u64 __umoddi3(u64 a, u64 b);
s64 __divdi3(s64 a, s64 b);
s64 __moddi3(s64 a, s64 b);
int __popcountdi2(u64);

#define LOOPS 1000000

int main(int argc, char *argv[])
{
    u64 a, b;
    int benchmark = 0;
    if (argc > 1) benchmark = 1;
    while(fscanf(stdin, "%llu %llu", &a, &b))
    {
        if (benchmark)
        {
            u64 tabs = now(); for (int x = 0; x < LOOPS; x++) __absvdi2((s64)a); tabs = now()-tabs;
            u64 tclz = now(); for (int x = 0; x < LOOPS; x++) __clzdi2(a); tclz = now()-tclz;
            u64 tctz = now(); for (int x = 0; x < LOOPS; x++) __ctzdi2(a); tctz = now()-tctz;
            u64 tffs = now(); for (int x = 0; x < LOOPS; x++) __ffsdi2(a); tffs = now()-tffs;
            u64 tdiv = now(); for (int x = 0; x < LOOPS; x++) __divdi3((s64)a, (s64)b); tdiv = now()-tdiv;
            u64 tmod = now(); for (int x = 0; x < LOOPS; x++) __moddi3((s64)a, (s64)b); tmod = now()-tmod;
            u64 tudiv = now(); for (int x = 0; x < LOOPS; x++) __udivdi3(a, b); tudiv = now()-tudiv;
            u64 tumod = now(); for (int x = 0; x < LOOPS; x++) __umoddi3(a, b); tumod = now()-tumod;
            u64 tpop = now(); for (int x = 0; x < LOOPS; x++) __popcountdi2(a); tpop = now()-tpop;
            fprintf(stdout, "%llu %llu: abs=%.2fnS clz=%.2fnS ctz=%.2fnS ffs=%.2fnS udiv=%.2fnS umod=%.2fnS div=%.2fnS mod=%.2fnS pop=%.2fnS\n",
                    a, b,
                    (float)tabs / LOOPS,
                    (float)tclz / LOOPS,
                    (float)tctz / LOOPS,
                    (float)tffs / LOOPS,
                    (float)tudiv / LOOPS,
                    (float)tumod / LOOPS,
                    (float)tdiv / LOOPS,
                    (float)tmod / LOOPS,
                    (float)tpop / LOOPS);
        } else
        {
            fprintf(stdout, "%llu %llu: abs=%lld clz=%d ctz=%d ffs=%d udiv=%llu umod=%llu div=%lld mod=%lld pop=%d\n",
                    a, b,
                    __absvdi2((s64)a),
                    a?__clzdi2(a):-1,
                    a?__ctzdi2(a):-1,
                    a?__ffsdi2(a):-1,
                    __udivdi3(a, b),
                    __umoddi3(a, b),
                    __divdi3((s64)a, (s64)b),
                    __moddi3((s64)a, (s64)b),
                    __popcountdi2(a));
        }
        fflush(stdout);
    }

    return 0;
}
