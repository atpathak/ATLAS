#!/usr/bin/env python2

##########################################################################################
#                                                                                        #
# Code to calculate the relative contribution of the fake factors in H->tautau->lephad   #
#                                                                                        #
#                                                                                        #
# Autor: Fabian Becherer                                                                 #
#                                                                                        #
#                                                                                        #
# Date: 28.09.2016                                                                       #
##########################################################################################

import argparse
from array import array

alias = "calcFakes"
aliasTQLibrary = "RelConFakes"

#Function to constrain constribution values without sense 
def constrain_R_values(relConW,relConTop,relConZ):
  
  Wtemp = [relConW[0],relConW[1]]
  Ztemp   = [relConZ[0],relConZ[1]]
  Toptemp = [relConTop[0],relConTop[1]]
  constrain=False
  if Wtemp[0]<0. or Ztemp[0]<0. or  Toptemp[0]<0.:
    print "WARNING: relCon under 0."
  if Wtemp[0]+Ztemp[0]+Toptemp[0]>1.:
    constrain=True
    relConWtemp=[Wtemp[0],Wtemp[1]]
    relConZtemp=[Ztemp[0],Ztemp[1]]
    relConToptemp=[Toptemp[0],Toptemp[1]]
    relsum=relConWtemp[0]+relConZtemp[0]+relConToptemp[0]

    Wtemp[0]=relConWtemp[0]/(relsum)
    Ztemp[0]=relConZtemp[0]/(relsum)
    Toptemp[0]=relConToptemp[0]/(relsum)

    Wtemp[1]=pow(pow(relConWtemp[1]/(relsum),2)+pow(relConZtemp[1]*relConWtemp[0]/pow((relsum),2),2)+pow(relConToptemp[1]*relConWtemp[0]/pow((relsum),2),2),0.5)
    Ztemp[1]=pow(pow(relConZtemp[1]/(relsum),2)+pow(relConWtemp[1]*relConZtemp[0]/pow((relsum),2),2)+pow(relConToptemp[1]*relConZtemp[0]/pow((relsum),2),2),0.5)
    Toptemp[1]=pow(pow(relConToptemp[1]/(relsum),2)+pow(relConWtemp[1]*relConToptemp[0]/pow((relsum),2),2)+pow(relConZtemp[1]*relConToptemp[0]/pow((relsum),2),2),0.5)

  return tuple(Wtemp),tuple(Toptemp),tuple(Ztemp),constrain   

#Function to calculate the relative contribution of one process in one region
def calcRelCon(background,category,prongs,trigger,region,samples,Bin,histo,reader,rebin,bins):
  processes = {'Data':'./bkg/mtau/Fake/data',
                 'W':'./bkg/mtau/Fake/mc/[WjetsSh+WjetsEWSh]',
                 'Z':'./bkg/mtau/Fake/mc/[ZllSh+ZllEWSh]',
                 'Ztt':'./bkg/mtau/Fake/mc/[ZttSh+ZttEWSh]',
                 'Top':'./bkg/mtau/Fake/mc/Top',
                 'DiBoson':'./bkg/mtau/Fake/mc/Diboson'
              }

  sumData = 0.
  sumDataErr = 0.
  integralTauDataErr=Double(0.)
  sumJet = 0.
  sumJetErr = 0.
  integralTauJetErr=Double(0.)
  sumNoJet = 0.
  sumNoJetErr = 0.
  integralOtherMCErr=Double(0.)
 
  if Bin==1:
    minBin=0
  else:
    minBin=Bin
  if Bin==len(bins)-1:
    maxBin=Bin+1
  else:
    maxBin=Bin

  for p in processes:
    path = processes[p]
    if p == 'Data':
      histogramTauData=reader.getHistogram(path,'Cut'+category+'Jet'+prongs+trigger+region+'/'+histo)
      if rebin:
        histogramTauData = histogramTauData.Rebin(len(bins)-1,"TauData",bins)
      sumData = histogramTauData.IntegralAndError(minBin,maxBin,integralTauDataErr)
      sumDataErr =  pow(integralTauDataErr,2)
    elif p == background:
      histogramTauJet=reader.getHistogram(path,'Cut'+category+'Jet'+prongs+trigger+region+'/'+histo)
      if rebin:
        histogramTauJet = histogramTauJet.Rebin(len(bins)-1,"TauJet",bins)
      sumJet = -1*histogramTauJet.IntegralAndError(minBin,maxBin,integralTauJetErr)
      sumJetErr = pow(integralTauJetErr,2)
      histogramOtherMC=reader.getHistogram(path,'Cut'+category+'NoJet'+prongs+trigger+region+'/'+histo)
      if rebin:
        histogramOtherMC = histogramOtherMC.Rebin(len(bins)-1,"TauJet",bins)
      sumNoJet += -1*histogramOtherMC.IntegralAndError(minBin,maxBin,integralOtherMCErr)
      sumNoJetErr += pow(integralOtherMCErr,2)
    else:
      histogramOtherMC=reader.getHistogram(path,'Cut'+category+'NoJet'+prongs+trigger+region+'/'+histo)
      if rebin:
        histogramOtherMC = histogramOtherMC.Rebin(len(bins)-1,"OtherMC",bins)
      sumNoJet += -1*histogramOtherMC.IntegralAndError(minBin,maxBin,integralOtherMCErr)
      sumNoJetErr += pow(integralOtherMCErr,2)

  if (sumData - sumNoJet) == 0.:
    print 'WARNING sumData equal to sumNoJet'
    relCon = 0.
    relConErr = 0.
  else:
    relCon = (sumJet)/(sumData - sumNoJet)
    relConErr = pow((sumDataErr+sumNoJetErr)*pow(sumJet/pow(sumData-sumNoJet,2),2)+sumJetErr/pow(sumData-sumNoJet,2),0.5)

  return (relCon,relConErr,sumJet,sumData,sumNoJet)

