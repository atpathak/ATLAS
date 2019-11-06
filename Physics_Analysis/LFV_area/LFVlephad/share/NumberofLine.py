import argparse
import os
import time
import sys
import ROOT

fname = sys.argv[1]
print(fname)
num_lines = 0
with open(fname, 'r') as f:
    for line in f:
        if line.strip() and not '#' in line:
            num_lines += 1
print("Number of lines:" " %d" %(num_lines))
#print(num_lines)
