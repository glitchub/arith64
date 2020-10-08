// GCC 32/64-bit integer arithmetic support for 32-bit systems that can't link
// to libgcc.

// Function prototypes and descriptions are taken from
// https://gcc.gnu.org/onlinedocs/gccint/Integer-library-routines.html.

// This file may be #include'd by another file, so we try not to pollute the
// namespace and we don't import any headers.

// All functions must be resolvable by the linker and therefore can't be inline
// or static, even if they're #included into the file where they'll be used.

// For best performance we try to avoid branching. This makes the code a little
// weird in places.

// See https://github.com/glitchub/arith64 for more information.
// This software is released as-is into the public domain, as described at
// https://unlicense.org. Do whatever you like with it.

#ifndef uint64_t
// mimic stdint.h
#define arith64_stdint
#define uint64_t unsigned long long int
#define int64_t signed long long int
#define uint32_t unsigned int
#define int32_t int
#endif

typedef union
{
    uint64_t u64;
    int64_t s64;
    struct
    {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        uint32_t hi; uint32_t lo;
#else
        uint32_t lo; uint32_t hi;
#endif
    } u32;
    struct
    {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        int32_t hi; int32_t lo;
#else
        int32_t lo; int32_t hi;
#endif
    } s32;
} _arith64_word;

// extract hi and lo 32-bit words from 64-bit value
#define hi(n) (_arith64_word){.u64=n}.u32.hi
#define lo(n) (_arith64_word){.u64=n}.u32.lo

// Negate a if b is negative, via invert and increment.
#define negate(a, b) (((a) ^ ((((int64_t)(b)) >= 0) - 1)) + (((int64_t)(b)) < 0))
#define abs(a) negate(a, a)

// Return the absolute value of a.
// Note LLINT_MIN cannot be negated.
int64_t __absvdi2(int64_t a)
{
    return abs(a);
}

// Return the result of shifting a left by b bits.
int64_t __ashldi3(int64_t a, int b)
{
    _arith64_word w = {.s64 = a};

    b &= 63;

    if (b >= 32)
    {
        w.u32.hi = w.u32.lo << (b - 32);
        w.u32.lo = 0;
    } else if (b)
    {
        w.u32.hi = (w.u32.lo >> (32 - b)) | (w.u32.hi << b);
        w.u32.lo <<= b;
    }
    return w.s64;
}

// Return the result of arithmetically shifting a right by b bits.
int64_t __ashrdi3(int64_t a, int b)
{
    _arith64_word w = {.s64 = a};

    b &= 63;

    if (b >= 32)
    {
        w.s32.lo = w.s32.hi >> (b - 32);
        w.s32.hi >>= 31; // 0xFFFFFFFF or 0
    } else if (b)
    {
        w.u32.lo = (w.u32.hi << (32 - b)) | (w.u32.lo >> b);
        w.s32.hi >>= b;
    }
    return w.s64;
}

// These functions return the number of leading 0-bits in a, starting at the
// most significant bit position. If a is zero, the result is undefined.
int __clzsi2(uint32_t a)
{
    int b, n = 0;
    b = !(a & 0xffff0000) << 4; n += b; a <<= b;
    b = !(a & 0xff000000) << 3; n += b; a <<= b;
    b = !(a & 0xf0000000) << 2; n += b; a <<= b;
    b = !(a & 0xc0000000) << 1; n += b; a <<= b;
    return n + !(a & 0x80000000);
}

int __clzdi2(uint64_t a)
{
    return hi(a) ? __clzsi2(hi(a)) : __clzsi2(lo(a)) + 32;
}

// These functions return the number of trailing 0-bits in a, starting at the
// least significant bit position. If a is zero, the result is undefined.
int __ctzsi2(uint32_t a)
{
    int b, n = 0;
    b = !(a & 0x0000ffff) << 4; n += b; a >>= b;
    b = !(a & 0x000000ff) << 3; n += b; a >>= b;
    b = !(a & 0x0000000f) << 2; n += b; a >>= b;
    b = !(a & 0x00000003) << 1; n += b; a >>= b;
    return n + !(a & 0x00000001);
}