#Function to variate the W contribution after calculation and to reset the other contributions
def varfunc(relConW,relConTop,relConZ,relConQCD,vary):
  Wtemp   = [relConW[0],relConW[1]]
  Ztemp   = [relConZ[0],relConZ[1]]
  Toptemp = [relConTop[0],relConTop[1]]
  QCDtemp = [relConQCD[0],relConQCD[1]]
  Wtempnew = [vary*Wtemp[0],vary*Wtemp[1]]
  if Wtempnew[0]>=1.:
    Wtemp=(1.,Wtempnew[1]/Wtempnew[0])
    Ztemp=(0.,0.)
    Toptemp=(0.,0.)
    QCDtemp=(0.,0.)
  else: 
    newdiff=1-Wtempnew[0]
    olddiff=1-Wtemp[0]
    fac = newdiff/olddiff
    Ztemp[1] = pow(pow(newdiff*Ztemp[1]/olddiff,2)+pow(Ztemp[0]*Wtemp[1]/olddiff,2)+pow(newdiff*relConZ[0]*Wtempnew[1]/pow(olddiff,2),2),0.5)
    Ztemp[0] = fac*Ztemp[0]
    Toptemp[1] = pow(pow(newdiff*Toptemp[1]/olddiff,2)+pow(Toptemp[0]*Wtemp[1]/olddiff,2)+pow(newdiff*relConTop[0]*Wtempnew[1]/pow(olddiff,2),2),0.5)
    Toptemp[0] = fac*Toptemp[0]
    QCDtemp[1] = pow(pow(newdiff*QCDtemp[1]/olddiff,2)+pow(QCDtemp[0]*Wtemp[1]/olddiff,2)+pow(newdiff*relConQCD[0]*Wtempnew[1]/pow(olddiff,2),2),0.5)
    QCDtemp[0] = fac*QCDtemp[0]
    Wtemp[0] = Wtempnew[0]
    Wtemp[1] = Wtempnew[1]
  return tuple(Wtemp),tuple(Toptemp),tuple(Ztemp),tuple(QCDtemp)

#Function to loop over all given regions and to calculate the relative contributions and save them 
def calcRelConFinal(categories,triggers,prongs,regions,histo,reader,samples,bins,variation,unit,outFile,year):
#Open Files for saving
  out=str(outFile)
  f = open(out[:len(out)-5]+'.tex', 'w')
  f.write( "\\documentclass[a4paper,10pt]{article}\n \\usepackage[utf8]{inputenc}\n \\usepackage{longtable} \n \\usepackage{lscape}\n \\begin{document}\n \\centering \n")
  hFile = TFile(str(outFile),'recreate')
#Open loop for given variations
  for vary in variation:
#define name appendix for variation
    if vary<1.:
      var="_dn"
    elif vary>1.:
      var="_up"
    else:
      var=""
#Open loop for categories and triggers
    for category in categories:  
      for trigger in triggers:
