# The test.c executable is built twice: as test.gcc linked against libgcc in
# the usual way; and as test.arith64 linked against arith64.c.

# test.py starts both programs in the background, feeding them the same
# random 64-bit numbers and making sure they produce the same output.

# Unless there's a mismatch test.py will run forever.

COPTS=-Wall -Werror -m32 -O3

.PHONY: test
test: both; ./test.py

# as above but just print average times, does not fail
.PHONY: bench
bench: both; ./test.py -b

# build in the usual way
test.gcc: test.c; gcc ${COPTS} -o $@ $<

# build with "-nodefaultlibs -lc" to prevent link to libgcc
test.arith64: test.c arith64.c; gcc ${COPTS} -nodefaultlibs -lc -o $@ $^

.PHONY: both
both: test.gcc test.arith64

.PHONY: clean
clean:; rm -f test.gcc test.arith64 *.o
