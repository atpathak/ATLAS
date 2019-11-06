import os, argparse, sys, subprocess
from argparse import ArgumentParser
import itertools

import EnvCheck

parser = ArgumentParser()
parser.add_argument( '--channels',   help='Valid names [lh, mh, eh]', nargs='+', type=str)
parser.add_argument( '--tauid',      help='Valid names [notloose, loosenotmedium, mediumnottight, tight]', nargs='+', type=str)
parser.add_argument( '--prong',      help='Valid names [1p, 3p]', nargs='+', type=str)
parser.add_argument( '--taupt',      help='Valid names [2025, 2530, 2030, 3040, 40]', nargs='+', type=str)
parser.add_argument( '--data',       help='Valid names [unblind, asimov, hybrid, mublind, rndmmu]', nargs='+', type=str)
parser.add_argument( '--input',      help='Input root file'                , type=str)
parser.add_argument( '--mu',         help='Value of mu to unblind'                       , type=int           )
parser.add_argument( '--version',    help='Version'                                      , type=str           )
parser.add_argument( '--printsh',    help='Print sh'                                     , action='store_true', default = False)
parser.add_argument( '--envelopes',  help='Print sh'                                     , action='store_true', default = False)
parser.add_argument( '--parameters', help='Parameters', nargs='+', type=str)

options = parser.parse_args() 

if options.channels:
    for x in options.channels: 
        if not x in ["lh", "mh", "eh"]:
            sys.exit(x+" not valid! Choose from lh, mh, eh")

if options.tauid:
    for x in options.tauid: 
        if not x in ["notloose","loosenotmedium","mediumnottight","tight"]:
            sys.exit(x+" not valid! Choose from notloose, loosenotmedium, mediumnottight, tight")

if options.prong:
    for x in options.prong:
        if not x in ["1p","3p"]:
            sys.exit(x+" not valid! Choose from 1p, 3p")

if options.taupt:
    for x in options.taupt:
        if not x in ["2025","2530","2030","3040","40"]:
            sys.exit(x+" not valid! Choose from 2025, 2530, 2030, 3040, 40")

if options.data:
    for x in options.data: 
        if not x in ["unblind", "asimov", "hybrid", "mublind", "rndmmu"]:
            sys.exit(x+" not valid! Choose from unblind, asimov, hybrid, mublind, rndmmu")

channels = options.channels if options.channels else ['lh', 'eh', 'mh']
tauid    = options.tauid  if options.tauid  else ['notloose', 'loosenotmedium', 'mediumnottight', 'tight']
prong    = options.prong  if options.prong  else ['1p', '3p']
taupt    = options.taupt  if options.taupt  else ['2025', '2530', '3040', '40']
data     = options.data if options.data     else ['asimov']
mu       = options.mu
inputf   = options.input
params   = options.parameters
env = options.envelopes

print "Running over combinations of : "
print "Channels : ", channels
print "TauID : ",    tauid
print "Prong : ",    prong
print "TauPt : ",    taupt
print "Data : ",     data
print "Mu : ",       mu
print "Input : ",    inputf
print "Parameters : ", params
raw_input('ENTER to continue')

## in for merged, 15 and 16 for split
# /afs/cern.ch/atlas/project/HSG4/HtautauRun2/combination/02Sep/input/

nf  = "WorkspaceBuilder/data/TauID/TauID_normfactors13TeV.txt"
of  = "WorkspaceBuilder/data/TauID/TauID_overallsystematics13TeV.txt"
xml = "WorkspaceBuilder/data/TauID/TauID_AllCombination.xml"
outputpath = "workspacesTauID"

def makeSubmitStr( x ):
  
    # the next argument defines the "StatAnalysis" as comes in TauID_AllCombination.xml
    outstr  = "BuildWorkspace -s TauID"
    outstr += "_MuHad" if x[0] == "mh" else "_ElHad" if x[0] == "eh" else "Combined_" if x[0] == "lh" else "Dummy"
    outstr += "_1P" if x[2] == "1p" else "_3P" if x[2] == "3p" else ""
    outstr += "_2025" if x[3] == "2025" else "_2530" if x[3] == "2530" else "_2030" if x[3] == "2030" else "_3040" if x[3] == "3040" else "_40" if x[3] == "40" else "_50" if x[3] == "50" else "_All" if x[3] == "All" else ""
    outstr +=  "_NotLooseID" if x[1] == "notloose" else "_LooseNotMediumID" if x[1] == "loosenotmedium" else "_MediumNotTightID" if x[1] == "mediumnottight"  else "_TightID" if x[1] == "tight" else "" 
    # other input arguments
    outstr += " -r " + options.input
    outstr += " -v " + options.version
    outstr += " -x " + xml
    outstr += " -n " + nf
    outstr += " -o " + of
    outstr += " -p " + " -p ".join(params) if params else ""
    outstr += " -a "
    if x[4] == "unblind": 
        outstr += " -u "
    else: 
        outstr += " -p BlindFullWithFloat" if x[4] == "asimov" else "-p BlindWithMu="+str(mu) if x[4] == "mublind" else "-p RunRandomMu" if x[4] == "rndmmu" else ""
    outstr += " -q " + outputpath
    # the last argument refers to the analysis as defined in utils/BuildWorkspace.cxx 
#    outstr += " -p AggressiveSymmLocal"
    outstr += " --tauid "
    outstr += " --envelope " if env else ""

    return outstr

if __name__ == '__main__':
    combinations = itertools.product( channels, tauid, prong, taupt, data )
    commands = map( makeSubmitStr, combinations )
    if options.printsh: 
        print "Writing to file", options.version+'.sh'
        outfile = open( options.version+".sh", "w" )
        for cmd in commands: outfile.write(cmd+'\n') 
        outfile.close()
        os.system("chmod +x "+options.version+".sh")
        os.system("./"+options.version+".sh")
    else: 
        for cmd in commands: 
            print cmd
            os.system(cmd)
