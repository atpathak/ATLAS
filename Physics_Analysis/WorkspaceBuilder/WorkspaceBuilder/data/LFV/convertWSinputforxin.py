#!/usr/bin/env python2

import sys
import os
import array


def splitPath(filepath):

    dirname, filename = os.path.split(filepath)
    return dirname, filename


def convertWS(filepath):

    dirname, filename = splitPath(filepath)
    infile = TFile(filepath, "read")
    outfile = TFile(dirname+"/convert_"+filename, "recreate")

    channel_list = ["llAll_lfv_inc_signal","llAll_lfv_inc_top","llAll_lfv_vbf_signal","llAll_lfv_vbf_top","llAll_lfv_vbf_ztt","llAll_lfv_inc_ztt"]
    input_sample_list = [["sig"],["VV"],["zll"],["ztt"],["Fake"],["Top"],["other"],["Data"]]
    sample_list = ["sig","VV","Zll","Ztt","Fake","Top","Htt","Data"]
    n_sample = 8
    
    for channel in channel_list:
        outfile.cd()
        outfile.mkdir(channel)
        for sample,insample in zip(sample_list,input_sample_list):
            outfile.mkdir(channel+"/"+sample)
            outfile.cd(channel+"/"+sample)
            print(channel)
            print(sample)
            f = infile.Get(channel+"/"+insample[0])
            print(f)
            for key in f.GetListOfKeys():
                histo = f.Get(key.GetName())
                histo.Write()
            outfile.cd(channel)
        lumi = TH1F("lumiininvpb","lumiininvpb",1,0,1)
        lumi.SetBinContent(1,36074.16)
        lumi.Write("lumiininvpb")
        outfile.cd("/")
    outfile.Close()

    
def main(args):
    print "Converting leplep WS input root file into lephad format"
    #for path in args:
    convertWS(args)
    print "Done"

if __name__ == "__main__":

    args = sys.argv[1]
    from ROOT import *
    gROOT.SetBatch(True)
    gStyle.SetOptStat(0)
    main(args)

