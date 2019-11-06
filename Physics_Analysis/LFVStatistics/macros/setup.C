#ifndef __SETUP
#define __SETUP

// Author: Aaron Armbruster
// Date:   2011-11-16
//
// Description:
//
// Setup global objects used throughout the framework. Namely:
// - vector of all regions. each region holds topo selections and a vector of channels
// - channel objects, which holds channel specifications and will hold rates for each sample for each systematic variation
// - vector of all systematics

#include "TString.h"

#include <string>
#include <set>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>

#include "macros/log.C"

using namespace std;

// This is just for initialization
// Set all variables and cuts in config cards

// MODE VARIABLES
int mode                 = 0;
int massMode             = 0;
int ZMode                = 0;
bool doABCD2j            = 0;
bool doPacman2j          = 0;
int statMode             = 0;
int wjMode               = 0; // 0 = QCDFF, 1 = ZFF, 2 = CombFF
bool CutMTandRebin       = 0;

// BOUNDARIES
int massBoundary         = 200;
int massBoundary2        = 300;
bool useHighMass         = false;
bool useHighMass2        = false;
bool useLowPt            = true;
bool useHighPt           = true;
bool useAltCRs           = false;

// YEAR, FLAVOUR, JETS
bool do2011              = 0;
bool do2012              = 1;
bool doee                = 1;
bool domm                = 1;
bool doem                = 1;
bool dome                = 1;
bool splitem             = 0;
bool merge2j             = 1;
bool skipSF2j            = 1;
bool doSingleBinSF2j     = 0;
bool doSFonly            = 0;
bool do0j                = 1;
bool do1j                = 1;
bool do2j                = 1;
bool combineCRs          = 1;
bool combineSFCRs        = 1;
bool mergeSFSR           = 0;
bool merge2jSFSR         = 0;
bool merge2jtopCR        = 0;
bool skipSFtopCR         = 0;
bool skipSFWWCR          = 0;
bool doWWCR2j            = 0;
bool doOF2jWWCR          = 0;
bool merge2jWWCR         = 0;
string subleadptbounds   = "10,15,20,1000";
string mllbounds         = "0,30,50";


// SAMPLES
bool doggf               = 1;
bool dovbf               = 1;
bool dowh                = 1;
bool dozh                = 1;
bool doww                = 1;
bool dowwew              = 1;
bool dowzzz              = 1;
bool dowzzzew            = 1;
bool dozjets             = 1;
bool dozjetsew           = 1;
bool doWjets             = 1;
bool doSameSignCR        = 0; // jdl
bool doOSmSS		 = 0; //Nina
bool dottbar             = 1;
bool dost                = 1;
bool doWg                = 1;
bool doWgs               = 1;
bool doVH                = 1;
bool splitww             = 0;
bool splitzjets          = 1;
bool useJHUspin0         = 0;
bool include125BG        = 1;

// SYSTEMATICS, UNCERTAINTIES
bool useDetSys           = 1;
bool useShape            = 0 && useDetSys;
bool useThSys            = 1;
bool useStatSys          = 1;
double stat_cutoff       = 0.05;
bool skipid              = 0;
bool skipUEPS            = 0;
bool skiptrackMET        = 0;
bool splitjes            = 0;
bool useDYmtshape        = 0;
bool decoFakes           = 0;
bool useOldBtag          = 0;

// SCALING
bool useTheoryWW         = 0;
bool useTheoryTop        = 0;
bool scaleZ              = 1;
bool scaleTop            = 1;
double wjetsScale        = 1.0;
double SameSignScale     = 1.0;
bool scaleLUMI           = 0;
double lumiSF            = 1.538461538;

// BLINDING OPTIONS
bool conditionalAsimov   = 0;
bool doData              = 1;
bool dosignalregion      = 1;
bool doblindsignalregion = 1;

// BINNING AND CUTTING OPTIONS
bool overriden           = 0;
bool overrideCuts        = 0;
bool doSingleBin2j       = 1;
int nrBins_0j            = 5;
int nrBins_1j            = 5;
int nrBins_2j            = 5;
int nrBins_lowPt         = 3;

// CHECKS
bool splitEfficiencies   = 0;
bool splitPacmanNFs      = 0;
bool doCRVRcheck         = 0;
bool doPreHCP            = 0;
bool doPostHCP           = 0;

// OTHER STUFF
bool useLumiAsPOI        = 0; // experimental, untested
bool signFix             = 0;
bool rewriteXML          = 1;
int flatInterpCode       = 4;
int shapeInterpCode      = 4;
bool doSlopeTest         = 0;
double srSlopeFactor     = 1;

// OLD STUFF
bool bt20GeV             = 1;

// PACMAN EFICIENCIES ETC
double f_NDY_all         = 0.688861;
double f_NDY_SR_0j       = 0.71;
double f_NDY_WWCR_0j     = 0.71;
double f_NDY_ZP_0j       = 0.71;
double f_DY_all_0j       = 0.27;
double f_NDY_SR_1j       = 0.80;
double f_NDY_WWCR_1j     = 0.85;
double f_NDY_ZP_1j       = 0.79;
double f_DY_all_1j       = 0.48;
double f_NDY_SR_2j       = 0.81117;
double f_NDY_WWCR_2j     = 0.85;
double f_NDY_ZP_2j       = 0.823529;
double f_DY_all_2j       = 0.377331;
double ratio_S_NDY_SR_0j = 0.915;
double ratio_S_NDY_SR_1j = 0.953;
double ratio_S_NDY_SR_2j = 1.21;
double epsilon0_data_0j  = 0.7;
double epsilon0_data_1j  = 0.7;
double epsilon0_data_2j  = 0.66;

// NFs
double NFTop0jLoPT     = 1.212;
double NFTop0jHiPT     = 1.074;
double NFDY0j          = 0.708697;
double NFDY1j          = 0.845;

// DEFAULT CUT VALUES
bool useTopMVA1j       = 0;
double CutCJVleadPT    = 20;
double CutDPhi         = 1.8;
double CutDPhi2j       = 1.8;
double CutDPhillMET    = 1.57;
double CutDPhiCR       = 10e9;
double CutDYjj         = 2.8;
double CutFRecoil0j    = 0.05;
double CutFRecoil1j    = 0.2;
double CutFRecoil2j    = 0.2;
double CutMETDF        = 20;
double CutMETSF2j      = 45;
double CutMETrelDF     = 25;
double CutMETrelSF0j   = 45;
double CutMETrelSF1j   = 45;
double CutMETstvfSF2j  = 35;
double CutMETrelSF2j     = 35;
double CutMETrelstvfSF2j = 35;
double CutMjj          = 500.0;
double CutMT           = 10e9;
double CutMT2j         = 10e9;
double CutMTUp         =  0;
double CutMTUp2j       =  0;
double CutMll          = 50;
double CutMll2j        = 60;
double CutMllCR        = 80;
double CutMllZBhi      = 106.1876;
double CutMllZBlo      = 76.1876;
double CutMttSpin      = 100;
double CutPTllDF       = 30;
double CutPTllSF       = 30;
double CutPTlljetsDF   =  0;
double CutPTlljetsSF   =  0;
double CutPTlljetsSF2j =  25;
double CutPTmeel       =  0;
double CutPTtot        = 30;
double CutPTtot2j      = 45;
double CutTrackMETSF0j = 45;
double CutTrackMETSF1j = 45;
double CutTrackMETSF2j = 35;
double CutTopMVA1j     = 0.05;

// INPUT FOLDER
string ntupleFolder = "output"; // mT fit
string basedir      = "output_2012"; // BDT FIXME unify this

// PROPERTIES
bool doSpin                = 0;
bool doSpin1D              = 0;
bool doCutBasedSpin        = 0; // also needs doMVA = 1
bool applySlopeSpin        = 0;
bool doSpin2plus           = 1;
bool doSpin1plus           = 1;
bool doSpin1minus          = 1;
bool doSpin0minus          = 1;
bool doSpin2plus_100qq     = 0;
bool doSpin2plus_75qq      = 0;
bool doSpin2plus_50qq      = 0;
bool doSpin2plus_25qq      = 0;
bool doSpin2plus_100gg     = 0;

// MVA OPTIONS
bool doMVA                 = 0;
bool useWWCR_MVA           = 0; // This is set in the config file, if it is set to 1, the WW CR is used for the low mass region, otherwise, the WW CR is not used
bool doWWCR_MVA            = 0; // This is the actual variable which is set (mass-dependent)
bool doZCR_MVA             = 0;
int nrBins0j_MVA           = 10;
int nrBins1j_MVA           = 5;
int nrBins2j_MVA           = 5;
int nrBins0j_MVA_X         = 10;
int nrBins0j_MVA_Y         = 10;
bool doZjetsSubtraining    = 1;
bool doRemoveEmptyBins     = 0;
bool doSmartRemapBDT0j     = 0;
bool doSmartRemapBDT1j     = 0;
bool doSmartRemapBDT2j     = 0;
double fraction1stBin0j    = 0.5;
double fraction1stBin1j    = 0.5;
double fraction1stBin2j    = 0.5;
bool doCombineRightmostBin = 0;
bool doRemoveFirstBins     = 0;
bool doDynamicBinning      = 0;

// VBF OPTIONS
bool doVBF2j               = 0;
bool splitmjj              = 0;
bool splitmjjtopCR         = 0;
bool profileggf            = 0;
bool doMjjReweight         = 0;
bool doFlatMjj             = 0;
bool doZttVeto             = 0;
bool doOneBTag             = 0;
bool doFlat2j              = 1;
bool variableBin2j         = 0;
bool doTrackMET2j          = 0;
string treeName            = "BDT";

// DEFINITION OF STRUCTS USED THROUGHOUT THE FRAMEWORK

// SYSTEMATIC VARIATIONS
struct Sys
{
  Sys(string _folder, string _fileUp, string _fileDown, set<string> _sampleNames)
  {
    folder      = _folder;
    fileUp      = _fileUp;
    fileDown    = _fileDown;
    isShape     = 0;
    isNorm      = 1;
    sampleNames = _sampleNames;
  }

  Sys(string _folder, string _fileUp, string _fileDown, bool _isShape, bool _isNorm, set<string> _sampleNames)
  {
    folder      = _folder;
    fileUp      = _fileUp;
    fileDown    = _fileDown;
    isShape     = _isShape;
    isNorm      = _isNorm;
    sampleNames = _sampleNames;
  }

  Sys(string _folder, string _fileUp, string _fileDown, bool _isShape, bool _isNorm, string _sampleName)
  {
    folder   = _folder;
    fileUp   = _fileUp;
    fileDown = _fileDown;
    isShape  = _isShape;
    isNorm   = _isNorm;
    sampleNames.insert(_sampleName);
  }

  string folder;
  string fileUp;
  string fileDown;
  bool isShape;
  bool isNorm;
  set<string> sampleNames;
  set<string> veto;
};

// DEFINITION OF CHANNELS
struct Channel
{
  Channel()
  {
    name    = "";
    jetName = "";
    ne      = 0;
    nm      = 0;
    nj      = 0;
  }

  Channel(string _name, string _jetName, int _ne, int _nm, int _nj)
  {
    name    = _name;
    jetName = _jetName;
    ne      = _ne;
    nm      = _nm;
    nj      = _nj;
  }

  string name;
  string jetName;
  int ne;
  int nm;
  int nj;
  map<string, map<string, multiset<pair<double, double> > > > sys_sample_rates;
};

// DEFINITION OF CUTS
struct CutFunc
{
  CutFunc()
  {

  }

  CutFunc(string _name, double* _var, string _op, double _val, int _ne=-1, int _nm=-1, int _nj=-1, string _sampleName="")
  {
    name       = _name;
    var        = _var;
    op         = _op;
    val        = _val;
    ne         = _ne;
    nm         = _nm;
    nj         = _nj;
    sampleName = _sampleName;
  }

  string name;
  double* var;
  string op;
  double val;
  string sampleName;
  int ne,nm,nj;

  bool passed(int _ne, int _nm, int _nj, string _sampleName)
  {
    // if (name == "zjets_mva_weight") cout << "DEBUG::" << _nj << " " << nj << " " << *var << " " << val << endl;
    // if (name == "isBlinded") cout << "DEBUG::" << *var << " " << val << endl;

    if (sampleName != "" && sampleName != _sampleName) return true;

    if (ne != -1 && ne != _ne) return true;
    if (nm != -1 && nm != _nm) return true;
    if (nj != -1 && nj != _nj) return true;

    if (op == ">")
    {
      return *var > val;
    }
    else if (op == ">=")
    {
      return *var >= val;
    }
    else if (op == "<")
    {
      return *var < val;
    }
    else if (op == "<=")
    {
      return *var <= val;
    }
    else if (op == "==")
    {
      return *var == val;
    }
    else if (op == "!=")
    {
      return *var != val;
    }
    // else if (op == "<||>")
    // {
    //   return *var < val || *var > val2;
    // }
    else
    {
      return false;
    }
  }

  void print()
  {
    cout << name << " " <<  op << " " << val << endl;
  }
};

// PERSISTENT VARIABLES
struct Variables
{
  double l0pt;
  double l1pt;
  double leadpt;
  double subleadpt;
  double mll;
  double mtt;
  double mt;
  double metrel;
  double metrelstvf;
  double met;
  double trackmet;
  double metstvf;
  double ptll;
  double ptlljets;
  double dphi;
  double bdphi;
  double bdpsi;  
  double bpleadlep;
  double besubleadlep;
  double besubleadneu;
  double bpsubleadlep;
  double esum;
  double llcharge;
  double dphillmet;
  double nbt;
  double nbt20;
  double w;
  double run;
  double islowpt;
  double frecoil;
  double mva_weight; //for MVA
  double subtrain_mva_weight; //for MVA
  double zjets_mva_weight;
  double Mjj; //for MVA which needs to mimic 2jet cutsbased signal region
  double etaProdJJ; //for MVA which needs to mimic 2jet cutsbased signal region
  double DeltaEtaJJ; //for MVA which needs to mimic 2jet cutsbased signal region
  // double cjv; //for MVA which needs to mimic 2jet cutsbased signal region
  double isBlinded;
  double ztautaumass;
  double pttot;
  double output_var_spin;
  double mllZ;
  double dYjj;
  double olv;
  double cjv_leadPt;
  double isSS;
  double smt_muon_pt;
  double BDT_1jet_4vars;
};

double* output_var = NULL;

// DEFINITION OF REGIONS (SR, WW CR, TOPBOX, ...)
struct Region
{
  Region(string _name)
  {
    name   = _name;
    isCR   = false;
    isSFCR = false;
  }

  bool operator==(const Region& rhs)
  {
    return name == rhs.name;
  }

  string name;

  vector<CutFunc> cuts;
  vector<Channel> channels;
  multiset<double> all_rates;
  bool isCR;
  bool isSFCR;

  void print()
  {
    cout << "Region: " << name << endl;
    cout << "Cuts: " << endl;
    for (int i=0;i<(int)cuts.size();i++)
    {
      cuts[i].print();
    }
    cout << endl;
  }
};

// RESPONSE TERMS
struct Response
{
  bool match(string c, string j, string s, string r)
  {
    return c == channel && j == jet && s == sample && r == region;
  }

  void print()
  {
    cout << "Name: " << name << "\n"
    << "ID: " << sample + "_" + channel + "_" + region + "_" + jet +"\n"
    << "Error: +" << hi << " -" << lo << "\n\n";

  }

  string name;
  string channel;
  string jet;
  string sample;
  string region;
  double hi;
  double lo;
  string hiFileName;
  string loFileName;
  string hiHistName;
  string loHistName;
};

// DEFINE SAMPLE BY NAME AND TYPE
struct Sample
{

  Sample(string _name, string _type)
  {
    name=_name;
    type=_type;
  }

  bool operator==(string rhs)
  {
    return name == rhs;
  }

  bool operator!=(string rhs)
  {
    return name != rhs;
  }

  string name;
  string type;
};

vector<Sys>* fileNames;
vector<Region>* regions = NULL;
Variables* vars_ptr     = NULL; // used for cut interface
vector<Sample>* samples = NULL;

void readInUncerts(string fileName, vector<Response>& vecR, double mass, bool skipCR = 0, bool skipWj = 0, bool skipZj = 0);

vector<string> parseString(string str, string sep);

bool _initialized = false;

