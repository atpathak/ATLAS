#!/usr/bin/python
from xml.dom import minidom
from ROOT import TFile, TH1F, TCanvas, TObject

channelfilename=""
channelname="lh11_boost"
variablename="BDT"
channelconvertmap={"lh11_boost":"lh_boost"}
sampleconvertmap={"VBF" : "VBFH", "Zl2t":"Zll"}
systematicconvertmap={"from" : "to"}
inputpath="../../othercombinationcode/workspaces_Dec11_2014/" #with / at the end

def readChannel(xmldoc):
    # read the information from the channel XML tag and set the channelname, channelfilename and return a root file pointer
    channels = xmldoc.getElementsByTagName('Channel')
    if channels[0].hasAttribute('Name'):
        global channelname
        channelname=channels[0].attributes['Name'].value
        if channels[0].hasAttribute('InputFile'):
            global channelfilename
            channelfilename =channels[0].attributes['InputFile'].value
            channelrootfile=TFile.Open(inputpath+channelfilename)
            print "Found channel ", channelname, " with file ", channelfilename
            return channelrootfile

    return 0

def readData(xmldoc, inrootfile, outrootfile,newchannelname):
    # read the information from the data XML tag and write the histogram to the new output file
    data = xmldoc.getElementsByTagName('Data')
    if data[0].hasAttribute('HistoName'):
        histname=data[0].attributes['HistoName'].value
        if data[0].hasAttribute('HistoPath'):
            histname=data[0].attributes['HistoPath'].value+data[0].attributes['HistoName'].value
        hist=inrootfile.Get(histname)
    channeldir=outrootfile.Get(newchannelname)
    datadir=channeldir.mkdir("Data")
    #systematicdir=channeldir.mkdir("JES_High")
    #systematicdir=channeldir.mkdir("JES_Low")
    #systematicdir=channeldir.mkdir("EL_EFF_High")
    #systematicdir=channeldir.mkdir("EL_EFF_Low")
    #systematicdir=channeldir.mkdir("nominal")
    datadir.cd()
    hist.SetDirectory(datadir)
    hist.SetName("nominal")
    hist.Write()

def readSample(xmlsample, inrootfile, outrootfile,newchannelname):
    # reads the data from teh Sample tag, writes the histogram and initiates the systematics treatment
    samplename=xmlsample.attributes['Name'].value

    if xmlsample.hasAttribute("HistoName"):
        histname=xmlsample.attributes['HistoName'].value
        if xmlsample.hasAttribute('HistoPath'):
            histname=xmlsample.attributes['HistoPath'].value+xmlsample.attributes['HistoName'].value
        hist=inrootfile.Get(histname)
        channeldir=outrootfile.Get(newchannelname)
        newsamplename=getNewSampleName(samplename)
        sampledir=channeldir.mkdir(newsamplename)
        sampledir.cd()
        hist.SetDirectory(sampledir)
        hist.SetName("nominal")
        hist.Write()
        normalisation=hist.GetSumOfWeights()
        JESUphist=hist.Clone()
        JESUphist.SetDirectory(sampledir)
        JESUphist.SetName("jet_es_high")
        JESUphist.SetBinContent(1,1.1*JESUphist.GetBinContent(1))
        JESUphist.SetBinContent(2,1.1*JESUphist.GetBinContent(2))
        JESUphist.SetBinContent(3,1.1*JESUphist.GetBinContent(3))
        JESUphist.Write()
        JESDownhist=hist.Clone()
        JESDownhist.SetDirectory(sampledir)
        JESDownhist.SetName("jet_es_low")
        JESDownhist.SetBinContent(1,0.9*JESDownhist.GetBinContent(1))
        JESDownhist.SetBinContent(2,0.9*JESDownhist.GetBinContent(2))
        JESDownhist.SetBinContent(3,0.9*JESDownhist.GetBinContent(3))
        JESDownhist.Write()
        ELUphist=hist.Clone()
        ELUphist.SetDirectory(sampledir)
        ELUphist.SetName("el_eff_id_high")
        ELUphist.SetBinContent(4,1.2*ELUphist.GetBinContent(4))
        ELUphist.SetBinContent(5,1.2*ELUphist.GetBinContent(5))
        ELUphist.Write()
        ELDownhist=hist.Clone()
        ELDownhist.SetDirectory(sampledir)
        ELDownhist.SetName("el_eff_id_low")
        ELDownhist.SetBinContent(4,0.8*ELDownhist.GetBinContent(4))
        ELDownhist.SetBinContent(5,0.8*ELDownhist.GetBinContent(5))
        ELDownhist.Write()

        #readSystematicsForSample(xmlsample,normalisation,inrootfile,outrootfile,newchannelname,newsamplename)
    else:
        print "ERROR: No histogram found for sample ", samplename

def writeSystHisto(systname,inrootfile,outrootfile,histname,newchannelname,newsamplename):
    hist=inrootfile.Get(histname)
    channeldir=outrootfile.Get(newchannelname)
    #sampledir=channeldir.Get(newsamplename)
    systematicdir=channeldir.Get(systname)
    if systematicdir==None:
        systematicdir=channeldir.mkdir(systname)
    systematicdir.cd()
    hist.SetDirectory(systematicdir)
    hist.SetName(newsamplename)
    hist.Write()

