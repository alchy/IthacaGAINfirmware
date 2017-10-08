#!/usr/bin/python

import math

correction_factor = 0.96875

corrected = []

for val in range(0,1024):
    corrected.append(int(val * correction_factor) + 32)
    

feed = 0    
for formated_data in corrected:
    
    if feed == 16:
        print "\\"
        feed = 0
    print formated_data, 
    print ', ',
    feed += 1
    
print
print len(corrected)

    