// SETUP STARTS HERE
//If doOnlySignal==false, adding only signal samples to samplesNames so that hists are built only for signal (interesting to get the mass points made in jobs different from the -common- background hists)
void setup(double mass, bool alt = false, bool doOnlySignal=false)
{
  if (_initialized) return;
  if (alt) // warningless compile
  {

  }

  // GET SIGNAL NAMES RIGHT

  stringstream ggfName;
  // if(!doVBF2j) ggfName << "ggf" << mass;
  // else ggfName << "ggf125";
  ggfName << "ggf" << mass; 

  stringstream ggfName0;// this will be used for spin analysis
  if(useJHUspin0) {
    ggfName0 << "ggH" << mass << "spin0p";
  }  
  else {
    ggfName0 << "ggf" << mass;
  }

  stringstream ggfName2;
  if(doSpin2plus)  ggfName2 << "ggH" << mass << "spin2p";
  else if(doSpin1plus)  ggfName2 << "ggH" << mass << "spin1p";
  else if(doSpin1minus)  ggfName2 << "ggH" << mass << "spin1m";
  else if(doSpin0minus)  ggfName2 << "ggH" << mass << "spin0m";

  stringstream vbfName;
  vbfName << "vbf" << mass;

  stringstream whName;
  whName << "wh" << mass;

  stringstream zhName;
  zhName << "zh" << mass;

  // FILL THE SAMPLES VECTOR
  static vector<Sample> sampleNames;
  if (sampleNames.size()) sampleNames.clear();
  if (include125BG)  {
    sampleNames.push_back(Sample("ggf125bg" , "bg")); 
    sampleNames.push_back(Sample("vbf125bg" , "bg")); 
    sampleNames.push_back(Sample("wh125bg"  , "bg")); 
    sampleNames.push_back(Sample("zh125bg"  , "bg")); 
  }
  // Spin 0 and Spin 2 ggf
  if (!doSpin) {
    // Use only vbf as signal and ggf as background in case of vbf analysis
    if (!doVBF2j) {
      if (doggf) sampleNames.push_back(Sample(ggfName.str() , "signal"));
    }
    else {
      if (doggf) sampleNames.push_back(Sample(ggfName.str() , "bg"));
    }
  }
  else {
    if (doggf) sampleNames.push_back(Sample(ggfName0.str() , "signal"));
    if (doggf) sampleNames.push_back(Sample(ggfName2.str() , "signal"));
  }
  if (dovbf) sampleNames.push_back(Sample(vbfName.str() , "signal"));
  if (mass <= 300 && doVH) {
    if (dowh) sampleNames.push_back(Sample(whName.str() , "signal"));
    if (dozh) sampleNames.push_back(Sample(zhName.str() , "signal"));
  }

  if (!doOnlySignal) {
    if (dottbar) sampleNames.push_back(Sample("ttbar" , "bg"));
    if (dost)    sampleNames.push_back(Sample("st"    , "bg"));
    if (doww) {
      if (!splitww) {
        sampleNames.push_back(Sample("ww" , "bg"));
      }
      else {
        sampleNames.push_back(Sample("ggww" , "bg"));
        sampleNames.push_back(Sample("qqww" , "bg"));
      }
    }
    
    if(doOSmSS) { // Nina
      sampleNames.push_back(Sample("ss" , "bg")); // Nina
      sampleNames.push_back(Sample("wjetsminusss" , "bg")); // Nina, for now i have just named this sample wjetsminusss to keep it seperate from regular wjets, will fix this in the prep.C code
    } else {
      if (doWjets) sampleNames.push_back(Sample("wjets" , "bg")); 
      if (dowwew)  sampleNames.push_back(Sample("wwew"  , "bg")); // Nina are these the samples we dont want in SS? are wwew zzz zzzew considered diboson samples?
      if (dowzzz)  sampleNames.push_back(Sample("wzzz"  , "bg")); // Nina is this diboson? are these the 2 jet samples?
      if (dowzzzew)sampleNames.push_back(Sample("wzzzew", "bg")); // Nina is this diboson? are these the 2 jet samples?
      if (doWgs)   sampleNames.push_back(Sample("wgs"   , "bg"));
      if (doWg)    sampleNames.push_back(Sample("wg"    , "bg"));
    }

    if (dozjets) {
      if (!splitzjets) sampleNames.push_back(Sample("zjets" , "bg"));
      else {
        sampleNames.push_back(Sample("zleplep" , "bg"));
        sampleNames.push_back(Sample("ztautau" , "bg"));
      }
    }
    if (dozjetsew) {
      if (!splitzjets) sampleNames.push_back(Sample("zjetsew" , "bg"));
      else {
        sampleNames.push_back(Sample("zleplepew" , "bg"));
        sampleNames.push_back(Sample("ztautauew" , "bg"));
      }
    }
    // if (doWjets) sampleNames.push_back(Sample("wjets" , "bg"));
  } //end of doOnlySignal
  samples = &sampleNames;

  // VARIABLES
  static Variables myVars;
  vars_ptr = &myVars;
  if (!doMVA)
  {
    if (mode != 3 && mode != 4 && mode != 5) output_var = &vars_ptr->mt;
    else output_var = &vars_ptr->dphi;
  }
  else
  {
    if (!doSpin) output_var = &vars_ptr->mva_weight;
    else if(doSpin && !doSpin1D) output_var = &vars_ptr->output_var_spin;
    else if(doSpin &&  doSpin1D) output_var = &vars_ptr->mva_weight; //dphi;
  }

  // FILL CHANNELS
  vector<Channel> channels;//  names, ne,nm,nj
  if (do0j && doee) channels.push_back(Channel("ee","0j",2,0,0));
  if (do0j && domm) channels.push_back(Channel("mm","0j",0,2,0));
  if (do0j && doem) channels.push_back(Channel("em","0j",1,1,0));
  if (splitem && do0j && dome) channels.push_back(Channel("me","0j",1,1,0));
  if (do1j && doee) channels.push_back(Channel("ee","1j",2,0,1));
  if (do1j && domm) channels.push_back(Channel("mm","1j",0,2,1));
  if (do1j && doem) channels.push_back(Channel("em","1j",1,1,1));
  if (splitem && do1j && dome) channels.push_back(Channel("me","1j",1,1,1));
  if (do2j && doee) channels.push_back(Channel("ee","2j",2,0,2));
  if (do2j && domm) channels.push_back(Channel("mm","2j",0,2,2));
  if (do2j && doem) channels.push_back(Channel("em","2j",1,1,2));
  if (splitem && do2j && dome) channels.push_back(Channel("me","2j",1,1,2));
  if (!channels.size())
  {
    cout << "ERROR::No channels selected!" << endl;
    exit(1);
  }

  vector<Channel> crChannels;
  if (combineCRs)
  {
    if (do0j && doem && dome) crChannels.push_back(Channel("OF","0j",1,1,0));
    if (do1j && doem && dome) crChannels.push_back(Channel("OF","1j",1,1,1));
    if (do2j && doem && dome) crChannels.push_back(Channel("OF","2j",1,1,2));
    if (!combineSFCRs)
    {
      if (do0j && doee) crChannels.push_back(Channel("ee","0j",2,0,0));
      if (do0j && domm) crChannels.push_back(Channel("mm","0j",0,2,0));
      if (do1j && doee) crChannels.push_back(Channel("ee","1j",2,0,1));
      if (do1j && domm) crChannels.push_back(Channel("mm","1j",0,2,1));
      if (do2j && doee) crChannels.push_back(Channel("ee","2j",2,0,2));
      if (do2j && domm) crChannels.push_back(Channel("mm","2j",0,2,2));
    }
  }
  if (combineSFCRs)
  {
    if (do0j && doee && domm) crChannels.push_back(Channel("SF","0j",2,0,0));
    // if (do0j && domm) crChannels.push_back(Channel("SF","0j",0,2,0));
    if (do1j && doee && domm) crChannels.push_back(Channel("SF","1j",2,0,1));
    // if (do1j && domm) crChannels.push_back(Channel("SF","1j",0,2,1));
    if (do2j && doee && domm) crChannels.push_back(Channel("SF","2j",2,0,2));
    // if (do2j && domm) crChannels.push_back(Channel("SF","2j",0,2,2));
    if (!combineCRs)
    {
      if (do0j && doem)            crChannels.push_back(Channel("em","0j",1,1,0));
      if (splitem && do0j && dome) crChannels.push_back(Channel("me","0j",1,1,0));
      if (do1j && doem)            crChannels.push_back(Channel("em","1j",1,1,1));
      if (splitem && do1j && dome) crChannels.push_back(Channel("me","1j",1,1,1));
      if (do2j && doem)            crChannels.push_back(Channel("em","2j",1,1,2));
      if (splitem && do2j && dome) crChannels.push_back(Channel("me","2j",1,1,2));
    }
  }
  if (!combineCRs && !combineSFCRs)
  {
    crChannels = channels;
  }

  // DEFINE ALL REGIONS
  vector<Region> srs;
  static vector<Region> myRegions;
  if (myRegions.size()) myRegions.clear();
  if (mode == 0 || mode == 1 || mode == 3 || mode == 4 || mode == 5 || mode == 6 || mode == 7 || mode == 8) // regions and cut specifications
  {
    // Common cuts for removing electroweak contributions in 0 and 1j, purely technical (Note OS cut is added after SS CR below)
    vector<CutFunc> KillEWCutsCommon01j;
    KillEWCutsCommon01j.push_back(CutFunc("mll" , &vars_ptr->mll , "=="  , 0 , -1 , -1 , 0 , "wwew"));
    KillEWCutsCommon01j.push_back(CutFunc("mll" , &vars_ptr->mll , "=="  , 0 , -1 , -1 , 1 , "wwew"));
    KillEWCutsCommon01j.push_back(CutFunc("mll" , &vars_ptr->mll , "=="  , 0 , -1 , -1 , 0 , "wzzzew"));
    KillEWCutsCommon01j.push_back(CutFunc("mll" , &vars_ptr->mll , "=="  , 0 , -1 , -1 , 1 , "wzzzew"));
    KillEWCutsCommon01j.push_back(CutFunc("mll" , &vars_ptr->mll , "=="  , 0 , -1 , -1 , 0 , "zjetsew"));
    KillEWCutsCommon01j.push_back(CutFunc("mll" , &vars_ptr->mll , "=="  , 0 , -1 , -1 , 1 , "zjetsew"));
    KillEWCutsCommon01j.push_back(CutFunc("mll" , &vars_ptr->mll , "=="  , 0 , -1 , -1 , 0 , "zleplepew"));
    KillEWCutsCommon01j.push_back(CutFunc("mll" , &vars_ptr->mll , "=="  , 0 , -1 , -1 , 1 , "zleplepew"));
    KillEWCutsCommon01j.push_back(CutFunc("mll" , &vars_ptr->mll , "=="  , 0 , -1 , -1 , 0 , "ztautauew"));
    KillEWCutsCommon01j.push_back(CutFunc("mll" , &vars_ptr->mll , "=="  , 0 , -1 , -1 , 1 , "ztautauew"));

    // SIGNALREGION
    if (!useHighMass && !useHighMass2 && !doVBF2j) {
      CutMll    = 50;
      CutMll2j  = 60;
      CutDPhi   = 1.8;
      CutDPhi2j = 1.8;
    }
    else if (useHighMass && !doVBF2j) {
      CutDPhi   = 10e9;
      CutMll    = 150;
      CutDPhi2j = 10e9;
      CutMll2j  = 150;
    }
    else if (!doVBF2j){
      CutDPhi   = 10e9;
      CutMll    = 10e9;
      CutDPhi2j = 10e9;
      CutMll2j  = 10e9;
    }

    if (doSpin) {
      CutMETrelDF = 20;
      CutPTllDF   = 20;
      CutDPhi     = 2.5;
      CutMll      = 75;
      CutMT       = 1.2*mass;
      CutMTUp     = 0.5*mass;
      CutPTtot    = 10e9;
    }

    // parse boundary strings
    vector<string> parsedsubleadptbounds = parseString(subleadptbounds, ",");
    vector<string> parsedmllbounds = parseString(mllbounds, ",");

    // Common cuts for DF SR  (note OS cut is after SS CR)
    vector<CutFunc> CommonCutsDFSR;
    CommonCutsDFSR = KillEWCutsCommon01j;

    CommonCutsDFSR.push_back(CutFunc("dphillmet"   , &vars_ptr->dphillmet   , ">"  , CutDPhillMET    , -1 , -1 ,  0 ));
    CommonCutsDFSR.push_back(CutFunc("metrel"      , &vars_ptr->metrel      , ">"  , CutMETrelDF     ,  1 ,  1 ,  0 ));
    CommonCutsDFSR.push_back(CutFunc("metrel"      , &vars_ptr->metrel      , ">"  , CutMETrelDF     ,  1 ,  1 ,  1 ));
    CommonCutsDFSR.push_back(CutFunc("metrel"      , &vars_ptr->metrel      , ">"  , CutMETrelSF0j   ,  2 ,  0 ,  0 ));
    CommonCutsDFSR.push_back(CutFunc("metrel"      , &vars_ptr->metrel      , ">"  , CutMETrelSF0j   ,  0 ,  2 ,  0 ));
    CommonCutsDFSR.push_back(CutFunc("metrel"      , &vars_ptr->metrel      , ">"  , CutMETrelSF1j   ,  2 ,  0 ,  1 ));
    CommonCutsDFSR.push_back(CutFunc("metrel"      , &vars_ptr->metrel      , ">"  , CutMETrelSF1j   ,  0 ,  2 ,  1 ));
    if (doPacman2j) {
      CommonCutsDFSR.push_back(CutFunc("frecoil"   , &vars_ptr->frecoil     , ">"  , CutFRecoil2j    ,  2 ,  0 ,  2 , "data"));
      CommonCutsDFSR.push_back(CutFunc("frecoil"   , &vars_ptr->frecoil     , ">"  , CutFRecoil2j    ,  0 ,  2 ,  2 , "data"));
      CommonCutsDFSR.push_back(CutFunc("metrel"    , &vars_ptr->metrel      , ">"  , CutMETrelSF2j   ,  2 ,  0 ,  2 ));
      CommonCutsDFSR.push_back(CutFunc("metrel"    , &vars_ptr->metrel      , ">"  , CutMETrelSF2j   ,  0 ,  2 ,  2 ));
      CommonCutsDFSR.push_back(CutFunc("ptlljets"  , &vars_ptr->ptlljets    , ">"  , CutPTlljetsSF2j ,  2 ,  0 ,  2 ));
      CommonCutsDFSR.push_back(CutFunc("ptlljets"  , &vars_ptr->ptlljets    , ">"  , CutPTlljetsSF2j ,  0 ,  2 ,  2 ));
    } else {
    CommonCutsDFSR.push_back(CutFunc("met"         , &vars_ptr->met         , ">"  , CutMETSF2j      ,  2 ,  0 ,  2 ));
    CommonCutsDFSR.push_back(CutFunc("met"         , &vars_ptr->met         , ">"  , CutMETSF2j      ,  0 ,  2 ,  2 ));
    }      
    CommonCutsDFSR.push_back(CutFunc("met"         , &vars_ptr->met         , ">"  , CutMETDF        ,  1 ,  1 ,  2 ));
    CommonCutsDFSR.push_back(CutFunc("islowpt"     , &vars_ptr->islowpt     , "==" , 0               , -1 , -1 , -1 ));
    CommonCutsDFSR.push_back(CutFunc("nbt"         , &vars_ptr->nbt         , "==" , 0               , -1 , -1 , -1 ));
    CommonCutsDFSR.push_back(CutFunc("smt"         , &vars_ptr->smt_muon_pt , "<"  , 4               , -1 , -1 ,  1 ));
    CommonCutsDFSR.push_back(CutFunc("ptll"        , &vars_ptr->ptll        , ">"  , CutPTllSF       ,  2 ,  0 ,  0 ));
    CommonCutsDFSR.push_back(CutFunc("ptll"        , &vars_ptr->ptll        , ">"  , CutPTllSF       ,  0 ,  2 ,  0 ));
    CommonCutsDFSR.push_back(CutFunc("ptll"        , &vars_ptr->ptll        , ">"  , CutPTllDF       ,  1 ,  1 ,  0 ));
    CommonCutsDFSR.push_back(CutFunc("ztautaumass" , &vars_ptr->ztautaumass , "==" , 0               , -1 , -1 ,  0 ));
    CommonCutsDFSR.push_back(CutFunc("ztautaumass" , &vars_ptr->ztautaumass , "==" , 0               , -1 , -1 ,  1 ));
    CommonCutsDFSR.push_back(CutFunc("ztautaumass" , &vars_ptr->ztautaumass , "==" , 0               , -1 , -1 ,  2 )); 
    CommonCutsDFSR.push_back(CutFunc("ptlljets"    , &vars_ptr->ptlljets    , ">"  , CutPTlljetsSF   ,  2 ,  0 ,  1 ));
    CommonCutsDFSR.push_back(CutFunc("ptlljets"    , &vars_ptr->ptlljets    , ">"  , CutPTlljetsSF   ,  0 ,  2 ,  1 ));
    CommonCutsDFSR.push_back(CutFunc("ptlljets"    , &vars_ptr->ptlljets    , ">"  , CutPTlljetsDF   ,  1 ,  1 ,  1 ));
    CommonCutsDFSR.push_back(CutFunc("pttot"       , &vars_ptr->pttot       , "<"  , CutPTtot2j      , -1 , -1 ,  2 ));
    CommonCutsDFSR.push_back(CutFunc("trackmet"    , &vars_ptr->trackmet    , ">"  , CutTrackMETSF0j ,  2 ,  0 ,  0 ));
    CommonCutsDFSR.push_back(CutFunc("trackmet"    , &vars_ptr->trackmet    , ">"  , CutTrackMETSF0j ,  0 ,  2 ,  0 ));
    CommonCutsDFSR.push_back(CutFunc("trackmet"    , &vars_ptr->trackmet    , ">"  , CutTrackMETSF1j ,  2 ,  0 ,  1 ));
    CommonCutsDFSR.push_back(CutFunc("trackmet"    , &vars_ptr->trackmet    , ">"  , CutTrackMETSF1j ,  0 ,  2 ,  1 ));
    CommonCutsDFSR.push_back(CutFunc("dYjj"        , &vars_ptr->dYjj        , ">"  , CutDYjj         , -1 , -1 ,  2 ));
    CommonCutsDFSR.push_back(CutFunc("olv"         , &vars_ptr->olv         , "==" , 1               , -1 , -1 ,  2 ));        
    CommonCutsDFSR.push_back(CutFunc("Mjj"         , &vars_ptr->Mjj         , ">"  , CutMjj          , -1 , -1 ,  2 ));
    CommonCutsDFSR.push_back(CutFunc("cjv_leadPt"  , &vars_ptr->cjv_leadPt  , "<"  , CutCJVleadPT    , -1 , -1 ,  2 ));
    if (!do2011) {
      if(doTrackMET2j) {
        CommonCutsDFSR.push_back(CutFunc("trackmet" , &vars_ptr->trackmet   , ">"  , CutTrackMETSF2j ,  2 ,  0 ,  2 ));
        CommonCutsDFSR.push_back(CutFunc("trackmet" , &vars_ptr->trackmet   , ">"  , CutTrackMETSF2j ,  0 ,  2 ,  2 ));
      } else if (doPacman2j) {
        CommonCutsDFSR.push_back(CutFunc("metrelstvf", &vars_ptr->metrelstvf  , ">"  , CutMETrelstvfSF2j ,  2 ,  0 ,  2 ));
        CommonCutsDFSR.push_back(CutFunc("metrelstvf", &vars_ptr->metrelstvf  , ">"  , CutMETrelstvfSF2j ,  0 ,  2 ,  2 ));
      } else {
        CommonCutsDFSR.push_back(CutFunc("metstvf" , &vars_ptr->metstvf     , ">"  , CutMETstvfSF2j  ,  2 ,  0 ,  2 ));
        CommonCutsDFSR.push_back(CutFunc("metstvf" , &vars_ptr->metstvf     , ">"  , CutMETstvfSF2j  ,  0 ,  2 ,  2 ));
      }
    }
    if (!doCRVRcheck) {
      CommonCutsDFSR.push_back(CutFunc("dphi"      , &vars_ptr->dphi        , "<"  , CutDPhi         , -1 , -1 ,  0 ));
      CommonCutsDFSR.push_back(CutFunc("dphi"      , &vars_ptr->dphi        , "<"  , CutDPhi         , -1 , -1 ,  1 ));
      CommonCutsDFSR.push_back(CutFunc("dphi"      , &vars_ptr->dphi        , "<"  , CutDPhi2j       , -1 , -1 ,  2 ));
      CommonCutsDFSR.push_back(CutFunc("mt"        , &vars_ptr->mt          , "<"  , CutMT           , -1 , -1 ,  0 ));
      CommonCutsDFSR.push_back(CutFunc("mt"        , &vars_ptr->mt          , ">"  , CutMTUp         , -1 , -1 ,  0 ));
      CommonCutsDFSR.push_back(CutFunc("mt"        , &vars_ptr->mt          , "<"  , CutMT           , -1 , -1 ,  1 ));
      CommonCutsDFSR.push_back(CutFunc("mt"        , &vars_ptr->mt          , ">"  , CutMTUp         , -1 , -1 ,  1 ));
      CommonCutsDFSR.push_back(CutFunc("mt"        , &vars_ptr->mt          , "<"  , CutMT2j         ,  1 ,  1 ,  2 ));
      if(doSingleBinSF2j) {
        if (!do2011) {
          CommonCutsDFSR.push_back(CutFunc("mt"    , &vars_ptr->mt          , "<"  , 1.2*mass        ,  0 ,  2 ,  2 )); // FIXME mH dependence
          CommonCutsDFSR.push_back(CutFunc("mt"    , &vars_ptr->mt          , "<"  , 1.2*mass        ,  2 ,  0 ,  2 )); // FIXME mH dependence
        }
        else {
          CommonCutsDFSR.push_back(CutFunc("mt"    , &vars_ptr->mt          , "<"  , 150             ,  0 ,  2 ,  2 )); // FIXME move to 2 bin mT fit instead of hard cut
          CommonCutsDFSR.push_back(CutFunc("mt"    , &vars_ptr->mt          , "<"  , 150             ,  2 ,  0 ,  2 )); // FIXME move to 2 bin mT fit instead of hard cut
        }
      }
      CommonCutsDFSR.push_back(CutFunc("mt"        , &vars_ptr->mt          , ">"  , CutMTUp2j       , -1 , -1 ,  2 ));
    }
    if (doSpin)    CommonCutsDFSR.push_back(CutFunc("ptll"  , &vars_ptr->ptll  , ">"  , CutPTllDF    ,  1 ,  1 ,  1 ));
    if (doSpin)    CommonCutsDFSR.push_back(CutFunc("Mtt"   , &vars_ptr->mtt   , "<"  , CutMttSpin   ,  1 ,  1 ,  0 ));
    if (bt20GeV)   CommonCutsDFSR.push_back(CutFunc("nbt20" , &vars_ptr->nbt20 , "==" , 0            , -1 , -1 ,  0 ));
    if (doPreHCP)  CommonCutsDFSR.push_back(CutFunc("run"   , &vars_ptr->run   , "<=" , 210308       , -1 , -1 , -1 , "data"));
    if (doPostHCP) CommonCutsDFSR.push_back(CutFunc("run"   , &vars_ptr->run   , ">"  , 210308       , -1 , -1 , -1 , "data"));
    if (doblindsignalregion) CommonCutsDFSR.push_back(CutFunc("isBlinded"      , &vars_ptr->isBlinded , "==" , 0 , -1 , -1 , -1 , "data" ));
    if (useTopMVA1j) CommonCutsDFSR.push_back(CutFunc("BDT_1jet_4vars" , &vars_ptr->BDT_1jet_4vars , ">"  , CutTopMVA1j , -1 , -1 ,  1 ));



    // SAME SIGN CONTROL REGION
    Region sscr("sscr");
    sscr.isCR = true;
    sscr.channels = crChannels;
    sscr.cuts = CommonCutsDFSR;
    sscr.cuts.push_back(CutFunc("isSS"   , &vars_ptr->isSS         , "==" , 1        , -1 , -1 , -1 ));
    sscr.cuts.push_back(CutFunc("mll"    , &vars_ptr->mll          , "<"  , CutMll   , -1 , -1 , -1 ));


    // Require opposite sign
    CommonCutsDFSR.push_back(CutFunc("isSS"  , &vars_ptr->isSS  , "=="  , 0    ,  -1 ,  -1 ,  -1 ));
    KillEWCutsCommon01j.push_back(CutFunc("isSS" , &vars_ptr->isSS , "=="  , 0 , -1 , -1 , -1  ));


    Region sr("signalLike");
    if (mode == 6 || (mode==1 && splitmjj)) sr.name = "signalLike1";
    sr.channels = channels;

    Region sr2("signalLike2");
    sr2.channels = channels;
    
    Region sr3("signalLike3");
    sr3.channels = channels;

    if (mode != 7 && mode != 8) {
      if (!doMVA) {
        sr.cuts = CommonCutsDFSR;

        if(mode == 1 && splitmjj){
	  sr2.cuts = CommonCutsDFSR;
	  sr.cuts.push_back(CutFunc("Mjj" , &vars_ptr->Mjj      , "<"  , 1000       , -1 , -1 , 2 ));
          sr2.cuts.push_back(CutFunc("Mjj" , &vars_ptr->Mjj      , ">"  , 1000       , -1 , -1 , 2 ));
	  sr2.cuts.push_back(CutFunc("mll"   , &vars_ptr->mll      , "<"  , CutMll2j , -1 , -1 , 2 ));
	}

        if (!doCRVRcheck) {
          if (mode == 6) {
            sr.cuts.push_back(CutFunc("mll" , &vars_ptr->mll      , "<"  , 30       , -1 , -1 , 0 ));
            sr.cuts.push_back(CutFunc("mll" , &vars_ptr->mll      , "<"  , 30       , -1 , -1 , 1 ));
          }
          else {
            sr.cuts.push_back(CutFunc("mll" , &vars_ptr->mll      , "<"  , CutMll   , -1 , -1 , 0 ));
            sr.cuts.push_back(CutFunc("mll" , &vars_ptr->mll      , "<"  , CutMll   , -1 , -1 , 1 ));
          }
          sr.cuts.push_back(CutFunc("mll"   , &vars_ptr->mll      , "<"  , CutMll2j , -1 , -1 , 2 ));
        }
        else {
          sr.cuts.push_back(CutFunc("mll"   , &vars_ptr->mll      , ">"  , 50       , -1 , -1 , 0 ));
          sr.cuts.push_back(CutFunc("mll"   , &vars_ptr->mll      , ">"  , 50       , -1 , -1 , 1 ));
          sr.cuts.push_back(CutFunc("mll"   , &vars_ptr->mll      , ">"  , 50       , -1 , -1 , 2 ));
          sr.cuts.push_back(CutFunc("mll"   , &vars_ptr->mll      , "<"  , 100      , -1 , -1 , 0 ));
          sr.cuts.push_back(CutFunc("mll"   , &vars_ptr->mll      , "<"  , 100      , -1 , -1 , 1 ));
          sr.cuts.push_back(CutFunc("mll"   , &vars_ptr->mll      , "<"  , 100      , -1 , -1 , 2 ));
        }

        if (mode == 6) {
          sr2.cuts = CommonCutsDFSR;

          sr2.cuts.push_back(CutFunc("mll" , &vars_ptr->mll       , ">"  , 30       , -1 , -1 , 0 ));
          sr2.cuts.push_back(CutFunc("mll" , &vars_ptr->mll       , ">"  , 30       , -1 , -1 , 1 ));
          sr2.cuts.push_back(CutFunc("mll" , &vars_ptr->mll       , "<"  , CutMll   , -1 , -1 , 0 ));
          sr2.cuts.push_back(CutFunc("mll" , &vars_ptr->mll       , "<"  , CutMll   , -1 , -1 , 1 ));
          sr2.cuts.push_back(CutFunc("mll" , &vars_ptr->mll       , "<"  , CutMll2j , -1 , -1 , 2 ));
        }
      }
      else {
        sr.cuts.push_back(CutFunc("nbt" , &vars_ptr->nbt , "==" , 0 ));
        if (!doVBF2j) { // mimic the cutbased 2 jet bin
          if (!doSpin) {
            sr.cuts.push_back(CutFunc("DeltaEtaJJ" , &vars_ptr->DeltaEtaJJ , ">"  , 3.8       , -1, -1, 2 ));
            sr.cuts.push_back(CutFunc("dphi"       , &vars_ptr->dphi       , "<"  , CutDPhi2j , -1, -1, 2 ));
            sr.cuts.push_back(CutFunc("etaProdJJ"  , &vars_ptr->etaProdJJ  , "<"  , 0         , -1, -1, 2 ));
            sr.cuts.push_back(CutFunc("Mjj"        , &vars_ptr->Mjj        , ">"  , CutMjj    , -1, -1, 2 ));
            sr.cuts.push_back(CutFunc("mll"        , &vars_ptr->mll        , "<"  , CutMll2j  , -1, -1, 2 ));
          }
          else {
            sr.cuts.push_back(CutFunc("ptll"       , &vars_ptr->ptll       , ">"  , 20        , -1 , -1 , -1 ));
            sr.cuts.push_back(CutFunc("mll"        , &vars_ptr->mll        , "<"  , 80        , -1 , -1 , -1 ));
            sr.cuts.push_back(CutFunc("dphi"       , &vars_ptr->dphi       , "<"  , 2.8       , -1 , -1 , -1 ));
            sr.cuts.push_back(CutFunc("llcharge"   , &vars_ptr->llcharge   , "<"  , 0.        , -1 , -1 , -1 ));
            
            if( mode == 6){
            sr.cuts.push_back(CutFunc("mva_weight"       , &vars_ptr->mva_weight       , ">"  , -0.2        , -1 , -1 , -1 ));
            sr.cuts.push_back(CutFunc("subtrain_mva_weight"       , &vars_ptr->subtrain_mva_weight       , ">"  , -0.2        , -1 , -1 , -1 ));
     
            sr2.cuts.push_back(CutFunc("ptll"       , &vars_ptr->ptll       , ">"  , 20         , -1 , -1 , -1 )); 
            sr2.cuts.push_back(CutFunc("mll"        , &vars_ptr->mll        , "<"  , 80         , -1 , -1 , -1 )); 
            sr2.cuts.push_back(CutFunc("dphi"       , &vars_ptr->dphi       , "<"  , 2.8        , -1 , -1 , -1 ));  
            sr2.cuts.push_back(CutFunc("subtrain_mva_weight"       , &vars_ptr->subtrain_mva_weight       , "<"  , -0.2       , -1 , -1 , -1 ));   
      
            sr3.cuts.push_back(CutFunc("ptll"       , &vars_ptr->ptll       , ">"  , 20         , -1 , -1 , -1 )); 
            sr3.cuts.push_back(CutFunc("mll"        , &vars_ptr->mll        , "<"  , 80         , -1 , -1 , -1 )); 
            sr3.cuts.push_back(CutFunc("dphi"       , &vars_ptr->dphi       , "<"  , 2.8        , -1 , -1 , -1 ));  
            sr3.cuts.push_back(CutFunc("subtrain_mva_weight"       , &vars_ptr->subtrain_mva_weight       , ">"  , -0.2       , -1 , -1 , -1 ));   
            sr3.cuts.push_back(CutFunc("mva_weight"       , &vars_ptr->mva_weight       , "<"  , -0.2       , -1 , -1 , -1 ));   
          }
          
          if(doRemoveFirstBins)  sr.cuts.push_back(CutFunc("mva_weight"       , &vars_ptr->mva_weight       , ">"  , -0.5        , -1 , -1 , -1 ));
          if(doRemoveFirstBins)  sr.cuts.push_back(CutFunc("subtrain_mva_weight"       , &vars_ptr->subtrain_mva_weight       , ">"  , -0.5        , -1 , -1 , -1 ));            
          }
          if(doSpin1D || doCutBasedSpin) {
            sr.cuts.push_back(CutFunc("mt" , &vars_ptr->mt , ">" , mass*0.5 , -1 , -1, -1 )); 
            sr.cuts.push_back(CutFunc("mt" , &vars_ptr->mt , "<" , mass*1.2 , -1 , -1, -1 ));           
          }
        }
        else {
          if (mass < 135)      sr.cuts.push_back(CutFunc("mt" , &vars_ptr->mt , "<" , mass+75 , -1 , -1, 2 ));
          else if (mass < 400) sr.cuts.push_back(CutFunc("mt" , &vars_ptr->mt , "<" , mass+85 , -1 , -1, 2 ));
        }
        if (doZjetsSubtraining) sr.cuts.push_back(CutFunc("zjets_mva_weight" , &vars_ptr->zjets_mva_weight , ">" ,  0   , -1 , -1 , 0 ));
        if (doZjetsSubtraining) sr.cuts.push_back(CutFunc("zjets_mva_weight" , &vars_ptr->zjets_mva_weight , ">" , -0.4 , -1 , -1 , 1 ));
        if (doWWCR_MVA) {
          if (!useHighMass && !useHighMass2) {
            CutMllCR = 80;
            sr.cuts.push_back(CutFunc("mll" , &vars_ptr->mll , "<=" , CutMllCR , -1 , -1 , 0 ));
            sr.cuts.push_back(CutFunc("mll" , &vars_ptr->mll , "<=" , CutMllCR , -1 , -1 , 1 ));
          }
        }
      }
    }
    else if (mode == 7) { // mode == 7
      int counter = 0;
      for (int mllBoundary = 25; mllBoundary <= 200; mllBoundary += 25) {
        stringstream srName;
        srName << "signalLike" << counter++;
        Region sr3(srName.str());
        sr3.channels = channels;
        sr3.cuts = CommonCutsDFSR;

        sr3.cuts.push_back(CutFunc("mll" , &vars_ptr->mll , ">" , mllBoundary-25 , -1 , -1 , 0 ));
        sr3.cuts.push_back(CutFunc("mll" , &vars_ptr->mll , ">" , mllBoundary-25 , -1 , -1 , 1 ));
        sr3.cuts.push_back(CutFunc("mll" , &vars_ptr->mll , "<" , mllBoundary    , -1 , -1 , 0 ));
        sr3.cuts.push_back(CutFunc("mll" , &vars_ptr->mll , "<" , mllBoundary    , -1 , -1 , 1 ));
        sr3.cuts.push_back(CutFunc("mt"  , &vars_ptr->mt  , "<" , 280            , -1 , -1 , 0 ));
        sr3.cuts.push_back(CutFunc("mt"  , &vars_ptr->mt  , "<" , 280            , -1 , -1 , 1 ));
        sr3.cuts.push_back(CutFunc("mt"  , &vars_ptr->mt  , ">" , 80             , -1 , -1 , 0 ));
        sr3.cuts.push_back(CutFunc("mt"  , &vars_ptr->mt  , ">" , 80             , -1 , -1 , 1 ));

        srs.push_back(sr3);
      }
    }
    else if (mode == 8) { // mode == 8
      for (unsigned int i_mll = 0; i_mll < parsedmllbounds.size()-1; i_mll++) {
        for (unsigned int i_subleadpt = 0; i_subleadpt < parsedsubleadptbounds.size()-1; i_subleadpt++) {

          stringstream srName;
          srName << "signalLike" << parsedmllbounds[i_mll] << parsedmllbounds[i_mll+1] << parsedsubleadptbounds[i_subleadpt] << parsedsubleadptbounds[i_subleadpt+1];

          LOG(logINFO) << "Making region " << srName.str();

          Region sr3(srName.str());
          sr3.channels = channels;
          sr3.cuts = CommonCutsDFSR;

          double mllBoundLo = atof(parsedmllbounds[i_mll].c_str());
          double mllBoundHi = atof(parsedmllbounds[i_mll+1].c_str());
          double subleadptBoundLo = atof(parsedsubleadptbounds[i_subleadpt].c_str());
          double subleadptBoundHi = atof(parsedsubleadptbounds[i_subleadpt+1].c_str());

          sr3.cuts.push_back(CutFunc("mll" , &vars_ptr->mll , ">" , mllBoundLo       , -1 , -1 , 0 ));
          sr3.cuts.push_back(CutFunc("mll" , &vars_ptr->mll , ">" , mllBoundLo       , -1 , -1 , 1 ));
          sr3.cuts.push_back(CutFunc("mll" , &vars_ptr->mll , "<" , mllBoundHi       , -1 , -1 , 0 ));
          sr3.cuts.push_back(CutFunc("mll" , &vars_ptr->mll , "<" , mllBoundHi       , -1 , -1 , 1 ));

          sr3.cuts.push_back(CutFunc("subleadpt" , &vars_ptr->subleadpt , ">" , subleadptBoundLo , -1 , -1 , 0 ));
          sr3.cuts.push_back(CutFunc("subleadpt" , &vars_ptr->subleadpt , ">" , subleadptBoundLo , -1 , -1 , 1 ));
          sr3.cuts.push_back(CutFunc("subleadpt" , &vars_ptr->subleadpt , "<" , subleadptBoundHi , -1 , -1 , 0 ));
          sr3.cuts.push_back(CutFunc("subleadpt" , &vars_ptr->subleadpt , "<" , subleadptBoundHi , -1 , -1 , 1 ));

          srs.push_back(sr3);
        }
      }
    }

    if (mode == 0 || CutMTandRebin)
    {
      double frac = 0.75;
      if (useHighMass)  frac = 0.6;
      if (useHighMass2) frac = 0.5;
      sr.cuts.push_back(CutFunc("mt" , &vars_ptr->mt , ">" , mass*frac , -1 , -1 , 0 ));
      sr.cuts.push_back(CutFunc("mt" , &vars_ptr->mt , "<" , mass      , -1 , -1 , 0 ));
      sr.cuts.push_back(CutFunc("mt" , &vars_ptr->mt , ">" , mass*frac , -1 , -1 , 1 ));
      sr.cuts.push_back(CutFunc("mt" , &vars_ptr->mt , "<" , mass      , -1 , -1 , 1 ));
      sr.cuts.push_back(CutFunc("mt" , &vars_ptr->mt , "<" , 1.2*mass  , -1 , -1 , 2 ));

      if (mode == 6)
      {
        sr2.cuts.push_back(CutFunc("mt" , &vars_ptr->mt , ">" , mass*frac , -1 , -1 , 0 ));
        sr2.cuts.push_back(CutFunc("mt" , &vars_ptr->mt , "<" , mass      , -1 , -1 , 0 ));
        sr2.cuts.push_back(CutFunc("mt" , &vars_ptr->mt , ">" , mass*frac , -1 , -1 , 1 ));
        sr2.cuts.push_back(CutFunc("mt" , &vars_ptr->mt , "<" , mass      , -1 , -1 , 1 ));
        sr2.cuts.push_back(CutFunc("mt" , &vars_ptr->mt , "<" , 1.2*mass  , -1 , -1 , 2 ));  
      }
    }

    // LOWPT SIGNALREGION // FIXME update the region if needed
    Region sr_lo("lowPt");
    sr_lo.channels = channels;

    if (!overrideCuts)
    {
      if (!useHighMass && !useHighMass2)
      {
        CutMll  = 50;
        CutDPhi = 1.8;
      }
      else if (!useHighMass)
      {
        CutDPhi = 10e9;
        CutMll  = 150;
      }
    }

    if (doPreHCP)
    {
      sr_lo.cuts.push_back(CutFunc("run"    , &vars_ptr->run      , "<="  , 210308        , -1 , -1 ,  -1 , "data"));
    }
    if (doPostHCP)
    {
      sr_lo.cuts.push_back(CutFunc("run"    , &vars_ptr->run      , ">"   , 210308        , -1 , -1 ,  -1 , "data"));
    }

    sr_lo.cuts.push_back(CutFunc("dphi"     , &vars_ptr->dphi     , "<"  , CutDPhi ));
    sr_lo.cuts.push_back(CutFunc("islowpt"  , &vars_ptr->islowpt  , "==" , 1 ));
    sr_lo.cuts.push_back(CutFunc("metrel"   , &vars_ptr->metrel   , ">"  , CutMETrelSF0j    ,  2 ,  0 ,  0 ));
    sr_lo.cuts.push_back(CutFunc("metrel"   , &vars_ptr->metrel   , ">"  , CutMETrelSF0j    ,  0 ,  2 ,  0 ));
    sr_lo.cuts.push_back(CutFunc("metrel"   , &vars_ptr->metrel   , ">"  , CutMETrelSF1j    ,  2 ,  0 ,  1 ));
    sr_lo.cuts.push_back(CutFunc("metrel"   , &vars_ptr->metrel   , ">"  , CutMETrelSF1j    ,  0 ,  2 ,  1 ));
    sr_lo.cuts.push_back(CutFunc("metrel"   , &vars_ptr->metrel   , ">"  , 30             ,  1 ,  1 , -1 ));
    sr_lo.cuts.push_back(CutFunc("mll"      , &vars_ptr->mll      , "<"  , CutMll ));
    sr_lo.cuts.push_back(CutFunc("mt"       , &vars_ptr->mt       , "<"  , 200 ));
    sr_lo.cuts.push_back(CutFunc("mt"       , &vars_ptr->mt       , ">"  , 90 ));
    sr_lo.cuts.push_back(CutFunc("mt"       , &vars_ptr->mt       , "<"  , CutMT ));
    sr_lo.cuts.push_back(CutFunc("nbt"      , &vars_ptr->nbt      , "==" , 0 ));
    sr_lo.cuts.push_back(CutFunc("ztautaumass" , &vars_ptr->ztautaumass , "==" , 0 ));
    sr_lo.cuts.push_back(CutFunc("ptll"     , &vars_ptr->ptll     , ">"  , CutPTllSF        ,  2 ,  0 ,  0 ));
    sr_lo.cuts.push_back(CutFunc("ptll"     , &vars_ptr->ptll     , ">"  , CutPTllSF        ,  0 ,  2 ,  0 ));
    sr_lo.cuts.push_back(CutFunc("ptll"     , &vars_ptr->ptll     , ">"  , CutPTllDF     ,  1 ,  1 ,  0 ));
    sr_lo.cuts.push_back(CutFunc("ptlljets"     , &vars_ptr->ptlljets     , ">"  , CutPTlljetsSF     ,  2 ,  0 ,  1 ));
    sr_lo.cuts.push_back(CutFunc("ptlljets"     , &vars_ptr->ptlljets     , ">"  , CutPTlljetsSF     ,  0 ,  2 ,  1 ));
    sr_lo.cuts.push_back(CutFunc("ptlljets"     , &vars_ptr->ptlljets     , ">"  , CutPTlljetsDF     ,  1 ,  1 ,  1 ));
    if (do2011)
    {
      sr_lo.cuts.push_back(CutFunc("ptlljets"     , &vars_ptr->ptlljets     , ">"  , CutPTlljetsSF     ,  2 ,  0 ,  1 ));
      sr_lo.cuts.push_back(CutFunc("ptlljets"     , &vars_ptr->ptlljets     , ">"  , CutPTlljetsSF     ,  0 ,  2 ,  1 ));
      sr_lo.cuts.push_back(CutFunc("ptlljets"     , &vars_ptr->ptlljets     , ">"  , CutPTlljetsDF     ,  1 ,  1 ,  1 ));
    }
    sr_lo.cuts.push_back(CutFunc("pttot"    , &vars_ptr->pttot    , "<"  , CutPTtot       , -1 , -1 ,  1 ));
    sr_lo.cuts.push_back(CutFunc("pttot"    , &vars_ptr->pttot    , "<"  , CutPTtot       , -1 , -1 ,  2 ));
    sr_lo.cuts.push_back(CutFunc("trackmet" , &vars_ptr->trackmet , ">"  , CutTrackMETSF0j    ,  2 ,  0 , 0 ));
    sr_lo.cuts.push_back(CutFunc("trackmet" , &vars_ptr->trackmet , ">"  , CutTrackMETSF0j    ,  0 ,  2 , 0 ));
    sr_lo.cuts.push_back(CutFunc("trackmet" , &vars_ptr->trackmet , ">"  , CutTrackMETSF1j    ,  2 ,  0 , 1 ));
    sr_lo.cuts.push_back(CutFunc("trackmet" , &vars_ptr->trackmet , ">"  , CutTrackMETSF1j    ,  0 ,  2 , 1 ));
    // sr_lo.cuts.push_back(CutFunc("trackmet" , &vars_ptr->trackmet , ">"  , trackmetCut_em ,  1 ,  1 , -1 ));

    if (mode == 0 || CutMTandRebin) // FIXME
    {
      double frac = 0.75;
      if (useHighMass) frac = 0.6;
      if (useHighMass2) frac = 0.5;
      sr_lo.cuts.push_back(CutFunc("mt", &vars_ptr->mt, ">", mass*frac , -1 , -1 , 0 ));
      sr_lo.cuts.push_back(CutFunc("mt", &vars_ptr->mt, "<", mass      , -1 , -1 , 0 )); 
      sr_lo.cuts.push_back(CutFunc("mt", &vars_ptr->mt, ">", mass*frac , -1 , -1 , 1 ));
      sr_lo.cuts.push_back(CutFunc("mt", &vars_ptr->mt, "<", mass      , -1 , -1 , 1 )); 
      sr_lo.cuts.push_back(CutFunc("mt", &vars_ptr->mt, "<", 1.2*mass  , -1 , -1 , 2 ));
    }

    // WW CONTROLREGION
    Region cr("mainControl");
    cr.channels = crChannels;
    cr.isCR = true;
    if (ZMode == 2) cr.isSFCR = false;
    //if (ZMode == 2 && !doPacman2j) cr.isSFCR = false;

    if (!overrideCuts)
    {
      if (!useHighMass && !useHighMass2)
      {
        CutMllCR  = 80;
        CutDPhiCR = 10e9;
      }
      else if (useHighMass)
      {
        CutMllCR  = 150;
        CutDPhiCR = 10e9;
      }
    }

    if (doSpin) // changed cuts only for signalregion (-> Peter)
    {
      CutMETrelDF = 25;
      CutPTllDF   = 30;
      CutDPhi     = 1.8;
      CutMll      = 50;
      CutMT       = 10e9;
      CutMTUp     = 0;
      CutPTtot    = 30;
    }

    cr.cuts = KillEWCutsCommon01j;

    if (!doMVA)
    {
      if (doPreHCP)  cr.cuts.push_back(CutFunc("run"    , &vars_ptr->run      , "<="  , 210308        , -1 , -1 ,  -1 , "data"));
      if (doPostHCP) cr.cuts.push_back(CutFunc("run"    , &vars_ptr->run      , ">"   , 210308        , -1 , -1 ,  -1 , "data"));
      if (bt20GeV)   cr.cuts.push_back(CutFunc("nbt20"     , &vars_ptr->nbt20, "==" , 0,     -1, -1, 0 ));
  
      cr.cuts.push_back(CutFunc("cjv_leadPt"  , &vars_ptr->cjv_leadPt  , "<"  , CutCJVleadPT    , -1 , -1 ,  2 ));
      cr.cuts.push_back(CutFunc("dphi"        , &vars_ptr->dphi        , "<"  , CutDPhiCR       , -1 , -1 ,  0 ));
      cr.cuts.push_back(CutFunc("dphi"        , &vars_ptr->dphi        , "<"  , CutDPhiCR       , -1 , -1 ,  1 ));
      cr.cuts.push_back(CutFunc("dphillmet"   , &vars_ptr->dphillmet   , ">"  , CutDPhillMET    , -1 , -1 ,  0 ));
      cr.cuts.push_back(CutFunc("dYjj"        , &vars_ptr->dYjj        , ">"  , CutDYjj         , -1 , -1 ,  2 ));
      if (doPacman2j) {
        cr.cuts.push_back(CutFunc("metrel"    , &vars_ptr->metrel      , ">"  , CutMETrelSF2j   ,  2 ,  0 ,  2 ));
        cr.cuts.push_back(CutFunc("metrel"    , &vars_ptr->metrel      , ">"  , CutMETrelSF2j   ,  0 ,  2 ,  2 ));
        cr.cuts.push_back(CutFunc("ptlljets"  , &vars_ptr->ptlljets    , ">"  , CutPTlljetsSF2j ,  2 ,  0 ,  2 ));
        cr.cuts.push_back(CutFunc("ptlljets"  , &vars_ptr->ptlljets    , ">"  , CutPTlljetsSF2j ,  0 ,  2 ,  2 ));
      } else {
        cr.cuts.push_back(CutFunc("met"       , &vars_ptr->met         , ">"  , CutMETSF2j      ,  2 ,  0 ,  2 ));
        cr.cuts.push_back(CutFunc("met"       , &vars_ptr->met         , ">"  , CutMETSF2j      ,  0 ,  2 ,  2 ));
      }
      cr.cuts.push_back(CutFunc("met"         , &vars_ptr->met         , ">"  , CutMETDF        ,  1 ,  1 ,  2 ));
      cr.cuts.push_back(CutFunc("metrel"      , &vars_ptr->metrel      , ">"  , CutMETrelSF0j   ,  2 ,  0 ,  0 ));
      cr.cuts.push_back(CutFunc("metrel"      , &vars_ptr->metrel      , ">"  , CutMETrelSF0j   ,  0 ,  2 ,  0 ));
      cr.cuts.push_back(CutFunc("metrel"      , &vars_ptr->metrel      , ">"  , CutMETrelSF1j   ,  2 ,  0 ,  1 ));
      cr.cuts.push_back(CutFunc("metrel"      , &vars_ptr->metrel      , ">"  , CutMETrelSF1j   ,  0 ,  2 ,  1 ));
      cr.cuts.push_back(CutFunc("metrel"      , &vars_ptr->metrel      , ">"  , CutMETrelDF     ,  1 ,  1 ,  0 ));
      cr.cuts.push_back(CutFunc("metrel"      , &vars_ptr->metrel      , ">"  , CutMETrelDF     ,  1 ,  1 ,  1 ));
      cr.cuts.push_back(CutFunc("Mjj"         , &vars_ptr->Mjj         , ">"  , CutMjj          , -1 , -1 ,  2 ));
      cr.cuts.push_back(CutFunc("mll"         , &vars_ptr->mll         , ">"  , CutMll2j        , -1 , -1 ,  2 ));
      cr.cuts.push_back(CutFunc("mllZ"        , &vars_ptr->mllZ        , ">"  , 15              ,  0 ,  2 ,  2 ));
      cr.cuts.push_back(CutFunc("mllZ"        , &vars_ptr->mllZ        , ">"  , 15              ,  2 ,  0 ,  2 ));
      cr.cuts.push_back(CutFunc("nbt"         , &vars_ptr->nbt         , "==" , 0               , -1 , -1 , 0 ));
      cr.cuts.push_back(CutFunc("nbt"         , &vars_ptr->nbt         , "==" , 0               , -1 , -1 , 1 ));
      cr.cuts.push_back(CutFunc("nbt"         , &vars_ptr->nbt         , "==" , 0               , -1 , -1 , 2 ));
      // bc test cr.cuts.push_back(CutFunc("nbt"         , &vars_ptr->nbt         , "==" , 1               , -1 , -1 , 2 ));
      cr.cuts.push_back(CutFunc("ztautaumass" , &vars_ptr->ztautaumass , "==" , 0               , -1 , -1 ,  0 ));
      cr.cuts.push_back(CutFunc("ztautaumass" , &vars_ptr->ztautaumass , "==" , 0               , -1 , -1 ,  1 ));
      cr.cuts.push_back(CutFunc("ztautaumass" , &vars_ptr->ztautaumass , "==" , 0               ,  1 ,  1 ,  2 ));
      cr.cuts.push_back(CutFunc("ptll"        , &vars_ptr->ptll        , ">"  , CutPTllSF       ,  2 ,  0 ,  0 ));
      cr.cuts.push_back(CutFunc("ptll"        , &vars_ptr->ptll        , ">"  , CutPTllSF       ,  0 ,  2 ,  0 ));
      cr.cuts.push_back(CutFunc("ptll"        , &vars_ptr->ptll        , ">"  , CutPTllDF       ,  1 ,  1 ,  0 ));
      cr.cuts.push_back(CutFunc("ptlljets"    , &vars_ptr->ptlljets    , ">"  , CutPTlljetsSF   ,  2 ,  0 ,  1 ));
      cr.cuts.push_back(CutFunc("ptlljets"    , &vars_ptr->ptlljets    , ">"  , CutPTlljetsSF   ,  0 ,  2 ,  1 ));
      cr.cuts.push_back(CutFunc("ptlljets"    , &vars_ptr->ptlljets    , ">"  , CutPTlljetsDF   ,  1 ,  1 ,  1 ));
      cr.cuts.push_back(CutFunc("olv"         , &vars_ptr->olv         , "==" , 1               , -1 , -1 ,  2 ));
      cr.cuts.push_back(CutFunc("pttot"       , &vars_ptr->pttot       , "<"  , CutPTtot2j      , -1 , -1 ,  2 ));
      cr.cuts.push_back(CutFunc("trackmet"    , &vars_ptr->trackmet    , ">"  , CutTrackMETSF0j ,  2 ,  0 ,  0 ));
      cr.cuts.push_back(CutFunc("trackmet"    , &vars_ptr->trackmet    , ">"  , CutTrackMETSF0j ,  0 ,  2 ,  0 ));
      cr.cuts.push_back(CutFunc("trackmet"    , &vars_ptr->trackmet    , ">"  , CutTrackMETSF1j ,  2 ,  0 ,  1 ));
      cr.cuts.push_back(CutFunc("trackmet"    , &vars_ptr->trackmet    , ">"  , CutTrackMETSF1j ,  0 ,  2 ,  1 ));
      cr.cuts.push_back(CutFunc("islowpt"     , &vars_ptr->islowpt     , "==" , 0               , -1 , -1 , -1 ));
      if (useAltCRs) {
        if (!doCRVRcheck) {
          cr.cuts.push_back(CutFunc("mll"     , &vars_ptr->mll         , ">"  , 50              , -1 , -1 ,  0 ));
          cr.cuts.push_back(CutFunc("mll"     , &vars_ptr->mll         , "<"  , 100             , -1 , -1 ,  0 ));
          cr.cuts.push_back(CutFunc("mll"     , &vars_ptr->mll         , ">"  , CutMllCR        , -1 , -1 ,  1 ));
        }
        else {
          cr.cuts.push_back(CutFunc("mll"     , &vars_ptr->mll         , ">"  , 100             , -1 , -1 ,  0 ));
          cr.cuts.push_back(CutFunc("mll"     , &vars_ptr->mll         , ">"  , 100             , -1 , -1 ,  1 )); 
        }
      }
      else {
        cr.cuts.push_back(CutFunc("mll"      , &vars_ptr->mll          , ">"  , CutMllCR        , -1 , -1 ,  0 ));
        cr.cuts.push_back(CutFunc("mll"      , &vars_ptr->mll          , ">"  , CutMllCR        , -1 , -1 ,  1 ));
      }
      if (do2011) {
        cr.cuts.push_back(CutFunc("ptlljets" , &vars_ptr->ptlljets     , ">"  , CutPTlljetsSF   , 2 , 0 ,   1 ));
        cr.cuts.push_back(CutFunc("ptlljets" , &vars_ptr->ptlljets     , ">"  , CutPTlljetsSF   , 0 , 2 ,   1 ));
        cr.cuts.push_back(CutFunc("ptlljets" , &vars_ptr->ptlljets     , ">"  , CutPTlljetsDF   , 1 , 1 ,   1 ));
      }
      else {
        if (doTrackMET2j) {
          cr.cuts.push_back(CutFunc("trackmet" , &vars_ptr->trackmet   , ">"  , CutTrackMETSF2j ,  2 ,  0 ,  2 ));
          cr.cuts.push_back(CutFunc("trackmet" , &vars_ptr->trackmet   , ">"  , CutTrackMETSF2j ,  0 ,  2 ,  2 ));
        } else if (doPacman2j) {
          cr.cuts.push_back(CutFunc("metrelstvf" , &vars_ptr->metrelstvf     , ">"  , CutMETrelstvfSF2j  ,  2 ,  0 ,  2 ));
          cr.cuts.push_back(CutFunc("metrelstvf" , &vars_ptr->metrelstvf     , ">"  , CutMETrelstvfSF2j  ,  0 ,  2 ,  2 ));
        } else {
          cr.cuts.push_back(CutFunc("metstvf" , &vars_ptr->metstvf     , ">"  , CutMETstvfSF2j  ,  2 ,  0 ,  2 ));
          cr.cuts.push_back(CutFunc("metstvf" , &vars_ptr->metstvf     , ">"  , CutMETstvfSF2j  ,  0 ,  2 ,  2 ));
        }
      }
    }
    else
    {
      if (!doSpin)
      {
        cr.cuts.push_back(CutFunc("DeltaEtaJJ" , &vars_ptr->DeltaEtaJJ , ">"  , 3.8    , -1 , -1 ,  2 ));
        cr.cuts.push_back(CutFunc("etaProdJJ"  , &vars_ptr->etaProdJJ  , "<"  , 0      , -1 , -1 ,  2 ));
        cr.cuts.push_back(CutFunc("Mjj"        , &vars_ptr->Mjj        , ">"  , CutMjj , -1 , -1 ,  2 ));
      }
      else
      {
        cr.cuts.push_back(CutFunc("ptll"       , &vars_ptr->ptll       , ">"  , 20     , -1 , -1 , -1 ));
        cr.cuts.push_back(CutFunc("mll"        , &vars_ptr->mll        , ">"  , 80     , -1 , -1 , -1 ));
        cr.cuts.push_back(CutFunc("nbt"        , &vars_ptr->nbt        , "==" , 0      , -1 , -1 , -1 ));
        cr.cuts.push_back(CutFunc("llcharge"   , &vars_ptr->llcharge   , "<"  , 0.     , -1 , -1 , -1 ));
      }
      if (doZjetsSubtraining) cr.cuts.push_back(CutFunc("zjets_mva_weight" , &vars_ptr->zjets_mva_weight , ">" ,  0   , -1 , -1 , 0 ));
      if (doZjetsSubtraining) cr.cuts.push_back(CutFunc("zjets_mva_weight" , &vars_ptr->zjets_mva_weight , ">" , -0.4 , -1 , -1 , 1 ));
    }
//    if (useTopMVA1j) cr.cuts.push_back(CutFunc("BDT_1jet_4vars" , &vars_ptr->BDT_1jet_4vars , ">"  , CutTopMVA1j , -1 , -1 ,  1 ));
  

    if (mode == 6 || (mode==1 && splitmjj)) sr.name = "signalLike1";

    // TOP CONTROLREGION
    Region tb("topbox");
    Region tb2("topbox2");

    if(splitmjjtopCR){
      tb.name = "topbox1";
      tb2.channels = crChannels;
      tb2.isCR = true;
      tb2.isSFCR = true;
    }

    tb.channels = crChannels;
    tb.isCR = true;
    tb.isSFCR = true;
    tb.cuts = KillEWCutsCommon01j;
    tb.cuts.push_back(CutFunc("metrel"      , &vars_ptr->metrel      , ">"  , CutMETrelSF0j   ,  2 ,  0 ,  0 ));
    tb.cuts.push_back(CutFunc("metrel"      , &vars_ptr->metrel      , ">"  , CutMETrelSF0j   ,  0 ,  2 ,  0 ));
    tb.cuts.push_back(CutFunc("metrel"      , &vars_ptr->metrel      , ">"  , CutMETrelDF     ,  1 ,  1 ,  0 ));
    tb.cuts.push_back(CutFunc("metrel"      , &vars_ptr->metrel      , ">"  , CutMETrelSF1j   ,  2 ,  0 ,  1 ));
    tb.cuts.push_back(CutFunc("metrel"      , &vars_ptr->metrel      , ">"  , CutMETrelSF1j   ,  0 ,  2 ,  1 ));
    tb.cuts.push_back(CutFunc("metrel"      , &vars_ptr->metrel      , ">"  , CutMETrelDF     ,  1 ,  1 ,  1 ));
    if (doPacman2j) {
      tb.cuts.push_back(CutFunc("metrel"    , &vars_ptr->metrel      , ">"  , CutMETrelSF2j   ,  2 ,  0 ,  2 ));
      tb.cuts.push_back(CutFunc("metrel"    , &vars_ptr->metrel      , ">"  , CutMETrelSF2j   ,  0 ,  2 ,  2 ));
      tb.cuts.push_back(CutFunc("ptlljets"  , &vars_ptr->ptlljets    , ">"  , CutPTlljetsSF2j ,  2 ,  0 ,  2 ));
      tb.cuts.push_back(CutFunc("ptlljets"  , &vars_ptr->ptlljets    , ">"  , CutPTlljetsSF2j ,  0 ,  2 ,  2 ));
    } else {
      tb.cuts.push_back(CutFunc("met"       , &vars_ptr->met         , ">"  , CutMETSF2j      ,  2 ,  0 ,  2 ));
      tb.cuts.push_back(CutFunc("met"       , &vars_ptr->met         , ">"  , CutMETSF2j      ,  0 ,  2 ,  2 ));
    }
    tb.cuts.push_back(CutFunc("met"         , &vars_ptr->met         , ">"  , CutMETDF        ,  1 ,  1 ,  2 ));
    tb.cuts.push_back(CutFunc("smt"         , &vars_ptr->smt_muon_pt , "<"  , 4   	      , -1 , -1 ,  1 ));
    tb.cuts.push_back(CutFunc("nbt"         , &vars_ptr->nbt         , ">=" , 1               , -1 , -1 ,  0 ));
    tb.cuts.push_back(CutFunc("nbt"         , &vars_ptr->nbt         , ">=" , 1               , -1 , -1 ,  1 ));
    tb.cuts.push_back(CutFunc("pttot"       , &vars_ptr->pttot       , "<"  , CutPTtot2j      , -1 , -1 ,  2 ));
    tb.cuts.push_back(CutFunc("ptll"        , &vars_ptr->ptll        , ">"  , CutPTllDF       ,  1 ,  1 ,  0 ));
    tb.cuts.push_back(CutFunc("olv"         , &vars_ptr->olv         , "==" , 1               , -1 , -1 ,  2 ));
    tb.cuts.push_back(CutFunc("dYjj"        , &vars_ptr->dYjj        , ">"  , CutDYjj         , -1 , -1 ,  2 ));
    tb.cuts.push_back(CutFunc("Mjj"         , &vars_ptr->Mjj         , ">"  , CutMjj          , -1 , -1 ,  2 ));
    tb.cuts.push_back(CutFunc("cjv_leadPt"  , &vars_ptr->cjv_leadPt  , "<"  , CutCJVleadPT    , -1 , -1 ,  2 ));
    tb.cuts.push_back(CutFunc("trackmet"    , &vars_ptr->trackmet    , ">"  , CutTrackMETSF0j ,  2 ,  0 ,  0 ));
    tb.cuts.push_back(CutFunc("trackmet"    , &vars_ptr->trackmet    , ">"  , CutTrackMETSF0j ,  0 ,  2 ,  0 ));
    tb.cuts.push_back(CutFunc("trackmet"    , &vars_ptr->trackmet    , ">"  , CutTrackMETSF1j ,  2 ,  0 ,  1 ));
    tb.cuts.push_back(CutFunc("trackmet"    , &vars_ptr->trackmet    , ">"  , CutTrackMETSF1j ,  0 ,  2 ,  1 ));
    tb.cuts.push_back(CutFunc("mllZ"        , &vars_ptr->mllZ        , ">"  , 15              ,  0 , 2 ,  -1 ));
    tb.cuts.push_back(CutFunc("mllZ"        , &vars_ptr->mllZ        , ">"  , 15              ,  2 , 0 ,  -1 ));
    tb.cuts.push_back(CutFunc("ztautaumass" , &vars_ptr->ztautaumass , "==" , 0               ,  1 ,  1 ,  2 ));
    if (!doMVA)    tb.cuts.push_back(CutFunc("islowpt" , &vars_ptr->islowpt , "==" , 0        , -1 , -1 , -1 ));
    if (doPreHCP)  tb.cuts.push_back(CutFunc("run"     , &vars_ptr->run     , "<=" , 210308   , -1 , -1 , -1 , "data"));
    if (doPostHCP) tb.cuts.push_back(CutFunc("run"     , &vars_ptr->run     , ">"  , 210308   , -1 , -1 , -1 , "data"));
    if (doOneBTag) tb.cuts.push_back(CutFunc("nbt"     , &vars_ptr->nbt     , "==" , 1        , -1 , -1 ,  2 ));
    else           tb.cuts.push_back(CutFunc("nbt"     , &vars_ptr->nbt     , ">=" , 1        , -1 , -1 ,  2 ));
    if (ZMode == 2)
    {
      tb.cuts.push_back(CutFunc("ztautaumass" , &vars_ptr->ztautaumass , "==" , 0 ,  1 , 1 , -1 ));
    }
    else
    {
      tb.cuts.push_back(CutFunc("ztautaumass" , &vars_ptr->ztautaumass , "==" , 0        , -1 , -1 ,  0 ));
      tb.cuts.push_back(CutFunc("ztautaumass" , &vars_ptr->ztautaumass , "==" , 0        , -1 , -1 ,  1 ));
      tb.cuts.push_back(CutFunc("pttot"       , &vars_ptr->pttot       , "<"  , CutPTtot , -1 , -1 ,  1 ));
    }
    if (!do2011)
    {
      if(doTrackMET2j)
      {
        tb.cuts.push_back(CutFunc("trackmet" , &vars_ptr->trackmet , ">" , CutTrackMETSF2j , 2 , 0 , 2 ));
        tb.cuts.push_back(CutFunc("trackmet" , &vars_ptr->trackmet , ">" , CutTrackMETSF2j , 0 , 2 , 2 ));
      }
      else if (doPacman2j)
      {
        tb.cuts.push_back(CutFunc("metrelstvf" , &vars_ptr->metstvf , ">" , CutMETrelstvfSF2j , 2 , 0 , 2 ));
        tb.cuts.push_back(CutFunc("metrelstvf" , &vars_ptr->metstvf , ">" , CutMETrelstvfSF2j , 0 , 2 , 2 ));
      }
      else
      {
        tb.cuts.push_back(CutFunc("metstvf" , &vars_ptr->metstvf , ">" , CutMETstvfSF2j , 2 , 0 , 2 ));
        tb.cuts.push_back(CutFunc("metstvf" , &vars_ptr->metstvf , ">" , CutMETstvfSF2j , 0 , 2 , 2 ));
      }
    }
    if (doMVA)
    {
      if (!doVBF2j && !doSpin)
      {
        tb.cuts.push_back(CutFunc("DeltaEtaJJ" , &vars_ptr->DeltaEtaJJ , ">"  , 3.8   , -1 , -1 , 2 ));
        tb.cuts.push_back(CutFunc("etaProdJJ"  , &vars_ptr->etaProdJJ  , "<"  , 0     , -1 , -1 , 2 ));
        tb.cuts.push_back(CutFunc("Mjj"        , &vars_ptr->Mjj        , ">"  , CutMjj , -1 , -1 , 2 ));
      }
      if (doZjetsSubtraining) tb.cuts.push_back(CutFunc("zjets_mva_weight" , &vars_ptr->zjets_mva_weight, ">",  0   , -1 , -1 , 0 ));
      if (doZjetsSubtraining) tb.cuts.push_back(CutFunc("zjets_mva_weight" , &vars_ptr->zjets_mva_weight, ">", -0.4 , -1 , -1 , 1 ));
    }
//    if (useTopMVA1j) tb.cuts.push_back(CutFunc("BDT_1jet_4vars" , &vars_ptr->BDT_1jet_4vars , ">"  , CutTopMVA1j , -1 , -1 ,  1 ));

    if(splitmjjtopCR){
      tb2.cuts = tb.cuts;
      tb2.name = "topbox2";
      tb.cuts.push_back(CutFunc("Mjj"         , &vars_ptr->Mjj         , "<"  , 1000          , -1 , -1 ,  2 ));      
      tb2.cuts.push_back(CutFunc("Mjj"         , &vars_ptr->Mjj        , ">"  , 1000          , -1 , -1 ,  2 ));      

    }


    // Z CONTROL REGION FOR SPIN
    Region zb("zbox");
    zb.channels = crChannels;
    zb.cuts.push_back(CutFunc("mll"      , &vars_ptr->mll      , "<"  , 80  , -1 , -1 , -1 ));
    zb.cuts.push_back(CutFunc("dphi"     , &vars_ptr->dphi     , ">"  , 2.8 , -1 , -1 , -1 ));
    zb.cuts.push_back(CutFunc("nbt"      , &vars_ptr->nbt      , "==" , 0   , -1 , -1 , -1 ));
    if (doPreHCP) zb.cuts.push_back(CutFunc("run"    , &vars_ptr->run      , "<="  , 210308        , -1 , -1 ,  -1 , "data"));
    zb.cuts.push_back(CutFunc("llcharge" , &vars_ptr->llcharge , "<"  , 0.  , -1 , -1 , -1 ));
    if (!doMVA) zb.cuts.push_back(CutFunc("islowpt" , &vars_ptr->islowpt , "==" , 0 ));
    
    // PACMAN STUFF STARTS HERE
    // Common cuts for Pacman WW
    vector<CutFunc> PacmanCutsCommonWW;
    PacmanCutsCommonWW = KillEWCutsCommon01j;
    PacmanCutsCommonWW.push_back(CutFunc("dphillmet" , &vars_ptr->dphillmet , ">"  , CutDPhillMET    , -1 , -1 ,  0 ));
    PacmanCutsCommonWW.push_back(CutFunc("metrel"    , &vars_ptr->metrel    , ">"  , CutMETrelSF0j   , -1 , -1 ,  0 ));
    PacmanCutsCommonWW.push_back(CutFunc("metrel"    , &vars_ptr->metrel    , ">"  , CutMETrelSF1j   , -1 , -1 ,  1 ));
    PacmanCutsCommonWW.push_back(CutFunc("metrel"    , &vars_ptr->metrel    , ">"  , CutMETrelSF2j   , -1 , -1 ,  2 ));
    PacmanCutsCommonWW.push_back(CutFunc("metrelstvf", &vars_ptr->metrelstvf , ">"  , CutMETrelstvfSF2j  , -1 , -1 ,  2 ));
    PacmanCutsCommonWW.push_back(CutFunc("mll"       , &vars_ptr->mll       , ">"  , CutMllZBhi      , -1 , -1 , -1 ));
    PacmanCutsCommonWW.push_back(CutFunc("nbt"       , &vars_ptr->nbt       , "==" , 0               , -1 , -1 , -1 ));
    PacmanCutsCommonWW.push_back(CutFunc("ptll"      , &vars_ptr->ptll      , ">"  , CutPTllSF       , -1 , -1 ,  0 ));
    PacmanCutsCommonWW.push_back(CutFunc("ptlljets"  , &vars_ptr->ptlljets  , ">"  , CutPTlljetsSF   , -1 , -1 ,  1 ));
    PacmanCutsCommonWW.push_back(CutFunc("trackmet"  , &vars_ptr->trackmet  , ">"  , CutTrackMETSF0j , -1 , -1 ,  0 ));
    PacmanCutsCommonWW.push_back(CutFunc("trackmet"  , &vars_ptr->trackmet  , ">"  , CutTrackMETSF1j , -1 , -1 ,  1 ));
    PacmanCutsCommonWW.push_back(CutFunc("islowpt"   , &vars_ptr->islowpt   , "==" , 0               , -1 , -1 , -1 ));
    PacmanCutsCommonWW.push_back(CutFunc("ptlljets"  , &vars_ptr->ptlljets  , ">"  , CutPTlljetsSF2j , -1 , -1 , 2 ));
    PacmanCutsCommonWW.push_back(CutFunc("Mjj"       , &vars_ptr->Mjj       , ">"  , CutMjj          , -1 , -1 , 2 ));
    PacmanCutsCommonWW.push_back(CutFunc("pttot"     , &vars_ptr->pttot     , "<"  , CutPTtot2j      , -1 , -1 , 2 ));
    if (doPreHCP)  PacmanCutsCommonWW.push_back(CutFunc("run" , &vars_ptr->run , "<=" , 210308 , -1 , -1 , -1 , "data"));
    if (doPostHCP) PacmanCutsCommonWW.push_back(CutFunc("run" , &vars_ptr->run , ">"  , 210308 , -1 , -1 , -1 , "data"));

    // HIGH MLL CONTROLREGION WITH RECOIL FAIL FOR PACMAN
    Region E("EWWCR");
    E.channels = crChannels;
    E.isCR = true;
    E.isSFCR = true;
    E.cuts = PacmanCutsCommonWW;
    E.cuts.push_back(CutFunc("frecoil" , &vars_ptr->frecoil , ">=" , CutFRecoil0j , -1 , -1 ,  0 , "data" ));
    E.cuts.push_back(CutFunc("frecoil" , &vars_ptr->frecoil , ">=" , CutFRecoil1j , -1 , -1 ,  1 , "data" ));
    E.cuts.push_back(CutFunc("frecoil" , &vars_ptr->frecoil , ">=" , CutFRecoil2j , -1 , -1 ,  2 , "data" ));

    // HIGH MLL CONTROLREGION WITH RECOIL PASS FOR PACMAN
    Region E_frec("EfrecWWCR");
    E_frec.channels = crChannels;
    E_frec.isCR = true;
    E_frec.isSFCR = true;
    E_frec.cuts = PacmanCutsCommonWW;
    E_frec.cuts.push_back(CutFunc("frecoil" , &vars_ptr->frecoil , "<" , CutFRecoil0j , -1 , -1 ,  0 , "data" ));
    E_frec.cuts.push_back(CutFunc("frecoil" , &vars_ptr->frecoil , "<" , CutFRecoil1j , -1 , -1 ,  1 , "data" ));
    E_frec.cuts.push_back(CutFunc("frecoil" , &vars_ptr->frecoil , "<" , CutFRecoil2j , -1 , -1 ,  2 , "data" ));

    // Common cuts for Pacman ZPeak
    vector<CutFunc> PacmanCutsCommonZP;
    PacmanCutsCommonZP = KillEWCutsCommon01j;
    PacmanCutsCommonZP.push_back(CutFunc("dphillmet", &vars_ptr->dphillmet , ">"  , CutDPhillMET    , -1 , -1 ,  0 ));
    PacmanCutsCommonZP.push_back(CutFunc("metrel"   , &vars_ptr->metrel    , ">"  , CutMETrelSF0j   , -1 , -1 ,  0 ));
    PacmanCutsCommonZP.push_back(CutFunc("metrel"   , &vars_ptr->metrel    , ">"  , CutMETrelSF1j   , -1 , -1 ,  1 ));
    PacmanCutsCommonZP.push_back(CutFunc("metrel"   , &vars_ptr->metrel    , ">"  , CutMETrelSF2j   , -1 , -1 ,  2 ));
    PacmanCutsCommonZP.push_back(CutFunc("metrelstvf" , &vars_ptr->metrelstvf   , ">"  , CutMETrelstvfSF2j  , -1 , -1 ,  2 ));
    PacmanCutsCommonZP.push_back(CutFunc("mll"      , &vars_ptr->mll       , ">"  , CutMllZBlo      , -1 , -1 , -1 ));
    PacmanCutsCommonZP.push_back(CutFunc("mll"      , &vars_ptr->mll       , "<"  , CutMllZBhi      , -1 , -1 , -1 ));
    PacmanCutsCommonZP.push_back(CutFunc("nbt"      , &vars_ptr->nbt       , "==" , 0               , -1 , -1 , -1 ));
    PacmanCutsCommonZP.push_back(CutFunc("ptll"     , &vars_ptr->ptll      , ">"  , CutPTllSF       , -1 , -1 ,  0 ));
    PacmanCutsCommonZP.push_back(CutFunc("ptlljets" , &vars_ptr->ptlljets  , ">"  , CutPTlljetsSF   , -1 , -1 ,  1 ));
    PacmanCutsCommonZP.push_back(CutFunc("trackmet" , &vars_ptr->trackmet  , ">"  , CutTrackMETSF0j , -1 , -1 ,  0 ));
    PacmanCutsCommonZP.push_back(CutFunc("trackmet" , &vars_ptr->trackmet  , ">"  , CutTrackMETSF1j , -1 , -1 ,  1 ));
    PacmanCutsCommonZP.push_back(CutFunc("islowpt"  , &vars_ptr->islowpt   ,"=="  , 0               , -1 , -1 , -1 ));
    PacmanCutsCommonZP.push_back(CutFunc("ptlljets" , &vars_ptr->ptlljets  , ">"  , CutPTlljetsSF2j , -1 , -1 ,  2 ));
    PacmanCutsCommonZP.push_back(CutFunc("Mjj"      , &vars_ptr->Mjj       , ">"  , CutMjj          , -1 , -1 ,  2 ));
    PacmanCutsCommonZP.push_back(CutFunc("pttot"    , &vars_ptr->pttot     , "<"  , CutPTtot2j      , -1 , -1 ,  2 ));
    if (doPreHCP)  PacmanCutsCommonZP.push_back(CutFunc("run" , &vars_ptr->run , "<="  , 210308 , -1 , -1 , -1 , "data"));
    if (doPostHCP) PacmanCutsCommonZP.push_back(CutFunc("run" , &vars_ptr->run , ">"   , 210308 , -1 , -1 , -1 , "data"));

    // Z WINDOW WITH RECOIL FAIL FOR PACMAN
    Region C("CZpeak");
    C.channels = crChannels;
    C.isCR = true;
    C.isSFCR = true;
    C.cuts = PacmanCutsCommonZP;
    C.cuts.push_back(CutFunc("frecoil" , &vars_ptr->frecoil , ">=" , CutFRecoil0j , -1 , -1 , 0 , "data" ));
    C.cuts.push_back(CutFunc("frecoil" , &vars_ptr->frecoil , ">=" , CutFRecoil1j , -1 , -1 , 1 , "data" ));
    C.cuts.push_back(CutFunc("frecoil" , &vars_ptr->frecoil , ">=" , CutFRecoil2j , -1 , -1 , 2 , "data" ));

    // Z WINDOW WITH RECOIL FAIL FOR PACMAN
    Region C_frec("CfrecZpeak");
    C_frec.channels = crChannels;
    C_frec.isCR = true;
    C_frec.isSFCR = true;
    C_frec.cuts = PacmanCutsCommonZP;
    C_frec.cuts.push_back(CutFunc("frecoil" , &vars_ptr->frecoil , "<" , CutFRecoil0j , -1 , -1 , 0 , "data" ));
    C_frec.cuts.push_back(CutFunc("frecoil" , &vars_ptr->frecoil , "<" , CutFRecoil1j , -1 , -1 , 1 , "data" ));
    C_frec.cuts.push_back(CutFunc("frecoil" , &vars_ptr->frecoil , "<" , CutFRecoil2j , -1 , -1 , 2 , "data" ));

    // Common cuts for Pacman SR
    if (!overrideCuts)
    {
      if (!useHighMass && !useHighMass2)
      {
        CutMll    = 50;
        CutMll2j  = 80;
        CutDPhi   = 1.8;
        CutDPhi2j = 1.8;
      }
      else if (useHighMass)
      {
        CutDPhi   = 10e9;
        CutMll    = 150;
        CutDPhi2j = 10e9;
        CutMll2j  = 150;
      }
      else
      {
        CutDPhi   = 10e9;
        CutMll    = 10e9;
        CutDPhi2j = 10e9;
        CutMll2j  = 10e9;
      }
    }
    
    vector<CutFunc> PacmanCutsCommonSR;
    PacmanCutsCommonSR = KillEWCutsCommon01j;
    PacmanCutsCommonSR.push_back(CutFunc("dphi"      , &vars_ptr->dphi      , "<"  , CutDPhi         , -1 , -1 ,  0 ));
    PacmanCutsCommonSR.push_back(CutFunc("dphi"      , &vars_ptr->dphi      , "<"  , CutDPhi         , -1 , -1 ,  1 ));
    PacmanCutsCommonSR.push_back(CutFunc("dphi"      , &vars_ptr->dphi      , "<"  , CutDPhi2j       , -1 , -1 ,  2 ));
    PacmanCutsCommonSR.push_back(CutFunc("dphillmet" , &vars_ptr->dphillmet , ">"  , CutDPhillMET    , -1 , -1 ,  0 ));
    PacmanCutsCommonSR.push_back(CutFunc("islowpt"   , &vars_ptr->islowpt   , "==" , 0               , -1 , -1 , -1 ));
    PacmanCutsCommonSR.push_back(CutFunc("metrel"    , &vars_ptr->metrel    , ">"  , CutMETrelSF0j   , -1 , -1 ,  0 ));
    PacmanCutsCommonSR.push_back(CutFunc("metrel"    , &vars_ptr->metrel    , ">"  , CutMETrelSF1j   , -1 , -1 ,  1 ));
    PacmanCutsCommonSR.push_back(CutFunc("metrel"    , &vars_ptr->metrel    , ">"  , CutMETrelSF2j   , -1 , -1 ,  2 ));
    PacmanCutsCommonSR.push_back(CutFunc("metrelstvf"   , &vars_ptr->metrelstvf   , ">"  , CutMETrelstvfSF2j  , -1 , -1 ,  2 ));
    PacmanCutsCommonSR.push_back(CutFunc("nbt"       , &vars_ptr->nbt       , "==" , 0               , -1 , -1 , -1 ));
    PacmanCutsCommonSR.push_back(CutFunc("ptll"      , &vars_ptr->ptll      , ">"  , CutPTllSF       , -1 , -1 ,  0 ));
    PacmanCutsCommonSR.push_back(CutFunc("ptlljets"  , &vars_ptr->ptlljets  , ">"  , CutPTlljetsSF   , -1 , -1 ,  1 ));
    PacmanCutsCommonSR.push_back(CutFunc("trackmet"  , &vars_ptr->trackmet  , ">"  , CutTrackMETSF0j , -1 , -1 ,  0 ));
    PacmanCutsCommonSR.push_back(CutFunc("trackmet"  , &vars_ptr->trackmet  , ">"  , CutTrackMETSF1j , -1 , -1 ,  1 ));
    PacmanCutsCommonSR.push_back(CutFunc("ptlljets" , &vars_ptr->ptlljets   , ">"  , CutPTlljetsSF2j , -1 , -1 ,  2 ));
    PacmanCutsCommonSR.push_back(CutFunc("Mjj"      , &vars_ptr->Mjj        , ">"  , CutMjj          , -1 , -1 ,  2 ));
    PacmanCutsCommonSR.push_back(CutFunc("pttot"    , &vars_ptr->pttot      , "<"  , CutPTtot2j      , -1 , -1 ,  2 ));
    if (!useHighMass && !useHighMass2) PacmanCutsCommonSR.push_back(CutFunc("mll"       , &vars_ptr->mll       , "<"  , 50     , -1 , -1 , -1 ));
    if ( useHighMass && !useHighMass2) PacmanCutsCommonSR.push_back(CutFunc("mll"       , &vars_ptr->mll       , "<"  , 150    , -1 , -1 , -1 ));
    if ( useHighMass ||  useHighMass2) PacmanCutsCommonSR.push_back(CutFunc("mllZ"      , &vars_ptr->mllZ      , ">"  , 15     , -1 , -1 , -1 ));
    if (doblindsignalregion)           PacmanCutsCommonSR.push_back(CutFunc("isBlinded" , &vars_ptr->isBlinded , "==" , 0      , -1 , -1 , -1 , "data" ));
    if (doPreHCP)                      PacmanCutsCommonSR.push_back(CutFunc("run"       , &vars_ptr->run       , "<=" , 210308 , -1 , -1 , -1 , "data"));
    if (doPostHCP)                     PacmanCutsCommonSR.push_back(CutFunc("run"       , &vars_ptr->run       , ">"  , 210308 , -1 , -1 , -1 , "data"));
    if (mode == 0 || CutMTandRebin)
    {
      double frac = 0.75;
      if (useHighMass)  frac = 0.6;
      if (useHighMass2) frac = 0.5;
      PacmanCutsCommonSR.push_back(CutFunc("mt" , &vars_ptr->mt , ">" , mass*frac , -1 , -1 , 0 ));
      PacmanCutsCommonSR.push_back(CutFunc("mt" , &vars_ptr->mt , "<" , mass      , -1 , -1 , 0 ));
      PacmanCutsCommonSR.push_back(CutFunc("mt" , &vars_ptr->mt , ">" , mass*frac , -1 , -1 , 1 ));
      PacmanCutsCommonSR.push_back(CutFunc("mt" , &vars_ptr->mt , "<" , mass      , -1 , -1 , 1 ));
      PacmanCutsCommonSR.push_back(CutFunc("mt" , &vars_ptr->mt , "<" , 1.2*mass  , -1 , -1 , 2 ));
    }

    // SIGNALREGION WITH RECOIL FAIL FOR PACMAN
    Region A("ASR");
    if (!mergeSFSR)
    {
      A.channels = channels;
      A.isCR = false;
      A.isSFCR = false;
    }
    else
    {
      A.channels = crChannels;
      A.isCR = true;
      A.isSFCR = true;
    }

    A.cuts = PacmanCutsCommonSR;
    A.cuts.push_back(CutFunc("frecoil" , &vars_ptr->frecoil , ">=" , CutFRecoil0j , -1 , -1 , 0 , "data" ));
    A.cuts.push_back(CutFunc("frecoil" , &vars_ptr->frecoil , ">=" , CutFRecoil1j , -1 , -1 , 1 , "data" ));
    A.cuts.push_back(CutFunc("frecoil" , &vars_ptr->frecoil , ">=" , CutFRecoil2j , -1 , -1 , 2 , "data" ));

    // SIGNALREGION WITH RECOIL PASS FOR PACMAN
    Region A_frec("AfrecSR");
    if (!mergeSFSR) 
    {
        A_frec.channels = channels;
        A_frec.isCR = false;
        A_frec.isSFCR = false;
    }
    else 
    {
      A_frec.channels = crChannels;
      A_frec.isCR = true;
      A_frec.isSFCR = true;
    }
    A_frec.cuts = PacmanCutsCommonSR;
    A_frec.cuts.push_back(CutFunc("frecoil" , &vars_ptr->frecoil , "<" , CutFRecoil0j , -1 , -1 , 0 , "data" ));
    A_frec.cuts.push_back(CutFunc("frecoil" , &vars_ptr->frecoil , "<" , CutFRecoil1j , -1 , -1 , 1 , "data" ));
    A_frec.cuts.push_back(CutFunc("frecoil" , &vars_ptr->frecoil , "<" , CutFRecoil2j , -1 , -1 , 2 , "data" ));

    // Same Sign CR
    if (doSameSignCR)
    {
	myRegions.push_back(sscr);
    }


    // DECIDE WHICH REGIONS TO CONSIDER
    // SR
    if (dosignalregion)
    {
      if (useHighPt)
      {
        if (mode < 6 && !(mode == 1 && splitmjj)) myRegions.push_back(sr);
        else if (mode == 6 || (mode == 1 && splitmjj)) 
        {
          myRegions.push_back(sr);
          myRegions.push_back(sr2);
          if (doSpin) myRegions.push_back(sr3);
        }
        else if (mode == 7 || mode == 8)
        {
          int nrSrs = srs.size();
          for (int i=0;i<nrSrs;i++)
          {
            myRegions.push_back(srs[i]);
          }
        }
      }
      if (useLowPt)   myRegions.push_back(sr_lo);
      if ( (ZMode == 2 || doPacman2j ) && (doee || domm)) myRegions.push_back(A);
      if ( (ZMode == 2 || doPacman2j ) && (doee || domm)) myRegions.push_back(A_frec);

    }
    // WW CR
    if (!doMVA)
    {
      if (!(overrideCuts && massMode == 3) && !useHighMass2 && mode != 7) myRegions.push_back(cr);
    }
    else if (doMVA && doWWCR_MVA)
    {
      myRegions.push_back(cr);
    }

    // TOP
    if(!doSpin) myRegions.push_back(tb);

    if(splitmjjtopCR){
      myRegions.push_back(tb2);
    }
   
    // Z CR
    if (doSpin && doMVA && doZCR_MVA)
    {
      myRegions.push_back(zb);
    }

    // PACMAN CR
    if ((ZMode == 2 || doPacman2j) && (doee || domm))
    {
      myRegions.push_back(C);
      myRegions.push_back(C_frec);
      if (!(overrideCuts && massMode == 3) && !useHighMass2) myRegions.push_back(E);
      if (!(overrideCuts && massMode == 3) && !useHighMass2) myRegions.push_back(E_frec);
    }
  }
  else if (mode == 2) // FIXME should PACMAN go here as well? // no. remove this! it's stupid and not useful.
  {
    const int nrMll     = 5;
    const int nrDphi    = 4;
    double mll[nrMll]   = {0, 50, 100, 150, 10e9};
    double dphi[nrDphi] = {0, 1.3, 1.8, 4};

    for (int im = 0; im < nrMll-1; im++)
    {
      for (int id = 0; id < nrDphi-1; id++)
      {
        stringstream regionName;
        regionName << "region" << im << id;

        Region r(regionName.str());
        r.channels = channels;
        r.cuts.push_back(CutFunc("mll", &vars_ptr->mll, ">", mll[im]));
        r.cuts.push_back(CutFunc("mll", &vars_ptr->mll, "<", mll[im+1]));
        r.cuts.push_back(CutFunc("dphi", &vars_ptr->dphi,">", dphi[id]));
        r.cuts.push_back(CutFunc("dphi", &vars_ptr->dphi,"<", dphi[id+1]));
        r.cuts.push_back(CutFunc("nbt", &vars_ptr->nbt, "<", 0));
        r.channels = channels;

        myRegions.push_back(r);
      }
    }
    Region tb("topbox");
    tb.channels = channels;
    tb.cuts.push_back(CutFunc("nbt", &vars_ptr->nbt,">=",1));

    myRegions.push_back(tb);
  }
  regions = &myRegions;

  // Prepare systematic specifications (basically folder names)
  set<string> _sampleNames;
  for (int i = 0; i < (int)samples->size(); i++)
  {
    string sampleName = (*samples)[i].name;
    if (sampleName == "wjets" || sampleName == "data" || sampleName == "ss" || sampleName == "wjetsminusss") continue; // Nina
    _sampleNames.insert(sampleName);
  }

  set<string> _allSampleNames=_sampleNames;
  if (!doOSmSS) {
    _allSampleNames.insert("wjets");
  } else {
    _allSampleNames.insert("wjetsminusss"); // Nina
    _allSampleNames.insert("ss"); // Nina
  }
  _allSampleNames.insert("data");

  static vector<Sys> _fileNames; // pair is <up, down>
  if (_fileNames.size()) _fileNames.clear();
  // Add Nominal
  _fileNames.push_back(Sys("Nominal" , "Normal" , "Normal" , _allSampleNames));

  // DETECTOR SYSTEMATICS
  if (useDetSys)
  {
    if (doSlopeTest)
    {
      if (!splitww)
      {
        _fileNames.push_back(Sys("ATLAS_SLOPE"               , "SlopeUp"                  , "SlopeUp"                  , 1 , 0 , "ww"));
      }
      else
      {
        _fileNames.push_back(Sys("ATLAS_SLOPE"               , "SlopeUp"                  , "SlopeUp"                  , 1 , 0 , "ggww"));
        _fileNames.push_back(Sys("ATLAS_SLOPE"               , "SlopeUp"                  , "SlopeUp"                  , 1 , 0 , "qqww"));
      }
    }
    if (useOldBtag)
    {
      _fileNames.push_back(Sys("ATLAS_BTag_BEFF"             , "BtagUp"                  , "BtagDown"                  , 1 , 1 , _sampleNames));
    }
    else
    {
      _fileNames.push_back(Sys("ATLAS_BTag_B1EFF"            , "BtagUp1"                 , "BtagDown1"                 , 1 , 1 , _sampleNames));
      _fileNames.push_back(Sys("ATLAS_BTag_B2EFF"            , "BtagUp2"                 , "BtagDown2"                 , 1 , 1 , _sampleNames));
      _fileNames.push_back(Sys("ATLAS_BTag_B3EFF"            , "BtagUp3"                 , "BtagDown3"                 , 1 , 1 , _sampleNames));
      _fileNames.push_back(Sys("ATLAS_BTag_B4EFF"            , "BtagUp4"                 , "BtagDown4"                 , 1 , 1 , _sampleNames));
      _fileNames.push_back(Sys("ATLAS_BTag_B5EFF"            , "BtagUp5"                 , "BtagDown5"                 , 1 , 1 , _sampleNames));
      _fileNames.push_back(Sys("ATLAS_BTag_B6EFF"            , "BtagUp6"                 , "BtagDown6"                 , 1 , 1 , _sampleNames));
      _fileNames.push_back(Sys("ATLAS_BTag_B7EFF"            , "BtagUp7"                 , "BtagDown7"                 , 1 , 1 , _sampleNames));
      _fileNames.push_back(Sys("ATLAS_BTag_B8EFF"            , "BtagUp8"                 , "BtagDown8"                 , 1 , 1 , _sampleNames));
      _fileNames.push_back(Sys("ATLAS_BTag_B9EFF"            , "BtagUp9"                 , "BtagDown9"                 , 1 , 1 , _sampleNames));
    }
    _fileNames.push_back(Sys("ATLAS_BTag_CEFF"             , "CtagUp"                  , "CtagDown"                  , 1 , 1 , _sampleNames));
    _fileNames.push_back(Sys("ATLAS_BTag_LEFF"             , "MtagUp"                  , "MtagDown"                  , 1 , 1 , _sampleNames));
    _fileNames.push_back(Sys("ATLAS_EL_EFF"                , "ElecEffUp"               , "ElecEffDown"               , 1 , 1 , _sampleNames));
    _fileNames.push_back(Sys("ATLAS_EL_RES"                , "ElecResolutionUp"        , "ElecResolutionDown"        , 0 , 1 , _sampleNames));
    _fileNames.push_back(Sys("ATLAS_EL_ESCALE"             , "ElecScaleUp"             , "ElecScaleDown"             , 0 , 1 , _sampleNames));
    _fileNames.push_back(Sys("ATLAS_MU_EFF"                , "MuonEffUp"               , "MuonEffDown"               , 1 , 1 , _sampleNames));
    _fileNames.push_back(Sys("ATLAS_MU_ID_RES"             , "IDUP"                    , "IDLOW"                     , 0 , 1 , _sampleNames));
    _fileNames.push_back(Sys("ATLAS_MU_MS_RES"             , "MSUP"                    , "MSLOW"                     , 0 , 1 , _sampleNames));
    if (!do2011) {
      _fileNames.push_back(Sys("ATLAS_MU_ESCALE"             , "MUONSCALEUP"             , "MUONSCALELOW"              , 0 , 1 , _sampleNames));
    }
    else {
      _fileNames.push_back(Sys("ATLAS_MU_ESCALE"             , "MuonScale"               , "MuonScale"                 , 0 , 1 , _sampleNames));
    }
    _fileNames.push_back(Sys("ATLAS_MET_RESOSOFT"          , "ResoSoftTermsUp_ptHard"  , "ResoSoftTermsDown_ptHard"  , 0 , 1 , _sampleNames));
    _fileNames.push_back(Sys("ATLAS_MET_SCALESOFT"         , "ScaleSoftTermsUp_ptHard" , "ScaleSoftTermsDown_ptHard" , 0 , 1 , _sampleNames));
    _fileNames.push_back(Sys("ATLAS_ISO"                   , "IsoUp"                   , "IsoDown"                   , 1 , 1 , _sampleNames));
    _fileNames.push_back(Sys("ATLAS_TRIGGER_HWW"           , "TriggerUp"               , "TriggerDown"               , 1 , 1 , _sampleNames));
    _fileNames.push_back(Sys("ATLAS_JER"                   , "JERUp"                   , "JERUp"                     , 0 , 1 , _sampleNames));
    if (!splitjes) {
      _fileNames.push_back(Sys("ATLAS_JES"                 , "JESUp"                   , "JESDown"                   , 0 , 1 , _sampleNames));
    }
    else {
      if (do2011) {
        _fileNames.push_back(Sys("ATLAS_JES_2011_Statistical1"  , "NP_Statistical1JESUp"   , "NP_Statistical1JESDown"     , 0 , 1 , _sampleNames));
        _fileNames.push_back(Sys("ATLAS_JES_2011_Modelling1"    , "NP_Modelling1JESUp"     , "NP_Modelling1JESDown"      , 0 , 1 , _sampleNames));
        _fileNames.push_back(Sys("ATLAS_JES_2011_Detector1"     , "NP_Detector1JESUp"      , "NP_Detector1JESDown"       , 0 , 1 , _sampleNames));
        _fileNames.push_back(Sys("ATLAS_JES_NonClosure_MC11c"   , "NonClosure_MC11cJESUp"   , "NonClosure_MC11cJESDown"   , 0 , 1 , _sampleNames));
        _fileNames.push_back(Sys("ATLAS_JES_2011_Eta_TotalStat" , "Eta_TotalStatJESUp"    , "Eta_TotalStatJESDown"      , 0 , 1 , _sampleNames));
      }
      else {
        _fileNames.push_back(Sys("ATLAS_JES_2012_Modelling1" , "NP_Modelling1JESUp"      , "NP_Modelling1JESDown"      , 0 , 1 , _sampleNames));
        _fileNames.push_back(Sys("ATLAS_JES_2012_Detector1"  , "NP_Detector1JESUp"       , "NP_Detector1JESDown"       , 0 , 1 , _sampleNames));
        // if (dost) _fileNames.push_back(Sys("ATLAS_JES_NonClosure_AFII" , "NonClosure_AFIIJESUp"    , "NonClosure_AFIIJESDown"    , 0 , 1 , "st"));
        // if (doww && splitww) _fileNames.push_back(Sys("ATLAS_JES_NonClosure_AFII" , "NonClosure_AFIIJESUp"    , "NonClosure_AFIIJESDown"    , 0 , 1 , "qqww"));
        // if (mass <= 95) {
          // if (dovbf) _fileNames.push_back(Sys("ATLAS_JES_NonClosure_AFII" , "NonClosure_AFIIJESUp" , "NonClosure_AFIIJESDown" , 0 , 1 , vbfName.str() ));
          // if (doggf) _fileNames.push_back(Sys("ATLAS_JES_NonClosure_AFII" , "NonClosure_AFIIJESUp" , "NonClosure_AFIIJESDown" , 0 , 1 , ggfName.str() ));
          // if (dowh)  _fileNames.push_back(Sys("ATLAS_JES_NonClosure_AFII" , "NonClosure_AFIIJESUp" , "NonClosure_AFIIJESDown" , 0 , 1 , whName.str() ));
          // if (dozh)  _fileNames.push_back(Sys("ATLAS_JES_NonClosure_AFII" , "NonClosure_AFIIJESUp" , "NonClosure_AFIIJESDown" , 0 , 1 , zhName.str() ));
        // }
        _fileNames.push_back(Sys("ATLAS_JES_2012_Eta_StatMethod", "Eta_StatMethodJESUp"  , "Eta_StatMethodJESDown"     , 0 , 1 , _sampleNames));
        _fileNames.push_back(Sys("ATLAS_JES_2012_PilePt"     , "PilePtJESUp"             , "PilePtJESDown"             , 0 , 1 , _sampleNames));
        _fileNames.push_back(Sys("ATLAS_JES_2012_PileRho_HWW", "PileRhoJESUp"            , "PileRhoJESDown"            , 0 , 1 , _sampleNames));
      }
      _fileNames.push_back(Sys("ATLAS_JES_CLOSEBY"         , "ClosebyJESUp"            , "ClosebyJESDown"            , 0 , 1 , _sampleNames));
      _fileNames.push_back(Sys("ATLAS_JES_BJET"            , "bJESUp"                  , "bJESDown"                  , 0 , 1 , _sampleNames));
      _fileNames.push_back(Sys("ATLAS_JES_NPV"             , "NPVJESUp"                , "NPVJESDown"                , 0 , 1 , _sampleNames));
      _fileNames.push_back(Sys("ATLAS_JES_MU"              , "MuJESUp"                 , "MuJESDown"                 , 0 , 1 , _sampleNames));
      _fileNames.push_back(Sys("ATLAS_JES_Eta_Modelling"   , "Eta_ModellingJESUp"      , "Eta_ModellingJESDown"      , 0 , 1 , _sampleNames));
      _fileNames.push_back(Sys("ATLAS_JES_HighPt"          , "HighPtJESUp"             , "HighPtJESDown"             , 0 , 1 , _sampleNames));
      _fileNames.push_back(Sys("ATLAS_JES_FlavResp"        , "FlavRespJESUp"           , "FlavRespJESDown"           , 0 , 1 , _sampleNames));
      if (!doSpin) {
        if (doggf)             _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_other" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , ggfName.str() ));
      }
      else {
        if (doggf)             _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_other" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , ggfName0.str() ));
        if (doggf)             _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_other" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , ggfName2.str() ));
      }
      if (dovbf)               _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_other" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , vbfName.str() ));
      if (mass <= 300 && doVH) {
          if (dowh)            _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_other" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , whName.str() ));
          if (dozh)            _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_other" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , zhName.str() ));
      }
      if (dottbar)             _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_tt" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , "ttbar" ));
      if (dost)                _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_tt" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , "st"    ));
      if (doww) {
        if (!splitww) {
          _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_WW" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , "ww"    ));
        }
        else {
          _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_WW" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , "ggww"    ));
          _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_WW" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , "qqww"    ));
        }
      }
      if(!doOSmSS){ // Nina this is for 2011, ss only for 2012 for now anyways
        if (dowwew)            _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_other" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , "wwew"  )); // Nina are these all diboson samples?
        if (dowzzz)            _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_other" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , "wzzz"  ));
        if (dowzzzew)          _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_other" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , "wzzzew"));
        if (doWgs)             _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_other" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , "wgs"   ));
        if (doWg)              _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_other" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , "wg"    ));
      }
      if (dozjets) {
        if (!splitzjets) {
          _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_other" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , "zjets" ));
        }
        else {
          _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_other" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , "zleplep" ));
          _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_other" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , "ztautau" ));
        }
      }
      if (dozjetsew) {
        if (!splitzjets) {
          _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_other" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , "zjetsew" ));
        }
        else {
          _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_other" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , "zleplepew" ));
          _fileNames.push_back(Sys("ATLAS_JES_FlavComp_HWW_other" , "FlavCompJESUp" , "FlavCompJESDown" , 0 , 1 , "ztautauew" ));
        }
      }
    }
    if (!do2011) {
      if (!useDYmtshape) {
        _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012"          , "MuRescaleUp"             , "MuRescaleDown"             , 0 , 1 , _sampleNames));
      }
      else { // FIXME break down to samples, zjets, zleplep, ztautau is shape + norm
        if (!doSpin) {
          if (doggf) _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 0 , 1 , ggfName.str() ));
        }
        else {
          if (doggf) _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 0 , 1 , ggfName0.str() ));
          if (doggf) _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 0 , 1 , ggfName2.str() ));
        }
        if (dovbf)               _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 0 , 1 , vbfName.str() ));
        if (mass <= 300 && doVH) {
          if (dowh)              _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 0 , 1 , whName.str() ));
          if (dozh)              _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 0 , 1 , zhName.str() ));
        }
        if (dottbar)             _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 0 , 1 , "ttbar" ));
        if (dost)                _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 0 , 1 , "st"    ));
        if (doww) {
          if (!splitww) {
            _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 0 , 1 , "ww"    ));
          }
          else {
            _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 0 , 1 , "ggww"    ));
            _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 0 , 1 , "qqww"    ));
          }
        }
        if(!doOSmSS){ // Nina, do we want the wwew zzz zzzew samples out too?
          if (dowwew)           _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 0 , 1 , "wwew"  ));
          if (dowzzz)           _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 0 , 1 , "wzzz"  ));
          if (dowzzzew)         _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 0 , 1 , "wzzzew"));
          if (doWgs)            _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 0 , 1 , "wgs"   ));
          if (doWg)             _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 0 , 1 , "wg"    ));
        }
        if (dozjets) {
          if (!splitzjets)      _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 1 , 0 , "zjets" ));
          else {
                                _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 1 , 0 , "zleplep" ));
                                _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 1 , 0 , "ztautau" ));
          }
        }
        if (dozjetsew) {
          if (!splitzjets)       _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 0 , 1 , "zjetsew" ));
          else {
                                _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 0 , 1 , "zleplepew" ));
                                _fileNames.push_back(Sys("ATLAS_MU_RESCALE_lvlv_2012" , "MuRescaleUp" , "MuRescaleDown" , 0 , 1 , "ztautauew" ));
          }
        }
      }
      if (!skiptrackMET) {
        _fileNames.push_back(Sys("ATLAS_TRACKMET_RESOSOFT"  , "ResoSoftTrackMetUp_ptHard" , "ResoSoftTrackMetDown_ptHard"  , 0 , 1 , _sampleNames));
        _fileNames.push_back(Sys("ATLAS_TRACKMET_SCALESOFT" , "ScaleSoftTrackMetUp_ptHard"  , "ScaleSoftTrackMetDown_ptHard" , 0 , 1 , _sampleNames));
      }
    }
    if (!decoFakes) { // Nina .. for decoralating e and m fakes?..do this for wjetsminusss as well? not for now...
      if(!doOSmSS){
        _fileNames.push_back(Sys("FakeRate_HWW"      , "SysFakeUp"     , "SysFakeDown"   , 1 , 1 , "wjets"));
      } else {
        _fileNames.push_back(Sys("FakeRate_HWW"      , "SysFakeUp"     , "SysFakeDown"   , 1 , 1 , "wjetsminusss")); // Nina..we want the same fake rate for wjetsminusss?
        _fileNames.push_back(Sys("DibosonShape_HWW"   , "SysDibosonShapeUp" , "SysDibosonShapeDown" , 1 , 0 , "ss")); // Nina this is only a shape systematic I thi
      }
    } else {
      if(!doOSmSS){
        _fileNames.push_back(Sys("FakeRate_EL_HWW"     , "SysFakeUp"     , "SysFakeDown"   , 1 , 1 , "wjets"));
        _fileNames.push_back(Sys("FakeRate_MU_HWW"     , "SysFakeUp"     , "SysFakeDown"   , 1 , 1 , "wjets"));
        // _fileNames.push_back(Sys("FakeRate_QCD_HWW"    , "SysFakeUp"     , "SysFakeDown"   , 1 , 1 , "wjets"));
      } else {
        _fileNames.push_back(Sys("FakeRate_EL_HWW"     , "SysFakeUp"     , "SysFakeDown"   , 1 , 1 , "wjetsminusss"));
        _fileNames.push_back(Sys("FakeRate_MU_HWW"     , "SysFakeUp"     , "SysFakeDown"   , 1 , 1 , "wjetsminusss"));
        _fileNames.push_back(Sys("DibosonShape_HWW"   , "SysDibosonShapeUp" , "SysDibosonShapeDown" , 1 , 0 , "ss")); // Nina this is only a shape systematic I thi
      }
    }
    if (!skipUEPS && doww) {
      if (!doSpin) {
        if (!splitww) {
          _fileNames.push_back(Sys("ATLAS_WW_MTSHAPE" , "PowhegShapeUp" , "PowhegShapeDown" , 1 , 0 , "ww")); 
        }
        else {
          _fileNames.push_back(Sys("ATLAS_WW_MTSHAPE" , "PowhegShapeUp" , "PowhegShapeDown" , 1 , 0 , "ggww")); 
          _fileNames.push_back(Sys("ATLAS_WW_MTSHAPE" , "PowhegShapeUp" , "PowhegShapeDown" , 1 , 0 , "qqww")); 
        }
      }
      else { // spin is here
        if (!splitww) {
        if(applySlopeSpin){
          _fileNames.push_back(Sys("UEPS_SHAPE" , "McatnloShapeUp" , "McatnloShapeUp" , 1 , 1 , "ww")); 
        }
        else{
          _fileNames.push_back(Sys("UEPS_SHAPE_PDF" , "ShapeUp" , "ShapeDown" , 1 , 0 , "ww"));
          _fileNames.push_back(Sys("UEPS_SHAPE_PS" , "ShapeUp" , "ShapeDown" , 1 , 0 , "ww"));
          _fileNames.push_back(Sys("UEPS_SHAPE_SCALE" , "ShapeUp" , "ShapeDown" , 1 , 0 , "ww"));
        }        
        }
        else {
          if (applySlopeSpin) {
            _fileNames.push_back(Sys("UEPS_SHAPE" , "McatnloShapeUp" , "McatnloShapeUp" , 1 , 1 , "ggww")); 
            _fileNames.push_back(Sys("UEPS_SHAPE" , "McatnloShapeUp" , "McatnloShapeUp" , 1 , 1 , "qqww")); 
          }
        }
      }
    }
  }

  fileNames = &_fileNames;
  std::cout << "fileNames has " << fileNames->size() << " syst variations as elements." << std::endl;

  _initialized = true;
}

