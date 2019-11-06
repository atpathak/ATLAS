#!/usr/bin/env python

import os
import sys
import ROOT
import argparse


parser = argparse.ArgumentParser()
parser.add_argument('ifile')
args=parser.parse_args()

#Set the path to the scripts
#path="/home/luke/atlas/work/httll/ichep2016/fixsys"
path="../scripts"

outRebinpre = (args.ifile.split(".root"))[0]
outRebin = (args.ifile.split(".root"))[0]+".rebinALGO.root"
os.system("python "+path+"/rebinFile_for_merged_2017_llALGO.py -i "+args.ifile+" -o "+outRebin)
os.system("echo 'gSystem->Load(\""+path+"/Make_MurMuf_And_Shower_Theory_Envelope_C.so\"); MakeTheoryAndShowerEnvelope(\""+outRebin +"\",2)' | root -l -b")
os.system("echo 'gSystem->Load(\""+path+"/Make_JER_Envelope_C.so\"); MakeJERFinalVar(\""+outRebinpre+".rebinALGO_WithTheoryEnv.root\")' | root -l -b")
os.system("python "+path+"/shapes_fake_stat.py "+outRebinpre+".rebinALGO_WithTheoryEnv_WithModJERVar.root "+outRebinpre+".rebinALGO_WithTheoryEnv_WithModJERVar_fakes.root") 
os.system("python "+path+"/ZttTheorySyst.py -ws "+outRebinpre+".rebinALGO_WithTheoryEnv_WithModJERVar_fakes.root")
os.system("python "+path+"/ZttTheoryMGvsShShape.py -ws "+outRebinpre+".rebinALGO_WithTheoryEnv_WithModJERVar_fakes_zttTheoryConstrained.root --ll_boost_tight_theory_ztt_MGvsSh_high 1.0257614422  --ll_boost_loose_theory_ztt_MGvsSh_high 0.9854057675  --ll_vbf_tight_theory_ztt_MGvsSh_high 0.9469958981  --ll_vbf_loose_theory_ztt_MGvsSh_high 1.0249683917")

