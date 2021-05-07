#!/usr/bin/python3

from random import randrange; import subprocess, sys, time
verbose=0
benchmark=0
try:
    if sys.argv[1] == "-b": benchmark=1
    elif sys.argv[1] == "-v": verbose=1
except: pass

commands = ["gcc -b", "arith64 -b"] if benchmark else ["gcc", "arith64"]

cases = 0
procs = []
for what in commands:
    procs.append(subprocess.Popen("./test.%s" % what, stdin=subprocess.PIPE, stdout=subprocess.PIPE, shell=True, universal_newlines=True))

if benchmark:
    gccbench={}
    arith64bench={}

# Return a random test value with .01% chance of an edge case
def rand():
    l = randrange(660000)
    if l <= 63: return 2 ** l
    if l == 64: return 0
    if l == 65: return (2 ** 64) - 1
    return randrange(2 ** 64)

if benchmark:
    print("""\
Starting benchmark test. Each test case is performed 1 million times. Reports
total cases and accumulated average execution time for each operation every 5
seconds.
""")
else:
    print("""\
Starting Monte-Carlo validation test. Exits immediately on failure, otherwise
reports total test cases performed every 5 seconds.
""")

next = time.time()+5

while True:
    A = rand()
    B = rand()

    # send to each proc
    for p in procs:
        p.stdin.write("%d %d\n" % (A, B));
        p.stdin.flush()

    # get each response
    out=[]
    for p in procs:
        out.append(p.stdout.readline())

    cases += 1

    if benchmark:
        # C7E48DA4BD40659A 43DC579FF1D129E2: abs=3.18 clzd=1.60 ctzd=1.64 div=4.52 mod=4.17 udiv=10.72 umod=11.93 clzs=1.29 ctsz=1.54 shr=3.16 ashr=3.11 ashl=3.29 ffs=1.65 popd=3.52 pops=2.25
        for op, uS in [t.split('=') for t in out[0].split()[2:]]: gccbench[op]=gccbench.get(op,0)+float(uS)
        for op, uS in [t.split('=') for t in out[1].split()[2:]]: arith64bench[op]=arith64bench.get(op,0)+float(uS)
        if time.time() >= next:
            print("{:,} cases".format(cases))
            print("         %8s %8s" % ("arith64", "gcc"))
            for op in sorted(gccbench.keys()):
                print("  %4s : %6.2fnS %6.2fnS" % (op, arith64bench[op]/cases, gccbench[op]/cases))
            print()
            next += 5
    else:
        if verbose or out[0] != out[1]:
            print("gcc    :",out[0].strip())
            print("arith64:",out[1].strip())
            if not verbose: exit(1)
            print()
        elif time.time() >= next:
            print("{:,} cases".format(cases))
            next += 5