// Parse boundary file
vector<double> readBoundaryFile(string fileName)
{

  cout << "Reading in mT boundary: " << fileName << endl;
  ifstream inFile(fileName.c_str());
  if (inFile.fail())
  {
    cout << "ERROR::Couldn't open file: " << fileName << endl;
    exit(1);
  }

  string junk;
  double bb;
  vector<double> bounds;
  inFile >> junk;
  while (!inFile.eof())
  {
    inFile >> bb;
    if(bounds.size()==0) bounds.push_back(bb);
    else if (bb!=bounds[bounds.size()-1]) bounds.push_back(bb);
  }

  if(bounds.size()==0)
  {
    cout << "ERROR::Failed to parse boundary file: " << fileName << endl;
    exit(1);
  }

  return bounds;

}

// PARSE SYSTEMATIC TXT FILES
void readInUncerts(string fileName, vector<Response>& vecR, double mass, bool skip, bool skipWj, bool skipZj)
{
  cout << "Reading in uncerts: " << fileName << endl;
  stringstream massStr;
  massStr << mass;
  string smass = massStr.str();

  ifstream inFile(fileName.c_str());
  if (inFile.fail())
  {
    cout << "ERROR::Couldn't open file: " << fileName << endl;
    exit(1);
  }

  vector<string> jetNames;
  jetNames.push_back("0j");
  jetNames.push_back("1j");
  jetNames.push_back("2j");

  vector<string> chanNames;
  chanNames.push_back("ee");
  chanNames.push_back("em");
  if (splitem && dome) chanNames.push_back("me");
  chanNames.push_back("mm");
  if (combineCRs) chanNames.push_back("OF");
  if (combineSFCRs) chanNames.push_back("SF");

  vector<string> regNames;
  int nrRegions = regions->size();
  for (int i=0;i<nrRegions;i++) regNames.push_back((*regions)[i].name);

  // MAPS OF ALTERNATE SPELLINGS
  map<string, string> cmap;
  cmap["ee"]   = "ee";
  cmap["mumu"] = "mm";
  cmap["emu"]  = "em";
  cmap["mm"]   = "mm";
  cmap["em"]   = "em";
  cmap["OF"]   = "OF";
  cmap["SF"]   = "SF";
  if (splitem && dome) cmap["me"] = "me";
  cmap["all"]  = "all";

  multimap<string, string> smap;
  smap.insert(make_pair("ggf"+smass,  "ggf"+smass));
  smap.insert(make_pair("ggH"+smass+"spin0p",  "ggH"+smass+"spin0p"));
  if(doSpin2plus)  smap.insert(make_pair("ggH"+smass+"spin2p",  "ggH"+smass+"spin2p"));
  else if (doSpin1plus)  smap.insert(make_pair("ggH"+smass+"spin1p",  "ggH"+smass+"spin1p"));
  else if(doSpin1minus)  smap.insert(make_pair("ggH"+smass+"spin1m",  "ggH"+smass+"spin1m"));
  else if(doSpin0minus)  smap.insert(make_pair("ggH"+smass+"spin0m",  "ggH"+smass+"spin0m"));
  smap.insert(make_pair("vbf"+smass,  "vbf"+smass));
  smap.insert(make_pair("wh"+smass,   "wh"+smass));
  smap.insert(make_pair("zh"+smass,   "zh"+smass));
  smap.insert(make_pair("wh",         "wh"+smass));
  smap.insert(make_pair("zh",         "zh"+smass));
  smap.insert(make_pair("vbf",        "vbf"+smass));
  smap.insert(make_pair("hww",        "ggf"+smass));
  smap.insert(make_pair("ggf",        "ggf"+smass));
  if (include125BG)
  {
    smap.insert(make_pair("ggf125",   "ggf125bg"));
    smap.insert(make_pair("vbf125",   "vbf125bg"));
    smap.insert(make_pair("wh125",    "wh125bg"));
    smap.insert(make_pair("zh125",    "zh125bg"));
    smap.insert(make_pair("wh",       "wh125bg"));
    smap.insert(make_pair("zh",       "zh125bg"));
    smap.insert(make_pair("vbf",      "vbf125bg"));
    smap.insert(make_pair("hww",      "ggf125bg"));
    smap.insert(make_pair("ggf",      "ggf125bg"));
    smap.insert(make_pair("ggf125bg", "ggf125bg"));
  }
  smap.insert(make_pair("ggHspin0p",  "ggH"+smass+"spin0p"));
  smap.insert(make_pair("ggHspin2p",  "ggH"+smass+"spin2p"));
  smap.insert(make_pair("ggHspin1p",  "ggH"+smass+"spin1p"));
  smap.insert(make_pair("ggHspin1m",  "ggH"+smass+"spin1m"));
  smap.insert(make_pair("ggHspin0m",  "ggH"+smass+"spin0m"));
  smap.insert(make_pair("top",        "ttbar"));
  smap.insert(make_pair("ttbar",      "ttbar"));
  smap.insert(make_pair("st",         "st"));
  smap.insert(make_pair("ww",         "ww"));
  smap.insert(make_pair("ggww",       "ggww"));
  smap.insert(make_pair("qqww",       "qqww"));
  smap.insert(make_pair("wwew",       "wwew"));
  smap.insert(make_pair("wzzz",       "wzzz"));
  smap.insert(make_pair("wzzzew",     "wzzzew"));
  smap.insert(make_pair("wg",         "wg"));
  smap.insert(make_pair("wgs",        "wgs"));
  smap.insert(make_pair("zjets",      "zjets"));
  smap.insert(make_pair("zjetsew",    "zjetsew"));
  smap.insert(make_pair("zleplep",    "zleplep"));
  smap.insert(make_pair("ztautau",    "ztautau"));
  smap.insert(make_pair("zleplepew",  "zleplepew"));
  smap.insert(make_pair("ztautauew",  "ztautauew"));
  smap.insert(make_pair("wjets",      "wjets"));
  smap.insert(make_pair("wjetsminusss", "wjetsminusss")); // Nina..what is this for?
  smap.insert(make_pair("ss",         "ss")); // Nina

  cout << "->Starting file loop" << endl;
  string junk;
  string term;
  string name;
  string type;
  inFile >> term;
  while (!inFile.eof())
  {
    type = "gaus";
    inFile >> name;

    inFile >> term;
    while (term != "uncert")
    {
      if (term == "type")
      {
        inFile >> type;
      }
      else if (term == "range")
      {
        inFile >> junk;
        inFile >> junk;
      }
      else
      {
        cout << "ERROR::Unrecognized term: " << term << ". Exiting." << endl;
        exit(1);
      }

      inFile >> term;
    }

    while (term == "uncert" && !inFile.eof())
    {
      string id;
      inFile >> id;
      // cout << "id: " << id << endl;
      vector<string> terms = parseString(id, "_");
      int nrTerms = terms.size();

      string j = terms[nrTerms-1];
      string c = cmap[terms[nrTerms-3]];
      string r = terms[nrTerms-2];

      // cout << "j = " << j << ", c = " << c << ", r = " << r << endl;

      double hi,lo;
      if (signFix)
      {
        inFile >> hi;
        if (type == "bifur_gaus")
        {
          inFile >> lo;
        }
        else
        {
          if (hi > 0) lo = 1./(1+hi)-1;
          else
          {
            lo = -hi;
            hi = 1./(1+lo)-1;
          }
        }
      }
      else
      {
        inFile >> hi;
        if (type == "bifur_gaus")
        {
          inFile >> lo;
        }
        else
        {
          lo = 1./(1+hi)-1;
        }
      }

      if (lo < -0.99)
      {
        cout << "WARNING::Truncating lo error from " << lo << " to -0.99" << endl;
        lo = -0.99;
      }

      if (hi < -0.99)
      {
        cout << "WARNING::Truncating hi error from " << hi << " to -0.99" << endl;
        hi = -0.99;
      }

      vector<string> jets;
      vector<string> chans;
      vector<string> regs;

      if (j == "all")
      {
        jets = jetNames;
      }
      else
      {
        jets.push_back(j);
      }

      if (c == "all")
      {
        chans = chanNames;
      }
      else
      {
        chans.push_back(c);
      }

      if (r == "all")
      {
        regs = regNames;
      }
      else
      {
        regs.push_back(r);
      }

      int nrJets = jets.size();
      int nrChans = chans.size();
      int nrRegs = regs.size();


      for (int ij = 0; ij < nrJets; ij++)
      {
        for (int ic = 0; ic < nrChans; ic++)
        {
          for (int ir = 0; ir < nrRegs; ir++)
          {
            // maybe skip ww and top samples if it's an uncertainty on the overall normalization but we're letting it float
            map<string, bool> is_sample;
            int nrSamples = samples->size();
            for (int is=0;is<nrSamples;is++)
            {
              is_sample[(*samples)[is].name] = false;
            }

            for (multimap<string, string>::iterator itr = smap.begin();itr!=smap.end();itr++)
            {
              if (itr->first != terms[0]) continue;
              for (int is=0;is<nrSamples;is++)
              {
                if ((*samples)[is] == itr->second) is_sample[(*samples)[is].name] = true;
              }
            }

            if (skip)
            {
              if (is_sample["ww"] || is_sample["ggww"] || is_sample["qqww"]) continue;
              if ((is_sample["ttbar"] || is_sample["st"]) && jets[ij] == "1j") continue;
            }

            if (skipWj && is_sample["wjets"]) continue; // Nina? Why are we skipping this? Not sure how to change it for SS, why is this here, for which theory uncertaintiess?
            if (skipZj && (is_sample["zjets"] || is_sample["zleplep"] || is_sample["ztautau"]) && chans[ic] != "em" && chans[ic] != "me") continue;

            string thisName = name;
            bool isSignal = false;
            // int nrSamples = samples->size();
            for (int is=0;is<nrSamples;is++)
            {
              if (is_sample[(*samples)[is].name] && (*samples)[is].type == "signal") // fix
              {
                isSignal = true;
                break;
              }
            }
            
            for (int is=0;is<nrSamples;is++)
            {
              string sampleName = (*samples)[is].name;
              if (!is_sample[sampleName]) continue;

              Response res;
              res.name = thisName;
              res.channel = chans[ic];
              res.jet = jets[ij];
              res.sample = sampleName;
              res.region = regs[ir];
              res.hi = hi;
              res.lo = lo;
              vecR.push_back(res);

              if (res.sample == "ttbar") // duplicate for st
              {
                Response res2;
                res2.name = thisName;
                res2.channel = chans[ic];
                res2.jet = jets[ij];
                res2.sample = "st";
                res2.region = regs[ir];
                res2.hi = hi;
                res2.lo = lo;
                vecR.push_back(res2);
              }
            }
          }
        }
      }
      inFile >> term;
    }
  }
  cout << "->Done" << fileName << endl;
  cout << endl;
}

