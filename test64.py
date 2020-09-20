#!/usr/bin/python3

import random, subprocess

loops = 0
procs = []
for what in ["gcc", "arith64"]:
    procs.append(subprocess.Popen("./test64.%s" % what, stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True))

while True:
    a = random.randint(0,2**64-1)
    if loops < 100000:
        b = 1
    elif loops < 200000:
        b = a
    else:
        b = random.randint(1,2**64-1) # never 0
    out=[]
    for p in procs:
        p.stdin.write("%d %d\n" % (a,b));
        p.stdin.flush()
        out.append(p.stdout.readline())
    if out[0] != out[1]:
        print("Fail:");
        print("  gcc    :",out[0].strip())
        print("  arith64:",out[1].strip())
    loops += 1
    if not loops % 10000: print("%.2fM" % (loops/1000000.0,))
