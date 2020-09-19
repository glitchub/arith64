// Given two unsigned 64-bit numbers on the command line, print result of various libgcc/artih64 functions.
#include <stdio.h>
#include <stdlib.h>
#define die(...) fprintf(stderr, __VA_ARGS__),exit(1)

#define u64 unsigned long long
#define s64 long long

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

int main(int argc, char *argv[])
{
    if (argc != 3) die("Usage: test64 number number\n");

    u64 a = strtoull(argv[1],NULL,0);
    u64 b = strtoull(argv[2],NULL,0);

    printf("%llu %llu: abs=%lld clz=%d ctz=%d ffs=%d udiv=%llu umod=%llu div=%lld mod=%lld pop=%d\n",
        a, b,
        __absvdi2((s64)a),
        __clzdi2(a),
        __ctzdi2(a),
        __ffsdi2(a),
        __udivdi3(a, b),
        __umoddi3(a, b),
        __divdi3((s64)a, (s64)b),
        __moddi3((s64)a, (s64)b),
        __popcountdi2(a));
    return 0;
}
