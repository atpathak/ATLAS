#!/usr/bin/env python2

import sys
import os


def splitPath(filepath):

    dirname, filename = os.path.split(filepath)
    return dirname, filename


def convertWS(filepath):

    dirname, filename = splitPath(filepath)
    infile = TFile(filepath, "read")
    outfile = TFile(dirname+"/convert_"+filename, "recreate")

    channel_list = ["llAll_lfv_inc_signal","llAll_lfv_inc_top","llAll_lfv_vbf_signal","llAll_lfv_vbf_top","llAll_lfv_vbf_ztt","llAll_lfv_inc_ztt"]
    input_sample_list = [["LFV_ggH"], ["LFV_VBFH"], ["VV"], ["Zll","ZllEWK"], ["Ztt","ZttEWK"], ["Fake"], ["Top"], ["ggH","VBFH","WH","ZH","ttH","ggHWW","VBFHWW"],["Data"]]
    sample_list = ["ggH","VBFH","VV","Zll","Ztt","Fake","Top","Htt","Data"]
    n_sample = 9

    for channel in channel_list:
        outfile.cd()
        outfile.mkdir(channel)
        for i in range(0,n_sample):
            outfile.mkdir(channel+"/"+sample_list[i])
            outfile.cd(channel+"/"+sample_list[i])
            file = open("sys_list", "r")
            for variation in file:
                variation = variation.strip()
                histo = None
                for insample in input_sample_list[i]:
                    if insample is 'Fake' and variation != 'll_fake_nonclosure_high' and variation != 'll_fake_nonclosure_low' and variation != 'll_fake_stat_high' and variation != 'll_fake_stat_low' :
                        histo = infile.Get(channel+"/"+insample+"/"+ 'nominal')
                        continue
                    if histo is None:
                        histo = infile.Get(channel+"/"+insample+"/"+variation)
                    if histo:
                        h = infile.Get(channel+"/"+insample+"/"+variation)
                        if h:
                            histo.Add(h)
                if not histo:
                    #err = "no " + sample_list[i] + " " + variation + " in " + channel
                    #print err
                    continue
                histo.Write(variation)
            file.close()
            outfile.cd(channel)
        lumi = TH1F("lumiininvpb","lumiininvpb",1,0,1)
        lumi.SetBinContent(1,36074.16)
        lumi.Write("lumiininvpb")
        outfile.cd("/")
    outfile.Close()


def main(args):
    print "Converting leplep WS input root file into lephad format"
    for path in args:
        convertWS(path)
    print "Done"

if __name__ == "__main__":

    args = sys.argv[1].split(',')
    from ROOT import *
    gROOT.SetBatch(True)
    gStyle.SetOptStat(0)
    main(args)

