#include <dirent.h>
#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooDataSet.h"
#include "RooMinimizerFcn.h"
#include "RooNLLVar.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"
#include "RooCategory.h"
#include "TPRegexp.h"
#include "TFile.h"
#include "TCanvas.h"
#include "RooPlot.h"

// C++
#include <iostream>
#include <set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <time.h>
#include <limits>

// Root
#include "TROOT.h"
#include "TSystem.h"
#include "TStopwatch.h"
#include "TVectorD.h"


#include "macros/makeAsimovData.C"
#include "macros/minimize.C"
#include "macros/findSigma.C"

bool blind(false);

double subtractError(double err12, double err1)
{
  double check(err12*err12-err1*err1);
  //if(check < 0) { cout << "NEGATIVE" << endl; }
  return sqrt(fabs(check));
}

double subtractFractionalError(double err12, double err1)
{
  double result = 1 - err1*err1/(err12*err12);
  return result;
}

double fractionalError(double err12, double err1)
{
    double result = err1*err1/(err12*err12);
    return result;
}

void SubtractAndSay(TString say, double err1_hi, double err1_lo, double err12_hi, double err12_lo)
{
  cout << say << " : + " << 1 - err1_hi*err1_hi/(err12_hi*err12_hi) << " /- " << 1 - err1_lo*err1_lo/(err12_lo*err12_lo) << endl;
  cout << " \t  : + " << subtractError(err12_hi, err1_hi) << " / - " << subtractError(err12_lo, err1_lo) << endl;
}

