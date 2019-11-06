#!/usr/bin/env python2

import sys
import os


def splitPath(filepath):

    dirname, filename = os.path.split(filepath)
    return dirname, filename


def convertWS(filepath):

    dirname, filename = splitPath(filepath)
    infile = TFile(filepath, "read")
    outfile = TFile(dirname+"/convert_sys_"+filename, "recreate")

    channel_list = ["mtau_cba_sr1_signal","mtau_cba_sr2_signal","mtau_cba_sr3_signal","mtau_cba_sr1_vbf_signal","mtau_cba_sr2_vbf_signal","mtau_cba_sr3_vbf_signal","mtau_cba_vbf_signal","etau_cba_sr1_signal","etau_cba_sr2_signal","etau_cba_sr3_signal","etau_cba_sr1_vbf_signal","etau_cba_sr2_vbf_signal","etau_cba_sr3_vbf_signal","etau_cba_vbf_signal"]
    input_sample_list = [["ggH"], ["VBFH"], ["VV"], ["Zll"], ["Ztt"], ["Fake"], ["Wjets"], ["Top"], ["Htt"],["Data"],["eFake"],["VH"]]
    sample_list = ["ggH","VBFH","VV","Zll","Ztt","Fake","Wjets","Top","Htt","Data","eFake","VH"]
    n_sample = 12

    for channel in channel_list:
        outfile.cd()
        outfile.mkdir(channel)
        for i in range(0,n_sample):
            outfile.mkdir(channel+"/"+sample_list[i])
            outfile.cd(channel+"/"+sample_list[i])
            file = open("sys_list_lephad", "r")
            for variation in file:
                variation = variation.strip()
                histo = None
                for insample in input_sample_list[i]:
                    histo = infile.Get(channel+"/"+insample+"/"+variation)
                    if not histo:
                        histo.Write(variation)
                        continue
            file.close()
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

