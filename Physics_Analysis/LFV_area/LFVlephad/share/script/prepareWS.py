#! /Usr/Bin/env python                                                                                                                                   
import sys,os,ROOT,math
from array import array
from ROOT import *
from QFramework import *
import argparse
import systematicsHandler
from array import array

copy_from_nominal=["Fake"]

d_names = {
    "H125_tautaull_ggf"           : "ggH",
    "H125_tautaull_vbf"           : "VBFH",
    "ZllM"                        : "Zll",
    "ZttM"                        : "Ztt",
    "ZllS22"                      : "ZllSh",
    "ZttS22"                      : "ZttSh",
    "ZttEW"                       : "ZttEWK",
    "ZllEW"                       : "ZllEWK",
    "W"                           : "W",
    "ttb+stop"                    : "Top",
    "stop"                        : "STop",
    "diboson"                     : "VV",
    "tfakes+tfakesdata"           : "Fake",
    "ggHWW"                       : "ggHWW",
    "VBFHWW"                      : "VBFHWW",
    "ttH"                         : "ttH",
    "WH"                          : "WH",
    "ZH"                          : "ZH",
    "*"                           : "Data"
}

## settings: categories = [region, cutStage, [variable, xlow, xhigh, rebin]]
categories  = []
#categories  = [["llAll_boost_signal"   , "CutBoostedCat"          , ["dilep_mmc_maxw_m" , 50 , 200 , 10]]]
#categories += [["llAll_boost_top"      , "CutDphiLLTopCR"         , ["dilep_mmc_maxw_m" , 50 , 200 , 10]]]
categories += [["llAll_boost_ztt"       , "CutBoostedCatZttFitCR" , ["PtLL"         , [100 , 140   , 400]  , 200 , 50]]]
#categories += [["llAll_boost_ztt_test"  , "CutBoostedCatZttFitCR" , ["PtTauTau"         , 0      , 200 , 5]]]
#categories += [["llAll_boost_loose_zll" , "CutBoostedCatAZllCR"   , ["PtTauTau"             , 0  , 300 , 30]]]
#categories += [["llAll_boost_tight_zll" , "CutBoostedCatBZllCR"   , ["PtTauTau"             , 0  , 300 , 30]]]
categories += [["llAll_boost_zll" , "CutBoostedCatZllCR"   , ["PtTauTau"             , 0  , 400 , 40]]]
#categories += [["llAll_vbf_signal"     , "CutVBFCat"             , ["dilep_mmc_maxw_m" , 50 , 200 , 5]]]
#categories += [["llAll_vbf_top"        , "CutDphiLLTopCR"        , ["dilep_mmc_maxw_m" , 50 , 200 , 5]]]
categories += [["llAll_vbf_ztt"         , "CutVBFCatZttFitCR"     , ["PtLL"         , [0, 100 , 140   , 400] , 200 , 50]]]
#categories += [["llAll_vbf_ztt_test"    , "CutVBFCatZttFitCR"     , ["PtTauTau"         , 0      , 200 , 5]]]
#categories += [["llAll_vbf_loose_zll"   , "CutVBFCatAZllCR"       , ["PtTauTau"             , 0  , 300 , 30]]]
#categories += [["llAll_vbf_tight_zll"   , "CutVBFCatBZllCR"       , ["PtTauTau"             , 0  , 300 , 30]]]
categories += [["llAll_vbf_zll"   , "CutVBFCatZllCR"       , ["PtTauTau"             , 0  , 400 , 40]]]
   


d_sample = {"sig" : ["H125_tautaull_ggf","H125_tautaull_vbf","ttH","ZH","WH"],
            "bkg" : ["ZllM","ZttM","ZllEW","ZttEW","ZllS22","ZttS22","ttb+stop","ggHWW","VBFHWW","diboson", "tfakes+tfakesdata"],
            "data": ["*"]
            }
d_sample_syst = {"sig" : ["H125_tautaull_ggf","H125_tautaull_vbf","ttH","ZH","WH"],
            "bkg" : ["ZllM","ZttM","ZllEW","ZttEW","ttb+stop","ggHWW","VBFHWW","diboson", "tfakes+tfakesdata"],
            }

