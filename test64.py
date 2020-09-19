#!/usr/bin/python3

import random, subprocess

loops = 0
try:
    while True:
        loops += 1
        a = str(random.randint(0,2**64-1))
        b = str(random.randint(1,2**64-1)) # never 0

        gcc=subprocess.run(["./test64.gcc",a,b], check=True, capture_output=True)
        arith64=subprocess.run(["./test64.arith64",a,b], check=True, capture_output=True)

        if gcc.stdout != arith64.stdout: break
        if not loops % 100: print(".",end='',flush=True)
except KeyboardInterrupt:
    pass

print("\nFailed after %d loops" % loops)
print("gcc:    ",gcc.stdout.decode().strip());
print("arith64:",arith64.stdout.decode().strip());
