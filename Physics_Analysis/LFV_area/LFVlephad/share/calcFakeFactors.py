#!/usr/bin/env python2

import argparse

alias = "calcFakes"

def main():
  
  # open the configfile
  INFO("reading configuration for '{:s}' from file '{:s}'".format(alias,args.cfgname))
  configreader = TQConfigReader(alias,args.cfgname)
  configreader.resolve()

  # parse config options from the command line
  if args.options:
    for s in args.options:
      opt = s.split(":",1)
      WARN("using config option '{:s} = {:s}' from command line".format(opt[0],opt[1]))
      configreader.set(alias+"."+opt[0],opt[1])  
    
  # export the config options to a taggable object    
  config = configreader.exportAsTags()
  if not config:
    BREAK("unable to retrieve configuration from file '{:s}' - please check the filename".format(args.cfgname))
  if config.getNTags() < 3:
    BREAK("very few configuration options were read -- are you sure you are reading the correct config file?")

  
  inFile = config.getTagStringDefault('inputFile','nominal.root:samples')
  dist = config.getTagStringDefault('distributionFF','tauPt') 
  binning = config.getTagVDouble('binningFF')
  processes = config.getTagVString('processes')
  prongs = config.getTagVString('prongs')
  triggers = config.getTagVString('triggers')
  categories = config.getTagVString('categories')
  year = config.getTagIntegerDefault('year',9999)
  outFile = config.getTagStringDefault('outputFileFF','output/dummy/dummy.root')

  samples = TQSampleFolder.loadLazySampleFolder(inFile)
  reader = TQSampleDataReader(samples)

  output = TFile(str(outFile),'recreate')

  processesAntiTau = {'Data':'./bkg/mtau/Fake/data',
                      'W':'./bkg/mtau/Fake/mc/[WjetsSh+WjetsEWSh]',
                      'Z':'./bkg/mtau/Fake/mc/[ZllSh+ZllEWSh]',
                      'Ztt':'./bkg/mtau/Fake/mc/[ZttSh+ZttEWSh]',
                      'Top':'./bkg/mtau/Fake/mc/Top',
                      'DiBoson':'./bkg/mtau/Fake/mc/Diboson'
                      }
  
  processesTau = {'Data':'./data',
                  'W':'./bkg/mtau/[WjetsSh+WjetsEWSh]',
                  'Z':'./bkg/mtau/[ZllSh+ZllEWSh]',
                  'Ztt':'./bkg/mtau/[ZttSh+ZttEWSh]',
                  'Top':'./bkg/mtau/Top',
                  'DiBoson':'./bkg/mtau/Diboson'
                  }

  binArray = []
  binning = array('d',binning)

  if not binning:
    print 'use original binning'
    dummyHisto = reader.getHistogram('./data','CutBase/'+dist)
    nBins = dummyHisto.GetNbinsX()
    for i in range(0,nBins+1):
      upperBinEdge = dummyHisto.GetXaxis().GetBinUpEdge(i)
      binArray.append(upperBinEdge)
      binArray = array('d',binArray)
  else:
    print 'use rebinning'
    nBins = len(binning)-1
    for i in binning:
      binArray.append(i)
    binArray = array('d',binArray)

  for process in processes:
    for prong in prongs:
      for category in categories:
        for trigger in triggers:
          name = str(process)+'_'+str(category)+'_'+str(prong)+'_'+str(trigger)+'_'+str(year)
          histo = TH1D(name,name,nBins,binArray)
          histo.GetXaxis().SetTitle(str(dist))
          histo.GetYaxis().SetTitle('Fake Factor')
          if process != 'QCD':
            nameMC = str(process)+'MC_'+str(category)+'_'+str(prong)+'_'+str(trigger)+'_'+str(year)
            histoMC = TH1D(nameMC,nameMC,nBins,binArray)
            histoMC.GetXaxis().SetTitle(str(dist))
            histoMC.GetYaxis().SetTitle('Fake Factor')
            #print 'Processing %s' %(nameMC)
          print 'Processing %s' %(name)
          for bin in range(1,nBins+1):
            if bin == 1:
              lowBin = 0 #underflow bin
              highBin = 1
            elif bin == nBins:
              lowBin = nBins
              highBin = nBins+1 #overflow bin
            else:
              lowBin = bin
              highBin = bin
              
            fakeFactorTuple = calcFakeFactor(reader, processesTau, processesAntiTau, process, prong, category, trigger, lowBin, highBin, dist, binArray)
            histo.AddBinContent(bin,fakeFactorTuple[0])
            histo.SetBinError(bin,fakeFactorTuple[1])

            if process != 'QCD':
              fakeFactorTupleMC = calcFakeFactorMC(reader, processesTau, processesAntiTau, process, prong, category, trigger, lowBin, highBin, dist, binArray)
              histoMC.AddBinContent(bin,fakeFactorTupleMC[0])
              histoMC.SetBinError(bin,fakeFactorTupleMC[1])

          histo.Write()
          if process != 'QCD':
            histoMC.Write()

  output.Close()

