#!/usr/bin/python
from xml.dom import minidom
from ROOT import TFile, TH1F, TCanvas, TObject
import sys

inputpath="../othercombinationcode/workspaces_Dec11_2014/" #with / at the end
filename=sys.argv[1]
inputpath=sys.argv[2]
#filename=inputpath+"config_2/ll_boost_mH_2011.xml"

def readHistoSys(channel, channelname, samplename,inrootfile):
    syss = channel.getElementsByTagName('HistoSys')
    for sys in syss:
        print "HistoSys ", sys.attributes['Name'].value, channelname, samplename

def readOverallSys(channel, channelname, samplename):
    syss = channel.getElementsByTagName('OverallSys')
    for sys in syss:
        print "OverallSys ", sys.attributes['Name'].value, channelname, samplename, "{:.3f}".format(float(sys.attributes['Low'].value)), "{:.3f}".format(float(sys.attributes['High'].value))

def readNormFactors(channel, channelname, samplename):
    nfs = channel.getElementsByTagName('NormFactor')
    for nf in nfs:
        print "NormFactor ", nf.attributes['Name'].value, channelname, samplename, nf.attributes['Val'].value, nf.attributes['Low'].value, nf.attributes['High'].value

#def readData(channel, channelname):

def readSamples(channel, channelname, inrootfile):
    samples = channel.getElementsByTagName('Sample')
    for sample in samples:
        if sample.hasAttribute("HistoName"):
            histname=sample.attributes['HistoName'].value
            if sample.hasAttribute('HistoPath'):
                histname=sample.attributes['HistoPath'].value+sample.attributes['HistoName'].value
            hist=inrootfile.Get(histname)
            normalisation=hist.GetSumOfWeights()
            #get StatError
            se = sample.getElementsByTagName('StatError')
            staterror="False"
            if len(se)>0:
                staterror=se[0].attributes['Activate'].value
            print "Sample ", sample.attributes['Name'].value, channelname, staterror
            if normalisation!=0: #don't care about systematics, NPs if no events in sample
                readNormFactors(sample,channelname, sample.attributes['Name'].value)
                readOverallSys(sample,channelname, sample.attributes['Name'].value)
                #not "_vzll" in channelname and not "_bzll" in channelname and not "_vtop" in channelname and not "_btop" in channelname: # don't care about histosys in 1 bin channels
                print "JULIAN: ", sample.attributes['Name'].value, hist.GetNbinsX()
                if hist.GetNbinsX()>1: # don't care about histosys in 1 bin channels
                    readHistoSys(sample,channelname, sample.attributes['Name'].value, inrootfile)
                #else:
                #    for sys in syss:
                #        print "IgnoredHistoSys ", sys.attributes['Name'].value, channelname, samplename
            #else
        else:
            print "ERROR: No histogram found for sample ", samplename



def analyzeFile(filename):
    xmldoc = minidom.parse(filename)
    # read the information from the channel XML tag
    channels = xmldoc.getElementsByTagName('Channel')
    for channel in channels:
        print "Channel ", channel.attributes['Name'].value
        if channel.hasAttribute('InputFile'):
            global inrootfile
            channelfilename =channels[0].attributes['InputFile'].value
            inrootfile=TFile.Open(inputpath+channelfilename)
        #readData(channel,channel.attributes['Name'].value)
        readSamples(channel,channel.attributes['Name'].value, inrootfile)
        


analyzeFile(filename)
