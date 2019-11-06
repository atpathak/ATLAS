import os, argparse, sys, subprocess
from argparse import ArgumentParser
import itertools

import EnvCheck

parser = ArgumentParser()
parser.add_argument( '--channels',   help='Valid names [Zll]', nargs='+', type=str)
parser.add_argument( '--categories', help='Valid names [combined]', nargs='+', type=str)
parser.add_argument( '--data',       help='Valid names [unblind, asimov, hybrid, mublind, rndmmu]', nargs='+', type=str)
parser.add_argument( '--input',      help='Input root file'                , default = "FakeFactors.root", type=str)
parser.add_argument( '--mu',         help='Value of mu to unblind'                       , type=int           )
parser.add_argument( '--version',    help='Version'                                      , type=str           )
parser.add_argument( '--printsh',    help='Print sh'                                     , action='store_true')
parser.add_argument( '--parameters', help='Parameters', nargs='+', type=str)

options = parser.parse_args() 

if options.channels:
    for x in options.channels: 
        if not x in [ "Zll"]:
            sys.exit(x+" not valid! Choose from: Zll, ...")

if options.categories:
    for x in options.categories: 
        if not x in ["combined"]:
            sys.exit(x+" not valid! Choose from: combined")

if options.data:
    for x in options.data: 
        if not x in ["unblind", "asimov", "hybrid", "mublind", "rndmmu"]:
            sys.exit(x+" not valid! Choose from: unblind, asimov, hybrid, mublind, rndmmu")

channels = options.channels if options.channels else ['Zll']
categories     = options.categories  if options.categories  else ['combined']
data     = options.data if options.data else ['asimov']
mu       = options.mu
inputf   = options.input
params   = options.parameters

print "Running over combinations of : "
print "Channels : ", channels
print "Categories : ",  categories
print "Data : ",     data
print "Mu : ",       mu
print "Input : ", inputf
print "Parameters : ", params
#raw_input('ENTER to continue')

nf  = "WorkspaceBuilder/data/Zll/Zll_normfactors13TeV.txt"
of  = "WorkspaceBuilder/data/Zll/Zll_overallsystematics13TeV.txt"
xml = "WorkspaceBuilder/data/Zll/Zll_AllCombination.xml"
outputpath = "workspacesZll"

def makeSubmitStr( x ):

    #the next argument defines the "StatAnalysis" as comes in Zll_AllCombination.xml
    outstr  = "BuildWorkspace -s "
    outstr += "Zll" if x[0] == "Zll" else "Dummy"
    outstr += "Combined" if x[1] == "combined" else "Dummy"

    # other input arguments
    outstr += " -r " + options.input
    outstr += " -v " + options.version
    outstr += " -x " + xml
    outstr += " -n " + nf
    outstr += " -o " + of
    outstr += " -p " + " -p ".join(params) if params else ""
    outstr += " -a "
    if x[2] == "unblind": 
        outstr += " -u "
    else: 
        outstr += " -p BlindFullWithFloat" if x[2] == "asimov" else "-p BlindWithMu="+str(mu) if x[2] == "mublind" else "-p RunRandomMu" if x[2] == "rndmmu" else ""
    outstr += " -q " + outputpath
    #the last argument refers to the analysis as defined in utils/BuildWorkspace.cxx
    outstr += " --fake"
    return outstr

if __name__ == '__main__':
    combinations = itertools.product( channels, categories, data )
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
