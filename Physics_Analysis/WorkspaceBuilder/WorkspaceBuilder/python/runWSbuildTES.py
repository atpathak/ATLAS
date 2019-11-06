## WS script for the insitu TES
##
## Z. Zinonos zenon@cern.ch
##
##
# /afs/cern.ch/atlas/project/HSG4/HtautauRun2/combination/02Sep/input/

import os, argparse, sys, subprocess
from argparse import ArgumentParser
import itertools

import EnvCheck

parser = ArgumentParser()
parser.add_argument( '--tes',        help='TES', nargs='+', type=str)
parser.add_argument( '--channels',   help='Channel', nargs='+', type=str)
parser.add_argument( '--prongs',     help='Prongs', nargs='+', type=str)
parser.add_argument( '--eta',        help='Eta', nargs='+', type=str)
parser.add_argument( '--pt',         help='pT', nargs='+', type=str)
parser.add_argument( '--data',       help='Data', nargs='+', type=str)
parser.add_argument( '--input',      help='Input root file', type=str)
parser.add_argument( '--mu',         help='Value of mu to unblind', type=int)
parser.add_argument( '--version',    help='Version', type=str)
parser.add_argument( '--printsh',    help='Print sh', action='store_true', default = False)
parser.add_argument( '--envelopes',  help='Systematic envelopes', action='store_true', default = False)
parser.add_argument( '--analyze',    help='Analyze WS', action='store_true', default = False)
parser.add_argument( '--parameters', help='Parameters', nargs='+', type=str)

options = parser.parse_args()

allowed_channels = ["muhad"]
allowed_tes      = ["calo", "mva"]
allowed_prongs   = ["1p", "3p", "1p3p", "1p3p-combined"]
allowed_eta      = ["inclusive", "central", "forward", "central-forward-combined"]
allowed_pt       = ["pTincl", "pTslice1", "pTslice2", "pTslice3", "pTslice4", "pTcomb"]
allowed_data     = ["unblind", "asimov"]

if options.channels:
    for x in options.channels: 
        if not x in allowed_channels:
            sys.exit(x+" not valid! Choose from" + ''.join(allowed_channels) )

if options.prongs:
    for x in options.prongs: 
        if not x in allowed_prongs:
            sys.exit(x+" not valid! Choose from" + ''.join(allowed_prongs) )

if options.tes:
    for x in options.tes: 
        if not x in allowed_tes:
            sys.exit(x+" not valid! Choose from" + ''.join(allowed_tes) )

if options.eta:
    for x in options.eta: 
        if not x in allowed_eta:
            sys.exit(x+" not valid! Choose from" + ''.join(allowed_eta) )

if options.pt:
    for x in options.pt: 
        if not x in allowed_pt:
            sys.exit(x+" not valid! Choose from" + ''.join(allowed_pt) )

if options.data:
    for x in options.data: 
        if not x in allowed_data:
            sys.exit(x+" not valid! Choose from" + ''.join(allowed_data) )

channels = options.channels if options.channels else allowed_channels
prongs   = options.prongs if options.prongs else allowed_prongs
tes      = options.tes  if options.tes else allowed_tes
eta      = options.eta  if options.eta else allowed_eta
pt       = options.pt  if options.pt else allowed_pt
data     = options.data if options.data else allowed_data
mu       = options.mu
inputf   = options.input
params   = options.parameters
env      = options.envelopes
analyze  = options.analyze

print "Running over combinations of : "
print "Channels : ", channels
print "Prongs : ", prongs
print "Tes : ",  tes
print "Eta : ",  eta
print "pT : ",  pt
print "Data : ",     data
print "Mu : ",       mu
print "Input : ", inputf
print "Parameters : ", params
print "Envelopes : ", env
print "Analysis : ", analyze

nf  = "WorkspaceBuilder/data/TES/TES_normfactors13TeV.txt"
of  = "WorkspaceBuilder/data/TES/TES_overallsystematics13TeV.txt"
xml = "WorkspaceBuilder/data/TES/TES_AllCombination.xml"
outputpath = "workspacesTES"

def makeSubmitStr( x ):

    # the next argument defines the "StatAnalysis" as comes in TES_AllCombination.xml
    stat_analysis = "TES"
    stat_analysis += "MVA" if x[0] == "mva" else "Calo" if x[0] == "calo" else "Dummy"
    stat_analysis += "_"
    stat_analysis += "Channel"
    stat_analysis += "MuHad" if x[1] == "muhad" else "EleHad" if x[1] == "elehad"  else "Dummy"
    stat_analysis += "_"
    stat_analysis += "1P" if x[2] == "1p" else "3P" if x[2] == "3p" else "1P3PCombined" if x[2] == "1p3p-combined" else "Dummy"
    stat_analysis += "_"
    stat_analysis += "InclEta" if x[3] == "inclusive" else "CentEta" if x[3] == "central" else "ForwEta" if x[3] == "forward" \
        else "CentForwEtaCombined" if x[3] == "central-forward-combined" else "Dummy"
    stat_analysis += "_"
    stat_analysis += "pTincl" if x[4] == "pTincl" \
        else "pTslice1" if x[4] == "pTslice1" \
        else "pTslice2" if x[4] == "pTslice2" \
        else "pTslice3" if x[4] == "pTslice3" \
        else "pTslice4" if x[4] == "pTslice4" \
        else "pTcomb" if x[4] == "pTcomb" \
        else "Dummy"


    #executable
    outstr  = "BuildWorkspace"
    outstr += " -s " + stat_analysis
    # other input arguments
    outstr += " -r " + options.input
    outstr += " -v " + options.version
    outstr += " -x " + xml
    outstr += " -n " + nf
    outstr += " -o " + of
    outstr += " -p " + " -p ".join(params) if params else ""
    if x[2] == "unblind": 
        outstr += " -u "
    else: 
        outstr += " -p BlindFullWithFloat" if x[2] == "asimov" else "-p BlindWithMu="+str(mu) if x[2] == "mublind" else "-p RunRandomMu" if x[2] == "rndmmu" else ""
    outstr += " -q " + outputpath
    # the last argument refers to the analysis as defined in utils/BuildWorkspace.cxx 
    outstr += " --mvates "
    outstr += " --envelope " if env else ""
    outstr += " --analyze " if analyze else ""

    return outstr

if __name__ == '__main__':
    combinations = itertools.product( tes, channels, prongs, eta, pt, data )
    commands = map( makeSubmitStr, combinations )

    if options.printsh: 
        print "Info: Writing to file", options.version+'.sh'
        outfile = open( options.version+".sh", "w" )
        for cmd in commands: outfile.write(cmd+'\n') 
        outfile.close()
        os.system("chmod +x "+options.version+".sh")
        os.system("./"+options.version+".sh")
    else: 
        for cmd in commands: 
            print cmd
            os.system(cmd)
