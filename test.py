#!/usr/bin/python3

from random import randint; import subprocess, sys, time;
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

if benchmark:
    gccbench={}
    arith64bench={}
    next = time.time()+5

while True:

    l = loops % 100000
    if l == 0:
        A = 0
    elif l < 65:
        A = (1<<64) >> l
    else:
        A = randint(0, 2 ** 64 -1)

    l = loops % 100001
    if l < 64:
        B = 1 << l
    elif l == 64:
        B = 0
    else:
        B = randint(0, 2 ** 64 -1)

    # send to each proc
    for p in procs:
        p.stdin.write("%d %d\n" % (A, B));
        p.stdin.flush()

    # get each response
    out=[]
    for p in procs:
        out.append(p.stdout.readline())

    if benchmark:
        # C7E48DA4BD40659A 43DC579FF1D129E2: abs=3.18 clzd=1.60 ctzd=1.64 div=4.52 mod=4.17 udiv=10.72 umod=11.93 clzs=1.29 ctsz=1.54 shr=3.16 ashr=3.11 ashl=3.29 ffs=1.65 popd=3.52 pops=2.25
        for op, uS in [t.split('=') for t in out[0].split()[2:]]: gccbench[op]=gccbench.get(op,0)+float(uS)
        for op, uS in [t.split('=') for t in out[1].split()[2:]]: arith64bench[op]=arith64bench.get(op,0)+float(uS)
        if time.time() >= next:
            print("%10d : %8s %8s" % (loops, "arith64", "gcc"))
            for op in sorted(gccbench.keys()):
                print("%10s : %8.2f %8.2f" % (op, arith64bench[op]/loops, gccbench[op]/loops))
            print()
            next += 5
    elif verbose or out[0] != out[1]:
        print("gcc    :",out[0].strip())
        print("arith64:",out[1].strip())
        if not verbose: exit(1)
        print()

    loops += 1
    if  not loops % 10000: print("%.2fM" % (loops/1000000.0,))