// PARSE 'SEP' DELIMITED STRING
vector<string> parseString(string str, string sep)
{
  vector<string> parsed;
  int pos    = 0;
  bool first = true;

  if (str.size() == 0) return parsed;
  if (str.find(sep) == string::npos)
  {
    parsed.push_back(str);
    return parsed;
  }

  while (true)
  {
    int newPos = str.find(sep, pos);
    if (str.find(sep, pos) == string::npos)
    {
      if (!first) parsed.push_back(str.substr(pos, newPos-pos));
      break;
    }
    string sub = str.substr(pos, newPos-pos);
    parsed.push_back(sub);
    pos = newPos+1;
    first = false;
  }
  return parsed;
}

// TAG CORE FILES FOR POSTERITY
void tag(double mass, string version, bool alt, bool doHists) // FIXME add 2012 stuff
{
  stringstream massStr;
  massStr << mass;
  string smass = massStr.str();

  stringstream altStr;
  if (alt)
  {
    altStr << "_alt";
  }
  string salt = altStr.str();

  vector<string> tagFiles;
  tagFiles.push_back("macros/runChain.C");
  tagFiles.push_back("macros/setup.C");
  if (doHists)
  {
    tagFiles.push_back("macros/makeNorms.C");
    tagFiles.push_back("macros/writeHistos.C");
  }
  tagFiles.push_back("macros/writeXML.C");
  tagFiles.push_back("macros/runInterp.C");
  tagFiles.push_back("macros/splitws.C");
  tagFiles.push_back("config"+string(do2012?"_2012/":"_2011/")+"ggf.C");
  tagFiles.push_back("config"+string(do2012?"_2012/":"_2011/")+"vbf.C");
  tagFiles.push_back("config"+string(do2012?"_2012/":"_2011/")+"br.C");
  tagFiles.push_back("config"+string(do2012?"_2012/":"_2011/")+"*.txt");
  tagFiles.push_back("config"+string(do2012?"_2012/":"_2011/")+"xs_files/xs_"+smass+".txt");
  int nrFiles = tagFiles.size();

  system(("mkdir -vp rev/" + version + "/tag").c_str());

  stringstream command;
  command << "tar czf rev/" + version + "/tag/" << smass+salt << ".tar.gz";
  for (int i = 0; i < nrFiles; i++)
  {
    command << " " << tagFiles[i];
  }
  system(command.str().c_str());
}

