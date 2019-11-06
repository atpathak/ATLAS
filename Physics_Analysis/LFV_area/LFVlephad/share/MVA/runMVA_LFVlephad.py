from QFramework import *
from ROOT import *
import re
from xml.dom import minidom

def runMVA(mva):
  # do the initial setup, provide the output file name
  # mva.createFactory("myBDTOutput.root","!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification")

  channel = "etau"
  #cutstage = "Cut01jet"
  #cutstage = "Cut2jet"
  cutstage = "CutPreselection"

  name = "BDT_" + channel + "_" + cutstage + "_" + mva.getTagStringDefault("eventSelector","").Data()
  mva.createFactory(name+".root","V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification")
    
  mva.setTagBool("makeCounters",True)
    
  # add the signal and background samples (by path)

  mva.addSignal("sig/"+channel)
  mva.addBackground("bkg/"+channel+"/ZllSh")
  mva.addBackground("bkg/"+channel+"/ZttSh")
  mva.addBackground("bkg/"+channel+"/Fake")
  mva.addBackground("bkg/"+channel+"/htt")
    
  # print the list of samples (debugging only)
  #mva.printListOfSamples(TQMVA.Signal)
  #mva.printListOfSamples(TQMVA.Background)
    
  # set the base cut to be used
  cut = mva.useCut(cutstage)
  INFO("using cut '{:s}'".format(cut.GetName()))
    
    # book the variables to be used
    # the arguments are:
    #   mva.bookVariable(name, expression, title, type, min, max)
    # where:
    #   name is some arbitrary identifier
    #   expression is the definition of the variable (using the actual branch names)
    #   title is the string to be used for the histogram and axis titles (latex-formatted)
    #   type is the TMVA variable type
    #   min and max are the lower and upper bounds forwarded to the TMVA
    
  mva.bookVariable("collMass"   , "coll_approx_lfv_m"     ,"m_{coll} [GeV]"          , 50.,300.)
  mva.bookVariable("transverseMassLepMET"   , "lephad_mt_lep0_met"     ,"m_{T}(l,MET) [GeV]"          , 0.,200.)
  mva.bookVariable("transverseMassTauMET"   , "taumet_transverse_mass"     ,"m_{T}(#tau,MET) [GeV]"          , 0.,200.)
  #mva.bookVariable("visibleMass"   , "lephad_vis_mass"     ,"m_{vis} [GeV]"          , 50.,300.)
  #mva.bookVariable("sumPT"   , "lephad_vect_sum_pt"     ,"#Sigma p_{T} [GeV]"          , 0.,200.)
  mva.bookVariable("leptonPt"   , "lep_0_pt"     ,"p_{T}^{lep} [GeV]"          , 20.,100.)
  mva.bookVariable("tauPt"   , "tau_0_pt"     ,"p_{T}^{#tau} [GeV]"          , 20.,100.)
  mva.bookVariable("dAlpha"   , "$(dAlpha)"     ,"#Delta#alpha"          , -5.,5.)
  mva.bookVariable("CosDPhi"   , "$(cosDPhi)"     ,"#sum cos(#Delta#phi)"          , -2.5,2.5)
  mva.bookVariable("met"   , "met_reco_et"     ,"MET"          , 0.,100.)
  #mva.bookVariable("dEtaJets"   , "jets_delta_eta"     ,"#Delta#eta_{jj}"          , 0.,7.)
  #mva.bookVariable("dRTauLep"   , "sqrt( $(dPhiTauLep)*$(dPhiTauLep) + $(dEtaTauLep)*$(dEtaTauLep) )"     ,"#Delta R(#tau,l)"          , 0.,4.)
  if cutstage is "Cut2jet":
    mva.bookVariable("Mjj"   , "jets_visible_mass"     ,"M_{jj}"          , 0.,2000.)
    mva.bookVariable("DEtajj"   , "jets_delta_eta"     ,"#Delta#eta_{jj}"          , 0.,7.)

  INFO("HERE")    
  
  # set the verbosity of the TQMVA object
  mva.setVerbose(True)
  
    # read in the samples 
    # this one actually contains the event loop and may take a long time
    # mva.readSamples()
    
  INFO("HERE")    
  if (mva.getTagStringDefault("eventSelector","") == "") :
      #mva.readSamples(TQMVA.AllTrainEventSelector())
    mva.readSamples()
  if (mva.getTagStringDefault("eventSelector","") == "EVEN") :
    mva.readSamples(TQMVA.EvenOddEventSelector())
  if (mva.getTagStringDefault("eventSelector","") == "ODD") :
    mva.readSamples(TQMVA.OddEvenEventSelector())
  if (mva.getTagStringDefault("eventSelector","") == "0") :
    mva.readSamples(TQMVA.Event0Selector())
  if (mva.getTagStringDefault("eventSelector","") == "1") :
    mva.readSamples(TQMVA.Event1Selector())
  if (mva.getTagStringDefault("eventSelector","") == "2") :
    mva.readSamples(TQMVA.Event2Selector())
  if (mva.getTagStringDefault("eventSelector","") == "3") :
    mva.readSamples(TQMVA.Event3Selector())
  if (mva.getTagStringDefault("eventSelector","") == "4") :
    mva.readSamples(TQMVA.Event4Selector())
  if (mva.getTagStringDefault("eventSelector","") == "5") :
    mva.readSamples(TQMVA.Event5Selector())
  if (mva.getTagStringDefault("eventSelector","") == "6") :
    mva.readSamples(TQMVA.Event6Selector())
  if (mva.getTagStringDefault("eventSelector","") == "7") :
    mva.readSamples(TQMVA.Event7Selector())
  if (mva.getTagStringDefault("eventSelector","") == "8") :
    mva.readSamples(TQMVA.Event8Selector())
  if (mva.getTagStringDefault("eventSelector","") == "9") :
    mva.readSamples(TQMVA.Event9Selector())

  # prepare the input trees
  mva.prepareTrees()
      
  # retrieve the TMVA factory object
  factory = mva.getFactory()
    

    # book the BDT  method
  BDT_opt ="!H:!V"
  BDT_opt += ":NTrees=1000"
  BDT_opt += ":BoostType=Grad"
  BDT_opt += ":Shrinkage=0.1"# 0.06 0.01  0.05" ##RunI Shrinkage=0.125 BDT_opt += ":Shrinkage=0.01"
  BDT_opt += ":UseBaggedBoost=True"
  #BDT_opt += ":GradBaggingFraction=0.01" ##RunI GradBaggingFraction=0.25
  #BDT_opt += ":UseBaggedGrad=True"
  BDT_opt += ":BaggedSampleFraction=0.5" ##0.5 0.01
  BDT_opt += ":nCuts=50" 
  BDT_opt += ":MaxDepth=5"
  BDT_opt += ":MinNodeSize=5"
  BDT_opt += ":PruneMethod=CostComplexity"
  #BDT_opt += ":PruneStrength=50"
  BDT_opt += ":NegWeightTreatment=IgnoreNegWeightsInTraining"
  #options for BDT's
  BDT_opt += ":SeparationType=GiniIndex"
  # BDT_opt += ":UseWeightedTrees=True";
  # monitoring the BDT options
  BDT_opt += ":DoBoostMonitor=True"
  BDT_opt += ":DoBoostMonitor"
  #BDT_Opt += ":";  
