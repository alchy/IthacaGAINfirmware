#!/usr/bin/python
#https://www.desmos.com/calculator/3fisjexbvp

import os
import sys

adsr = []

'''
#this sound ok
a = -1
b = 1.8
c = -256+64+16
'''
a = -1
b = 1.5
c = 0

x = 1
x_step = 0.017

cell = 0

def drange(start, stop, step):
    r = start
    while r < stop:
        yield r
        r += step

    
for cell in range(1, 1025):
    y = a * (b ** x) + c
    x = x + x_step
    #print '%d: %d' %(cell, y)
    adsr.append(y)
    

#sys.exit(0)   
cell = 0
y_divider = adsr[1023] / 1023

for val in adsr:
   
    print "%d, " % (((val / y_divider))),
    cell += 1
   
    if cell > 7:
        cell = 0
        print

print "len: %d" %(len(adsr))