// Decide which regions and channels to skip
bool skipRegion(Region* r, string channelName, string jetName)
{
  bool skip = 0;
  if (!(!doOF2jWWCR && merge2jWWCR && doWWCR2j && jetName == "2j" && r->name.find("mainControl") != string::npos && channelName != "ee" && channelName != "mm") && combineCRs && r->isCR && !r->isSFCR && channelName != "OF") skip = 1;  // inelegant fix for 2jWWCR
  if(doWWCR2j && combineCRs && (channelName == "em" || channelName == "me") && r->name.find("mainControl") != string::npos && jetName == "2j") skip = 1;
  if (!r->isCR && !r->isSFCR && channelName == "OF") skip = 1;
  if (combineSFCRs && r->isSFCR && channelName != "OF" && channelName != "SF") skip = 1;
  if (!(!doOF2jWWCR && merge2jWWCR && doWWCR2j && jetName == "2j" && r->name.find("mainControl") != string::npos && channelName != "ee" && channelName != "mm") && !r->isSFCR && channelName == "SF") skip = 1; // inelegant fix for 2jWWCR
  if (combineCRs && !combineSFCRs && r->isCR && channelName != "OF" && channelName != "mm" && channelName != "ee") skip = 1;
  if (combineCRs && !combineSFCRs && r->isSFCR && channelName == "OF" && r->name.find("topbox") == string::npos) skip = 1;
  if ( jetName == "0j" && r->name.find("topbox") != string::npos) skip = 1;
  if (((jetName == "2j" && !doWWCR2j) || doSFonly) && r->name.find("mainControl") != string::npos) skip = 1;
  if ( /*jetName == "0j"              &&*/ r->name.find("mainControl") != string::npos && (channelName == "ee" || channelName == "mm") && ZMode == 2 ) skip = 1;
  if ( jetName == "2j"              && r->name.find("lowPt")       != string::npos) skip = 1;
  //-- johanna's lines (check later) --//
  //if (( (ZMode == 2 || doPacman2j ) && (channelName == "em" || channelName == "me") && !doee && !domm)            && r->name.find("ASR")         != string::npos) skip =1;
  //if (( (ZMode == 2 || doPacman2j ) && (channelName == "em" || channelName == "me") && !doee && !domm)            && r->name.find("AfrecSR")     != string::npos) skip =1;
  //if (( (ZMode == 2 || doPacman2j ) && (channelName == "em" || channelName == "me") && !doee && !domm)            && r->name.find("CZpeak")      != string::npos) skip =1;
  //if (( (ZMode == 2 || doPacman2j ) && (channelName == "em" || channelName == "me") && !doee && !domm)            && r->name.find("CfrecZpeak")  != string::npos) skip =1;
  //if (( (ZMode == 2 || doPacman2j ) && (channelName == "em" || channelName == "me") && !doee && !domm)            && r->name.find("EWWCR")       != string::npos) skip =1;
  //if (( (ZMode == 2 || doPacman2j ) && (channelName == "em" || channelName == "me") && !doee && !domm)            && r->name.find("EfrecWWCR")   != string::npos) skip =1;
  // --- stefan's lines -- //
  if (((jetName == "2j" && !doPacman2j) || ((ZMode == 2 || (jetName == "2j" && doPacman2j)) && (channelName == "em" || channelName == "me") && !doee && !domm))            && r->name.find("ASR")         != string::npos) skip = 1;
  if (((jetName == "2j" && !doPacman2j) || ((ZMode == 2 || (jetName == "2j" && doPacman2j)) && (channelName == "em" || channelName == "me") && !doee && !domm))            && r->name.find("AfrecSR")     != string::npos) skip = 1;
  if (((jetName == "2j" && !doPacman2j) || ((ZMode == 2 || (jetName == "2j" && doPacman2j)) && (channelName == "em" || channelName == "me") && !doee && !domm))            && r->name.find("CZpeak")      != string::npos) skip = 1;
  if (((jetName == "2j" && !doPacman2j) || ((ZMode == 2 || (jetName == "2j" && doPacman2j)) && (channelName == "em" || channelName == "me") && !doee && !domm))            && r->name.find("CfrecZpeak")  != string::npos) skip = 1;
  if (((jetName == "2j" && !doPacman2j) || ((ZMode == 2 || (jetName == "2j" && doPacman2j)) && (channelName == "em" || channelName == "me") && !doee && !domm))            && r->name.find("EWWCR")       != string::npos) skip = 1;
  if (((jetName == "2j" && !doPacman2j) || ((ZMode == 2 || (jetName == "2j" && doPacman2j)) && (channelName == "em" || channelName == "me") && !doee && !domm))            && r->name.find("EfrecWWCR")   != string::npos) skip = 1;
  //---------//
  if ((jetName != "2j" && doSFonly) && r->name.find("signalLike")  != string::npos) skip = 1;
  if ( jetName != "2j"              && r->name.find("signalLike")  != string::npos && (channelName == "ee" || channelName == "mm") && ZMode == 2) skip = 1;
  if (r->name.find("topbox") != string::npos && (jetName != "0j" && doSFonly && (channelName == "em" || channelName == "me" || channelName == "OF"))) skip = 1;
  if ( skipSFtopCR && jetName != "2j" &&  (channelName == "ee" || channelName == "mm" || channelName == "SF") &&  r->name.find("topbox") != string::npos ) skip = 1;
  if ( skipSFWWCR  && (r->name.find("EWWCR") != string::npos || r->name.find("EfrecWWCR") != string::npos ) ) skip = 1;
  if (merge2j && channelName == "me" && jetName == "2j") skip = 1;
  if (merge2jSFSR && channelName == "mm" && jetName == "2j" && r->name.find("signalLike") != string::npos) skip = 1;
  if (merge2jtopCR && (channelName == "em" || channelName == "me" || channelName == "OF") && jetName == "2j" && r->name.find("topbox") != string::npos) skip = 1;
  if (merge2jWWCR && !doOF2jWWCR && (channelName == "em" || channelName == "me" || channelName == "OF") && jetName == "2j" && r->name.find("mainControl") != string::npos) skip = 1;
  //if (r->name.find("signalLike2") != string::npos && jetName == "2j") skip = 1;
  if (skipSF2j && (channelName == "ee" || channelName == "mm" || channelName == "SF") && jetName == "2j") skip = 1;
  // -- stefan's statement -- not sure why it is here
  //if (doPacman2j && jetName == "2j" && (channelName == "ee" || channelName == "mm") && r->name.find("signalLike")  != string::npos) skip = 1;

  return skip;
}


int getNbinsForRegion(Region* r, Channel* c) {
        int theseBins = nrBins_0j;

	//we want only one mT bin in CRs
	if (r->name.find("mainControl") != string::npos ||
	    r->name.find("topbox")      != string::npos ||
	    r->name.find("zbox")        != string::npos ||
	    r->name.find("ASR")         != string::npos ||
	    (r->name.find("AfrecSR")    != string::npos && (c->name == "em" || c->name == "me" || c->name == "OF" )) ||
	    r->name.find("CZpeak")      != string::npos ||
	    r->name.find("CfrecZpeak")  != string::npos ||
	    r->name.find("EWWCR")       != string::npos ||
	    r->name.find("EfrecWWCR")   != string::npos)
	  return 1;


        if (r->name.find("lowPt") != string::npos) theseBins = nrBins_lowPt;
        else if (c->nj == 1)
        {
          theseBins = nrBins_1j;
        }
        else if (c->nj == 2)
        {
          theseBins = nrBins_2j;
        }
        if (doMVA)
        {
          theseBins=nrBins0j_MVA;
          if (c->nj == 1) theseBins = nrBins1j_MVA;
          if (c->nj == 2) theseBins = nrBins2j_MVA;
        }
	return theseBins;
}


#endif
