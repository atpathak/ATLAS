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
    outfile = TFile(dirname+"/convert_bining_"+filename, "recreate")

    channel_list = ["mtau_cba_sr1_signal","mtau_cba_sr2_signal","mtau_cba_sr3_signal","mtau_cba_sr1_vbf_signal","mtau_cba_sr2_vbf_signal","mtau_cba_sr3_vbf_signal","mtau_cba_vbf_signal","etau_cba_sr1_signal","etau_cba_sr2_signal","etau_cba_sr3_signal","etau_cba_sr1_vbf_signal","etau_cba_sr2_vbf_signal","etau_cba_sr3_vbf_signal","etau_cba_vbf_signal"]
    sample_list = ["ggH","VBFH","VV","Zll","Ztt","Fake","Wjets","Top","Htt","Data","eFake","VH"]
    n_sample = 12
    
    for channel in channel_list:
        outfile.cd()
        outfile.mkdir(channel)
        for sample in sample_list: 
            outfile.mkdir(channel+"/"+sample)
            outfile.cd(channel+"/"+sample)
            d = infile.Get(channel+"/"+sample)
            #print(d)
            for key in d.GetListOfKeys():
                histo1 = d.Get(key.GetName())
                histo2 = histo1.Rebin(9,key.GetName(),array.array('d',[60.0, 80.0, 90.0, 100.0, 110.0, 130.0, 150.0, 180.0, 190.0, 200.0]))
                histo2.Write()
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
