import json
import argparse
import os
import time
import errno
import ConfigParser
import sys
import ROOT
from ROOT import TChain, TSelector, TTree

out = open('out.txt', 'a')
results = {}
with open('maps/lfv_lephad.map') as f:
   for line in f:
     if line.strip() and not '#' in line: # this skips empty lines
        line=line.replace('/','_').replace('_$(channel)','').replace('_bkg','bkg').replace('_sig','sig')
        id,sample = line.split(" ")
        results.setdefault(sample.strip(), []).append(int(id.strip()))
#for key in results:
 #   print(key, results[key])
#print(results)
#print(results["bkg_Top_1"][0])
path =[]
path_file = "/eos/atlas/user/a/ademaria/Slim_July_Production/mc_nom/"
for k,v in results.items():
    Entries=[]
    for i in range(0,len(v)):
        path = os.listdir(path_file)
        fname = [f for f in path if str(results[k][i]) in f][0]
        #print(fname)
        root_path = path_file+"/"+fname
        # root_files = [f1 for f1 in os.listdir(root_path) if f1.endswith('.root')]
        root_files = os.listdir(root_path)
        mychain1= TChain("NOMINAL")
        for rootname in root_files:
            mychain1.Add(root_path+"/"+rootname)
        Entries.append(mychain1.GetEntries())
    #print(Entries)
    TotEntries = sum(Entries)
    #print ("%18s   %d" %(k,int(TotEntries)))
    out.write("%18s   %d\n" %(k,int(TotEntries)))
    #print(path)
out.close()
