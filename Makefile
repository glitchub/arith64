# Build the test64.c executable twice: as test64.gcc linked against libgcc in
# the usual way; and as test64.arith64, linked against arith64.c.

# test64.py just repeatedly runs these programs, checking that they produce the
# same output given the same two randomly generated 64-bit numbers.

# Unless there's a mismatch test64.py will run forever.

COPTS=-Wall -Werror -m32

.PHONY: test
test: test64.gcc test64.arith64; ./test64.py

test64.gcc: test64.c; gcc ${COPTS} -o $@ $<

# "-nodefault -lc" prevents link to libgcc but not libc
test64.arith64: test64.c arith64.c; gcc ${COPTS} -nodefaultlibs -lc -o $@ $^

.PHONY: clean
clean:; rm -f test64.gcc test64.arith64 *.o
