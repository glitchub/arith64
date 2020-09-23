// Read a set of random numbers from stdin, perform all the operations in
// arith64 and output the results on a single line.

// If any command line arg is supplied then instead perform each operation
// 1,000,000 times and print the average nanoseconds on a single line.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

// Functions defined by arith64.c are also in libgcc
int64_t __absvdi2(int64_t a);
int __clzdi2(uint64_t a);
int __clzsi2(uint32_t a);
int __ctzdi2(uint64_t a);
int __ctzsi2(uint32_t a);
int64_t __divdi3(int64_t a, int64_t b);
int __ffsdi2(uint64_t a);
uint64_t __lshrdi3(uint64_t a, int b);
int64_t __moddi3(int64_t a, int64_t b);
int __popcountdi2(uint64_t);
int __popcountsi2(uint32_t a);
uint64_t __udivdi3(uint64_t a, uint64_t b);
uint64_t __umoddi3(uint64_t a, uint64_t b);

// monotonic nanoseconds
uint64_t nS(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return ((uint64_t)t.tv_sec * 1000000000ULL) + (uint64_t)t.tv_nsec;
}

#define LOOPS 1000000

// perform one benchmark or operation and print the result
#define dotest(name, fmt, op) \
    if (dobench) { \
        uint64_t t=nS(); for (int x=0; x<LOOPS; x++) op; printf(" " name "=%.2f", (float)(nS()-t)/LOOPS); \
    } else printf(" " name "=" fmt, op)

int main(int argc, char *argv[])
{
    int dobench = argc > 1; // any arg enables benchmark

    uint64_t A, B;

    // read pairs of 64-bit numbers from stdin an perform various operations
    while(fscanf(stdin, "%llu %llu", &A, &B) == 2)
    {
        printf("%.16llX %.16llX:", A, B);

        if (A != 1ULL<<63)
        {
            dotest("abs", "%lld", __absvdi2((int64_t)A));
        }

        if (A)
        {
            dotest("clzd", "%d",   __clzdi2(A));
            dotest("ctzd", "%d",   __ctzdi2(A));
        }

        if (B)
        {
            dotest("div",  "%lld", __divdi3((int64_t)A, (int64_t)B));
            dotest("mod",  "%lld", __moddi3((int64_t)A, (int64_t)B));
            dotest("udiv", "%llu", __udivdi3(A, B));
            dotest("umod", "%llu", __umoddi3(A, B));
        }

        uint32_t a = A;
        if (a != 0)
        {
            dotest("clzs", "%d",   __clzsi2(a));
            dotest("ctsz", "%d",   __ctzsi2(a));
        }

        dotest("shr",  "%llu", __lshrdi3(A, a & 63));
        dotest("ffs",  "%d",   __ffsdi2(A));
        dotest("popd", "%d",   __popcountdi2(A));
        dotest("pops", "%d",   __popcountsi2(a));
        printf("\n");
        fflush(stdout);
    }

    return 0;
}
