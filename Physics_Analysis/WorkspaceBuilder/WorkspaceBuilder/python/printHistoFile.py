#!/usr/bin/python
from ROOT import TFile, TH1F, TCanvas, TObject, gROOT, TDirectory, TH1
import argparse

def printDirectory(directory, prefix, args):
    for key in directory.GetListOfKeys():
        classname = key.GetClassName()
        cl = gROOT.GetClass(classname)
        if (not cl):
            continue
        if (cl.InheritsFrom(TDirectory.Class())):
            print prefix, key.GetName()
            printDirectory(key.ReadObj(),prefix+"   ", args)
        elif (cl.InheritsFrom(TH1.Class())):
            hist=key.ReadObj()
            if args.systematics or hist.GetName()=="nominal" or hist.GetTitle()=="nominal":
                if args.full:
                    print prefix,hist.GetName(),hist.GetSumOfWeights()
                    for i in range(0,hist.GetNbinsX()+2):
                        print prefix,"   ",hist.GetBinContent(i), " +/- ", hist.GetBinError(i)
                else:
                    print prefix,key.GetName(),hist.GetSumOfWeights()
        else:
            print "Unknown", key.GetName()

parser = argparse.ArgumentParser(description='Print number of events from root file')
parser.add_argument('-s','--systematics', help='also print information for systematics', dest='systematics', action='store_true', required=False)
parser.add_argument('-F','--full', help='print all bin contents', dest='full', action='store_true', required=False)
parser.add_argument('-f','--filename', help='filename and path of the root file', required=True)

args = parser.parse_args()

#print args.systematics
#print args.full

#print args

rootfile=TFile(args.filename, "READ")
printDirectory(rootfile,"", args)
