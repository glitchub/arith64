// GCC 64-bit integer arithmetic support for systems that can't link
// libgcc.

// This software is released as-is into the public domain, as described at
// https://unlicense.org. Do whatever you like with it.

// Function prototypes and descriptions are taken from
// https://gcc.gnu.org/onlinedocs/gccint/Integer-library-routines.html.

// Note these functions must be resolvable by the linker and therefore can't be
// inline or static, even if they're defined in the file where they'll be used.

#ifndef uint64_t
// mimic stdarg.h if need be
#define uint64_t unsigned long long int
#define int64_t signed long long int
#define uint32_t unsigned int
#define int32_t int
#endif

// Return the absolute value of a.
int64_t __absvdi2(int64_t a)
{
    return (a < 0) ? -a : a;
}

// Return the number of leading 0-bits in a, starting at the most significant
// bit position. If a is zero, the result is undefined.
int __clzdi2(uint64_t a)
{
    return (a>>32) ? __builtin_clz(a>>32) : (__builtin_clz(a)+32);
}

// Return the number of trailing 0-bits in a, starting at the least significant
// bit position. If a is zero, the result is undefined.
int __ctzdi2(uint64_t a)
{
    return (uint32_t)a ?  __builtin_ctz(a) : (__builtin_ctz(a>>32) + 32);
}

// Return the index of the least significant 1-bit in a, or the value zero if a
// is zero. The least significant bit is index one.
int __ffsdi2(uint64_t a)
{
    return a ? __ctzdi2(a) + 1 : 0;
}

// Calculate both the quotient and remainder of the unsigned division of a and
// b. The return value is the quotient, and the remainder is placed in variable
// pointed to by c (if it's not NULL).
uint64_t __divmoddi4(uint64_t a, uint64_t b, uint64_t *c)
{
    if (!b) { volatile char x = 0; x=1 / x; }   // divisor == 0, force exception
    if (b > a)                                  // divisor > numerator
    {
        if (c) *c = a;                          // remainder = numerator
        return 0;                               // quotient = 0
    }
    if (b == a)                                 // divisor == numerator
    {
        if (c) *c = 0;                          // remainder = 0
        return 1;                               // quotient = 1
    }
    if (b == 1)                                 // divide by 1
    {
        if (c) *c = 0;                          // remainder = 0
        return a;                               // quotient = numerator
    }
    if (b == 2)                                 // divide by 2
    {
        if (c) *c = a & 1;                      // remainder = LSB
        return a >> 1;                          // quotient = numerator >> 1
    }
    if (!(a>>32) && !(b>>32))                   // 32-bit only
    {
        if (c) *c = (uint32_t)a % (uint32_t)b;  // use generic 32-bit operators
        return (uint32_t)a / (uint32_t)b;
    }

    // let's do long division
    char bits = (b ? __clzdi2(b) : 64) -        // number of bits to iterate
                (a ? __clzdi2(a) : 64)
                + 1;
    uint64_t rem = a >> bits;                   // init remainder
    a <<= 64 - bits;                            // shift numerator to the high bit
    uint64_t wrap = 0;                          // start with wrap = 0
    while (bits-- > 0)                          // for each bit
    {
        rem = (rem << 1) | (a >> 63);           // shift numerator MSB to remainder LSB
        a = (a << 1) | (wrap & 1);              // shift out the numerator, shift in wrap
        wrap = ((int64_t)(b - rem - 1) >> 63);  // wrap = (b > rem) ? 0 : 0xffffffffffffffff
        rem -= b & wrap;                        // if (wrap) rem -= b
    }
    if (c) *c = rem;                            // maybe set remainder
    return (a << 1) | (wrap & 1);               // return the quotient
}

// Return the quotient of the unsigned division of a and b.
uint64_t __udivdi3(uint64_t a, uint64_t b)
{
    return __divmoddi4(a, b, 0);
}

// Return the remainder of the unsigned division of a and b.
uint64_t __umoddi3(uint64_t a, uint64_t b)
{
    uint64_t r;
    __divmoddi4(a, b, &r);
    return r;
}

// Return the quotient of the signed division of a and b.
int64_t __divdi3(int64_t a, int64_t b)
{
    uint64_t q = __divmoddi4(__absvdi2(a), __absvdi2(b), 0); // *c is NULL
    return ((a ^ b) < 0) ? -(int64_t)q : (int64_t)q;
}

// Return the remainder of the signed division of a and b.
int64_t __moddi3(int64_t a, int64_t b)
{
    uint64_t r;
    __divmoddi4(__absvdi2(a), __absvdi2(b), &r);
    return (a > 0) ? (int64_t)r : -(int64_t)r; // remainder has same sign as numerator
}

// Return the number of bits set in a.
int __popcountdi2(uint64_t a)
{
    return __builtin_popcount((uint32_t)a) + __builtin_popcount(a>>32);
}

// Depending on the compiler, these 32-bit functions may also be required, for
// example to resolve the builtin_xxx functions used above.

// Return the number of leading 0-bits in a, starting at the most significant
// bit position. If a is zero, the result is undefined.
int __clzsi2(uint32_t a)
{
    int n = 0;
    if (!(a & 0xffff000000000000ULL)) n += 16, a <<= 16;
    if (!(a & 0xff00000000000000ULL)) n += 8, a <<= 8;
    if (!(a & 0xf000000000000000ULL)) n += 4, a <<= 4;
    if (!(a & 0xc000000000000000ULL)) n += 2, a <<= 2;
    if (!(a & 0x8000000000000000ULL)) n += 1;
    return n;
}

// Return the number of trailing 0-bits in a, starting at the least significant
// bit position. If a is zero, the result is undefined.
int __ctzsi2(uint32_t a)
{
    int n = 0;
    if (!(a & 0x000000000000ffffULL)) n += 16, a >>= 16;
    if (!(a & 0x00000000000000ffULL)) n += 8, a >>= 8;
    if (!(a & 0x000000000000000fULL)) n += 4, a >>= 4;
    if (!(a & 0x0000000000000003ULL)) n += 2, a >>= 2;
    if (!(a & 0x0000000000000001ULL)) n += 1;
    return n;
}

// Return the number of bits set in a.
int __popcountsi2(uint32_t a)
{
    if (!a) return 0;
    // collect sums into two low bytes
    a = a - ((a >> 1) & 0x55555555);
    a = ((a >> 2) & 0x33333333) + (a & 0x33333333);
    a = (a + (a >> 4)) & 0x0F0F0F0F;
    a = (a + (a >> 16));
    // add the bytes, delete the cruft, return 1 to 32
    return (a + (a >> 8)) & 63; }