def getHistograms(reader,sample,channels,processes,cut,variable):
    first=True
    new_hist=None
    for c in channels:
        for p in processes:
            if p=="*": p=""
            path=sample+"/"+c+"/"+p
            #DEBUG ("systematic get histogram "+path)
            if type(reader)==list:
                subfirst=True
                for r in reader:
                    subhist=r.getHistogram(path,cut+"/"+variable)
                    if subfirst:
                        hist=subhist.Clone("newsubhist")
                        subfirst=False
                    else:
                        hist.Add(subhist)
            else:
                hist=reader.getHistogram(path,cut+"/"+variable)
            if (not hist):
                WARN("histogram "+path+ " does not exist!")
            if first:
              new_hist=hist.Clone("newhist")
              first=False
            else:
              new_hist.Add(hist)
              hist.Delete()
    return new_hist

def readInputFile(in_file_name, d_sample, categories, systematic, out_file, savehists=False, comparehists=None ,createdirs=False,normhists=None,savetofile=True,verbose=False):
    INFO ("systematic "+systematic+ " in file "+str(in_file_name))
    print d_sample

    if type(in_file_name)==list:
        in_file_name
        samples = [ TQSampleFolder.loadLazySampleFolder(i) for i in in_file_name ]
        reader  = [ TQSampleDataReader(s) for s in samples ]
    else:
        samples = TQSampleFolder.loadLazySampleFolder(in_file_name)
        reader  = TQSampleDataReader(samples)
    saved_hist={}

    for category in categories:
        #DEBUG ("systematic "+systematic+ " category "+category[0])
        print category
        region = category[0]
        cut    = category[1]
        var    = category[2]
        xlow = var[1]
        xhigh = var[2]
        rebin = var[3]
        subdir_name = region
        #print "path", out_file.FindObject(subdir_name)
        out_file.cd()
        if not out_file.FindObject(subdir_name) and createdirs:
            #print "make new path",subdir_name
            #out_file.ls()
            subdir=out_file.mkdir(subdir_name)
        out_file.cd(subdir_name)
        for i_sample in d_sample:
            tmp_sample = d_sample[i_sample]
            for i_proc in tmp_sample:
                subdir2_name = d_names[i_proc]
                #print "path 2", out_file.FindObject(subdir2_name)
                if not out_file.FindObject(subdir2_name) and createdirs:
                    #print "make new path 2",subdir_name+"/"+subdir2_name
                    #out_file.ls()
                    subdir2 = out_file.mkdir(subdir_name+"/"+subdir2_name)
                out_file.cd(subdir_name+"/"+subdir2_name)
                if verbose:
                  INFO ("systematic "+systematic+ " category "+category[0]+" sample "+i_sample+" process "+i_proc+" in dir "+subdir_name+"/"+subdir2_name)
                #DEBUG ("systematic "+systematic+ " category "+category[0]+" sample "+i_sample+" process "+i_proc+" in dir "+subdir_name+"/"+subdir2_name)
                processes=i_proc.split("+")
                channels=["ee","mm","me","em"]
                if comparehists and subdir2_name in copy_from_nominal and not "fake" in systematic:
                    INFO("copy "+subdir2_name+" from nominal")
                    name=subdir_name+"/"+subdir2_name+"/"+"nominal"
                    new_hist=comparehists[name].Clone(systematic)
                else: 
                    new_hist=getHistograms(reader,i_sample,channels,processes,cut,var[0])
                    # add different channels if you wanr
                    if type(xlow)==list:
                        new_hist.SetName(systematic+"_tmp")
                        rebin=len(xlow)-1
                        rebinarray=array("d",xlow)
                        new_hist.Rebin(rebin,systematic,rebinarray)
                        new_hist=gROOT.FindObject(systematic)
                    else:
                      new_hist.SetName(systematic)
                      new_hist.Rebin(rebin)
                lastbin=new_hist.GetNbinsX()
                overflow=new_hist.GetBinContent(lastbin+1)
                overflowerror=new_hist.GetBinError(lastbin+1)
                #print "ov",lastbin,overflow,overflowerror,new_hist.GetBinContent(lastbin), new_hist.GetBinError(lastbin)
                new_hist.SetBinContent(lastbin,new_hist.GetBinContent(lastbin)+overflow)
                new_hist.SetBinError(lastbin,(new_hist.GetBinError(lastbin)**2.0+overflowerror**2.0)**0.5)
                #print "ov",lastbin,overflow,overflowerror,new_hist.GetBinContent(lastbin), new_hist.GetBinError(lastbin)
                new_hist.SetBinContent(lastbin+1,0)
                new_hist.SetBinError(lastbin+1,0)
                if normhists != None:
                    namenorm=subdir_name+"/"+subdir2_name+"/"+"nom"
                    namenom=subdir_name+"/"+subdir2_name+"/"+"nominal"
                    if normhists[namenorm].Integral() != 0:
                      scale=comparehists[namenom].Integral()/normhists[namenorm].Integral()
                    else:
                      scale=0.0
                    print "rescale ",comparehists[namenom].Integral(), normhists[namenorm].Integral(),new_hist.Integral(),scale,
                    new_hist.Scale(scale)
                    print new_hist.Integral()
                if savetofile: new_hist.Write()
                maxbin=new_hist.GetXaxis().GetNbins();
                zerobin=[]
                negbin=[]
                for bin in xrange(1,maxbin+1):
                    if new_hist.GetBinContent(bin) == 0:
                        zerobin.append(bin)
                    if new_hist.GetBinContent(bin) < 0:
                        zerobin.append(-bin)
                if len(zerobin) >0:
                    INFO ("systematic "+systematic+ " category "+category[0]+" sample "+i_sample+" process "+i_proc+" in dir "+subdir_name+"/"+subdir2_name+" zero bins:"+str(zerobin))
                if comparehists !=None:
                    name=subdir_name+"/"+subdir2_name+"/"+"nominal"
                    compare=comparehists[name].Integral()
                    thishist=new_hist.Integral()
                    if abs(compare-thishist)/(compare+1e-10) > 0.1:
                      pass
                      #INFO ("systematic "+systematic+ " category "+category[0]+" sample "+i_sample+" process "+i_proc+" in dir "+subdir_name+"/"+subdir2_name+" "+str(compare)+"->"+str(thishist)+" "+str((compare-thishist)/(compare+1E-10)*100.0)+"%")
                      print "systematic "+systematic+ " category "+category[0]+" sample "+i_sample+" process "+i_proc+" in dir "+subdir_name+"/"+subdir2_name+" "+str(compare)+"->"+str(thishist)+" "+str((compare-thishist)/(compare+1E-10)*100.0)+"%"
                if savehists:
                  new_hist.SetDirectory(0)
                  saved_hist[subdir_name+"/"+subdir2_name+"/"+systematic]=new_hist
                else:
                  new_hist.Delete()
    if type(samples)==list:
        for s in samples: s.Delete()
    else:
        samples.Delete()
    if savehists: return saved_hist


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='create workspace')
    parser.add_argument('--systematicmap', dest="systematicmap", action="store", help='systematic map', default="maps/systematicMap_leplep.map")
    parser.add_argument('--samplefile', dest="samplefile", action="store", help='sample file', default="samples-htt-leplep_ds2015.root")
    parser.add_argument('--samplename', dest="samplename", action="store", help='sample name', default="samples")
    parser.add_argument('--inputdir', dest="inputdir", action="store", help='input dir', default ="output" )
    parser.add_argument('--inputfile', dest="inputfile", action="store", help='nominal input file', default =["samples-htt-leplep_ds2015.root","samples-htt-leplep_ds2016.root"] )
    #parser.add_argument('--inputfile', dest="inputfile", action="store", help='nominal input file', default ="samples-htt-leplep_ds2016.root" )
    #parser.add_argument('--inputfile', dest="inputfile", action="store", help='nominal input file', default ="samples-htt-leplep_ds2015.root" )
    parser.add_argument('--outputfile', dest="outputfile", action="store", help='output file', default="inputWS.root")
    parser.add_argument('--skipnominal', dest="skipnominal", action="store_true", help='skip nom'  , default=False)
    parser.add_argument('--option', dest="option", action="store", help='skip nom'  , default="")
    args = parser.parse_args()
   


    print args.option
   
    if args.option=="ptll":
        categories  = []
        categories += [["llAll_boost_ztt" , "CutBoostedCatZttFitCR" , ["PtLL" , 0 , 400 , 1]]]
        categories += [["llAll_boost_zll" , "CutBoostedCatZllCR"    , ["PtLL" , 0 , 400 , 1]]]
        categories += [["llAll_vbf_ztt"   , "CutVBFCatZttFitCR"     , ["PtLL" , 0 , 400 , 1]]]
        categories += [["llAll_vbf_zll"   , "CutVBFCatZllCR"        , ["PtLL" , 0 , 400 , 1]]]
        
    if args.option=="pttautau":
        categories  = []
        categories += [["llAll_boost_ztt" , "CutBoostedCatZttFitCR" , ["PtTauTau" , 0 , 400 , 1]]]
        categories += [["llAll_boost_zll" , "CutBoostedCatZllCR"    , ["PtTauTau" , 0 , 400 , 1]]]
        categories += [["llAll_vbf_ztt"   , "CutVBFCatZttFitCR"     , ["PtTauTau" , 0 , 400 , 1]]]
        categories += [["llAll_vbf_zll"   , "CutVBFCatZllCR"        , ["PtTauTau" , 0 , 400 , 1]]]
        
    if args.option=="ptllbin":
        categories  = []
        categories += [["llAll_boost_ztt" , "CutBoostedCatZttFitCR" , ["PtLL" , [100,140,300] , 400 , 40]]]
        categories += [["llAll_boost_zll" , "CutBoostedCatZllCR"    , ["PtLL" , 0 , 400 , 40]]]
        categories += [["llAll_vbf_ztt"   , "CutVBFCatZttFitCR"     , ["PtLL" , [0, 100 , 140   , 300] , 400 , 40]]]
        categories += [["llAll_vbf_zll"   , "CutVBFCatZllCR"        , ["PtLL" , 0 , 400 , 40]]]
        
    if args.option=="pttautaubin":
        categories  = []
        categories += [["llAll_boost_ztt" , "CutBoostedCatZttFitCR" , ["PtTauTau" , 0 , 400 , 40]]]
        categories += [["llAll_boost_zll" , "CutBoostedCatZllCR"    , ["PtTauTau" , 0 , 400 , 40]]]
        categories += [["llAll_vbf_ztt"   , "CutVBFCatZttFitCR"     , ["PtTauTau" , 0 , 400 , 40]]]
        categories += [["llAll_vbf_zll"   , "CutVBFCatZllCR"        , ["PtTauTau" , 0 , 400 , 40]]]

    if args.option=="djeteta":
        categories  = []
        categories += [["llAll_boost_ztt" , "CutBoostedCatZttFitCR" , ["DJetEta" , 0 , 10 , 1]]]
        categories += [["llAll_boost_zll" , "CutBoostedCatZllCR"    , ["DJetEta" , 0 , 10 , 1]]]
        categories += [["llAll_vbf_ztt"   , "CutVBFCatZttFitCR"     , ["DJetEta" , 0 , 10 , 1]]]
        categories += [["llAll_vbf_zll"   , "CutVBFCatZllCR"        , ["DJetEta" , 0 , 10 , 1]]]

    if args.option=="mjj":
        categories  = []
        categories += [["llAll_boost_ztt" , "CutBoostedCatZttFitCR" , ["Mjj2" , 0 , 2000 , 1]]]
        categories += [["llAll_boost_zll" , "CutBoostedCatZllCR"    , ["Mjj2" , 0 , 2000 , 1]]]
        categories += [["llAll_vbf_ztt"   , "CutVBFCatZttFitCR"     , ["Mjj2" , 0 , 2000 , 1]]]
        categories += [["llAll_vbf_zll"   , "CutVBFCatZllCR"        , ["Mjj2" , 0 , 2000 , 1]]]
    
    if args.option=="mvis":
        categories  = []
        categories += [["llAll_boost_ztt" , "CutBoostedCatZttFitCR" , ["mvis2" , 80, 100 , 1]]]
        categories += [["llAll_boost_zll" , "CutBoostedCatZllCR"    , ["mvis2" , 80, 100 , 1]]]
        categories += [["llAll_vbf_ztt"   , "CutVBFCatZttFitCR"     , ["mvis2" , 80, 100 , 1]]]
        categories += [["llAll_vbf_zll"   , "CutVBFCatZllCR"        , ["mvis2" , 80, 100 , 1]]]
    
    inputdir=args.inputdir
    if type(args.inputfile) == list:
        inputfile=[ e+":"+args.samplename for e in args.inputfile ]
    else:
        inputfile=args.inputfile+":"+args.samplename
    
    INFO("creating workspace from "+str(inputfile)+" in "+inputdir)
    
    systematicsMap=None
    if args.systematicmap:
       INFO("reading systematics map"+str(args.systematicmap))
       systematicsMap=systematicsHandler.readSystematicsMap(args.systematicmap)

    INFO("creating output file "+args.outputfile)
    out_file      = ROOT.TFile(args.outputfile,"RECREATE")
    
    if type(inputfile)==list:
        full_inputname=[ os.path.join(inputdir,e) for e in inputfile ]
    else:
        full_inputname=os.path.join(inputdir,inputfile)
    INFO("creating WS for nominal in file "+str(full_inputname))
    savedhistos= readInputFile(full_inputname,d_sample,categories,"nominal",out_file,savehists=True,createdirs=True,verbose=True,savetofile=not args.skipnominal)
    out_file.Close()
    print savedhistos

    first=False
    if systematicsMap:
        systematics= [ s for s in systematicsMap.values() if not "nom" in s.name ]
        systematics.sort(key=lambda e: e.name)
        systematics=  [ s for s in systematicsMap.values() if "nom" in s.name ] + systematics
        d_sample=d_sample_syst
        for systematic in systematics:
            if systematic.name=="fake_weight_up":
                fake_weight_up=systematic
            if systematic.name=="fake_weight_down":
                fake_weight_down=systematic
        nomsysthists=None
        for systematic in systematics:
            out_file      = ROOT.TFile(args.outputfile,"UPDATE")
            if type(inputfile)==list:
                full_inputname=[ os.path.join(inputdir,systematic.changeFileName(i)) for i in inputfile ]
            else:
                full_inputname=os.path.join(inputdir,systematic.changeFileName(inputfile))
            if "fakes_" in systematic.name and "low" in systematic.name:
                if type(inputfile)==list:
                    full_inputname=[ os.path.join(inputdir,fake_weight_down.changeFileName(i)) for i in inputfile ]
                else:
                    full_inputname=os.path.join(inputdir,fake_weight_down.changeFileName(inputfile))
            if "fakes_" in systematic.name and "high" in systematic.name:
                if type(inputfile)==list:
                    full_inputname=[ os.path.join(inputdir,fake_weight_up.changeFileName(i)) for  i in inputfile ]
                else:
                    full_inputname=os.path.join(inputdir,fake_weight_up.changeFileName(inputfile))
            if "2015" in systematic.name:
                full_inputname[1]=os.path.join(inputdir,inputfile[1])
            if "2016" in systematic.name:
                full_inputname[0]=os.path.join(inputdir,inputfile[0])
            INFO("checking for systematic "+systematic.name+" in file "+str(full_inputname))
            if type(full_inputname)== list or  os.path.exists(full_inputname.split(":")[0]):
                if ("nom" in systematic.name):
                    INFO("reading nom for systematic "+systematic.name+" in file "+str(full_inputname))
                    nomsysthists=readInputFile(full_inputname,d_sample,categories,systematic.wsalias,out_file,savehists=True,comparehists=savedhistos,savetofile=False)
                else:
                    INFO("creating WS for systematic "+systematic.name+" in file "+str(full_inputname))
                    readInputFile(full_inputname,d_sample,categories,systematic.wsalias,out_file,comparehists=savedhistos,normhists=nomsysthists)
            else:
               WARN("systematic "+systematic.name+" in file "+full_inputname+" does not exist")
            out_file.Close()
            
