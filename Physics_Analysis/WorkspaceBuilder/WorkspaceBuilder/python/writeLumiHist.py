#!/usr/bin/python

from ROOT import TFile, TH1F, TCanvas, TObject

lumi={'leplep':3210,'lephad':3210,'hadhad':3210,'7TeV':4600,'8TeV':20300,'hh15_cba':3210,"lh15_mva":3210}

 #in pb
analyses = {'leplep': ['ll15_boost_signal','ll15_boost_top','ll15_boost_zll','ll15_vbf_signal','ll15_vbf_top','ll15_vbf_zll'], 'lephad': ['eh15_boost_signal','eh15_boost_top','eh15_vbf_signal','eh15_vbf_top','mh15_boost_signal','mh15_boost_top','mh15_vbf_signal','mh15_vbf_top',], 'hadhad': ['hh_boost','hh_vbf','hh_rest'], '7TeV':["hh11_boost","hh11_rest","hh11_vbf","lh11_boost_signal","lh11_boost_top","lh11_vbf_signal","lh11_vbf_top","ll11_boost_signal","ll11_boost_top","ll11_boost_zll","ll11_vbf_signal","ll11_vbf_top","ll11_vbf_zll"],'8TeV':["hh12_boost","hh12_rest","hh12_vbf","lh12_boost_signal","lh12_boost_top","lh12_vbf_signal","lh12_vbf_top","ll12_boost_signal","ll12_boost_top","ll12_boost_zll","ll12_vbf_signal","ll12_vbf_top","ll12_vbf_zll"]}
analyses = {'leplep': ['ll15_boost_signal','ll15_boost_top','ll15_boost_zll','ll15_vbf_signal','ll15_vbf_top','ll15_vbf_zll'], 'lephad': ['eh_boost_signal','eh_boost_top','eh_vbf_loose_signal','eh_vbf_tight_signal','eh_vbf_top','mh_boost_signal','mh_boost_top','mh_vbf_loose_signal','mh_vbf_tight_signal','mh_vbf_top',], 'hadhad': ['hh_boost','hh_vbf','hh_rest'], '7TeV':["hh11_boost","hh11_rest","hh11_vbf","lh11_boost_signal","lh11_boost_top","lh11_vbf_signal","lh11_vbf_top","ll11_boost_signal","ll11_boost_top","ll11_boost_zll","ll11_vbf_signal","ll11_vbf_top","ll11_vbf_zll"],'8TeV':["hh12_boost","hh12_rest","hh12_vbf","lh12_boost_signal","lh12_boost_top","lh12_vbf_signal","lh12_vbf_top","ll12_boost_signal","ll12_boost_top","ll12_boost_zll","ll12_vbf_signal","ll12_vbf_top","ll12_vbf_zll"], 'hh15_cba':["hh15_vbf_lowdr","hh15_vbf_highdr_loose","hh15_vbf_highdr_tight", "hh15_boost_tight","hh15_boost_loose"],'lh15_mva':["lh15_boost_signal","lh15_vbf_signal"]}

for analysis in analyses:
    rootfile=TFile.Open("lumi_"+analysis+".root","RECREATE")
    channels=analyses[analysis]
    for channel in channels:
        rootfile.mkdir(channel)
        rootdir=rootfile.Get(channel)
        rootdir.cd()
        hist=TH1F("lumiininvpb","lumiininvpb",1,-0.5,0.5)
        hist.SetBinContent(1,lumi[analysis])
        hist.Write()
    rootfile.Write()
    rootfile.Close
