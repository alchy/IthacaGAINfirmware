#!/usr/bin/python

import math
from numpy import arange

charge = []
R = 1
C = 0.1

Vc = 0.0
for t in arange(0, 1.1, 1 / (1024.0 * 1.1)):
    Vc = 1 - math.exp(-t/(R*C))
    charge.append(Vc)

pos = 0
spl = 0
top1023 = []
top1023 = charge[:1024]
#top1023.reverse()

for item in top1023:
    print "%d, " % (((1 - item) * 1048575)),
    pos += 1
    spl += 1
    if spl > 15:
        spl = 0
        print
print
print "elements: %d" % len(top1023)