void newGetMuHat(const char* workspace,
        const bool doAsimov = 0,
          //0 = do observed error
          //1 = do expected mu=1 error
        const int mode = 1,
          //0 = standard
          //1 = cb
        const char* mass = "125",
        TString poiStr              = "SigXsecOverSM",
        const char* wsName          = "combined",
        const char* modelConfigName = "ModelConfig",
        const char* dataName        = "obsData",
        const bool doDummy          = false,
        // false = normal running
        // true =don't call findSigma, run just to get set names
	const bool doHesse          = false,
	double mu_asimov = 1,
        // for calling and printing Hesse for each NP set
	const TString& overhead_path   = "")
        // storing the total error and snapshots of the overhead for a given workspace; looks like result_prefix+(.root|.txt)
{

  TStopwatch*timer=new TStopwatch();
  timer->Start();
  TString inFileName = "workspaces/";
  inFileName += workspace;  inFileName += "/combined/";
  inFileName += mass;       inFileName += ".root";
  TFile* file = new TFile(inFileName.Data());
  RooWorkspace* ws = (RooWorkspace*)file->Get(wsName);
  if (!ws)
  {

    cout << "ERROR::Workspace: " << wsName << " doesn't exist!" << endl;
    return;
  }
  ModelConfig* mc = (ModelConfig*)ws->obj(modelConfigName);
  if (!mc)
  {
    cout << "ERROR::ModelConfig: " << modelConfigName << " doesn't exist!" << endl;
    return;
  }


  RooDataSet* data = (RooDataSet*)ws->data(dataName);
  if (!data && doAsimov == 0)
  {
    cout << "ERROR::Dataset: " << dataName << " doesn't exist!" << endl;
    return;
  }

  cout << "Loaded Workspace "<< workspace << "  - "<< ws->GetName()<<" , configuration and dataset." << endl;
  //ws->loadSnapshot("nominalNuis");


  const int npsets_max  = 60;
  const int nstrmax = 30;
  TString *npsetname[npsets_max];
  TString *npsetstring[npsets_max][nstrmax];
  vector<TString*>npsetands(npsets_max,new TString(""));
  int npnstr[npsets_max];
  int npsets = 0;
  bool npsetexclude[npsets_max];

  double err_raw_hi[npsets_max];
  double err_raw_lo[npsets_max];
  double err_npset_hi[npsets_max];
  double err_npset_lo[npsets_max];

  for (int iset = 0; iset < npsets_max; iset++){
      npsetname[iset]  = new TString("xxxxx");
      for (int istr = 0; istr < nstrmax; istr++){
          npsetstring[iset][istr]  = new TString("xxxxx");
      }
      npsetexclude[iset]  = false;
      err_raw_hi[iset]  = 0.0;
      err_raw_lo[iset]  = 0.0;
      err_npset_hi[iset]  = 0.0;
      err_npset_lo[iset]  = 0.0;
  }

  //*********************************************************************************
  //*********************************************************************************
  //                           Common to all modes
  //*********************************************************************************
  //*********************************************************************************

    npsetname[0] = new TString("Total");

    npsetname[1]        = new TString("DataStat");
    npsetexclude[1]     = true;

    npsetname[2]        = new TString("FullSyst");

  // Warning: Start all NP set indexes with 2 in the mode-specific section!
    
  //*********************************************************************************
  //*********************************************************************************
  //                           Mode 0
  //*********************************************************************************
  //*********************************************************************************


  if (mode == 0)
  {

    npsetname[3]        = new TString("Floating norms");
    npsetstring[3][0]   = new TString("norm");

    npsetname[4]        = new TString("All Norms");
    npsetstring[4][0]   = new TString("norm");
    npsetstring[4][1]   = new TString("Norm");

    npsetname[5]        = new TString("AllOtherSysts");
    npsetstring[5][0]   = new TString("norm");
    npsetstring[5][1]   = new TString("Norm");
    npsetexclude[5]     = true;

    npsetname[6]        = new TString("ThH");
    npsetstring[6][0]   = new TString("Theory");

    npsetname[7]        = new TString("ThD");
    npsetstring[7][0]   = new TString("VV");




  //*********************************************************************************
  //*********************************************************************************
  //                           Mode 1
  //*********************************************************************************
  //*********************************************************************************
  }
  else if (mode == 1)
  {

    npsetname[3]        = new TString("Floating normalizations");
    npsetstring[3][0]     = new TString("norm");

    npsetname[4]        = new TString("All normalizations");
    npsetstring[4][0]   = new TString("norm");
    npsetstring[4][1]   = new TString("Norm");

    npsetname[5]        = new TString("All but normalizations");
    npsetstring[5][0]   = new TString("norm");
    npsetstring[5][1]   = new TString("Norm");
    npsetexclude[5]     = true;

    npsetname[6]        = new TString("Jets MET");
    npsetstring[6][0]   = new TString("SysJET");
    npsetstring[6][1]   = new TString("SysMET");
    npsetstring[6][2]   = new TString("SysFATJET");
    npsetstring[6][3]   = new TString("SysJER");

    npsetname[7]        = new TString("BTag");
    npsetstring[7][0]   = new TString("SysFT");

    npsetname[8]        = new TString("Leptons");
    npsetstring[8][0]   = new TString("SysEG");
    npsetstring[8][1]   = new TString("SysEL");
    npsetstring[8][2]   = new TString("SysMUONS");
      
    npsetname[9]        = new TString("Luminosity");
    npsetstring[9][0]     = new TString("LUMI");

    npsetname[10]        = new TString("Diboson");
    npsetstring[10][0]   = new TString("VV");
    npsetstring[10][1]   = new TString("WW");
    npsetstring[10][2]   = new TString("ZZ");
    npsetstring[10][3]   = new TString("WZ");

    npsetname[11]        = new TString("Zjets");
    npsetstring[11][0]   = new TString("ZDPhi");
    npsetstring[11][1]   = new TString("ZMbb");
    npsetstring[11][2]   = new TString("ZPt");
    npsetstring[11][3]   = new TString("Zcl");
    npsetstring[11][4]   = new TString("Zbb");
    npsetstring[11][5]   = new TString("Zl");
    npsetstring[11][6]   = new TString("Zbc");
    npsetstring[11][7]   = new TString("Zbl");
    npsetstring[11][8]   = new TString("Zcc");

    npsetname[12]        = new TString("Wjets");
    npsetstring[12][0]   = new TString("WDPhi");
    npsetstring[12][1]   = new TString("WMbb");
    npsetstring[12][2]   = new TString("WbbMbb");
    npsetstring[12][3]   = new TString("WPt");
    npsetstring[12][4]   = new TString("Wbb");
    npsetstring[12][5]   = new TString("Wbc");
    npsetstring[12][6]   = new TString("Wbl");
    npsetstring[12][7]   = new TString("Wcc");
    npsetstring[12][8]   = new TString("Wcl");
    npsetstring[12][9]   = new TString("Wl");

    npsetname[13]        = new TString("Model ttbar");
    npsetstring[13][0]   = new TString("TtBar");
    npsetstring[13][1]   = new TString("ttBarHigh");
    npsetstring[13][2]   = new TString("TopPt");
    npsetstring[13][3]   = new TString("TTbarPTV");
    npsetstring[13][4]   = new TString("TTbarMBB");
    npsetstring[13][5]   = new TString("ttbar");

    npsetname[14]        = new TString("Model Single Top");
    npsetstring[14][0]   = new TString("stop");
    npsetstring[14][1]   = new TString("Stop");
      
    npsetname[15]        = new TString("Model Multi Jet");
    npsetstring[15][0]   = new TString("MJ");
      
    npsetname[16]        = new TString("Signal Systematics");
    npsetstring[16][0]     = new TString("Theory");
    npsetstring[16][1]     = new TString("VH");
      

    npsetname[17]        = new TString("MC stat");
    npsetstring[17][0]     = new TString("gamma");



  //*********************************************************************************
  //*********************************************************************************
  //                           Mode 3
  //                      VH run 1 paper table
  //*********************************************************************************
  //*********************************************************************************
  }  else if (mode == 3)
  {

    int i = 2;
    int j = 0;

    i++; j = 0;
    npsetname[i]        = new TString("W+jets");
    npsetstring[i][j++]     = new TString("WPtV");
    npsetstring[i][j++]     = new TString("WDPhi");
    npsetstring[i][j++]     = new TString("WMbb");
    npsetstring[i][j++]     = new TString("WbbRatio");
    npsetstring[i][j++]     = new TString("WlNorm");
    npsetstring[i][j++]     = new TString("WclNorm");
    npsetstring[i][j++]     = new TString("WhfNorm");
    npsetstring[i][j++]     = new TString("norm_W");

    i++; j = 0;
    npsetname[i]        = new TString("W+jets norm");
    npsetstring[i][j++]     = new TString("norm_W");

    i++; j = 0;
    npsetname[i]        = new TString("W+jets model");
    npsetstring[i][j++]     = new TString("WPtV");
    npsetstring[i][j++]     = new TString("WDPhi");
    npsetstring[i][j++]     = new TString("WMbb");
    npsetstring[i][j++]     = new TString("WbbRatio");
    npsetstring[i][j++]     = new TString("WlNorm");
    npsetstring[i][j++]     = new TString("WclNorm");
    npsetstring[i][j++]     = new TString("WhfNorm");

    //HERE
    i++; j = 0;
    npsetname[i]        = new TString("Z+jets");
    npsetstring[i][j++]     = new TString("ZPtV");
    npsetstring[i][j++]     = new TString("ZDPhi");
    npsetstring[i][j++]     = new TString("ZMbb");
    npsetstring[i][j++]     = new TString("ZbbRatio");
    npsetstring[i][j++]     = new TString("ZlNorm");
    npsetstring[i][j++]     = new TString("ZclNorm");
    npsetstring[i][j++]     = new TString("ZbbNorm");
    npsetstring[i][j++]     = new TString("norm_Z");

    i++; j = 0;
    npsetname[i]        = new TString("Z+jets norm");
    npsetstring[i][j++]     = new TString("norm_Z");

    i++; j = 0;
    npsetname[i]        = new TString("Z+jets modelling");
    npsetstring[i][j++]     = new TString("ZPtV");
    npsetstring[i][j++]     = new TString("ZDPhi");
    npsetstring[i][j++]     = new TString("ZMbb");
    npsetstring[i][j++]     = new TString("ZbbRatio");
    npsetstring[i][j++]     = new TString("ZlNorm");
    npsetstring[i][j++]     = new TString("ZclNorm");
    npsetstring[i][j++]     = new TString("ZbbNorm");

    i++; j = 0;
    npsetname[i]        = new TString("Signal Theory");
    npsetstring[i][j++]     = new TString("Theory");

    i++; j = 0;
    npsetname[i]        = new TString("All b-tagging");
    npsetstring[i][j++]     = new TString("BTag");
    npsetstring[i][j++]     = new TString("TruthTagDR");

    i++; j = 0;
    npsetname[i]        = new TString("Diboson");
    npsetstring[i][j++]     = new TString("VVMbb");
    npsetstring[i][j++]     = new TString("VVJet");

    i++; j = 0;
    npsetname[i]        = new TString("b-tagging (c-jets)");
    npsetstring[i][j++]     = new TString("BTagC");
    npsetstring[i][j++]     = new TString("TruthTagDR");

    i++; j = 0;
    npsetname[i]        = new TString("b-tagging (b-jets)");
    npsetstring[i][j++]     = new TString("BTagB");

    i++; j = 0;
    npsetname[i]        = new TString("b-tagging (light-jets)");
    npsetstring[i][j++]     = new TString("BTagL");

    i++; j = 0;
    npsetname[i]        = new TString("MJ");
    npsetstring[i][j++]     = new TString("MJ");

    i++; j = 0;
    npsetname[i]        = new TString("Jets");
    npsetstring[i][j++]     = new TString("JVF");
    npsetstring[i][j++]     = new TString("JetN");
    npsetstring[i][j++]     = new TString("JetE");
    npsetstring[i][j++]     = new TString("JetF");
    npsetstring[i][j++]     = new TString("JetPile");
    npsetstring[i][j++]     = new TString("JetB");
    npsetstring[i][j++]     = new TString("JetM");
    npsetstring[i][j++]     = new TString("JetR");

    i++; j = 0;
    npsetname[i]        = new TString("MET");
    npsetstring[i][j++]     = new TString("MET");

    i++; j = 0;
    npsetname[i]        = new TString("Jets&MET");
    npsetstring[i][j++]     = new TString("JVF");
    npsetstring[i][j++]     = new TString("JetN");
    npsetstring[i][j++]     = new TString("JetE");
    npsetstring[i][j++]     = new TString("JetF");
    npsetstring[i][j++]     = new TString("JetPile");
    npsetstring[i][j++]     = new TString("JetB");
    npsetstring[i][j++]     = new TString("JetM");
    npsetstring[i][j++]     = new TString("JetR");
    npsetstring[i][j++]     = new TString("MET");

    i++; j = 0;
    npsetname[i]        = new TString("Leptons");
    npsetstring[i][j++]     = new TString("Elec");
    npsetstring[i][j++]     = new TString("Muon");
    npsetstring[i][j++]     = new TString("LepVeto");

    i++; j = 0;
    npsetname[i]        = new TString("Single Top");
    npsetstring[i][j++]     = new TString("stop");
    npsetstring[i][j++]     = new TString("Chan");

    i++; j = 0;
    npsetname[i]        = new TString("ttbar");
    npsetstring[i][j++]     = new TString("norm_ttbar");
    npsetstring[i][j++]     = new TString("TopPt");
    npsetstring[i][j++]     = new TString("Ttbar");
    npsetstring[i][j++]     = new TString("ttbarHighPtV");
    npsetstring[i][j++]     = new TString("ttbarNorm");

    i++; j = 0;
    npsetname[i]        = new TString("ttbar Norm");
    npsetstring[i][j++]     = new TString("norm_ttbar");

    i++; j = 0;
    npsetname[i]        = new TString("ttbar Model");
    npsetstring[i][j++]     = new TString("TopPt");
    npsetstring[i][j++]     = new TString("Ttbar");
    npsetstring[i][j++]     = new TString("ttbarHighPtV");
    npsetstring[i][j++]     = new TString("ttbarNorm");

    i++; j = 0;
    npsetname[i]        = new TString("Float + Norm");
    npsetstring[i][j++]     = new TString("norm");
    npsetstring[i][j++]     = new TString("Norm");
    npsetstring[i][j++]     = new TString("Ratio");
    npsetstring[i][j++]     = new TString("MJ_");

    i++; j = 0;
    npsetname[i]        = new TString("Float");
    npsetstring[i][j++]     = new TString("norm");

    i++; j = 0;
    npsetname[i]        = new TString("Model Norm");
    npsetstring[i][j++]     = new TString("Norm");
    npsetstring[i][j++]     = new TString("Ratio");
    npsetstring[i][j++]     = new TString("MJ_");

    i++; j = 0;
    npsetname[i]        = new TString("Lumi");
    npsetstring[i][j++]     = new TString("LUMI_2012");

  }
  
  // STUDYING JES SYSTEMATICS 
  // comparing 19NP and 3NP scheme for SM VH analysis
   else if (mode == 4)
  {
    npsetname[3]         = new TString("19NP JES systematics");
    npsetstring[3][0]    = new TString("JET_19NP_JET_EffectiveNP_1");
    npsetstring[3][1]    = new TString("JET_19NP_JET_EffectiveNP_2");
    npsetstring[3][2]    = new TString("JET_19NP_JET_EffectiveNP_3");
    npsetstring[3][3]    = new TString("JET_19NP_JET_EffectiveNP_4");
    npsetstring[3][4]    = new TString("JET_19NP_JET_EffectiveNP_5");
    npsetstring[3][5]    = new TString("JET_19NP_JET_EffectiveNP_6restTerm");
    npsetstring[3][6]    = new TString("JET_19NP_JET_EtaIntercalibration_Modelling");
    npsetstring[3][7]    = new TString("JET_19NP_JET_EtaIntercalibration_TotalStat");
    npsetstring[3][8]    = new TString("JET_19NP_JET_EtaIntercalibration_NonClosure");
    npsetstring[3][9]    = new TString("JET_19NP_JET_Pileup_OffsetMu");
    npsetstring[3][10]   = new TString("JET_19NP_JET_Pileup_OffsetNPV");
    npsetstring[3][11]   = new TString("JET_19NP_JET_Pileup_PtTerm");
    npsetstring[3][12]   = new TString("JET_19NP_JET_Pileup_RhoTopology");
    npsetstring[3][13]   = new TString("JET_19NP_JET_Flavor_Composition");
    npsetstring[3][14]   = new TString("JET_19NP_JET_Flavor_Response");
    npsetstring[3][15]   = new TString("JET_19NP_JET_BJES_Response");
    npsetstring[3][16]   = new TString("JET_19NP_JET_PunchThrough_MCTYPE");
    npsetstring[3][17]   = new TString("JET_19NP_JET_SingleParticle_HighPt");

    npsetname[4]         = new TString("19NP JES systematics: Eff NP");
    npsetstring[4][0]    = new TString("JET_19NP_JET_EffectiveNP_1");
    npsetstring[4][1]    = new TString("JET_19NP_JET_EffectiveNP_2");
    npsetstring[4][2]    = new TString("JET_19NP_JET_EffectiveNP_3");
    npsetstring[4][3]    = new TString("JET_19NP_JET_EffectiveNP_4");
    npsetstring[4][4]    = new TString("JET_19NP_JET_EffectiveNP_5");
    npsetstring[4][5]    = new TString("JET_19NP_JET_EffectiveNP_6restTerm");

    //npsetname[3]        = new TString("3NP JES systematics");
    //npsetstring[3][0]   = new TString("JET_SR1_JET_GroupedNP_1");
    //npsetstring[3][1]   = new TString("JET_SR1_JET_GroupedNP_2");
    //npsetstring[3][2]   = new TString("JET_SR1_JET_GroupedNP_3");
    //npsetstring[3][3]   = new TString("JET_19NP_JET_EtaIntercalibration_NonClosure");

    npsetname[5]        = new TString("All but JES systematics");
    npsetstring[5][0]    = new TString("JET_19NP_JET_EffectiveNP_1");
    npsetstring[5][1]    = new TString("JET_19NP_JET_EffectiveNP_2");
    npsetstring[5][2]    = new TString("JET_19NP_JET_EffectiveNP_3");
    npsetstring[5][3]    = new TString("JET_19NP_JET_EffectiveNP_4");
    npsetstring[5][4]    = new TString("JET_19NP_JET_EffectiveNP_5");
    npsetstring[5][6]    = new TString("JET_19NP_JET_EffectiveNP_6restTerm");
    npsetstring[5][6]    = new TString("JET_19NP_JET_EtaIntercalibration_Modelling");
    npsetstring[5][7]    = new TString("JET_19NP_JET_EtaIntercalibration_TotalStat");
    npsetstring[5][8]    = new TString("JET_19NP_JET_EtaIntercalibration_NonClosure");
    npsetstring[5][9]    = new TString("JET_19NP_JET_Pileup_OffsetMu");
    npsetstring[5][10]   = new TString("JET_19NP_JET_Pileup_OffsetNPV");
    npsetstring[5][11]   = new TString("JET_19NP_JET_Pileup_PtTerm");
    npsetstring[5][12]   = new TString("JET_19NP_JET_Pileup_RhoTopology");
    npsetstring[5][13]   = new TString("JET_19NP_JET_Flavor_Composition");
    npsetstring[5][14]   = new TString("JET_19NP_JET_Flavor_Response");
    npsetstring[5][15]   = new TString("JET_19NP_JET_BJES_Response");
    npsetstring[5][16]   = new TString("JET_19NP_JET_PunchThrough_MCTYPE");
    npsetstring[5][17]   = new TString("JET_19NP_JET_SingleParticle_HighPt");
    //npsetstring[4][0]   = new TString("JET_SR1_JET_GroupedNP_1");
    //npsetstring[4][1]   = new TString("JET_SR1_JET_GroupedNP_2");
    //npsetstring[4][2]   = new TString("JET_SR1_JET_GroupedNP_3");
    //npsetstring[4][3]   = new TString("JET_19NP_JET_EtaIntercalibration_NonClosure");
    npsetexclude[5]     = true;
  }

   else if (mode == 5)
  {
    npsetname[3]         = new TString("Top shape systematics");
    //npsetstring[3][0]    = new TString("SysTTbarPTV");
    //npsetstring[3][1]    = new TString("SysTTbarMBB");
    npsetstring[3][0]    = new TString("SysTTbarPTVMBB");
    //
    npsetname[4]         = new TString("All but top shape systematics");
    //npsetstring[4][0]    = new TString("SysTTbarPTV");
    //npsetstring[4][1]    = new TString("SysTTbarMBB");
    npsetstring[4][0]    = new TString("SysTTbarPTVMBB");
    npsetexclude[4]     = true;
  }

    else if (mode == 5)
    {
        npsetname[3]        = new TString("MC stat");
        npsetstring[3][0]     = new TString("gamma");
        
        npsetname[4]        = new TString("All Sys");
        npsetstring[4][0]     = new TString("gamma");
        npsetexclude[4]     = true;
    }
    
    else if (mode == 6)
    {
        npsetname[3]        = new TString("TTbar syst");
        npsetstring[3][0]     = new TString("TTbar");
        
        npsetname[4]        = new TString("TTbar syst and ttbar norm");
        npsetstring[4][0]     = new TString("TTbar");
        npsetstring[4][1]     = new TString("ttbar");
        
        npsetname[4]        = new TString("All but TTBar syst");
        npsetstring[4][0]     = new TString("TTbar");
        npsetexclude[4]     = true;
        
        npsetname[5]        = new TString("All but TTBar syst and ttbar norm");
        npsetstring[5][0]     = new TString("TTbar");
        npsetstring[5][1]     = new TString("ttbar");
        npsetexclude[5]     = true;
        
    }
    else if (mode == 7)
    {
        npsetname[3]        = new TString("V syst");
        npsetstring[3][0]     = new TString("ZPtV");
        npsetstring[3][1]     = new TString("WPtV");
        npsetstring[3][2]     = new TString("ZMbb");
        npsetstring[3][3]     = new TString("WMbb");

        
        npsetname[4]        = new TString("All but V syst");
        npsetstring[4][0]     = new TString("ZPtV");
        npsetstring[4][1]     = new TString("WPtV");
        npsetstring[4][2]     = new TString("ZMbb");
        npsetstring[4][3]     = new TString("WMbb");
        npsetexclude[4]     = true;
        
    }
    else if (mode == 8)
    {
      // SM Htautau.
      npsetname[3]        = new TString("Ztt normalization");
      npsetstring[3][0]     = new TString("_Ztt");

      npsetname[4]        = new TString("All normalizations");
      npsetstring[4][0]   = new TString("norm");
      npsetstring[4][1]   = new TString("Norm");

      npsetname[5]        = new TString("All but normalizations");
      npsetstring[5][0]   = new TString("norm");
      npsetstring[5][1]   = new TString("Norm");
      npsetexclude[5]     = true;

      npsetname[6]        = new TString("Jets MET");
      npsetstring[6][0]   = new TString("MET");
      npsetstring[6][1]   = new TString("JES");
      npsetstring[6][2]   = new TString("JER");
      npsetstring[6][3]   = new TString("JVT");

      npsetname[7]        = new TString("BTag");
      npsetstring[7][0]   = new TString("FT_EFF");

      npsetname[8]        = new TString("Electron Muon");
      npsetstring[8][0]   = new TString("EG");
      npsetstring[8][1]   = new TString("EL_EFF");
      npsetstring[8][2]   = new TString("MUON");

      npsetname[9]        = new TString("Tau");
      npsetstring[9][0]   = new TString("TAU");

      npsetname[10]        = new TString("Pileup reweighting");
      npsetstring[10][0]   = new TString("PRW");

      npsetname[11]        = new TString("Fake estimation");
      npsetstring[11][0]   = new TString("fake");

      npsetname[12]        = new TString("Luminosity");
      npsetstring[12][0]   = new TString("LumiUnc");

      npsetname[13]        = new TString("Theory unc. (Signal)");
      npsetstring[13][0]     = new TString("ATLAS_BR_tautau");
      npsetstring[13][1]     = new TString("ATLAS_UE");
      npsetstring[13][2]     = new TString("QCDscale_");
      npsetstring[13][3]     = new TString("NLO_EW_Higgs");
      npsetstring[13][4]     = new TString("pdf_Higgs");

      npsetname[14]        = new TString("Theory unc. (BG)");
      npsetstring[14][0]     = new TString("Z_EWK");
      npsetstring[14][1]     = new TString("Theo_Ztt");

      npsetname[15]        = new TString("MC stat");
      npsetstring[15][0]     = new TString("gamma");
    } else if (mode == 9)
    {
      // HHbbtautau.

      npsetname[3]        = new TString("All normalizations");
      npsetstring[3][0]   = new TString("norm");
      npsetstring[3][1]   = new TString("Norm");
      npsetname[4]        = new TString("All but normalizations");
      npsetstring[4][0]   = new TString("norm");
      npsetstring[4][1]   = new TString("Norm");
      npsetexclude[4]     = true;
      npsetname[5]        = new TString("Jets MET");
      npsetstring[5][0]   = new TString("MET");
      npsetstring[5][1]   = new TString("JES");
      npsetstring[5][2]   = new TString("JER");
      npsetstring[5][3]   = new TString("JVT");
      npsetstring[5][4]   = new TString("JET");

      npsetname[6]        = new TString("BTag");
      npsetstring[6][0]   = new TString("FT_EFF");

      npsetname[7]        = new TString("Electron Muon");
      npsetstring[7][0]   = new TString("EG");
      npsetstring[7][1]   = new TString("EL_EFF");
      npsetstring[7][2]   = new TString("MUON");
      npsetstring[7][3]   = new TString("MUONS");

      npsetname[8]        = new TString("Tau");
      npsetstring[8][0]   = new TString("TAUS");

      npsetname[9]        = new TString("Pileup reweighting");
      npsetstring[9][0]   = new TString("PRW");

      npsetname[10]        = new TString("Fake estimation");
      npsetstring[10][0]   = new TString("Fake");
      npsetstring[10][1]   = new TString("ANTITAU");
      npsetstring[10][2]   = new TString("MCINCREASEF");
      npsetstring[10][3]   = new TString("DECREASEFF");
      npsetstring[10][4]   = new TString("FFStat");
      npsetstring[10][5]   = new TString("SS");
      npsetstring[10][6]   = new TString("Subtract");
      npsetstring[10][7]   = new TString("FF_MTW");
      npsetstring[10][8]   = new TString("FFQCD");

      npsetname[11]        = new TString("Luminosity");
      npsetstring[11][0]   = new TString("LUMI");

      npsetname[12]        =  new TString("Top Modelling");
      npsetstring[12][0]     = new TString("TTBAR");
      npsetstring[12][1]     = new TString("TTbarMBB");
      npsetstring[12][2]     = new TString("TTbarPTH");
      npsetstring[12][3]     = new TString("Stop");
      npsetstring[12][4]     = new TString("norm_ttbar"); // Should we include norm?

      npsetname[13]        =  new TString("Ztautau Modelling");
      npsetstring[13][0]     = new TString("Ztautau");
      npsetstring[13][1]     = new TString("norm_Zbb"); // Should we include norm?


      npsetname[14]        = new TString("MC stat");
      npsetstring[14][0]     = new TString("gamma");
  //*********************************************************************************
  //*********************************************************************************
  //              Mode 20---Combined Run1+Run2 breakdown 
  //            (can be run on Run1, Run2, or combined ok)
  //*********************************************************************************
  //*********************************************************************************
  }
  else if (mode == 20)
  {
    int i=2,j=0;

    i++; j = 0;
    npsetname[i]        = new TString("Floating normalizations");
    npsetstring[i][j++]     = new TString("norm");

    i++; j = 0;
    npsetname[i]        = new TString("All normalizations");
    npsetstring[i][j++]   = new TString("norm");
    npsetstring[i][j++]   = new TString("Norm");

    i++; j = 0;
    npsetname[i]        = new TString("All but normalizations");
    npsetstring[i][j++]   = new TString("norm");//W,Z,ttbar (also multijet in Run 1)
    npsetstring[i][j++]   = new TString("Norm");
    npsetexclude[i]     = true;

    i++; j = 0;
    npsetname[i]        = new TString("Jets, MET");
    npsetstring[i][j++]   = new TString("SysJET");
    npsetstring[i][j++]   = new TString("SysMET");
    npsetstring[i][j++]   = new TString("SysJet");//Run1
    npsetstring[i][j++]   = new TString("JVF");//Run1
    npsetstring[i][j++]   = new TString("SysFATJET");
    npsetstring[i][j++]   = new TString("SysJER");

    i++; j = 0;
    npsetname[i]        = new TString("Jets");
    npsetstring[i][j++]   = new TString("SysJET");
    npsetstring[i][j++]   = new TString("SysJet");//Run1
    npsetstring[i][j++]   = new TString("JVF");//Run1
    npsetstring[i][j++]   = new TString("SysFATJET");
    npsetstring[i][j++]   = new TString("SysJER");

    i++; j = 0;
    npsetname[i]        = new TString("MET");
    npsetstring[i][j++]   = new TString("SysMET");

    i++; j = 0;
    npsetname[i]        = new TString("BTag");
    npsetstring[i][j++]   = new TString("SysFT");
    npsetstring[i][j++]   = new TString("BTag");
    npsetstring[i][j++]   = new TString("TruthTagDR");

    i++; j = 0;
    npsetname[i]        = new TString("BTag b");
    npsetstring[i][j++]   = new TString("SysFT_EFF_Eigen_B");
    npsetstring[i][j++]   = new TString("BTagB");

    i++; j = 0;
    npsetname[i]        = new TString("BTag c");
    npsetstring[i][j++]   = new TString("SysFT_EFF_Eigen_C");
    npsetstring[i][j++]   = new TString("BTagC");
    npsetstring[i][j++]   = new TString("SysFT_EFF_extrapolation_from_charm");
    npsetstring[i][j++]   = new TString("TruthTagDR");

    i++; j = 0;
    npsetname[i]        = new TString("BTag light");
    npsetstring[i][j++]   = new TString("SysFT_EFF_Eigen_Light");
    npsetstring[i][j++]   = new TString("BTagL");
    npsetstring[i][j++]   = new TString("SysFT_EFF_extrapolation_1");

    i++; j = 0;
    npsetname[i]        = new TString("Leptons");
    npsetstring[i][j++]   = new TString("SysEG");
    npsetstring[i][j++]   = new TString("SysEL");
    npsetstring[i][j++]   = new TString("SysMUON");
    npsetstring[i][j++]     = new TString("Elec");
    npsetstring[i][j++]     = new TString("Muon");
    npsetstring[i][j++]     = new TString("LepVeto");
      
    i++; j = 0;
    npsetname[i]        = new TString("Luminosity");
    npsetstring[i][j++]     = new TString("LUMI");

    i++; j = 0;
    npsetname[i]        = new TString("Diboson");
    npsetstring[i][j++]   = new TString("SysVV");//to keep out Run1 "VHVV" systs; which are, incidentally, jets
    npsetstring[i][j++]   = new TString("WW");
    npsetstring[i][j++]   = new TString("ZZ");
    npsetstring[i][j++]   = new TString("WZ");

    i++; j = 0;
    npsetname[i]        = new TString("Model Zjets");
    npsetstring[i][j++]   = new TString("ZDPhi");
    npsetstring[i][j++]   = new TString("ZcDPhi");//7 TeV
    npsetstring[i][j++]   = new TString("ZbDPhi");//7 TeV
    npsetstring[i][j++]   = new TString("ZMbb");
    npsetstring[i][j++]   = new TString("ZPt");
    npsetstring[i][j++]   = new TString("Zcl");
    npsetstring[i][j++]   = new TString("sZbb");//for Zbb systs other than floating normalization, which always look like "*SysZbb*" (as opposed to "*norm_Z*")
    npsetstring[i][j++]   = new TString("Zl");
    npsetstring[i][j++]   = new TString("Zbc");
    npsetstring[i][j++]   = new TString("Zbl");
    npsetstring[i][j++]   = new TString("Zhf");//Run1 only
    npsetstring[i][j++]   = new TString("Zcc");

    i++; j = 0;
    npsetname[i]        = new TString("Zjets flt. norm.");
    npsetstring[i][j++]   = new TString("norm_Z");

    i++; j = 0;
    npsetname[i]        = new TString("Model Wjets");
    /*Run 1 W+jets from mode 3
      "WPtV" (fine with "WPt");"WDPhi","WMbb" (same); "WbbRatio","WlNorm","WclNorm" (covered by "sWbb", "Wl", "Wcl"); "WhfNorm" (new "Whf")
    */
    npsetstring[i][j++]   = new TString("WJet");//7 TeV
    npsetstring[i][j++]   = new TString("WDPhi");
    npsetstring[i][j++]   = new TString("WMbb");
    npsetstring[i][j++]   = new TString("WbbMbb");
    npsetstring[i][j++]   = new TString("WPt");
    npsetstring[i][j++]   = new TString("sWbb");//for Wbb systs other than floating normalization, which always look like "*SysWbb*" (as opposed to "*norm_W*")
    npsetstring[i][j++]   = new TString("Wbc");
    npsetstring[i][j++]   = new TString("Wbl");
    npsetstring[i][j++]   = new TString("Whf");//Run1 only
    npsetstring[i][j++]   = new TString("Wcc");
    npsetstring[i][j++]   = new TString("WclNorm");
    npsetstring[i][j++]   = new TString("Wl");

    i++; j = 0;
    npsetname[i]        = new TString("Wjets flt. norm.");
    npsetstring[i][j++]   = new TString("norm_W");

    i++; j = 0;
    npsetname[i]        = new TString("Model ttbar");
    npsetstring[i][j++]   = new TString("TtBar");
    npsetstring[i][j++]   = new TString("Ttbar");
    npsetstring[i][j++]   = new TString("ttBarHigh");
    npsetstring[i][j++]   = new TString("TopPt");
    npsetstring[i][j++]   = new TString("TTbarPTV");
    npsetstring[i][j++]   = new TString("TTbarMBB");
    npsetstring[i][j++]   = new TString("TopMBB");//7 TeV
    npsetstring[i][j++]   = new TString("sttbar");

    i++; j = 0;
    npsetname[i]        = new TString("ttbar flt. norm.");
    npsetstring[i][j++]   = new TString("norm_ttbar");

    i++; j = 0;
    npsetname[i]        = new TString("Model Single Top");
    npsetstring[i][j++]   = new TString("stop");
    npsetstring[i][j++]   = new TString("Stop");
    npsetstring[i][j++]   = new TString("SysWt");//7 TeV
      
    i++; j = 0;
    npsetname[i]        = new TString("Model Multi Jet");
    npsetstring[i][j++]   = new TString("MJ");
    npsetstring[i][j++]   = new TString("Multijet");
      
    i++; j = 0;
    npsetname[i]        = new TString("Signal Systematics");
    npsetstring[i][j++]     = new TString("Theory");
    npsetstring[i][j++]     = new TString("SysVH");
    npsetstring[i][j++]     = new TString("BR");

    i++; j = 0;
    npsetname[i]        = new TString("MC stat");
    npsetstring[i][j++]     = new TString("gamma");


  //*********************************************************************************
  //*********************************************************************************
  //              Mode 21---just b-tagging (Run1 and/or Run2)
  //*********************************************************************************
  //*********************************************************************************
  }
  else if (mode == 21)
  {
    int i=2,j=0;
    i++; j = 0;
    npsetname[i]        = new TString("BTag");
    npsetstring[i][j++]   = new TString("SysFT");
    npsetstring[i][j++]   = new TString("BTag");
    npsetstring[i][j++]   = new TString("TruthTagDR");

    i++; j = 0;
    npsetname[i]        = new TString("BTag b");
    npsetstring[i][j++]   = new TString("SysFT_EFF_Eigen_B");
    npsetstring[i][j++]   = new TString("BTagB");

    i++; j = 0;
    npsetname[i]        = new TString("BTag c");
    npsetstring[i][j++]   = new TString("SysFT_EFF_Eigen_C");
    npsetstring[i][j++]   = new TString("BTagC");
    npsetstring[i][j++]   = new TString("SysFT_EFF_extrapolation_from_charm");
    npsetstring[i][j++]   = new TString("TruthTagDR");

    i++; j = 0;
    npsetname[i]        = new TString("BTag light");
    npsetstring[i][j++]   = new TString("SysFT_EFF_Eigen_Light");
    npsetstring[i][j++]   = new TString("BTagL");
    npsetstring[i][j++]   = new TString("SysFT_EFF_extrapolation_1");


  //*********************************************************************************
  //*********************************************************************************
  //              Mode 22---just Jets and total BTag (Run1 and/or Run2)
  //*********************************************************************************
  //*********************************************************************************
  }
  else if (mode == 22)
  {
    int i=2,j=0;
    i++; j = 0;
    npsetname[i]        = new TString("Jets");
    npsetstring[i][j++]   = new TString("SysJET");
    npsetstring[i][j++]   = new TString("SysJet");//Run1
    npsetstring[i][j++]   = new TString("JVF");//Run1
    npsetstring[i][j++]   = new TString("SysFATJET");
    npsetstring[i][j++]   = new TString("SysJER");

    i++; j = 0;
    npsetname[i]        = new TString("BTag");
    npsetstring[i][j++]   = new TString("SysFT");
    npsetstring[i][j++]   = new TString("BTag");
    npsetstring[i][j++]   = new TString("TruthTagDR");

  //*********************************************************************************
  //*********************************************************************************
  //              Mode 23---Run1 and/or Run2 modeling
  //*********************************************************************************
  //*********************************************************************************
  }
  else if (mode == 23)
  {
    int i=2,j=0;
    i++; j = 0;
    npsetname[i]        = new TString("Model Zjets, combined");
    npsetstring[i][j++]   = new TString("ZDPhi");
    npsetstring[i][j++]   = new TString("ZcDPhi");//7 TeV
    npsetstring[i][j++]   = new TString("ZbDPhi");//7 TeV
    npsetstring[i][j++]   = new TString("ZMbb");
    npsetstring[i][j++]   = new TString("ZPt");
    npsetstring[i][j++]   = new TString("Zcl");
    npsetstring[i][j++]   = new TString("sZbb");//for Zbb systs other than floating normalization, which always look like "*SysZbb*" (as opposed to "*norm_Z*")
    npsetstring[i][j++]   = new TString("Zl");
    npsetstring[i][j++]   = new TString("Zbc");
    npsetstring[i][j++]   = new TString("Zbl");
    npsetstring[i][j++]   = new TString("Zhf");//Run1 only
    npsetstring[i][j++]   = new TString("Zcc");

    i++; j = 0;
    npsetname[i]        = new TString("Zjets flt. norm.");
    npsetstring[i][j++]   = new TString("norm_Z");

    i++; j = 0;
    npsetname[i]        = new TString("Model Wjets");
    /*Run 1 W+jets from mode 3
      "WPtV" (fine with "WPt");"WDPhi","WMbb" (same); "WbbRatio","WlNorm","WclNorm" (covered by "sWbb", "Wl", "Wcl"); "WhfNorm" (new "Whf")
    */
    npsetstring[i][j++]   = new TString("WJet");//7 TeV
    npsetstring[i][j++]   = new TString("WDPhi");
    npsetstring[i][j++]   = new TString("WMbb");
    npsetstring[i][j++]   = new TString("WbbMbb");
    npsetstring[i][j++]   = new TString("WPt");
    npsetstring[i][j++]   = new TString("sWbb");//for Wbb systs other than floating normalization, which always look like "*SysWbb*" (as opposed to "*norm_W*")
    npsetstring[i][j++]   = new TString("Wbc");
    npsetstring[i][j++]   = new TString("Wbl");
    npsetstring[i][j++]   = new TString("Whf");//Run1 only
    npsetstring[i][j++]   = new TString("Wcc");
    npsetstring[i][j++]   = new TString("WclNorm");
    npsetstring[i][j++]   = new TString("Wl");

    i++; j = 0;
    npsetname[i]        = new TString("Wjets flt. norm.");
    npsetstring[i][j++]   = new TString("norm_W");

    i++; j = 0;
    npsetname[i]        = new TString("Model ttbar");
    npsetstring[i][j++]   = new TString("TtBar");
    npsetstring[i][j++]   = new TString("Ttbar");
    npsetstring[i][j++]   = new TString("ttBarHigh");
    npsetstring[i][j++]   = new TString("TopPt");
    npsetstring[i][j++]   = new TString("TTbarPTV");
    npsetstring[i][j++]   = new TString("TTbarMBB");
    npsetstring[i][j++]   = new TString("TopMBB");//7 TeV
    npsetstring[i][j++]   = new TString("sttbar");

    i++; j = 0;
    npsetname[i]        = new TString("ttbar flt. norm.");
    npsetstring[i][j++]   = new TString("norm_ttbar");

    i++; j = 0;
    npsetname[i]        = new TString("Model Single Top");
    npsetstring[i][j++]   = new TString("stop");
    npsetstring[i][j++]   = new TString("Stop");
    npsetstring[i][j++]   = new TString("SysWt");//7 TeV
      
    i++; j = 0;
    npsetname[i]        = new TString("Diboson");
    npsetstring[i][j++]   = new TString("SysVV");//to keep out Run1 "VHVV" systs; which are, incidentally, jets
    npsetstring[i][j++]   = new TString("WW");
    npsetstring[i][j++]   = new TString("ZZ");
    npsetstring[i][j++]   = new TString("WZ");

    i++; j = 0;
    npsetname[i]        = new TString("Model Multi Jet");
    npsetstring[i][j++]   = new TString("MJ");
    npsetstring[i][j++]   = new TString("Multijet");

  //*********************************************************************************
  //*********************************************************************************
  //              Mode 24---Run1, Run2, and combined breakdowns (SLOW)
  //*********************************************************************************
  //*********************************************************************************
  }
  else if (mode == 24)
  {
    int i=2,j=0;

    i++; j = 0;
    npsetname[i]        = new TString("Floating normalizations");
    npsetstring[i][j++]     = new TString("norm");

    i++; j = 0;
    npsetname[i]        = new TString("All normalizations");
    npsetstring[i][j++]   = new TString("norm");
    npsetstring[i][j++]   = new TString("Norm");

    i++; j = 0;
    npsetname[i]        = new TString("All but normalizations");
    npsetstring[i][j++]   = new TString("norm");//W,Z,ttbar (also multijet in Run 1)
    npsetstring[i][j++]   = new TString("Norm");
    npsetexclude[i]     = true;

    i++; j = 0;
    npsetname[i]        = new TString("Jets, MET, combined");
    npsetstring[i][j++]   = new TString("SysJET");
    npsetstring[i][j++]   = new TString("SysMET");
    npsetstring[i][j++]   = new TString("SysJet");//Run1
    npsetstring[i][j++]   = new TString("JVF");//Run1
    npsetstring[i][j++]   = new TString("SysFATJET");
    npsetstring[i][j++]   = new TString("SysJER");
    i++; j = 0;
    npsetname[i]        = new TString("MET, combined");
    npsetstring[i][j++]   = new TString("SysMET");


    i++; j = 0;
    npsetname[i]        = new TString("Jets, combined");
    npsetstring[i][j++]   = new TString("SysJET");
    npsetstring[i][j++]   = new TString("SysJet");//Run1
    npsetstring[i][j++]   = new TString("JVF");//Run1
    npsetstring[i][j++]   = new TString("SysFATJET");
    npsetstring[i][j++]   = new TString("SysJER");

    i++; j = 0;
    npsetname[i]        = new TString("Jets, Run1");
    npsetstring[i][j++]   = new TString("SysJet");//Run1
    npsetstring[i][j++]   = new TString("JVF");//Run1
    npsetands[i]          = new TString("_78TeV");//Run1

    i++; j = 0;
    npsetname[i]        = new TString("Jets, Run2");
    npsetstring[i][j++]   = new TString("SysJET");
    npsetstring[i][j++]   = new TString("SysFATJET");
    npsetstring[i][j++]   = new TString("SysJER");
    npsetands[i]          = new TString("_13TeV");//Run2

    i++; j = 0;
    npsetname[i]        = new TString("BTag, combined");
    npsetstring[i][j++]   = new TString("SysFT");
    npsetstring[i][j++]   = new TString("BTag");
    npsetstring[i][j++]   = new TString("TruthTagDR");

    i++; j = 0;
    npsetname[i]        = new TString("BTag b, combined");
    npsetstring[i][j++]   = new TString("SysFT_EFF_Eigen_B");
    npsetstring[i][j++]   = new TString("BTagB");

    i++; j = 0;
    npsetname[i]        = new TString("BTag b, Run1");
    npsetstring[i][j++]   = new TString("BTagB");
    npsetands[i]          = new TString("_78TeV");//Run1

    i++; j = 0;
    npsetname[i]        = new TString("BTag b, Run2");
    npsetstring[i][j++]   = new TString("SysFT_EFF_Eigen_B");
    npsetands[i]          = new TString("_13TeV");//Run2

    i++; j = 0;
    npsetname[i]        = new TString("BTag c");
    npsetstring[i][j++]   = new TString("SysFT_EFF_Eigen_C");
    npsetstring[i][j++]   = new TString("BTagC");
    npsetstring[i][j++]   = new TString("SysFT_EFF_extrapolation_from_charm");
    npsetstring[i][j++]   = new TString("TruthTagDR");

    i++; j = 0;
    npsetname[i]        = new TString("BTag c, Run1");
    npsetstring[i][j++]   = new TString("BTagC");
    npsetstring[i][j++]   = new TString("TruthTagDR");
    npsetands[i]          = new TString("_78TeV");//Run1

    i++; j = 0;
    npsetname[i]        = new TString("BTag c, Run2");
    npsetstring[i][j++]   = new TString("SysFT_EFF_Eigen_C");
    npsetstring[i][j++]   = new TString("SysFT_EFF_extrapolation_from_charm");
    npsetands[i]          = new TString("_13TeV");//Run2

    i++; j = 0;
    npsetname[i]        = new TString("BTag light");
    npsetstring[i][j++]   = new TString("SysFT_EFF_Eigen_Light");
    npsetstring[i][j++]   = new TString("BTagL");
    npsetstring[i][j++]   = new TString("SysFT_EFF_extrapolation_1");

    i++; j = 0;
    npsetname[i]        = new TString("BTag light, Run1");
    npsetstring[i][j++]   = new TString("BTagL");
    npsetands[i]          = new TString("_78TeV");//Run1

    i++; j = 0;
    npsetname[i]        = new TString("BTag light, Run2");
    npsetstring[i][j++]   = new TString("SysFT_EFF_Eigen_Light");
    npsetstring[i][j++]   = new TString("SysFT_EFF_extrapolation_1");
    npsetands[i]          = new TString("_13TeV");//Run2

    i++; j = 0;
    npsetname[i]        = new TString("Model Zjets, combined");
    npsetstring[i][j++]   = new TString("ZDPhi");
    npsetstring[i][j++]   = new TString("ZcDPhi");//7 TeV
    npsetstring[i][j++]   = new TString("ZbDPhi");//7 TeV
    npsetstring[i][j++]   = new TString("ZMbb");
    npsetstring[i][j++]   = new TString("ZPt");
    npsetstring[i][j++]   = new TString("Zcl");
    npsetstring[i][j++]   = new TString("sZbb");//for Zbb systs other than floating normalization, which always look like "*SysZbb*" (as opposed to "*norm_Z*")
    npsetstring[i][j++]   = new TString("Zl");
    npsetstring[i][j++]   = new TString("Zbc");
    npsetstring[i][j++]   = new TString("Zbl");
    npsetstring[i][j++]   = new TString("Zhf");//Run1 only
    npsetstring[i][j++]   = new TString("Zcc");

    i++; j = 0;
    npsetname[i]        = new TString("Model Zjets, Run1");
    npsetstring[i][j++]   = new TString("ZDPhi");
    npsetstring[i][j++]   = new TString("ZcDPhi");//7 TeV
    npsetstring[i][j++]   = new TString("ZbDPhi");//7 TeV
    npsetstring[i][j++]   = new TString("ZMbb");
    npsetstring[i][j++]   = new TString("ZPt");
    npsetstring[i][j++]   = new TString("Zcl");
    npsetstring[i][j++]   = new TString("sZbb");//for Zbb systs other than floating normalization, which always look like "*SysZbb*" (as opposed to "*norm_Z*")
    npsetstring[i][j++]   = new TString("Zl");
    npsetstring[i][j++]   = new TString("Zbc");
    npsetstring[i][j++]   = new TString("Zbl");
    npsetstring[i][j++]   = new TString("Zhf");//Run1 only
    npsetstring[i][j++]   = new TString("Zcc");
    npsetands[i]          = new TString("_78TeV");//Run1

    i++; j = 0;
    npsetname[i]        = new TString("Model Zjets, Run2");
    npsetstring[i][j++]   = new TString("ZDPhi");
    npsetstring[i][j++]   = new TString("ZMbb");
    npsetstring[i][j++]   = new TString("ZPt");
    npsetstring[i][j++]   = new TString("Zcl");
    npsetstring[i][j++]   = new TString("sZbb");//for Zbb systs other than floating normalization, which always look like "*SysZbb*" (as opposed to "*norm_Z*")
    npsetstring[i][j++]   = new TString("Zl");
    npsetstring[i][j++]   = new TString("Zbc");
    npsetstring[i][j++]   = new TString("Zbl");
    npsetstring[i][j++]   = new TString("Zcc");
    npsetands[i]          = new TString("_13TeV");//Run2

    i++; j = 0;
    npsetname[i]        = new TString("Zjets flt. norm., combined");
    npsetstring[i][j++]   = new TString("norm_Z");

    i++; j = 0;
    npsetname[i]        = new TString("Zjets flt. norm., Run1");
    npsetstring[i][j++]   = new TString("norm_Z");
    npsetands[i]          = new TString("_78TeV");//Run1

    i++; j = 0;
    npsetname[i]        = new TString("Zjets flt. norm., Run2");
    npsetstring[i][j++]   = new TString("norm_Z");
    npsetands[i]          = new TString("_13TeV");//Run2

    i++; j = 0;
    npsetname[i]        = new TString("Model Wjets, combined");
    /*Run 1 W+jets from mode 3
      "WPtV" (fine with "WPt");"WDPhi","WMbb" (same); "WbbRatio","WlNorm","WclNorm" (covered by "sWbb", "Wl", "Wcl"); "WhfNorm" (new "Whf")
    */
    npsetstring[i][j++]   = new TString("WJet");//7 TeV
    npsetstring[i][j++]   = new TString("WDPhi");
    npsetstring[i][j++]   = new TString("WMbb");
    npsetstring[i][j++]   = new TString("WbbMbb");
    npsetstring[i][j++]   = new TString("WPt");
    npsetstring[i][j++]   = new TString("sWbb");//for Wbb systs other than floating normalization, which always look like "*SysWbb*" (as opposed to "*norm_W*")
    npsetstring[i][j++]   = new TString("Wbc");
    npsetstring[i][j++]   = new TString("Wbl");
    npsetstring[i][j++]   = new TString("Whf");//Run1 only
    npsetstring[i][j++]   = new TString("Wcc");
    npsetstring[i][j++]   = new TString("WclNorm");
    npsetstring[i][j++]   = new TString("Wl");

    i++; j = 0;
    npsetname[i]        = new TString("Model Wjets, Run1");
    npsetstring[i][j++]   = new TString("WJet");//7 TeV
    npsetstring[i][j++]   = new TString("WDPhi");
    npsetstring[i][j++]   = new TString("WMbb");
    npsetstring[i][j++]   = new TString("WbbMbb");
    npsetstring[i][j++]   = new TString("WPt");
    npsetstring[i][j++]   = new TString("sWbb");//for Wbb systs other than floating normalization, which always look like "*SysWbb*" (as opposed to "*norm_W*")
    npsetstring[i][j++]   = new TString("Wbc");
    npsetstring[i][j++]   = new TString("Wbl");
    npsetstring[i][j++]   = new TString("Whf");//Run1 only
    npsetstring[i][j++]   = new TString("Wcc");
    npsetstring[i][j++]   = new TString("WclNorm");
    npsetstring[i][j++]   = new TString("Wl");
    npsetands[i]          = new TString("_78TeV");//Run1

    i++; j = 0;
    npsetname[i]        = new TString("Model Wjets, Run2");
    npsetstring[i][j++]   = new TString("WDPhi");
    npsetstring[i][j++]   = new TString("WMbb");
    npsetstring[i][j++]   = new TString("WbbMbb");
    npsetstring[i][j++]   = new TString("WPt");
    npsetstring[i][j++]   = new TString("sWbb");//for Wbb systs other than floating normalization, which always look like "*SysWbb*" (as opposed to "*norm_W*")
    npsetstring[i][j++]   = new TString("Wbc");
    npsetstring[i][j++]   = new TString("Wbl");
    npsetstring[i][j++]   = new TString("Wcc");
    npsetstring[i][j++]   = new TString("WclNorm");
    npsetstring[i][j++]   = new TString("Wl");
    npsetands[i]          = new TString("_13TeV");//Run2

    i++; j = 0;
    npsetname[i]        = new TString("Wjets flt. norm., combined");
    npsetstring[i][j++]   = new TString("norm_W");


    i++; j = 0;
    npsetname[i]        = new TString("Wjets flt. norm., Run1");
    npsetstring[i][j++]   = new TString("norm_W");
    npsetands[i]          = new TString("_78TeV");//Run1

    i++; j = 0;
    npsetname[i]        = new TString("Wjets flt. norm., Run2");
    npsetstring[i][j++]   = new TString("norm_W");
    npsetands[i]          = new TString("_13TeV");//Run2

    i++; j = 0;
    npsetname[i]        = new TString("Model ttbar, combined");
    npsetstring[i][j++]   = new TString("TtBar");
    npsetstring[i][j++]   = new TString("Ttbar");
    npsetstring[i][j++]   = new TString("ttBarHigh");
    npsetstring[i][j++]   = new TString("TopPt");
    npsetstring[i][j++]   = new TString("TTbarPTV");
    npsetstring[i][j++]   = new TString("TTbarMBB");
    npsetstring[i][j++]   = new TString("TopMBB");//7 TeV
    npsetstring[i][j++]   = new TString("sttbar");

    i++; j = 0;
    npsetname[i]        = new TString("Model ttbar, Run1");
    npsetstring[i][j++]   = new TString("TtBar");
    npsetstring[i][j++]   = new TString("Ttbar");
    npsetstring[i][j++]   = new TString("ttBarHigh");
    npsetstring[i][j++]   = new TString("TopPt");
    npsetstring[i][j++]   = new TString("TTbarPTV");
    npsetstring[i][j++]   = new TString("TTbarMBB");
    npsetstring[i][j++]   = new TString("TopMBB");//7 TeV
    npsetstring[i][j++]   = new TString("sttbar");
    npsetands[i]          = new TString("_78TeV");//Run1

    i++; j = 0;
    npsetname[i]        = new TString("Model ttbar, Run2");
    npsetstring[i][j++]   = new TString("TtBar");
    npsetstring[i][j++]   = new TString("Ttbar");
    npsetstring[i][j++]   = new TString("ttBarHigh");
    npsetstring[i][j++]   = new TString("TopPt");
    npsetstring[i][j++]   = new TString("TTbarPTV");
    npsetstring[i][j++]   = new TString("TTbarMBB");
    npsetstring[i][j++]   = new TString("sttbar");
    npsetands[i]          = new TString("_13TeV");//Run2

    i++; j = 0;
    npsetname[i]        = new TString("ttbar flt. norm., combined");
    npsetstring[i][j++]   = new TString("norm_ttbar");

    i++; j = 0;
    npsetname[i]        = new TString("ttbar flt. norm., Run1");
    npsetstring[i][j++]   = new TString("norm_ttbar");
    npsetands[i]          = new TString("_78TeV");//Run1

    i++; j = 0;
    npsetname[i]        = new TString("ttbar flt. norm., Run2");
    npsetstring[i][j++]   = new TString("norm_ttbar");
    npsetands[i]          = new TString("_13TeV");//Run2

    i++; j = 0;
    npsetname[i]        = new TString("Model Single Top, combined");
    npsetstring[i][j++]   = new TString("stop");
    npsetstring[i][j++]   = new TString("Stop");
    npsetstring[i][j++]   = new TString("SysWt");//7 TeV

    i++; j = 0;
    npsetname[i]        = new TString("Model Single Top, Run1");
    npsetstring[i][j++]   = new TString("stop");
    npsetstring[i][j++]   = new TString("Stop");
    npsetstring[i][j++]   = new TString("SysWt");//7 TeV
    npsetands[i]          = new TString("_78TeV");//Run2
      
    i++; j = 0;
    npsetname[i]        = new TString("Model Single Top, Run2");
    npsetstring[i][j++]   = new TString("stop");
    npsetstring[i][j++]   = new TString("Stop");
    npsetands[i]          = new TString("_13TeV");//Run2
      
    i++; j = 0;
    npsetname[i]        = new TString("Diboson, combined");
    npsetstring[i][j++]   = new TString("SysVV");//to keep out Run1 "VHVV" systs; which are, incidentally, jets
    npsetstring[i][j++]   = new TString("WW");
    npsetstring[i][j++]   = new TString("ZZ");
    npsetstring[i][j++]   = new TString("WZ");

    i++; j = 0;
    npsetname[i]        = new TString("Diboson, Run1");
    npsetstring[i][j++]   = new TString("SysVV");//to keep out Run1 "VHVV" systs; which are, incidentally, jets
    npsetstring[i][j++]   = new TString("WW");
    npsetstring[i][j++]   = new TString("ZZ");
    npsetstring[i][j++]   = new TString("WZ");
    npsetands[i]          = new TString("_78TeV");//Run1

    i++; j = 0;
    npsetname[i]        = new TString("Diboson, Run2");
    npsetstring[i][j++]   = new TString("SysVV");//to keep out Run1 "VHVV" systs; which are, incidentally, jets
    npsetstring[i][j++]   = new TString("WW");
    npsetstring[i][j++]   = new TString("ZZ");
    npsetstring[i][j++]   = new TString("WZ");
    npsetands[i]          = new TString("_13TeV");//Run2

    i++; j = 0;
    npsetname[i]        = new TString("Model Multi Jet, combined");
    npsetstring[i][j++]   = new TString("MJ");
    npsetstring[i][j++]   = new TString("Multijet");
      
    i++; j = 0;
    npsetname[i]        = new TString("Model Multi Jet, Run1");
    npsetstring[i][j++]   = new TString("MJ");
    npsetstring[i][j++]   = new TString("Multijet");
    npsetands[i]          = new TString("_78TeV");//Run2

    i++; j = 0;
    npsetname[i]        = new TString("Model Multi Jet, Run2");
    npsetstring[i][j++]   = new TString("MJ");
    npsetstring[i][j++]   = new TString("Multijet");
    npsetands[i]          = new TString("_13TeV");//Run2
  //*********************************************************************************
  //*********************************************************************************
  //              Mode 25---all but signal systematics
  //*********************************************************************************
  //*********************************************************************************
  }
  else if (mode == 25)
  {
    int i=2,j=0;


    i++; j = 0;
    npsetname[i]        = new TString("Signal Systematics");
    npsetstring[i][j++]     = new TString("Theory");
    npsetstring[i][j++]     = new TString("SysVH");
    npsetstring[i][j++]     = new TString("BR");

    i++; j = 0;
    npsetname[i]        = new TString("all but signal systematics");
    npsetstring[i][j++]     = new TString("Theory");
    npsetstring[i][j++]     = new TString("SysVH");
    npsetstring[i][j++]     = new TString("BR");
    npsetexclude[i]     = true;

  }

  //*********************************************************************************
  //*********************************************************************************
  //                           End of mode-specific settings
  //*********************************************************************************
  //*********************************************************************************


  // This must come **after** setting npnstr
  int setnamemax = 0;
  for (int iset = 0; iset < npsets_max; iset++){
      npnstr[iset]        = 0;
      for (int istr = 0; istr < nstrmax; istr++){
          if (npsetstring[iset][istr]->CompareTo("xxxxx")) npnstr[iset]++;
      }
       if (npsetname[iset]->CompareTo("xxxxx")) npsets++;

  }
  if(mode==-1)npsets=1;//HACK MODE, JUST DO TOTAL, a baseline

  //STEAL FROM runPulls.C
  cout<<"start doing "<<endl;
  //not div(ided) up are the ones we don't split: Total (mode -1), DataSyst and FullSyst
  //(the latter relies upon the former, so is one extra--job0 will include this; 
  //this is a conscious but admittedly random choice (instead of just making it its own or putting it in the mode -1 job (i.e. mode 0))); 
  int nJobs = 1, iJob = 0, not_divup=3,total_categories = npsets-not_divup;
  bool only_some=false;
  set<int>run_these={0};
  TPRegexp pat(".*_job(\\d+)of(\\d+).*");
  TObjArray* res=pat.MatchS(workspace);
  if (res->GetSize() >= 3) {
    if (res->At(1) && res->At(2)) {
      int nJobsTmp = atoi(((TObjString*)(res->At(2)))->GetString());
      int iJobTmp = atoi(((TObjString*)(res->At(1)))->GetString());
      if (nJobsTmp > 0 && iJobTmp >= 0 && iJobTmp < nJobsTmp) {
	nJobs = nJobsTmp;
	iJob = iJobTmp;
      }
    }
  }
  if(nJobs>1){
    //FIX ME ALWAYS DO DATASTAT AND FULLSYST TOGETHER
    only_some=true;
    if(iJob==0){
      run_these.insert(1);
      run_these.insert(2);
    }
    int leftover=total_categories%nJobs,per_job=total_categories/nJobs,lo,hi;
    //try to spread out the categories as evenly as possible over jobs
    //so the leftovers go to the first n jobs
    if(iJob<leftover){
      lo=(iJob)*(per_job+1)+not_divup;
      hi=lo+per_job+1;
    }else{
      lo=iJob*per_job+leftover+not_divup;
      hi=lo+per_job;
    }
    for(int i=lo;i<hi;i++)run_these.insert(i);
    if(run_these.size()<2)return;//you always have the total; if there isn't another one in segmented jobs, this job won't do anything
  }
  cout << "Running job " << iJob+1 << " of " << nJobs << "...for NP cats: ";
  if(only_some){
    for(const auto&i:run_these)cout<<i<<" "<<npsetname[i]->Data()<<" ";
    cout<<endl;
  }
  else cout<<" (ALL of them for mode "<<mode<<")"<<endl;
  RooRealVar* poi           = 0;
  RooDataSet* asimovData    = 0;
  RooArgSet globObs;
  RooNLLVar* nll            = 0;
  double nll_val_true       = 0;
  double muhat              = 0;
  double err_guess          = 0;

  //mode -1 means we're doing the parallel overhead calculations, so don't load previous results
  //if we are not loading previous results, we will take this to mean we want to save them
  bool load_previous_result=(mode!=-1 && overhead_path!=""),minimize_anyway=false;
  TString overhead_file_name=(overhead_path==""?"./bd-results/breakdown-fitresults.root":overhead_path);
  RooFitResult*rfr=0;
  TFile*overhead=0;
  if(load_previous_result){
    string okay=overhead_file_name.Data(),front=okay.substr(0,okay.find_last_of("/")),back=okay.substr(okay.find_last_of("/")+1);
    DIR *fdir=opendir(front.c_str()); struct dirent *ent=NULL; bool found=false;
    assert(fdir!=NULL);
    while((ent=readdir(fdir))){
      //cout<<"Look at "<<ent->d_name<<", is it "<<back<<endl;
      if(back.compare(ent->d_name)==0){
	found=true; break;
      }
    }
    if(!found)load_previous_result=false;
    else{
      overhead=TFile::Open(overhead_file_name);
      rfr=(RooFitResult*)overhead->Get("nominalNuis")->Clone();
      if(!rfr){
	load_previous_result=false;
	overhead->Close();
      }
    }
  }
  if(!load_previous_result){
    //make the directory for storing the initial fit result if it doesn't exist
    string path="./",name=overhead_file_name.Data();
    if(name.find("/")!=string::npos){
      path=name.substr(0,name.find_last_of("/")+1);
    }
    DIR*directory=opendir(path.c_str());
    if(directory==NULL)system(("mkdir -p "+path).c_str());
    overhead=TFile::Open(overhead_file_name,"recreate");
  }
  bool multpoi=mc->GetParametersOfInterest()->getSize()>1;
  if (!doDummy)
  {
      ws->loadSnapshot("vars_final");
      //let's make all the POI's floating just in case....
      TIterator*it=mc->GetParametersOfInterest()->createIterator();
      RooRealVar*n=(RooRealVar*)it->Next();
      while(n){
	n->setConstant(0);
	cout<<n->GetName()<<" set to constant 0"<<endl;
	n=(RooRealVar*)it->Next();
      }
      poi = (RooRealVar*)mc->GetParametersOfInterest()->first();
      mc->GetGlobalObservables()->Print("v");
      globObs = *mc->GetGlobalObservables();
      if( poiStr != "SigXsecOverSM" ) {
	// load_previous_result=false;
          poi->setConstant(0);
          cout << "SWITCHING POI TO " << poiStr << endl;
          poi = ws->var(poiStr.Data());
          if(!poi) {
              cout << "Cannot find " << poiStr << " in workspace" << endl;
              return;
          }
      }

      //RooDataSet* asimovData = makeAsimovData(mc, 0, ws, mc->GetPdf(), data, 1); // unconditional fit and build asimov with mu=1

      //the doAsimov conditional and the lines making the nll had a !doDummy condition on them...inside a if(!doDummy) condition...
      if (doAsimov) {
	asimovData = makeAsimovData(mc, 0, ws, mc->GetPdf(), data, mu_asimov); // unconditional fit and build asimov with mu=mu_asimov
	data = asimovData;
	cout << "Made Asimov data. with mu = " << mu_asimov << endl;
      }
      RooArgSet nuisAndPOI(*mc->GetNuisanceParameters(),*mc->GetParametersOfInterest());
      
      if(load_previous_result){
	  //these load something like a fit result into a RooArgSet (taken from doPlotFromWS to load FCC results)
	  RooArgList*fpf=(RooArgList*)rfr->floatParsFinal().Clone();
	  fpf->add(rfr->constPars());

	  //now add information to relevant RooArgSets 
	  nuisAndPOI.assignValueOnly(*(RooAbsCollection*)fpf);
	  globObs.assignValueOnly(*(RooAbsCollection*)fpf);

	  //these only load the central values, so we add the sqrt(POI_variance) from the RFR
  	  int index_for_poi=fpf->index(poiStr);
  	  err_guess = sqrt(rfr->covarianceMatrix()(index_for_poi,index_for_poi));
      }
      TString nCPU_str = getenv("NCORE");
      int nCPU = nCPU_str.Atoi();
      nll = (RooNLLVar*)mc->GetPdf()->createNLL(*data, GlobalObservables(globObs), Offset(1), Optimize(2), NumCPU(nCPU,3)/*, Constrain(nuis)*/);
      //  nll->enableOffsetting(kTRUE);
      cout << "Created NLL." << endl;
      ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");
      ROOT::Math::MinimizerOptions::SetDefaultStrategy(1);
      ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(-1);
      /*
	Strategy 1 is "intermediate" FCN's for Minuit (in between fast (0) and expensive (2))
	Default error level of 0.5 corresponds to +/- 1 sigma
	The standard epsilon value of 1.0 corresponds to some sort of tolerance
       */

      
      poi->setConstant(0);
      cout << "POI" << endl;
      cout << poi->getMin() << endl;
      cout << poi->getMax() << endl;
      
      cout << "setting min to -max" << endl;
      poi->setMin( -1 * poi->getMax() );

      if(!load_previous_result){
	//If there is no fit result loaded, run the nominal fit and save the result
	minimize(nll,false,overhead,"nominalNuis");//was just minimize(nll);
	err_guess = poi->getError();
      }/*else{
	minimize(nll);
	err_guess = poi->getError();
	}*/
      cout << "Minimum2 " << nll->getVal() << endl;
      ws->saveSnapshot("tmp_shot",nuisAndPOI);


      nll_val_true = nll->getVal();
      muhat = poi->getVal();

      typedef std::numeric_limits< double > dbl;
      cout.precision(dbl::digits10);
      cout << "nll_val_true " << nll_val_true << endl;
      if(!blind) { cout << "muhat " << muhat << endl; }
      cout << "err_guess " << err_guess << endl;
  }



  const RooArgSet* nuis = mc->GetNuisanceParameters();
  TIterator* nitr = nuis->createIterator();
  RooRealVar* var;

  timer->Stop();
  //read off how long the over head took in real and cpu time
  double real_front=timer->RealTime(),cpu_front=timer->CpuTime();
  //and store those values in the total
  double real_tot=real_front,cpu_tot=cpu_front;
  cout<<"The mode-independent overhead took "<<real_front<<" real sec, "<<cpu_front<<" CPU sec"<<endl;
  timer->Start(kTRUE);
  
  for (int iset = 0; iset < npsets; iset++)
  {
    if(only_some&&!run_these.count(iset))continue;
    if(iset==0&&load_previous_result&&!multpoi){//the error ought to be different for each POI
      TVectorD*numbers=(TVectorD*)overhead->Get("errors");
      if(numbers){
	cout<<"LOAD values from "<<overhead_file_name<<" for TOT error +"<<numbers->operator[](0)<<"/ -"<<numbers->operator[](1)<<endl;
	err_npset_hi[0]= numbers->operator[](0);
	err_npset_lo[0]= numbers->operator[](1);
	err_raw_hi[0]= numbers->operator[](0);
	err_raw_lo[0]= numbers->operator[](1);
	overhead->Close();
	continue;
      }
      overhead->Close();
    }
      if (setnamemax < npsetname[iset]->Length()) setnamemax = npsetname[iset]->Length();
      if (iset == 2) {// FullSyst is just Total -Stat (quadratically)
          err_raw_hi[iset] = subtractError(err_raw_hi[0], err_raw_hi[1]);
          err_raw_lo[iset] = subtractError(err_raw_lo[0], err_raw_lo[1]);
          err_npset_lo[iset] = err_raw_lo[iset];
          err_npset_hi[iset] = err_raw_hi[iset];
          cout << npsetname[iset]->Data() << " errors from quadratical subrtraction of Total and Stat." <<endl;
          cout << npsetname[iset]->Data() << " _err_npset_hi same as raw: " << err_npset_hi[iset] <<endl;
          cout << npsetname[iset]->Data() << " _err_npset_lo same as raw:  " << err_npset_lo[iset] <<endl;
          continue; 
      }
      
      if (!doDummy) ws->loadSnapshot("tmp_shot");
      nitr->Reset();
      cout << endl << "Holding constant the "<<npsetname[iset]->Data() <<
      " nuisance parameter set, which is: "<< endl;

      while ((var = (RooRealVar*)nitr->Next()))
      {
	if(!TString(var->GetName()).Contains(npsetands[iset]->Data())){
// 	  std::cout<<"\t[SKC CHECK Can't find "<<npsetands[iset]<<" in "<<var->GetName()<<"]";
	  continue;
	}
          bool aux_isnp = false;
          for (int istr = 0; istr < npnstr[iset]; istr++)
          {
              aux_isnp = aux_isnp ||
              (string(var->GetName()).find(npsetstring[iset][istr]->Data())
              != string::npos);// is this var the iset _th NP?
          }

          if (npsetexclude[iset]) aux_isnp = !aux_isnp;
          if (aux_isnp) var->setConstant(1);
          else { continue; }
          cout << "\t" << var->GetName() << endl;
      }

      if (!doDummy){

	if (doHesse) {
          minimize(nll, true);
          if(!blind) { cout << "muhat " << poi->getVal() << endl; }
          cout << "err_hesse " << poi->getError() << endl;
	}

          err_raw_hi[iset] = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, 1, 0.01);
          err_raw_lo[iset] = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, -1, 0.01);
      }

      cout << npsetname[iset]->Data() << " constant"<< endl;
      cout << npsetname[iset]->Data() << " _err_raw_hi straight from fit: " << err_raw_hi[iset] <<endl;
      cout << npsetname[iset]->Data() << " _err_raw_lo straight from fit: " << err_raw_lo[iset] <<endl;
      
      err_npset_lo[iset] = err_raw_lo[iset];
      err_npset_hi[iset] = err_raw_hi[iset];
      // if (iset != 0 && npsetexclude[iset] == false) {
      if (iset != 0 && iset != 1) {
          err_npset_hi[iset] = subtractError(err_raw_hi[0], err_raw_hi[iset]);
          err_npset_lo[iset] = subtractError(err_raw_lo[0], err_raw_lo[iset]);
          cout << npsetname[iset]->Data() << " _err_npset_hi quadratically subtracted from tot: " << err_npset_hi[iset] <<endl;
          cout << npsetname[iset]->Data() << " _err_npset_lo quadratically subtracted from tot:  " << err_npset_lo[iset] <<endl;
      } else {
          cout << npsetname[iset]->Data() << " _err_npset_hi same as raw: " << err_npset_hi[iset] <<endl;
          cout << npsetname[iset]->Data() << " _err_npset_lo same as raw:  " << err_npset_lo[iset] <<endl;
      }
      
      //save the total error if applicable
      if(iset==0&&!load_previous_result&&!multpoi){
	TVectorD numbers(3);
	numbers[0]=err_npset_hi[0];
	numbers[1]=err_npset_lo[0];
	numbers[2]=err_guess;
	overhead->cd();
	numbers.Write("errors");
	overhead->Close();
      }      
      timer->Stop();
      //read off how long this NP set took and add it ot the total
      double real_back=timer->RealTime(),cpu_back=timer->CpuTime();
      real_tot+=real_back;cpu_tot+=cpu_back;
      cout<<"NP set "<<npsetname[iset]->Data()<<" took "<<real_back<<" real sec, "<<cpu_back<<" CPU sec"<<endl;
      timer->Start(kTRUE);
  }
  
   TString dirName = "./plots/";
   dirName += workspace; dirName += "/breakdown";
   system(TString("mkdir -vp "+dirName));
 
  TString fname = dirName;
  fname += "/muHatTable_";  fname += TString(workspace).ReplaceAll("/","_");
  fname += "_mode";         fname += mode;
  if (doAsimov) fname += "_Asimov1"; else fname += "_Asimov0";
  fname += "_";             fname += poiStr;
  TString fnameTex = fname;
  TString fnameTxt = fname;
  fnameTex += ".tex";
  fnameTxt += ".txt";
  ofstream fmuHatTex(fnameTex.Data());  
  
  
  time_t rawtime;  time (&rawtime);
  FILE *fmuHatTxt = fopen(fnameTxt.Data(),"w");
  bool print_preamble=!only_some||iJob==0,print_postamble=!only_some||iJob==(nJobs-1);
  if(print_preamble){
    fmuHatTex << endl;
    fmuHatTex << "\\begin{tabular}{|l|c|}" << endl;
    fmuHatTex << "\\hline" << endl;
    fmuHatTex << "Set of nuisance & Impact on error  \\\\\\hline" << endl;
    fmuHatTex << " parameters     &   \\\\\\hline" << endl;
    //fmuHat << "\\hline" << endl;

    fprintf(fmuHatTxt,"\n\tOutput of newGetMuHat ran at: %s\n",ctime (&rawtime));
    fprintf(fmuHatTxt,"----------------------------------------------------------\n\tOptions:\n");
    fprintf(fmuHatTxt,"Workspace: %s\n",workspace);
    if (doAsimov) fprintf(fmuHatTxt,"Asimov: 1, ");  else fprintf(fmuHatTxt,"Asimov: 0, ");
    if (doDummy)  fprintf(fmuHatTxt,"Dummy ,");
    fprintf(fmuHatTxt,"Mode: %d, \n",mode);  
    fprintf(fmuHatTxt,"Mass: %s GeV; poi: %s; wsName: %s\n",mass,poiStr.Data(),wsName);
    fprintf(fmuHatTxt,"ModelConfigName: %s ; dataName: %s\n",modelConfigName,dataName);
    fprintf(fmuHatTxt,"----------------------------------------------------------\n\n");
    
    if(!blind) { fprintf(fmuHatTxt, "muhat: %7.5f\n\n",muhat); }
    if(!blind) { cout << "muhat: " << muhat << endl; }
    fprintf(fmuHatTxt, "nll_val_true: %f\n\n",nll_val_true); 
  
  
  
    fprintf(fmuHatTxt,"  Set of nuisance          Impact on error\n");
    fprintf(fmuHatTxt,"       parameters          \n");
    fprintf(fmuHatTxt,"----------------------------------------------------------\n");
  }
  cout << "Raw impact of nuisance parameter sets, quadratically substracted from total." <<endl;

  bool reducedStyle = false;
  char const* tmp = getenv("ANALYSISTYPE");
  if ( tmp == NULL ) {
    cout << "Couldn't getenv ANALYSISTYPE" <<endl;
  } else {
    std::string s( tmp );
    if ( s.compare("VHbbRun2") == 0 ){
      cout << "Reduced output format for VHbbRun2" <<endl;
      reducedStyle = true;
    }
  }

  fmuHatTex.precision(3);

  for (int iset  = 0; iset < npsets; iset++){
    if(!print_preamble && iset==0)continue;//the total error is part of the preamble
    if(only_some&&!run_these.count(iset))continue;
    //if (iset == 1) continue;
      
    if(reducedStyle){
      fmuHatTex << fixed << npsetname[iset]->Data()
                << " & +" << err_npset_hi[iset]
                << " / -" << err_npset_lo[iset] << " \\\\";
      if((strcmp(npsetname[iset]->Data(),"Total")==0) || (strcmp(npsetname[iset]->Data(),"FullSyst")==0) ) fmuHatTex << "\\hline";
      fmuHatTex << endl;
    }
    else{
      fmuHatTex << npsetname[iset]->Data() 
		<< " & $^{+ " << err_npset_hi[iset]
		<< "}_{-" << err_npset_lo[iset] << "}$"
		<< " & $\\pm " << 	(err_npset_hi[iset]+err_npset_lo[iset])/2. <<"$ \\\\" << endl;
    }

        cout << npsetname[iset]->Data() << " : + " 
	     << err_npset_hi[iset] << " / - "
	     << err_npset_lo[iset] 
	     << (err_npset_hi[iset]+err_npset_lo[iset])/2.
	     << endl;
            
        fprintf(fmuHatTxt,"%25s  + %6.3f  - %6.3f  +- %6.3f\n",
		npsetname[iset]->Data(), 
		err_npset_hi[iset],
		err_npset_lo[iset], 
		(err_npset_hi[iset]+err_npset_lo[iset])/2.);

  }
  
  if(print_postamble){
    //breakdown bottom of tables for both txt and tex
    fmuHatTex << "\\hline" << endl;
    fmuHatTex << "\\end{tabular}" << endl;
    fmuHatTex << endl;
    
    fprintf(fmuHatTxt,"Impact on error quadratically subtracted from total, except for:\n");
    for (int iset  = 0; iset < npsets; iset++){
      if (npsetexclude[iset]) fprintf(fmuHatTxt,"%s ",npsetname[iset]->Data());
    }
    fprintf(fmuHatTxt,"\n----------------------------------------------------------\n\n");
  }
  if(!print_preamble||!print_postamble){
    //for breakdowns broken up into multiple jobs, have a simple delimiter to separate the 
    //absolute NP lines from the fractional ones
    fprintf(fmuHatTxt,"*\n");
    fmuHatTex <<"*"<<endl;
  }
  if(print_preamble){
    //fractional impact table preambles
    fmuHatTex << "\\begin{tabular}{|l|c|}" << endl;
    fmuHatTex << "\\hline" << endl;
    fmuHatTex << "Set of nuisance & Fractional impact on error  \\\\\\hline" << endl;
    fmuHatTex << " parameters     &  (square of fraction of total) \\\\\\hline" << endl;
    fmuHatTex << "\\hline" << endl;

    fprintf(fmuHatTxt,"  Set of nuisance          Fractional impact on error\n");
    fprintf(fmuHatTxt,"       parameters          (square of fraction of total)\n");
    fprintf(fmuHatTxt,"----------------------------------------------------------\n");
  }

  
  cout << endl;
  cout << "Fractional impact of nuisance parameter sets, quadratically substracted from total." <<endl;
  for (int iset  = 1; iset < npsets; iset++){
    if(only_some&&!run_these.count(iset))continue;
    //if (iset == 1) continue;
 
        fmuHatTex.precision(2);
        fmuHatTex << npsetname[iset]->Data() 
		  << " & $^{+ " << fractionalError(err_npset_hi[0], err_npset_hi[iset])
		  << "}_{-" << fractionalError(err_npset_lo[0], err_npset_lo[iset]) <<"}$" 
		  << "& $ \\pm" << fractionalError((err_npset_hi[0]+err_npset_lo[0])/2., (err_npset_hi[iset]+err_npset_lo[iset])/2.) <<"$ \\\\" << endl;
        cout << npsetname[iset]->Data() << " : + " 
	     << fractionalError(err_npset_hi[0], err_npset_hi[iset]) << " / - "
	     << fractionalError(err_npset_lo[0], err_npset_lo[iset]) << " +- "
	     << fractionalError((err_npset_hi[0]+err_npset_lo[0])/2., (err_npset_hi[iset]+err_npset_lo[iset])/2.) 
	     << endl;

        fprintf(fmuHatTxt,"%25s  + %5.2f  - %5.2f  +-%5.2f\n",
		npsetname[iset]->Data(), 
		fractionalError(err_npset_hi[0], err_npset_hi[iset]),
		fractionalError(err_npset_lo[0], err_npset_lo[iset]),
		fractionalError((err_npset_hi[0]+err_npset_lo[0])/2., (err_npset_hi[iset]+err_npset_lo[iset])/2.) 
		);
      
  }
  if(print_postamble){
    //the very end of the fractional impact tables
    fmuHatTex << "\\hline" << endl;
    fmuHatTex << "\\end{tabular}" << endl;
    
    fprintf(fmuHatTxt,"----------------------------------------------------------\n\n");
  }
  //we always need to close the files
  fmuHatTex.close();
  fclose(fmuHatTxt);

  
  //cout << "Data - no NPs  : +" << 1 -err_npset_hi[1]/err_npset_hi[0] << " / -"
  //<< 1 - err_npset_lo[1]/err_npset_lo[0] << endl;
  cout << "DataStat - no NPs  : +" << err_npset_hi[1] << " / -" << err_npset_lo[1] << endl;
  cout << "FullSyst = Tot -(quad) Stat  : +" << err_npset_hi[2] << " / -" << err_npset_lo[2] << endl;
  cout << "TOT  : +" << err_npset_hi[0] << " / -" << err_npset_lo[0] << endl;

  //and now read off the final times
  timer->Stop();real_tot+=timer->RealTime(),cpu_tot+=timer->CpuTime();
  cout<<"Total time "<<real_tot<<" real sec, "<<cpu_tot<<" CPU sec"<<endl;

}
