#ifndef SETFLAGS
#define SETFLAGS

#include "macros/setup.C"
#include "writeHistos.C"

#include "TString.h"
#include "TEnv.h"

using namespace std;

void setFlags(TString configfile = "config_2012/config_2012.cfg") {

  LOG(logWARNING) << "Using input configuration file: " << configfile;
  TEnv * env = new TEnv(configfile.Data());
  
  // RUN MODE
  // 0 = apply mT cut
  // 1 = cut dphi and fit mT
  // 2 = crazy mode at your CPU's expense
  // 3 = cut mT and fit dphi
  // 4 = mT < mH, fit dphi
  // 5 = 0.75*mH < mT < mH, fit dphi
  // 6 = fit mT, split SR by mll < 25 and 25-50
  // 7 = 2D fit for 0 and 1 jet DF, for testing
  mode              = int(env->GetValue("Setup.mode",6));
  CutMTandRebin     = bool(env->GetValue("Setup.CutMTandRebin",0));

  // MAPPING MODE
  // 0 = nothing
  // 1 = map to emu_signalLike_0j range
  // 2 = map to flat background
  mappingMode       = int(env->GetValue("Setup.mappingMode",2));
  
  // MASS MODE
  // 0 = new low mass (50,1.8)
  // 1 = new high mass (150,inf)
  // 2 = old mid-mass (65,1.8)
  // 3 = alt high mass (inf, inf)
  massMode          = int(env->GetValue("Setup.massMode",3));
  massBoundary      = int(env->GetValue("Setup.massBoundary",200));
  massBoundary2     = int(env->GetValue("Setup.massBoundary2",300));
  useHighMass       = bool(env->GetValue("Setup.useHighMass",0));
  useHighMass2      = bool(env->GetValue("Setup.useHighMass2",0));
  useHighPt         = bool(env->GetValue("Setup.useHighPt",1));
  useLowPt          = bool(env->GetValue("Setup.useLowPt",0));
  useAltCRs         = bool(env->GetValue("Setup.useAltCRs",1));
  doSlopeTest       = bool(env->GetValue("Setup.doSlopeTest",0));
  srSlopeFactor     = bool(env->GetValue("Setup.srSlopeFactor",0));

  // YEAR, PERIODS
  do2011            = bool(env->GetValue("Setup.do2011",0));
  do2012            = bool(env->GetValue("Setup.do2012",1));

  // Z+jets MODE
  // 0 = old SF method
  // 1 = ABCD method
  // 2 = Pacman fit
  ZMode               = int(env->GetValue("Setup.ZMode",2));
  splitEfficiencies   = bool(env->GetValue("Setup.splitEfficiencies",1));
  splitPacmanNFs      = bool(env->GetValue("Setup.splitPacmanNFs",0));
  doABCD2j            = bool(env->GetValue("Setup.doABCD2j",1));
  doPacman2j            = bool(env->GetValue("Setup.doPacman2j",0));

  f_NDY_all           = double(env->GetValue("Setup.f_NDY_all",0.688861));
  f_NDY_SR_0j         = double(env->GetValue("Setup.f_NDY_SR_0j",0.71));
  f_NDY_WWCR_0j       = double(env->GetValue("Setup.f_NDY_WWCR_0j",0.71));
  f_NDY_ZP_0j         = double(env->GetValue("Setup.f_NDY_ZP_0j",0.71));
  f_DY_all_0j         = double(env->GetValue("Setup.f_DY_all_0j",0.27));
  f_NDY_SR_1j         = double(env->GetValue("Setup.f_NDY_SR_1j",0.80));
  f_NDY_WWCR_1j       = double(env->GetValue("Setup.f_NDY_WWCR_1j",0.85));
  f_NDY_ZP_1j         = double(env->GetValue("Setup.f_NDY_ZP_1j",0.79));
  f_DY_all_1j         = double(env->GetValue("Setup.f_DY_all_1j",0.48));
  
  f_NDY_SR_2j         = double(env->GetValue("Setup.f_NDY_SR_2j",0.81117));
  f_NDY_WWCR_2j       = double(env->GetValue("Setup.f_NDY_WWCR_2j",0.85));
  f_NDY_ZP_2j         = double(env->GetValue("Setup.f_NDY_ZP_2j",0.823529));
  f_DY_all_2j         = double(env->GetValue("Setup.f_DY_all_2j", 0.377331));
  ratio_S_NDY_SR_0j   = double(env->GetValue("Setup.ratio_S_NDY_SR_0j",0.915));
  ratio_S_NDY_SR_1j   = double(env->GetValue("Setup.ratio_S_NDY_SR_1j",0.953));
  ratio_S_NDY_SR_2j   = double(env->GetValue("Setup.ratio_S_NDY_SR_2j",1.21));
  epsilon0_data_0j    = double(env->GetValue("Setup.epsilon0_data_0j",0.7));
  epsilon0_data_1j    = double(env->GetValue("Setup.epsilon0_data_1j",0.7));
  epsilon0_data_2j    = double(env->GetValue("Setup.epsilon0_data_2j",0.66));

  // NFs
  NFTop0jLoPT         = double(env->GetValue("Setup.NFTop0jLoPT",1.212));
  NFTop0jHiPT         = double(env->GetValue("Setup.NFTop0jHiPT",1.074));
  NFDY0j              = double(env->GetValue("Setup.NFDY0j",0.708697));
  NFDY1j              = double(env->GetValue("Setup.NFDY1j",0.845));

  // Set correct input directories
  ntupleFolder        = string(env->GetValue("Setup.ntupleFolder","output_2012"));
  basedir             = string(env->GetValue("Setup.basedir","output_2012"));

  // CHANNELS
  // Define flavours to use
  doee                = bool(env->GetValue("Setup.doee",1));
  domm                = bool(env->GetValue("Setup.domm",1));
  doem                = bool(env->GetValue("Setup.doem",1));
  dome                = bool(env->GetValue("Setup.doem",1));
  splitem             = bool(env->GetValue("Setup.splitem",1));
  merge2j             = bool(env->GetValue("Setup.merge2j",1));
  skipSF2j            = bool(env->GetValue("Setup.skipSF2j",1));
  doSFonly            = bool(env->GetValue("Setup.doSFonly",0));
  doSingleBinSF2j     = bool(env->GetValue("Setup.doSingleBinSF2j",1));

  // Define jet multiplicities to use
  do0j                = bool(env->GetValue("Setup.do0j",1));
  do1j                = bool(env->GetValue("Setup.do1j",1));
  do2j                = bool(env->GetValue("Setup.do2j",0));
  combineCRs          = bool(env->GetValue("Setup.combineCRs",1));
  combineSFCRs        = bool(env->GetValue("Setup.combineSFCRs",1));
  mergeSFSR           = bool(env->GetValue("Setup.mergeSFSR",1));
  merge2jSFSR         = bool(env->GetValue("Setup.merge2jSFSR",1));
  merge2jtopCR        = bool(env->GetValue("Setup.merge2jtopCR",1));
  skipSFtopCR         = bool(env->GetValue("Setup.skipSFtopCR",1));
  skipSFWWCR          = bool(env->GetValue("Setup.skipSFWWCR",1));
  doWWCR2j            = bool(env->GetValue("Setup.doWWCR2j",0));
  doOF2jWWCR          = bool(env->GetValue("Setup.doOF2jWWCR",0));
  merge2jWWCR         = bool(env->GetValue("Setup.merge2jWWCR",1));
  subleadptbounds     = string(env->GetValue("Setup.subleadptbounds","10,15,20,1000"));
  mllbounds           = string(env->GetValue("Setup.mllbounds","0,30,50"));

  // SAMPLES
  doggf               = bool(env->GetValue("Setup.doggf",1));
  dovbf               = bool(env->GetValue("Setup.dovbf",1));
  dowh                = bool(env->GetValue("Setup.dowh",1));
  dozh                = bool(env->GetValue("Setup.dozh",1));
  doww                = bool(env->GetValue("Setup.doww",1));
  dowwew              = bool(env->GetValue("Setup.dowwew",0));
  dowzzz              = bool(env->GetValue("Setup.dowzzz",1));
  dowzzzew            = bool(env->GetValue("Setup.dowzzzew",0));
  dozjets             = bool(env->GetValue("Setup.dozjets",1));
  dozjetsew           = bool(env->GetValue("Setup.dozjetsew",0));
  doWjets             = bool(env->GetValue("Setup.doWjets",1));
  doOSmSS             = bool(env->GetValue("Setup.doOSmSS",0)); //Nina
  doSameSignCR	      = bool(env->GetValue("Setup.doSameSignCR",0)); //jdl
  dottbar             = bool(env->GetValue("Setup.dottbar",1));
  dost                = bool(env->GetValue("Setup.dost",1));
  doWg                = bool(env->GetValue("Setup.doWg",1));
  doWgs               = bool(env->GetValue("Setup.doWgs",1));
  doVH                = bool(env->GetValue("Setup.doVH",1));
  splitww             = bool(env->GetValue("Setup.splitww",0));
  splitzjets          = bool(env->GetValue("Setup.splitzjets",1));
  useJHUspin0         = bool(env->GetValue("Setup.useJHUspin0",0));
  include125BG        = bool(env->GetValue("Setup.include125BG",0));

  // SCALING
  useTheoryWW         = bool(env->GetValue("Setup.useTheoryWW",0));
  useTheoryTop        = bool(env->GetValue("Setup.useTheoryTop",0));
  scaleZ              = bool(env->GetValue("Setup.scaleZ",1));
  scaleTop            = bool(env->GetValue("Setup.scaleTop",1));
  wjetsScale          = double(env->GetValue("Setup.wjetsScale",1.0));
  SameSignScale       = double(env->GetValue("Setup.SameSignScale",1.0));//Nina
  scaleLUMI           = bool(env->GetValue("Setup.scaleLUMI",0));
  lumiSF              = double(env->GetValue("Setup.lumiSF",1.538461538));

  // SYSTEMATICS, UNCERTAINTIES
  useDetSys           = bool(env->GetValue("Setup.useDetSys",1));
  useThSys            = bool(env->GetValue("Setup.useThSys",1));
  useStatSys          = bool(env->GetValue("Setup.useStatSys",1));
  statMode            = int(env->GetValue("Setup.statMode",0));
  stat_cutoff         = double(env->GetValue("Setup.stat_cutoff",0.00));
  useShape            = bool(env->GetValue("Setup.useShape",1));
  skipid              = bool(env->GetValue("Setup.skipid",0));
  skipUEPS            = bool(env->GetValue("Setup.skipUEPS",0));
  skiptrackMET        = bool(env->GetValue("Setup.skiptrackMET",0));
  splitjes            = bool(env->GetValue("Setup.splitjes",1));
  useDYmtshape        = bool(env->GetValue("Setup.useDYmtshape",1));
  decoFakes           = bool(env->GetValue("Setup.decoFakes",0));
  useOldBtag          = bool(env->GetValue("Setup.useOldBtag",0));

  // BLINDING OPTIONS
  doData              = bool(env->GetValue("Setup.doData",1));
  conditionalAsimov   = bool(env->GetValue("Setup.conditionalAsimov",1));
  dosignalregion      = bool(env->GetValue("Setup.dosignalregion",1));
  doblindsignalregion = bool(env->GetValue("Setup.doblindsignalregion",0));

  // BINNING AND CUTTING OPTIONS
  nrBins_0j           = int(env->GetValue("Setup.nrBins_0j",5));
  nrBins_1j           = int(env->GetValue("Setup.nrBins_1j",3));
  nrBins_2j           = int(env->GetValue("Setup.nrBins_2j",4));
  nrBins_lowPt        = int(env->GetValue("Setup.nrBins_lowPt",2));
  doSingleBinCR       = bool(env->GetValue("Setup.doSingleBinCR",1));
  doSingleBin2j       = bool(env->GetValue("Setup.doSingleBin2j",0));
  variableBin2j       = bool(env->GetValue("Setup.variableBin2j",1));
  overrideCuts        = bool(env->GetValue("Setup.overrideCuts",0));
  
  // CHECKS
  doCRVRcheck         = bool(env->GetValue("Setup.doCRVRcheck",0));
  doPreHCP            = bool(env->GetValue("Setup.doPreHCP",0));
  doPostHCP           = bool(env->GetValue("Setup.doPostHCP",0));

  // OTHER STUFF
  rewriteXML          = bool(env->GetValue("Setup.rewriteXML",1));
  flatInterpCode      = int(env->GetValue("Setup.flatInterpCode",4));
  shapeInterpCode     = int(env->GetValue("Setup.shapeInterpCode",4));
  useLumiAsPOI        = bool(env->GetValue("Setup.useLumiAsPOI",0));
  wjMode              = int(env->GetValue("Setup.wjMode",0));
  signFix             = bool(env->GetValue("Setup.signFix",1));

  // OLD STUFF
  bt20GeV             = bool(env->GetValue("Setup.bt20GeV",0));

  // VBF
  doVBF2j             = bool(env->GetValue("Setup.doVBF2j",0));
  splitmjj            = bool(env->GetValue("Setup.splitmjj",0));
  splitmjjtopCR       = bool(env->GetValue("Setup.splitmjjtopCR",0));
  profileggf          = bool(env->GetValue("Setup.profileggf",0));
  doMjjReweight       = bool(env->GetValue("Setup.doMjjReweight",0));
  doFlatMjj           = bool(env->GetValue("Setup.doFlatMjj",0));
  doZttVeto           = bool(env->GetValue("Setup.doZttVeto",1));
  doOneBTag           = bool(env->GetValue("Setup.doOneBTag",1));
  doTrackMET2j        = bool(env->GetValue("Setup.doTrackMET2j",0));
  doFlat2j            = bool(env->GetValue("Setup.doFlat2j",0));

  // PROPERTIES
  doSpin              = bool(env->GetValue("Setup.doSpin",0));
  doSpin1D            = bool(env->GetValue("Setup.doSpin1D",0));
  doCutBasedSpin      = bool(env->GetValue("Setup.doCutBasedSpin",0)); // needs doMVA = 1
  applySlopeSpin      = bool(env->GetValue("Setup.applySlopeSpin",0));

  // MVA
  doMVA               = bool(env->GetValue("Setup.doMVA",0));
  treeName            = string(env->GetValue("Setup.treeName","MVATree"));
  nrBins0j_MVA        = int(env->GetValue("Setup.nrBins0j_MVA",10));
  nrBins1j_MVA        = int(env->GetValue("Setup.nrBins1j_MVA",5));
  nrBins2j_MVA        = int(env->GetValue("Setup.nrBins2j_MVA",5));

  // Cut values
  useTopMVA1j         = bool(env->GetValue("Setup.useTopMVA1j",0));
  CutCJVleadPT        = double(env->GetValue("Setup.CutCJVleadPT",20));
  CutDPhi             = double(env->GetValue("Setup.CutDPhi",1.8));
  CutDPhi2j           = double(env->GetValue("Setup.CutDPhi2j",1.8));
  CutDPhillMET        = double(env->GetValue("Setup.CutDPhillMET",1.57));
  CutDPhiCR           = double(env->GetValue("Setup.CutDPhiCR",10e9));
  CutDYjj             = double(env->GetValue("Setup.CutDYjj",2.8));
  CutFRecoil0j        = double(env->GetValue("Setup.CutFRecoil0j",0.05));
  CutFRecoil1j        = double(env->GetValue("Setup.CutFRecoil1j",0.2));
  CutFRecoil2j        = double(env->GetValue("Setup.CutFRecoil2j",0.2));
  CutMETDF            = double(env->GetValue("Setup.CutMETDF",20));
  CutMETSF2j          = double(env->GetValue("Setup.CutMETSF2j",45));
  CutMETrelDF         = double(env->GetValue("Setup.CutMETrelDF",25));
  CutMETrelSF0j       = double(env->GetValue("Setup.CutMETrelSF0j",45));
  CutMETrelSF1j       = double(env->GetValue("Setup.CutMETrelSF1j",45));
  CutMETstvfSF2j      = double(env->GetValue("Setup.CutMETstvfSF2j",35));
  CutMETrelSF2j       = double(env->GetValue("Setup.CutMETrelSF2j",35));
  CutMETrelstvfSF2j   = double(env->GetValue("Setup.CutMETrelstvfSF2j",35));
  CutMjj              = double(env->GetValue("Setup.CutMjj",500.0));
  CutMT               = double(env->GetValue("Setup.CutMT",10e9));
  CutMT2j             = double(env->GetValue("Setup.CutMT2j",10e9));
  CutMTUp             = double(env->GetValue("Setup.CutMTUp",0));
  CutMTUp2j           = double(env->GetValue("Setup.CutMTUp2j",0));
  CutMll              = double(env->GetValue("Setup.CutMll",50));
  CutMll2j            = double(env->GetValue("Setup.CutMll2j",60));
  CutMllCR            = double(env->GetValue("Setup.CutMllCR",80));
  CutMllZBhi          = double(env->GetValue("Setup.CutMllZBhi",106.1876));
  CutMllZBlo          = double(env->GetValue("Setup.CutMllZBlo",76.1876));
  CutMttSpin          = double(env->GetValue("Setup.CutMttSpin",100));
  CutPTllDF           = double(env->GetValue("Setup.CutPTllDF",30));
  CutPTllSF           = double(env->GetValue("Setup.CutPTllSF",30));
  CutPTlljetsDF       = double(env->GetValue("Setup.CutPTlljetsDF",0));
  CutPTlljetsSF       = double(env->GetValue("Setup.CutPTlljetsSF",0));
  CutPTlljetsSF2j     = double(env->GetValue("Setup.CutPTlljetsSF2j",25));
  CutPTmeel           = double(env->GetValue("Setup.CutPTmeel",0));
  CutPTtot            = double(env->GetValue("Setup.CutPTtot",30));
  CutPTtot2j          = double(env->GetValue("Setup.CutPTtot2j",45));
  CutTrackMETSF0j     = double(env->GetValue("Setup.CutTrackMETSF0j",45));
  CutTrackMETSF1j     = double(env->GetValue("Setup.CutTrackMETSF1j",45));
  CutTrackMETSF2j     = double(env->GetValue("Setup.CutTrackMETSF2j",35));
  CutTopMVA1j         = double(env->GetValue("Setup.CutTopMVA1j",0.05));

  std::cout << "==========================================" << std::endl;
  std::cout << "Printing local settings which will be used" << std::endl;
  env->Print();
  std::cout << "==========================================" << std::endl;
}

#endif