#Load bin distribution of orign histogram if no binning in the config file is given
        if not bins:
          dummyHisto = reader.getHistogram('./data','Cut'+category+'NoJet1Prong'+trigger+'SR/'+histo)
          nBins = dummyHisto.GetNbinsX()
          for i in range(0,nBins+1):
            bins.append(dummyHisto.GetBinLowEdge(i+1))
            bins=array('d',bins)
            rebin=False
        else: 
          rebin=True
#Open loop for prongs
        for prong in prongs:
#Write table head in latex file
          f.write("\\begin{longtable}{|l|l|l|l|l|l|} \n \\hline\n \\multicolumn{6}{|l|}{%s, %s, Variation:%s} \\\ \n"%(category, prong ,str(vary)))
          f.write("\\hline \n Region & Bin & W & Top & Z & QCD\\\ \n \\hline \n")
#Open loop for regions
          for region in regions:
#Define and name histograms for output root file
            h1 = TH1D('W_'+category+'_'+prong+'_'+trigger+'_'+region+'_'+str(year)+var,'',len(bins)-1,bins)
            h2 = TH1D('Top_'+category+'_'+prong+'_'+trigger+'_'+region+'_'+str(year)+var,'',len(bins)-1,bins)
            h3 = TH1D('Z_'+category+'_'+prong+'_'+trigger+'_'+region+'_'+str(year)+var,'',len(bins)-1,bins)
            h4 = TH1D('QCD_'+category+'_'+prong+'_'+trigger+'_'+region+'_'+str(year)+var,'',len(bins)-1,bins)
            h1.GetXaxis().SetTitle("%s [%s]"%(histo,unit))
            h1.GetYaxis().SetTitle('R_W')      
            h2.GetXaxis().SetTitle("%s [%s]"%(histo,unit))
            h2.GetYaxis().SetTitle('R_Top')
            h3.GetXaxis().SetTitle("%s [%s]"%(histo,unit))
            h3.GetYaxis().SetTitle('R_Z')
            h4.GetXaxis().SetTitle("%s [%s]"%(histo,unit))
            h4.GetYaxis().SetTitle('R_QCD')
#Open loop for number of bins
            for n in range(1,len(bins)):
              print 'Now running: '+category+' '+prong+' '+trigger+' '+str(vary)+' '+region+' '+str(n) 
#Calculation of relative contribution W,Z,Top
              relConWv,relConWErr,sumJet,sumData,sumNoJet=calcRelCon('W',category,prong,trigger,region,samples,n,histo,reader,rebin,bins)
              relConTopv,relConTopErr,sumJet,sumData,sumNoJet=calcRelCon('Top',category,prong,trigger,region,samples,n,histo,reader,rebin,bins)
              relConZv,relConZErr,sumJet,sumData,sumNoJet=calcRelCon('Z',category,prong,trigger,region,samples,n,histo,reader,rebin,bins)
#Save relative contribution and error in tuple
              relConW=[relConWv,relConWErr]
              relConTop=[relConTopv,relConTopErr]
              relConZ=[relConZv,relConZErr]
#Calculate relative contribution of QCD 
              relConQCD = (1. - relConW[0] - relConZ[0] - relConTop[0], pow(pow(relConW[1],2)+pow(relConZ[1],2)+pow(relConTop[1],2),0.5))
#Write relative contributions in latex file
              f.write("%s & %s & %.3f$\pm$%.3f & %.3f$\pm$%.3f & %.3f$\pm$%.3f & %.3f$\pm$%.3f \\\ \n"%(region,str(n),relConW[0],relConW[1],relConTop[0],relConTop[1],relConZ[0],relConZ[1],relConQCD[0],relConQCD[1]))
#Constrain the values of all relative contributions
              constrained=False
              relConW,relConTop,relConZ,constrained = constrain_R_values(relConW,relConTop,relConZ)
#If constrain was necessary write new contribuion in latex file
              if constrained:
                relConQCD=(0.,0.)
                f.write("%s & %s & %.3f$\pm$%.3f & %.3f$\pm$%.3f & %.3f$\pm$%.3f & %.3f$\pm$%.3f \\\ \n"%(region,str(n),relConW[0],relConW[1],relConTop[0],relConTop[1],relConZ[0],relConZ[1],relConQCD[0],relConQCD[1]))
#Variate relativ contribution and constrain the values
              relConW,relConTop,relConZ,relConQCD=varfunc(relConW,relConTop,relConZ,relConQCD,vary)