def calcFakeFactor(reader, processesTau, processesAntiTau, process, prong, category, trigger, lowBin, highBin, dist, binArray):
  
  controlRegion = process+'CR'

  sumTauData = 0.
  sumTauDataErr = 0.
  integralTauDataErr = Double(0.)
  
  sumTauNoJet = 0.
  sumTauNoJetErr = 0.
  integralTauNoJetErr = Double(0.)
  
  
  sumAntiTauData = 0.
  sumAntiTauDataErr = 0.
  integralAntiTauDataErr = Double(0.)
  
  sumAntiTauNoJet = 0.
  sumAntiTauNoJetErr = 0.
  integralAntiTauNoJetErr = Double(0.)

  fakeFactor = 0.
  fakeFactorErr = 0.

  for p in processesTau:
    path = processesTau[p]
    
    if p == 'Data':
                
      histogramTauData = reader.getHistogram(path,'Cut'+category+'NoJet'+prong+trigger+controlRegion+'/'+dist)
      rebinnedHistogramTauData = histogramTauData.Rebin(len(binArray)-1,'',binArray)
      sumTauData = rebinnedHistogramTauData.IntegralAndError(lowBin,highBin,integralTauDataErr)
      sumTauDataErr = pow(integralTauDataErr,2)
      
    else:
      
      histogramTauNoJet = reader.getHistogram(path,'Cut'+category+'NoJet'+prong+trigger+controlRegion+'/'+dist)
      rebinnedHistogramTauNoJet= histogramTauNoJet.Rebin(len(binArray)-1,'',binArray)
      sumTauNoJet += rebinnedHistogramTauNoJet.IntegralAndError(lowBin,highBin,integralTauNoJetErr)
      sumTauNoJetErr += pow(integralTauNoJetErr,2)

  for p in processesAntiTau:
    path = processesAntiTau[p]
    
    if p == 'Data':
                
      histogramAntiTauData = reader.getHistogram(path,'Cut'+category+'NoJet'+prong+trigger+controlRegion+'/'+dist)
      rebinnedHistogramAntiTauData = histogramAntiTauData.Rebin(len(binArray)-1,'',binArray)
      sumAntiTauData = rebinnedHistogramAntiTauData.IntegralAndError(lowBin,highBin,integralAntiTauDataErr)
      sumAntiTauDataErr = pow(integralAntiTauDataErr,2)
      
    else:
      
      histogramAntiTauNoJet = reader.getHistogram(path,'Cut'+category+'NoJet'+prong+trigger+controlRegion+'/'+dist)
      rebinnedHistogramAntiTauNoJet = histogramAntiTauNoJet.Rebin(len(binArray)-1,'',binArray)
      sumAntiTauNoJet += -1*rebinnedHistogramAntiTauNoJet.IntegralAndError(lowBin,highBin,integralAntiTauNoJetErr)
      sumAntiTauNoJetErr += pow(integralAntiTauNoJetErr,2)

  if (sumAntiTauData - sumAntiTauNoJet) != 0 and (sumTauData - sumTauNoJet) != 0:
    fakeFactor = (sumTauData - sumTauNoJet)/(sumAntiTauData - sumAntiTauNoJet)
    fakeFactorErr = fakeFactor*pow(((sumTauDataErr + sumTauNoJetErr)/pow((sumTauData - sumTauNoJet),2))+((sumAntiTauDataErr + sumAntiTauNoJetErr)/pow((sumAntiTauData - sumAntiTauNoJet),2)),0.5)
  else:
    if not 'BOOST' in str(category):
      print 'Dividing by zero in bin %i' %(lowBin)
    fakeFactor = 0.0001
    fakeFactorErr = 0.0001

  if fakeFactor < 0.:
    print 'FF is negative in bin %i' %(lowBin) 
    relErr = abs(fakeFactorErr/fakeFactor)
    fakeFactor = 0.0001
    fakeFactorErr = fakeFactor*relErr
    print 'rel. Error %f, new abs. Error %f' %(relErr,fakeFactorErr)

  return (float(fakeFactor), float(fakeFactorErr))