#factory.BookMethod( TMVA.Types.kBDT, name, "!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:DoBoostMonitor:MaxDepth=3:NegWeightTreatment=IgnoreNegWeightsInTraining" );
  ##  RunI : !H:!V:NTrees=1000:MaxDepth=5:nEventsMin=1000:BoostType=Grad:Shrinkage=0.125:UseBaggedGrad=True:GradBaggingFraction=0.25:DoBoostMonitor:IgnoreNegWeightsInTraining=False
      
  factory.BookMethod( TMVA.Types.kBDT, name,BDT_opt)
  
  # factory.BookMethod( TMVA.Types.kFisher, "myFisher", "H:!V:Fisher" );
  
  # perform the TMVA magic
  factory.TrainAllMethods()
  factory.TestAllMethods()
  factory.EvaluateAllMethods()
  
  # save and close the output file
  mva.closeOutputFile()
  
  # read in the xml weights file 
        # this is necessary because the xml encoding of TMVA xml files is occasionally broken
        #f = open('weights/runFisher_myFisher.weights.xml','r')
  #f = open('weights/run2-vbf-MVA_BTD-TotBkg.weights.xml','r')
  #sanitized = ""
  #for line in f: 
  #sanitized = sanitized + re.sub(r'[^\x00-\x7F]+','', line)
  
  # use an xml-parser to loop over the coefficients and variables
  # extract the values and expressions to compile a transformation expression
  # that maps the input variables to the Fisher discriminant 
  # xmldoc = minidom.parseString(sanitized)
  # coefficients = xmldoc.getElementsByTagName('Coefficient') 
  # trf = ""
  # INFO("Fisher coefficients:")
  # for c in coefficients :
  # index = int(c.attributes['Index'].value)
  # value = float(c.attributes['Value'].value)
  # print("\tc{:d} = {:f}".format(index,value))
  # if index == 0:
  # trf = "{:f}".format(value)
  # else:
  # for var in xmldoc.getElementsByTagName('Variable'):
  # varidx = int(var.attributes['VarIndex'].value)
  # if varidx+1 == index:
  # name = var.attributes['Internal'].value
  # expr = mva.getVariableExpression(name)
  # if value > 0:
  # trf += " + ({:f} * {:s})".format(abs(value),expr)
  # else:
  # trf += " - ({:f} * {:s})".format(abs(value),expr)
  # # print the resulting expression
  # INFO("Fisher transformation: '{:s}'".format(trf))
  
  return True