#If a variation was done, write the new values in latex file
              if not vary==1.: 
                f.write(" & & %.3f$\pm$%.3f & %.3f$\pm$%.3f & %.3f$\pm$%.3f & %.3f$\pm$%.3f \\\ \n"%(relConW[0],relConW[1],relConTop[0],relConTop[1],relConZ[0],relConZ[1],relConQCD[0],relConQCD[1]))
#Write the raw data of this region in the latex file
              f.write(" & & sumJet= %.0f & sumData= %.0f & sumNoJet= %.0f & \\\ \n"%(sumJet,sumData,sumNoJet))
              f.write("\\hline\n")
#Fill and save the histograms 
              h1.AddBinContent(n,relConW[0])
              h1.SetBinError(n,relConW[1])
              h2.AddBinContent(n,relConTop[0])
              h2.SetBinError(n,relConTop[1])
              h3.AddBinContent(n,relConZ[0])
              h3.SetBinError(n,relConZ[1])
              h4.AddBinContent(n,relConQCD[0])
              h4.SetBinError(n,relConQCD[1])
            h1.Write()
            h2.Write()
            h3.Write()
            h4.Write()
#End table in latex file
          f.write( "\\end{longtable} \n \\clearpage \n")
  f.write("\n \\end{document} \n")
#Close file
  hFile.Close()
  return 

def main():
  
  #Open the configfile
  INFO("reading configuration for '{:s}' from file '{:s}'".format(alias,args.cfgname))
  configreader = TQConfigReader(alias,args.cfgname)
  configreader.resolve()
  
  #Parse config options from the command line
  if args.options:
    for s in args.options:
      opt = s.split(":",1)
      WARN("using config option '{:s} = {:s}' from command line".format(opt[0],opt[1]))
      configreader.set(alias+"."+opt[0],opt[1])  
  
  #Export the config options to a taggable object    
  config = configreader.exportAsTags()
  if not config:
    BREAK("unable to retrieve configuration from file '{:s}' - please check the filename".format(args.cfgname))
  if config.getNTags() < 3:
    BREAK("very few configuration options were read -- are you sure you are reading the correct config file?")
  
  #Load settings from config file
  inFile = config.getTagStringDefault("inputFile","input")
  outFile = config.getTagStringDefault("outputFileR","relconfakes.root")
  distribution = config.getTagStringDefault("distributionR","transverseMassLepMET")
  unit=config.getTagStringDefault("distributionUnit","")
  year = config.getTagStringDefault("year","2016")
  binning = config.getTagVString("binningR")
  prongs = config.getTagVString("prongs")
  triggers = config.getTagVString("triggers")
  categories = config.getTagVString("categories")
  regions = config.getTagVString("regions")
  vary = config.getTagVString("varyR")
  
  #Define aliasses
  samples = TQSampleFolder.loadLazySampleFolder(inFile)
  reader = TQSampleDataReader(samples)
  
  #Convert binning an variation list in arrays
  if binning[0]== "":
    bins=[]
  else:
    bins=[]
    for i in range(0,len(binning)):
      bins.append(float(str(binning[i])))
  bins=array('d',bins)
  variation=[]
  variation.append(1.)
  if not vary[0]=="":
    for i in range(0,len(vary)):
      variation.append(float(str(vary[i])))
  variation=array('d',variation)

  #Start relative contribution calculation
  calcRelConFinal(categories,triggers,prongs,regions,distribution,reader,samples,bins,variation,unit,outFile,year)

#Starting process for start the code by hand (config file is necessary)
if __name__ == "__main__":
  #parse the CLI arguments
  parser = argparse.ArgumentParser(description='RelCon calculation')
  parser.add_argument('cfgname', metavar='CONFIG', type=str, 
            default="config/htt_lephad_fake_calc.cfg",
            help='path to the analysis config file to be processed')
  parser.add_argument('--options', metavar='KEY:VALUE', type=str, nargs='*',
            default="", 
            help='set (overwrite) an option with respect to the config file')
  args = parser.parse_args()

  from QFramework import *  
  from ROOT import *
  ROOT.gROOT.LoadMacro("~/atlasstyle-00-03-05/AtlasStyle.C")
  ROOT.gROOT.LoadMacro("~/atlasstyle-00-03-05/AtlasUtils.C")
  ROOT.gROOT.SetBatch(kTRUE)
  #SetAtlasStyle()
  TQLibrary.getQLibrary().setApplicationName(aliasTQLibrary);

  pair = ROOT.std.pair('TString','TString')

  gROOT.SetBatch(True)
 
  main(); 
