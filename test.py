#!/usr/bin/python3

from random import randint; import subprocess, sys;
verbose=0
benchmark=0
try:
    if sys.argv[1] == "-b": benchmark=1
    elif sys.argv[1] == "-v": verbose=1
except: pass

commands = ["gcc -b", "arith64 -b"] if benchmark else ["gcc", "arith64"]

loops = 0
procs = []
for what in commands:
    procs.append(subprocess.Popen("./test.%s" % what, stdin=subprocess.PIPE, stdout=subprocess.PIPE, shell=True, text=True))

# Generate a random number of specified bit width with bias towards interesting
# test cases
def rand64():
    r = randint(0,6800)                            # 1% chance of random special case
    if r < 64: return 2**r                         # a single one
    if r == 64: return 2**64-1                     # all ones
    if r == 64+2: return randint(0, 2**32-1)       # low 32 bits
    if r == 64+3: return randint(0, 2**32-1) << 32 # high 32 bits
    return randint(0, 2**64-1)                     # full 64 bit

while True:
    # gen two 64-bit numbers
    A = rand64()
    B = rand64()

    # send to each proc
    for p in procs:
        p.stdin.write("%d %d\n" % (A, B));
        p.stdin.flush()

    # get each respond
    out=[]
    for p in procs:
        out.append(p.stdout.readline())

    # maybe report
    if verbose or benchmark or out[0] != out[1]:
        print("gcc    :",out[0].strip())
        print("arith64:",out[1].strip())
        if not verbose and not benchmark: exit(1)
        print()

    loops += 1
    if  not loops % 10000: print("%.2fM" % (loops/1000000.0,))
