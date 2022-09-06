# executables
BOTH = test.gcc test.arith64

# build executables and run the comparison test
.PHONY: test
test: ${BOTH}; ./test.py

# build executables and run the benchmark test
.PHONY: bench
bench: ${BOTH}; ./test.py -b

COPTS = -Wall -Werror -m32 -O3 -fno-stack-protector
# uncomment for older gcc e.g. centos 7
# COPTS += -std=gnu99

# build test.gcc in the usual way
test.gcc: test.c; gcc ${COPTS} -o $@ $<

# build test.arith64 with arith64 instead of libgcc
test.arith64: test.c arith64.c; gcc ${COPTS} -nodefaultlibs -lc -o $@ $^

.PHONY: clean
clean:; rm -f ${BOTH}
