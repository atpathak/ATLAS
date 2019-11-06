#!/usr/bin/python
from xml.dom import minidom
from ROOT import TFile, TH1F, TCanvas, TObject
from sets import Set

inputpath="../othercombinationcode/workspaces_Dec11_2014/" #with / at the end

allsamples={}
allsyst=Set()

channelconvertmap={ "ll11_boost":"ll11_boost_signal",
                    "ll12_boost":"ll12_boost_signal",
                    "ll11_btop" :"ll11_boost_top",
                    "ll12_btop" :"ll12_boost_top",
                    "ll11_bzll" :"ll11_boost_zll",
                    "ll12_bzll" :"ll12_boost_zll",
                    "ll11_vbf"  :"ll11_vbf_signal",
                    "ll12_vbf"  :"ll12_vbf_signal",
                    "ll11_vtop" :"ll11_vbf_top",
                    "ll12_vtop" :"ll12_vbf_top",
                    "ll11_vzll" :"ll11_vbf_zll",
                    "ll12_vzll" :"ll12_vbf_zll",
                    "lh11_boost":"lh11_boost_signal",
                    "lh12_boost":"lh12_boost_signal",
                    "lh11_btop" :"lh11_boost_top",
                    "lh12_btop" :"lh12_boost_top",
                    "lh11_vbf"  :"lh11_vbf_signal",
                    "lh12_vbf"  :"lh12_vbf_signal",
                    "lh11_vtop" :"lh11_vbf_top",
                    "lh12_vtop" :"lh12_vbf_top"
                   }

sampleconvertmap={"VBF" : "VBFH", "Zl2t" : "Zll"}

def getNewChannelName(oldchannelname):
    if oldchannelname in channelconvertmap:
        return channelconvertmap[oldchannelname]
    return oldchannelname

def getNewSampleName(oldsamplename):
    if oldsamplename in sampleconvertmap:
        return sampleconvertmap[oldsamplename]
    return oldsamplename

def getSamples(syst,filename):
    ret=[]
    xmldoc = minidom.parse(filename)
    # read the information from the channel XML tag
    channels = xmldoc.getElementsByTagName('Channel')
    for channel in channels:
        channelname=getNewChannelName(channel.attributes['Name'].value)
        samplelist = channel.getElementsByTagName('Sample')
        for sample in samplelist:
            samplename=getNewSampleName(sample.attributes['Name'].value)
            os=False
            hs=False
            overallsyslist = sample.getElementsByTagName('OverallSys')
            for sys in overallsyslist:
                if sys.attributes['Name'].value==syst:
                    os=True
            histosyslist = sample.getElementsByTagName('HistoSys')
            for sys in histosyslist:
                if sys.attributes['Name'].value==syst:
                    hs=True
            if hs and os:
                ret.append((channelname, samplename, "OH"))
            elif hs:
                ret.append((channelname, samplename, "H"))
            elif os:
                ret.append((channelname, samplename, "O"))
    return ret

def findAllSyst(filename):
    xmldoc = minidom.parse(filename)
    # read the information from the channel XML tag
    channels = xmldoc.getElementsByTagName('Channel')
    for channel in channels:
        channelname=channel.attributes['Name'].value
        samplelist = channel.getElementsByTagName('Sample')
        samples=[]
        for sample in samplelist:
            samples.append(sample.attributes['Name'].value)
            overallsyslist = sample.getElementsByTagName('OverallSys')
            for sys in overallsyslist:
                allsyst.add(sys.attributes['Name'].value)
            histosyslist = sample.getElementsByTagName('HistoSys')
            for sys in histosyslist:
                allsyst.add(sys.attributes['Name'].value)
        allsamples[channelname]=samples

#

filenames=[ "workspacesTES/calo_tes_20170702/xml/TESCalo_ChannelMuHad_1P_CentEta_pTincl_TESCalo_ChannelMuHad_1P_CentEta_pTincl_RegSignal.xml"]

for filename in filenames:
    findAllSyst(filename)

l_allsys=list(allsyst)
sorted(l_allsys)

samples=[]
for syst in l_allsys:
    appsamples=[]
    for filename in filenames:
        appsamples=appsamples+getSamples(syst,filename)
    samples.append((syst,appsamples))
    for filename in filenames:
        findAllSyst(filename)

for syst in samples:

    print "  <Systematic Name=\""+syst[0]+"\">"
    print "    <SystematicNameInFile>"+syst[0]+"</SystematicNameInFile>"
    
    samples=[]
    for sample in syst[1]:
        samples.append((sample[1],sample[0]))
    samples.sort(key=lambda tup: tup[0]+tup[1], reverse=True)

    for sample in samples:
        print "    <ApplyToInputSample InputChannel=\""+sample[1]+"\">"+sample[0]+"</ApplyToSample>"
    print "  </Systematic>\n"

print allsamples
