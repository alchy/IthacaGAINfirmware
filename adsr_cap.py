#!/usr/bin/python
''' discharging capacitor with natural logarithm '''

DEBUG = False

t = 0.0
v_in = 1.0
c = 10
discharge = []

start = 0.0
stop = 1023.0
step = 0.7

def drange(start, stop, step):
    r = start
    while r < stop:
        yield r
        r += step

for t in drange(start, stop, step):
    v_out = v_in * (1.0 - 2.7182818284 ** (-t * 10.0/1023.0))
    discharge.append(1023 - int(v_out * 1024))

cell = 0
line = 0

if DEBUG:
    print '0:',
        
no_zeroes = discharge[:1024]
for val in reversed(no_zeroes):
   
    print "%d, " % val,
    cell += 1
   
    if cell > 7:
        cell = 0
        line += 1
        print

        if DEBUG:
            print '%d:' % line,
        
print
print "len: %d" %(len(no_zeroes))
