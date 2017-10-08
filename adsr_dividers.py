#!/usr/bin/python
''' discharging capacitor with natural logarithm '''

DEBUG = False

t = 0.0
v_in = 1.0
c = 10
dividers = []

start = 0.0
stop = 1073741823.0 / 2.4
step = 0.0125	

def drange(start, stop, step):
    r = start
    while r < stop:
        yield r
        r += step

cycle = 0
for t in drange(start, stop, step):
    v_out = stop * (2.7182818284 ** -t)
    #print v_out
    dividers.append(v_out)
    cycle += 1
    if cycle > 1023:
        break


cell = 0
line = 0
for val in dividers:
   
    print "%d, " % val,
    cell += 1

    if cell > 7:
        cell = 0
        line += 1
        print

        if DEBUG:
            print '%d:' % line,

print 'LEN: ', len(dividers)