def getHistoSys(xmlsample,normalisation,inrootfile,systematicname):
    histosyslist = xmlsample.getElementsByTagName('HistoSys')
    for histosys in histosyslist:
        if histosys.attributes["Name"].value==systematicname:
            histnamehigh=histosys.attributes["HistoNameHigh"].value
            if histosys.hasAttribute('HistoPathHigh'):
                histnamehigh=histosys.attributes['HistoPathHigh'].value+histosys.attributes['HistoNameHigh'].value
            if histosys.hasAttribute("HistoFileHigh"):
                tmprootfile=TFile.Open(inputpath+histosys.attributes["HistoFileHigh"].value,"READ")
                highhist=tmprootfile.Get(histnamehigh)
                highhist.SetDirectory(0)
                tmprootfile.Close()
            else:
                highhist=inrootfile.Get(histnamehigh)
                highhist.SetDirectory(0)
            
            histnamelow=histosys.attributes["HistoNameLow"].value
            if histosys.hasAttribute('HistoPathLow'):
                histnamelow=histosys.attributes['HistoPathLow'].value+histosys.attributes['HistoNameLow'].value
            if histosys.hasAttribute("HistoFileLow"):
                tmprootfile=TFile.Open(inputpath+histosys.attributes["HistoFileLow"].value,"READ")
                lowhist=tmprootfile.Get(histnamelow)
                lowhist.SetDirectory(0)
                #writeSystHisto(newsystematicname+"Low",tmprootfile,outrootfile,histnamelow,newchannelname,newsamplename)
                tmprootfile.Close()
            else:
                lowhist=inrootfile.Get(histnamelow)
                lowhist.SetDirectory(0)
                #writeSystHisto(newsystematicname+"Low",inrootfile,outrootfile,histnamelow,newchannelname,newsamplename)
            return [lowhist, highhist]

    lowhist=TH1F(xmlsample.attributes['Name'].value+systematicname+"_Low",xmlsample.attributes['Name'].value+systematicname+"_Low",1,-0.5,0.5)
    lowhist.SetDirectory(0)
    lowhist.SetBinContent(1,normalisation)
    highhist=TH1F(xmlsample.attributes['Name'].value+systematicname+"_High",xmlsample.attributes['Name'].value+systematicname+"_High",1,-0.5,0.5)
    highhist.SetDirectory(0)
    highhist.SetBinContent(1,normalisation)
    return [lowhist,highhist]
                    
def getOverallSys(xmlsample,systematicname):
    overallsyslist = xmlsample.getElementsByTagName('OverallSys')
    for overallsys in overallsyslist:
        if overallsys.attributes["Name"].value==systematicname:
            high=float(overallsys.attributes["High"].value)
            low=float(overallsys.attributes["Low"].value)
            return [low,high]
    return [1.,1.]

def readSystematicsForSample(xmlsample,normalisation,inrootfile,outrootfile,newchannelname,newsamplename):
    # reads the HistoSys and OverallSys tags and writes histograms to the output root file
    
    # get the list of systematics
    systematics=[]
    histosyslist = xmlsample.getElementsByTagName('HistoSys')
    for histosys in histosyslist:
        systematicname=histosys.attributes["Name"].value
        systematics.append(systematicname)
    overallsyslist = xmlsample.getElementsByTagName('OverallSys')
    for overallsys in overallsyslist:
        systematicname=overallsys.attributes["Name"].value
        systematics.append(systematicname)

    print "Writing sample ", newsamplename, " with systematics ",  systematics

    for systematicname in systematics:
        newsystematicname=getNewSystematicName(systematicname)
        histosys=getHistoSys(xmlsample,normalisation,inrootfile,systematicname)
        overallsys=getOverallSys(xmlsample,systematicname)

        inrootfile.cd()
        #scale the histograms
        histosys[0].Scale(overallsys[0])
        histosys[1].Scale(overallsys[1])

        #write these histograms
        channeldir=outrootfile.Get(newchannelname)
        systematicdirhigh=channeldir.Get(newsystematicname+"_High")
        if systematicdirhigh==None:
            systematicdirhigh=channeldir.mkdir(newsystematicname+"_High")
        systematicdirhigh.cd()
        histosys[1].SetDirectory(systematicdirhigh)
        histosys[1].SetName(newsamplename)
        histosys[1].Write(histosys[1].GetName(),TObject.kOverwrite)

        channeldir=outrootfile.Get(newchannelname)
        systematicdirlow=channeldir.Get(newsystematicname+"_Low")
        if systematicdirlow==None:
            systematicdirlow=channeldir.mkdir(newsystematicname+"_Low")
        systematicdirlow.cd()
        histosys[0].SetDirectory(systematicdirlow)
        histosys[0].SetName(newsamplename)
        histosys[0].Write(histosys[0].GetName(),TObject.kOverwrite)

def getNewChannelName(oldchannelname):
    if oldchannelname in channelconvertmap:
        return channelconvertmap[oldchannelname]
    return oldchannelname

def getNewSampleName(oldsamplename):
    if oldsamplename in sampleconvertmap:
        return sampleconvertmap[oldsamplename]
    return oldsamplename

def getNewSystematicName(oldsystematicname):
    if oldsystematicname in systematicconvertmap:
        return systematicconvertmap[oldsystematicname]

    systematicname=oldsystematicname
    return systematicname.replace("ATLAS_","")

#return systematicname

def analyzeFile(filename):
    xmldoc = minidom.parse(filename)
    # read the information from the channel XML tag
    inrootfile=readChannel(xmldoc)
    print "ChannelName:", channelname
    newchannelname=getNewChannelName(channelname)
    #open the output root file
    outrootfile=TFile.Open(newchannelname+"_"+variablename+".root","RECREATE")
    #create the channel name directory
    outrootfile.mkdir(newchannelname)
    # read the information from the data XML tag
    readData(xmldoc, inrootfile,outrootfile,newchannelname)
    
    # loop over the samples
    samplelist = xmldoc.getElementsByTagName('Sample')
    for sample in samplelist:
        readSample(sample,inrootfile,outrootfile,newchannelname)

filenames=[ inputpath+"config_2/lh_boost_mH_2011.xml"]

for filename in filenames:
    analyzeFile(filename)