def calcFakeFactorMC(reader, processesTau, processesAntiTau, process, prong, category, trigger, lowBin, highBin, dist, binArray):
  
  controlRegion = process+'CR'

  sumTauJet = 0.
  sumTauJetErr = 0.
  integralTauJetErr = Double(0.)
  
  sumAntiTauJet = 0.
  sumAntiTauJetErr = 0.
  integralAntiTauJetErr = Double(0.)

  fakeFactor = 0.
  fakeFactorErr = 0.

  for p in processesTau:
    path = processesTau[p]
    
    if p == process:
                
      histogramTauJet = reader.getHistogram(path,'Cut'+category+'Jet'+prong+trigger+controlRegion+'/'+dist)
      rebinnedHistogramTauJet= histogramTauJet.Rebin(len(binArray)-1,'',binArray)
      sumTauJet = rebinnedHistogramTauJet.IntegralAndError(lowBin,highBin,integralTauJetErr)
      sumTauJetErr = integralTauJetErr

  for p in processesAntiTau:
    path = processesAntiTau[p]
    
    if p == process:
                
      histogramAntiTauJet = reader.getHistogram(path,'Cut'+category+'Jet'+prong+trigger+controlRegion+'/'+dist)
      rebinnedHistogramAntiTauJet = histogramAntiTauJet.Rebin(len(binArray)-1,'',binArray)
      sumAntiTauJet = -1*rebinnedHistogramAntiTauJet.IntegralAndError(lowBin,highBin,integralAntiTauJetErr)
      sumAntiTauJetErr = integralAntiTauJetErr

  if sumAntiTauJet != 0 and sumTauJet != 0:
    fakeFactor = sumTauJet/sumAntiTauJet
    fakeFactorErr = fakeFactor*pow(pow(sumTauJetErr/sumTauJet,2) + pow(sumAntiTauJetErr/sumAntiTauJet,2),0.5)
  else:
    if not 'BOOST' in str(category):
      print 'MC: Dividing by zero in bin %i' %(lowBin)
    fakeFactor = 0.0001
    fakeFactorErr = 0.0001

  if fakeFactor < 0.:
    print 'MC: FF is negative in bin %i' %(lowBin) 
    relErr = abs(fakeFactorErr/fakeFactor)
    fakeFactor = 0.0001
    fakeFactorErr = fakeFactor*relErr
    print 'MC: rel. Error %f, new abs. Error %f' %(relErr,fakeFactorErr)

  return (float(fakeFactor), float(fakeFactorErr))

if __name__ == "__main__":
  #parse the CLI arguments
  parser = argparse.ArgumentParser(description='Calculate Fake Factors')
  parser.add_argument('cfgname', metavar='CONFIG', type=str, 
            default="config/readCutBased.cfg",
            help='path to the analysis config file to be processed')
  parser.add_argument('--options', metavar='KEY:VALUE', type=str, nargs='*',
            default="", 
            help='set (overwrite) an option with respect to the config file')
  args = parser.parse_args()

  from QFramework import *  
  from ROOT import *
  from array import array

  TQLibrary.getQLibrary().setApplicationName(alias);

  pair = ROOT.std.pair('TString','TString')

  gROOT.SetBatch(True)
  main(); 
