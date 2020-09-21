# The test64.c executable is built twice: as test64.gcc linked against libgcc in
# the usual way; and as test64.arith64 linked against arith64.c.

# test64.py starts both programs in the background, feeding them the same
# random 64-bit numbers and making sure they produce the same output.

# Unless there's a mismatch test64.py will run forever.

COPTS=-Wall -Werror -m32 -O3

.PHONY: test
test: both; ./test64.py

# as above but just print average times, does not fail
.PHONY: bench
bench: both; ./test64.py -b

# build in the usual way
test64.gcc: test64.c; gcc ${COPTS} -o $@ $<

# build with "-nodefaultlibs -lc" to prevent link to libgcc
test64.arith64: test64.c arith64.c; gcc ${COPTS} -nodefaultlibs -lc -o $@ $^

.PHONY: both
both: test64.gcc test64.arith64

.PHONY: clean
clean:; rm -f test64.gcc test64.arith64 *.o
