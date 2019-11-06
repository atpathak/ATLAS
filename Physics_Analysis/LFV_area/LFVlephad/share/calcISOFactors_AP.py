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
            name = str(process)+'_'+str(category)+'_'+str(isolation)+'_'+str(prong)+'_'+str(trigger)+'_'+str(year)
            histo = TH1D(name,name,nBins,binArray)
            histo.GetXaxis().SetTitle(str(dist))
            histo.GetYaxis().SetTitle('Fake Factor')
            #if process != 'QCD':
             # nameMC = str(process)+'MC_'+str(category)+'_'+str(isolation)+'_'+str(prong)+'_'+str(trigger)+'_'+str(year)
              #histoMC = TH1D(nameMC,nameMC,nBins,binArray)
              #histoMC.GetXaxis().SetTitle(str(dist))
              #histoMC.GetYaxis().SetTitle('Fake Factor')
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
                
              ISOFactorTuple = calcISOFactor(reader, processesAntiTau, isolation, process, prong, category, trigger, lowBin, highBin, dist, binArray)
              histo.AddBinContent(bin,fakeFactorTuple[0])
              histo.SetBinError(bin,fakeFactorTuple[1])

              #if process != 'QCD':
               #     fakeFactorTupleMC = calcISOFactor(reader, processesAntiTau, isolation, process, prong, category, trigger, lowBin, highBin, dist, binArray)
                #    histoMC.AddBinContent(bin,fakeFactorTupleMC[0])
                 #   histoMC.SetBinError(bin,fakeFactorTupleMC[1])

            histo.Write()
                    #if process != 'QCD':
                     # histoMC.Write()

  output.Close()

def calcISOFactor(reader, processesAntiTau, isolation, process, prong, category, trigger, lowBin, highBin, dist, binArray):
  
  controlRegion = process

  sumAntiTauDataISO = 0.
  sumAntiTauDataISOErr = 0.
  integralAntiTauDataISOErr = Double(0.)

  sumAntiTauDataANTIISO = 0.
  sumAntiTauDataANTIISOErr = 0.
  integralAntiTauDataANTIISOErr = Double(0.)

  sumAntiTauISOTrueLep = 0.
  sumAntiTauISOTrueLepErr = 0.
  integralAntiTauDataISOTrueLepErr = Double(0.)

  sumAntiTauANTIISOTrueLep = 0.
  sumAntiTauANTIISOTrueLepErr = 0.
  integralAntiTauDataANTIISOTrueLepErr = Double(0.)

  fakeFactor = 0.
  fakeFactorErr = 0.

  for p in processesAntiTau:
    path = processesAntiTau[p]
    
    if p == 'Data':
                
      histogramAntiTauDataISO = reader.getHistogram(path,'Cut'+category+'ISO'+'TrueLep'+prong+trigger+controlRegion+'/'+dist)
      rebinnedHistogramAntiTauDataISO = histogramAntiTauDataISO.Rebin(len(binArray)-1,'',binArray)
      sumAntiTauDataISO = rebinnedHistogramAntiTauDataISO.IntegralAndError(lowBin,highBin,integralAntiTauDataISOErr)
      sumAntiTauDataISOErr = pow(integralAntiTauDataISOErr,2)

      histogramAntiTauDataANTIISO = reader.getHistogram(path,'Cut'+category+'ANTIISO'+'TrueLep'+prong+trigger+controlRegion+'/'+dist)
      rebinnedHistogramAntiTauDataANTIISO = histogramAntiTauDataANTIISO.Rebin(len(binArray)-1,'',binArray)
      sumAntiTauDataANTIISO = rebinnedHistogramAntiTauDataANTIISO.IntegralAndError(lowBin,highBin,integralAntiTauDataANTIISOErr)
      sumAntiTauDataANTIISOErr = pow(integralAntiTauDataANTIISOErr,2)
      
    else:
      
      histogramAntiTauISOTrueLep = reader.getHistogram(path,'Cut'+category+'ISO'+'TrueLep'+prong+trigger+controlRegion+'/'+dist)
      rebinnedHistogramAntiTauISOTrueLep = histogramAntiTauISOTrueLep.Rebin(len(binArray)-1,'',binArray)
      sumAntiTauISOTrueLep += -1*rebinnedHistogramAntiTauISOTrueLep.IntegralAndError(lowBin,highBin,integralAntiTauDataISOTrueLepErr)
      sumAntiTauISOTrueLepErr += pow(integralAntiTauDataISOTrueLepErr,2)

      histogramAntiTauANTIISOTrueLep = reader.getHistogram(path,'Cut'+category+'ANTIISO'+'TrueLep'+prong+trigger+controlRegion+'/'+dist)
      rebinnedHistogramAntiTauANTIISOTrueLep = histogramAntiTauANTIISOTrueLep.Rebin(len(binArray)-1,'',binArray)
      sumAntiTauANTIISOTrueLep += -1*rebinnedHistogramAntiTauANTIISOTrueLep.IntegralAndError(lowBin,highBin,integralAntiTauDataANTIISOTrueLepErr)
      sumAntiTauANTIISOTrueLepErr += pow(integralAntiTauDataANTIISOTrueLepErr,2)

  if (sumAntiTauDataISO - sumAntiTauISOTrueLep) != 0 and (sumAntiTauDataANTIISO - sumAntiTauANTIISOTrueLep) != 0:
    ISOFactor = (sumAntiTauDataISO - sumAntiTauISOTrueLep)/(sumAntiTauDataANTIISO - sumAntiTauANTIISOTrueLep)
    ISOFactorErr = ISOFactor*pow(((sumTauDataErr + sumTauNoJetErr)/pow((sumTauData - sumTauNoJet),2))+((sumAntiTauDataErr + sumAntiTauNoJetErr)/pow((sumAntiTauData - sumAntiTauNoJet),2)),0.5)
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
