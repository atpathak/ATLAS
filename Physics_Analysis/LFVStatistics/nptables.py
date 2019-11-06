#!/usr/bin/env python
import json
import argparse
import os
import time
import errno
import ConfigParser
import sys
import ROOT
import re
import subprocess
import operator
from ROOT import TChain, TSelector, TTree

np_list = []
ws_list = ["lhinc_mtau_mva","lhvbf_mtau_mva","lh_mtau_mva"] #,"lhvbf_mtau_mva_decorr","lh_mtau_mva_decorr"]
value=[]
errhigh=[]
errlow =[]
with open('compare_NPscan_test_summary.txt') as f:
    for line in f:
        if line.strip() and not '#' in line: # this skips empty lines
            np_list.append(line.strip())
#print(np_list)

for nps in np_list:
    for ws in ws_list:
        logs = open("log/scans/"+ws+"/NPscan_"+nps+"_-1.5_1.5.log", "r")
        for line in logs:
            if "FitVal" in line:
                mystring = line
                value.append(mystring.split(" ")[6])
                errhigh.append(mystring.split(" ")[7])
                errlow.append(mystring.rstrip('\n').split(" ")[10])
    print value
    print (value[0]"\boldmath${^{"errhigh[0]"}_{"errlow[0]"}}$")
    #print value[0]
    #print value[1]
    #print value[2]
    del value[:]
    del errhigh[:]
    del errlow[:]

    

