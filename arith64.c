// GCC 64-bit integer arithmetic support for systems that can't link libgcc.
// This software is released as-is into the public domain, as described at
// https://unlicense.org. Do whatever you like with it.

// Function prototypes and descriptions are taken from
// https://gcc.gnu.org/onlinedocs/gccint/Integer-library-routines.html.

// Note these functions must be resolvable by the linker and therefore cannot
// be inline or static, even if they're defined in the file where they'll be
// used.

// Make sure we're using 64-bit numbers, "long" by itself is ambiguous.
#define u64 unsigned long long int
#define s64 signed long long int

// Return the absolute value of a.
s64 __absvdi2(s64 a)
{
    return (a < 0) ? -a : a;
}

// Return the number of leading 0-bits in a, starting at the most significant
// bit position. If a is zero, the result is 64.
int __clzdi2(u64 a)
{
    if (!a) return 64;
    int n = 0;
    if (!(a & 0xffffffff00000000ULL)) n += 32, a <<= 32;
    if (!(a & 0xffff000000000000ULL)) n += 16, a <<= 16;
    if (!(a & 0xff00000000000000ULL)) n += 8, a <<= 8;
    if (!(a & 0xf000000000000000ULL)) n += 4, a <<= 4;
    if (!(a & 0xc000000000000000ULL)) n += 2, a <<= 2;
    if (!(a & 0x8000000000000000ULL)) n += 1;
    return n;
}

// Return the number of trailing 0-bits in a, starting at the least significant
// bit position. If a is zero, the result is -1.
int __ctzdi2(u64 a)
{
    if (!a) return -1;
    int n = 0;
    if (!(a & 0x00000000ffffffffULL)) n += 32, a >>= 32;
    if (!(a & 0x000000000000ffffULL)) n += 16, a >>= 16;
    if (!(a & 0x00000000000000ffULL)) n += 8, a >>= 8;
    if (!(a & 0x000000000000000fULL)) n += 4, a >>= 4;
    if (!(a & 0x0000000000000003ULL)) n += 2, a >>= 2;
    if (!(a & 0x0000000000000001ULL)) n += 1;
    return n;
}

// Return the index of the least significant 1-bit in a, or the value zero if a
// is zero. The least significant bit is index one.
int __ffsdi2(u64 a)
{
    return __ctzdi2(a)+1;
}

// Calculate both the quotient and remainder of the unsigned division of a and
// b. The return value is the quotient, and the remainder is placed in variable
// pointed to by c (if it's not NULL).
u64 __divmoddi4(u64 a, u64 b, u64 *c)
{
    u64 r = 0;                              // start with remainder 0
    if (!b) { volatile char x=0; x=1/x; }   // divisor == 0, force divide-by-zero exception
    if (a == b) a = 1;                      // divisor == numerator, return quotient = 1, remainder = 0
    else if (a < b) r = a, a = 0;           // divisor < numerator, return quotient = 0, remainder = numerator
    else if (b != 1)                        // divide by 1, return quotient = numerator, remainder = 0
    {
        char n = __clzdi2(a);               // skip numerator's leading zeros (XXX is this really a worthwhile optimization?)
        a <<= n;
        while (n++ < 64)                    // for each remaining bit
        {
            r = (r << 1) | ((s64)a < 0);    // shift numerator MSB to remainder LSB
            a <<= 1;
            if (r >= b) r -= b, a |= 1;     // if remainder is greater than divisor, wrap and update quotient
        }
    }
    if (c) *c = r;                          // maybe set remainder
    return a;                               // return the quotient
}

// Return the quotient of the unsigned division of a and b.
u64 __udivdi3(u64 a, u64 b)
{
    return __divmoddi4(a, b, 0);
}

// Return the remainder of the unsigned division of a and b.
u64 __umoddi3(u64 a, u64 b)
{
    u64 r;
    __divmoddi4(a, b, &r);
    return r;
}

// Return the quotient of the signed division of a and b.
s64 __divdi3(s64 a, s64 b)
{
    u64 q = __divmoddi4(__absvdi2(a), __absvdi2(b), 0); // *c is NULL
    return ((a > 0) ^ (b > 0)) ? -(s64)q : (s64)q;
}

// Return the remainder of the signed division of a and b.
s64 __moddi3(s64 a, s64 b)
{
    u64 r;
    __divmoddi4(__absvdi2(a), __absvdi2(b), &r);
    return (a > 0) ? (s64)r : -(s64)r;      // remainder has same sign as numerator
}

// Return the number of bits set in a.
int __popcountdi2(u64 a)
{
    int n = 0;
    while (a) n++, a &= a - 1;
    return n;
}