int __ctzdi2(uint64_t a)
{
    return lo(a) ? __ctzsi2(lo(a)) : __ctzsi2(hi(a)) + 32;
}

// Calculate both the quotient and remainder of the unsigned division of a and
// b. The return value is the quotient, and the remainder is placed in variable
// pointed to by c (if it's not NULL).
uint64_t __divmoddi4(uint64_t a, uint64_t b, uint64_t *c)
{
    if (b > a)                                  // divisor > numerator?
    {
        if (c) *c = a;                          // remainder = numerator
        return 0;                               // quotient = 0
    }
    if (!hi(b))                                 // divisor is 32-bit
    {
        if (b == 0)                             // divide by 0
        {
            volatile char x = 0; x = 1 / x;     // force an exception
        }
        if (b == 1)                             // divide by 1
        {
            if (c) *c = 0;                      // remainder = 0
            return a;                           // quotient = numerator
        }
        if (!hi(a))                             // numerator is also 32-bit
        {
            if (c) *c = lo(a) % lo(b);          // use generic 32-bit operators
            return lo(a) / lo(b);
        }
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
        wrap = ((int64_t)(b - rem - 1) >> 63);  // wrap = (b > rem) ? 0 : 0xffffffffffffffff (via sign extension)
        rem -= b & wrap;                        // if (wrap) rem -= b
    }
    if (c) *c = rem;                            // maybe set remainder
    return (a << 1) | (wrap & 1);               // return the quotient
}

// Return the quotient of the signed division of a and b.
int64_t __divdi3(int64_t a, int64_t b)
{
    uint64_t q = __divmoddi4(abs(a), abs(b), (void *)0);
    return negate(q, a^b); // negate q if a and b signs are different
}

// Return the index of the least significant 1-bit in a, or the value zero if a
// is zero. The least significant bit is index one.
int __ffsdi2(uint64_t a)
{
    return a ? __ctzdi2(a) + 1 : 0;
}

// Return the result of logically shifting a right by b bits.
uint64_t __lshrdi3(uint64_t a, int b)
{
    _arith64_word w = {.u64 = a};

    b &= 63;

    if (b >= 32)
    {
        w.u32.lo = w.u32.hi >> (b - 32);
        w.u32.hi = 0;
    } else if (b)
    {
        w.u32.lo = (w.u32.hi << (32 - b)) | (w.u32.lo >> b);
        w.u32.hi >>= b;
    }
    return w.u64;
}

// Return the remainder of the signed division of a and b.
int64_t __moddi3(int64_t a, int64_t b)
{
    uint64_t r;
    __divmoddi4(abs(a), abs(b), &r);
    return negate(r, a); // negate remainder if numerator is negative
}

// Return the number of bits set in a.
int __popcountsi2(uint32_t a)
{
    // collect sums into two low bytes
    a = a - ((a >> 1) & 0x55555555);
    a = ((a >> 2) & 0x33333333) + (a & 0x33333333);
    a = (a + (a >> 4)) & 0x0F0F0F0F;
    a = (a + (a >> 16));
    // add the bytes, return 0 to 32
    return (a + (a >> 8)) & 63;
}

// Return the number of bits set in a.
int __popcountdi2(uint64_t a)
{
    return __popcountsi2(hi(a)) + __popcountsi2(lo(a));
}

// Return the quotient of the unsigned division of a and b.
uint64_t __udivdi3(uint64_t a, uint64_t b)
{
    return __divmoddi4(a, b, (void *)0);
}

// Return the remainder of the unsigned division of a and b.
uint64_t __umoddi3(uint64_t a, uint64_t b)
{
    uint64_t r;
    __divmoddi4(a, b, &r);
    return r;
}

// Remove macros in case we're #include'd
#undef hi
#undef lo
#undef negate
#undef abs
#ifdef arith64_stdint
  #undef uint64_t
  #undef int64_t
  #undef uint32_t
  #undef int32_t
  #undef arith64_stdint
#endif
