#!/usr/bin/python3

import random, subprocess, sys;
benchmark = len(sys.argv) > 1
commands = ["gcc -b", "arith64 -b"] if benchmark else ["gcc", "arith64"]

loops = 0
procs = []
for what in commands:
    procs.append(subprocess.Popen("./test64.%s" % what, stdin=subprocess.PIPE, stdout=subprocess.PIPE, shell=True, text=True))

while True:
    a = random.randint(0,2**64-1)
    if loops % 1024 < 2:
        # divide by one
        a = random.randint(0,2**64-1)
        b = 1
    elif loops % 1024 < 4:
        # divide by two
        a = random.randint(0,2**64-1)
        b = 2
    elif loops % 1024 < 32:
        # small numbers
        a = random.randint(0,2**16-1)
        b = random.randint(1,2**16-1) # never 0
    elif loops % 1024 < 60:
        # 32-bit numbers
        a = random.randint(0,2**32-1)
        b = random.randint(1,2**32-1) # never 0
    elif loops % 1024 < 64:
        # huge numerator`
        a = 0xFFFFFFFFFFFFFFFF
        b = random.randint(1,2**32-1) # never 0
    else:
        a = random.randint(0,2**64-1)
        b = random.randint(1,2**64-1) # never 0

    for p in procs:
        p.stdin.write("%d %d\n" % (a,b));
        p.stdin.flush()
    out=[]
    for p in procs:
        out.append(p.stdout.readline())
    if benchmark or out[0] != out[1]:
        print("gcc    :",out[0].strip())
        print("arith64:",out[1].strip())
        if benchmark: print()
        else: exit(1)
    loops += 1
    if not loops % 10000: print("%.2fM" % (loops/1000000.0,))
