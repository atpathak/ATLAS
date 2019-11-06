#include "TROOT.h"

#include "TMath.h"
#include "Math/ProbFuncMathCore.h"
#include "Math/QuantFuncMathCore.h"
#include "TFile.h"
#include "TH1D.h"
#include "TGraph.h"

#include "RooWorkspace.h"
#include "RooNLLVar.h"
#include "RooStats/ModelConfig.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "Math/MinimizerOptions.h"
#include "TStopwatch.h"
#include "RooMinimizerFcn.h"
#include "RooMinimizer.h"
#include "RooCategory.h"
#include "RooSimultaneous.h"
#include "RooProduct.h"

#include <map>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace RooFit;
using namespace RooStats;

//band configuration
bool betterBands           = 1; // (recommendation = 1) improve bands by using a more appropriate asimov dataset for those points
bool betterNegativeBands   = 0; // (recommendation = 0) improve also the negative bands
bool profileNegativeAtZero = 0; // (recommendation = 0) profile asimov for negative bands at zero


//other configuration
string defaultMinimizer    = "Minuit2";     // or "Minuit"
int defaultPrintLevel      = -1;            // Minuit print level
int defaultStrategy        = 1;             // Minimization strategy. 0-2. 0 = fastest, least robust. 2 = slowest, most robust
bool killBelowFatal        = 1;             // In case you want to suppress RooFit warnings further, set to 1
bool doBlind               = 0;             // in case your analysis is blinded
//bool conditionalExpected   = 1 && !doBlind; // Profiling mode for Asimov data: 0 = conditional MLEs, 1 = nominal MLEs
bool doTilde               = 1;             // bound mu at zero if true and do the \tilde{q}_{mu} asymptotics
bool doExp                 = 1;             // compute expected limit
//bool doObs                 = 1 && !doBlind; // compute observed limit
double precision           = 0.01;        // % precision in mu that defines iterative cutoff
bool verbose               = 0;             // 1 = very spammy
bool usePredictiveFit      = 0;             // experimental, extrapolate best fit nuisance parameters based on previous fit results
bool extrapolateSigma      = 0;             // experimantal, extrapolate sigma based on previous fits
int maxRetries             = 3;             // number of minimize(fcn) retries before giving up
bool doOne                 = 1;

double confidenceLevel = 0.95;
TString pname;
double xmass=125; // what else?


//don't touch!
map<RooNLLVar*, double> map_nll_muhat;
map<RooNLLVar*, double> map_muhat;
map<RooDataSet*, RooNLLVar*> map_data_nll;
map<RooNLLVar*, string> map_snapshots;
map<RooNLLVar*, map<double, double> > map_nll_mu_sigma;
RooWorkspace* w = NULL;
ModelConfig* mc = NULL;
RooDataSet* data = NULL;
RooRealVar* firstPOI = NULL;
RooNLLVar* asimov_0_nll = NULL;
RooNLLVar* asimov_1_nll = NULL;
RooNLLVar* obs_nll = NULL;
int nrMinimize=0;
int direction=1;
int global_status=0;
double target_CLs=0.05;



//main
void runAsymptoticsCLs(const char* infile,
		       const char* workspaceName,
		       const char* modelConfigName,
		       const char* dataName,
		       const char* asimovDataName,
		       string folder,
		       string mass,
		       double CL);

 //for backwards compatibility
void runAsymptoticsCLs(const char* infile,
		       const char* workspaceName = "combWS",
		       const char* modelConfigName = "ModelConfig",
		       const char* dataName = "combData",
		       const char* asimovDataName = "asimovData_0",
		       const char* conditionalSnapshot = "conditionalGlobs_0",
		       const char* nominalSnapshot = "nominalGlobs",
		       string folder = "test",
		       string mass = "130",
		       double CL = 0.95);

double getLimit(RooNLLVar* nll, double initial_guess = 0);
double getSigma(RooNLLVar* nll, double mu, double muhat, double& qmu);
double getQmu(RooNLLVar* nll, double mu);
void saveSnapshot(RooNLLVar* nll, double mu);
void loadSnapshot(RooNLLVar* nll, double mu);
void doPredictiveFit(RooNLLVar* nll, double mu1, double m2, double mu);
RooNLLVar* createNLL(RooDataSet* _data);
double getNLL(RooNLLVar* nll);
double findCrossing(double sigma_obs, double sigma, double muhat);
void setMu(double mu);
double getQmu95_brute(double sigma, double mu);
double getQmu95(double sigma, double mu);
double calcCLs(double qmu_tilde, double sigma, double mu);
double calcPmu(double qmu_tilde, double sigma, double mu);
double calcPb(double qmu_tilde, double sigma, double mu);
double calcDerCLs(double qmu, double sigma, double mu);
int minimize(RooNLLVar* nll);
int minimize(RooAbsReal* nll);
//RooDataSet* makeAsimovData2(RooDataSet* conditioningData, double mu_true, double mu_prof = -999, string* mu_str = NULL, string* mu_prof_str = NULL);
//RooDataSet* makeAsimovData2(RooNLLVar* conditioningNLL, double mu_true, double mu_prof = -999, string* mu_str = NULL, string* mu_prof_str = NULL);

void unfoldConstraints(RooArgSet& initial, RooArgSet& final, RooArgSet& obs, RooArgSet& nuis, int& counter);
RooDataSet* makeAsimovData(bool doConditional, RooNLLVar* conditioning_nll, double mu_val, string* mu_str = NULL, string* mu_prof_str = NULL, double mu_val_profile = -999, bool doFit = true);

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////



void runAsymptoticsCLs(const char* infile,
		       const char* workspaceName,
		       const char* modelConfigName,
		       const char* dataName,
		       const char* asimovDataName,
		       const char* conditionalSnapshot,
		       const char* nominalSnapshot,
		       string folder,
		       string mass,
		       double CL)
{
  stringstream smass;
  smass << mass;

  conditionalSnapshot = ""; // warningless compile
  nominalSnapshot = "";     // warningless compile

  runAsymptoticsCLs(infile, workspaceName, modelConfigName, dataName, asimovDataName, folder, smass.str(), CL);
}



void runAsymptoticsCLs(const char* infile,
		       const char* workspaceName,
		       const char* modelConfigName,
		       const char* dataName,
		       const char* asimovDataName,
		       string folder,
		       string mass,
		       double CL)
{

  bool conditionalExpected   = 1 && !doBlind; // Profiling mode for Asimov data: 0 = conditional MLEs, 1 = nominal MLEs
  bool doObs                 = 1 && !doBlind; // compute observed limit
  //
  //
  TStopwatch timer;
  timer.Start();

  if (killBelowFatal) RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
  ROOT::Math::MinimizerOptions::SetDefaultMinimizer(defaultMinimizer.c_str());
  ROOT::Math::MinimizerOptions::SetDefaultStrategy(defaultStrategy);
  ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(defaultPrintLevel);
  //RooNLLVar::SetIgnoreZeroEntries(1);
  
  //check inputs
  TFile* f = TFile::Open(infile);
  w = (RooWorkspace*)f->Get(workspaceName);
  if (!w)
    {
      cout << "ERROR::Workspace: " << workspaceName << " doesn't exist!" << endl;
      return;
    }
  
  mc = (ModelConfig*)w->obj(modelConfigName);
  if (!mc)
    {
      cout << "ERROR::ModelConfig: " << modelConfigName << " doesn't exist!" << endl;
      return;
    }
  firstPOI = (RooRealVar*)mc->GetParametersOfInterest()->first();

  data = (RooDataSet*)w->data(dataName);
  if (!data)
    {
      cout << "ERROR::Dataset: " << dataName << " doesn't exist!" << endl;
      return;
    }
  
  data->convertToVectorStore() ;
  cout << "Data:: " << endl; data->Print("v");
  cout << "Data:: isWeighted = " << data->isWeighted() << " isNonPoissonWeighted = " << data->isNonPoissonWeighted() << endl;
  cout << "Data:: numEntries = " << data->numEntries() << " sumEntries = " << data->sumEntries() << endl;
  
  //RooAbsPdf* pdf = mc->GetPdf();
  obs_nll = createNLL(data);//(RooNLLVar*)pdf->createNLL(*data);
  map_snapshots[obs_nll] = "nominalGlobs";
  map_data_nll[data] = obs_nll;
  w->saveSnapshot("nominalGlobs",*mc->GetGlobalObservables());
  w->saveSnapshot("nominalNuis",*mc->GetNuisanceParameters());
  
  global_status=0;
  RooDataSet* asimovData_0 = (RooDataSet*)w->data(asimovDataName);
  if (!asimovData_0)
    {
      asimovData_0 = makeAsimovData(conditionalExpected, obs_nll, 0);
      
      //asimovData_0 = makeAsimovData2((conditionalExpected ? obs_nll : (RooNLLVar*)NULL), 0., 0.);
    }
  int asimov0_status=global_status;
  
  cout << "asimov0_status = " << asimov0_status << endl;
  cout << "AsimovData_0:: " << endl; asimovData_0->Print("v");
  cout << "AsimovData_0:: isWeighted = " << asimovData_0->isWeighted() << " isNonPoissonWeighted = " << asimovData_0->isNonPoissonWeighted() << endl;
  cout << "AsimovData_0:: numEntries = " << asimovData_0->numEntries() << " sumEntries = " << asimovData_0->sumEntries() << endl;
  
  asimov_0_nll = createNLL(asimovData_0);//(RooNLLVar*)pdf->createNLL(*asimovData_0);
  map_snapshots[asimov_0_nll] = "conditionalGlobs_0";
  map_data_nll[asimovData_0] = asimov_0_nll;
  setMu(0);
  map_muhat[asimov_0_nll] = 0;
  saveSnapshot(asimov_0_nll, 0);
  w->loadSnapshot("conditionalNuis_0");
  w->loadSnapshot("conditionalGlobs_0");
  map_nll_muhat[asimov_0_nll] = asimov_0_nll->getVal();

  target_CLs=1-CL;
  
  double med_limit = doExp ? getLimit(asimov_0_nll, 1.0) : 1.0;
  int med_status=global_status;
  cout << "med_status = " << med_status << " med_limit = " << med_limit << endl;

  double sigma = med_limit/sqrt(3.84); // pretty close
  double mu_up_p2_approx = sigma*(ROOT::Math::gaussian_quantile(1 - target_CLs*ROOT::Math::gaussian_cdf( 2), 1) + 2);
  double mu_up_p1_approx = sigma*(ROOT::Math::gaussian_quantile(1 - target_CLs*ROOT::Math::gaussian_cdf( 1), 1) + 1);
  double mu_up_n1_approx = sigma*(ROOT::Math::gaussian_quantile(1 - target_CLs*ROOT::Math::gaussian_cdf(-1), 1) - 1);
  double mu_up_n2_approx = sigma*(ROOT::Math::gaussian_quantile(1 - target_CLs*ROOT::Math::gaussian_cdf(-2), 1) - 2);

  double mu_up_p2 = mu_up_p2_approx;
  double mu_up_p1 = mu_up_p1_approx;
  double mu_up_n1 = mu_up_n1_approx;
  double mu_up_n2 = mu_up_n2_approx;

  firstPOI->setRange(-5*sigma, 5*sigma);
  map<int, int> N_status;
  if (betterBands && doExp) // no better time than now to do this
  {
    //find quantiles, starting with +2, since we should be at +1.96 right now

    double init_targetCLs = target_CLs;
    firstPOI->setRange(-5*sigma, 5*sigma);
    for (int N=2;N>=-2;N--)
    {
      if (N < 0 && !betterNegativeBands) continue;
      if (N == 0) continue;
      target_CLs=2*(1-ROOT::Math::gaussian_cdf(fabs(N))); // change this so findCrossing looks for sqrt(qmu95)=2
      if (N < 0) direction = -1;

      cout << "N = " << N << " quantile = " << 1-ROOT::Math::gaussian_cdf(fabs(N)) << " target_CLs = " << target_CLs << endl;

      //get the acual value
      double NtimesSigma = getLimit(asimov_0_nll, N*med_limit/sqrt(3.84)); // use N * sigma(0) as an initial guess
      N_status[N] += global_status;
      sigma = NtimesSigma/N;
      cout << endl;
      cout << "Found N * sigma = " << N << " * " << sigma << endl;

      string muStr,muStrPr;
      bool load_status=w->loadSnapshot("conditionalGlobs_0");
      if (load_status) {
	cout << "Loaded snapshot with name = conditionalGlobs_0 " << endl;
      } else {
	cout << "No snapshot found with name = conditionalGlobs_0 " << endl;
      }
      double pr_val = NtimesSigma;
      if (N < 0 && profileNegativeAtZero) pr_val = 0;
      RooDataSet* asimovData_N = makeAsimovData(1, asimov_0_nll, NtimesSigma, &muStr, &muStrPr, pr_val, 0);
      //RooDataSet* asimovData_N = makeAsimovData2(asimov_0_nll, NtimesSigma, pr_val, &muStr, &muStrPr);


      RooNLLVar* asimov_N_nll = createNLL(asimovData_N);//(RooNLLVar*)pdf->createNLL(*asimovData_N);
      map_data_nll[asimovData_N] = asimov_N_nll;
      map_snapshots[asimov_N_nll] = "conditionalGlobs"+muStrPr;
      w->loadSnapshot(map_snapshots[asimov_N_nll].c_str());
      w->loadSnapshot(("conditionalNuis"+muStrPr).c_str());
      setMu(NtimesSigma);

      double nll_val = asimov_N_nll->getVal();
      saveSnapshot(asimov_N_nll, NtimesSigma);
      map_muhat[asimov_N_nll] = NtimesSigma;
      if (N < 0 && doTilde)
      {
	setMu(0);
	firstPOI->setConstant(1);
	nll_val = getNLL(asimov_N_nll);
      }
      map_nll_muhat[asimov_N_nll] = nll_val;

      target_CLs = init_targetCLs;
      direction=1;
      double initial_guess = findCrossing(NtimesSigma/N, NtimesSigma/N, NtimesSigma);
      double limit = getLimit(asimov_N_nll, initial_guess);
      N_status[N] += global_status;

      if (N == 2) mu_up_p2 = limit;
      else if (N == 1) mu_up_p1 = limit;
      else if (N ==-1) mu_up_n1 = limit;
      else if (N ==-2) mu_up_n2 = limit;
      //return;
    }
    direction = 1;
    target_CLs = init_targetCLs;

  }
  /*
  //--------------------------
  cout << "Loading conditionalGlobs_0 1" << endl;
  w->loadSnapshot("conditionalGlobs_0");
  mc->GetGlobalObservables()->Print("v");
  
  cout << "Loading conditionalNuis_0 1" << endl;
  w->loadSnapshot("conditionalNuis_0");
  mc->GetNuisanceParameters()->Print("v");

  //--------------------------

  cout << "Loading nominalGlobs 1" << endl;
  w->loadSnapshot("nominalGlobs");
  mc->GetGlobalObservables()->Print("v");

  cout << "Loading nominalNuis 1" << endl;
  w->loadSnapshot("nominalNuis");
  mc->GetNuisanceParameters()->Print("v");
  
  //----------------------------
  double imass=125.0;
  TString sname ="AllSYS";
  TString ifname =  Form("updated_with_snapshots/%s_%3.0f_combined_%s_model.root",pname.Data(),imass,sname.Data());
  
  TFile *_ifile = TFile::Open(ifname);
  cout << "Reading workspace = wspace and dataset = asimovData_1 from ifile = " << ifname << endl;
  RooWorkspace *iws = (RooWorkspace*)_ifile->Get("wspace");
  //  iws->Print();
  RooDataSet *idata;
  //
  idata = (RooDataSet*)iws->data("asimovData_1");
  if (!idata) {
    cout << dataName << " not found in iws = combined in file = " << ifname << endl;
  } else {
    cout << "idata : " << endl;
    idata->Print("v");
  }
  int status_1 = w->import(*idata);
  cout << "import:: status_1 = " << status_1 << endl; 
  //
  const RooArgSet *igobs_1 = iws->set("conditionalGlobs_1");
  assert(igobs_1);
  iws->loadSnapshot("conditionalGlobs_1");
  //
  const RooArgSet *gobs_1= mc->GetGlobalObservables();
  gobs_1 = igobs_1;
  w->saveSnapshot("conditionalGlobs_1",*gobs_1,true);
  if (verbose) {cout << "gobs_1 : " << endl; gobs_1->Print("v");}
  //
  const RooArgSet *inuis_1 = iws->set("conditionalNuis_1"); 
  iws->loadSnapshot("conditionalNuis_1");
  //
  const RooArgSet *nuis_1 = mc->GetNuisanceParameters();
  nuis_1 = inuis_1;
  w->saveSnapshot("conditionalNuis_1",*nuis_1,true);
  if (verbose) {cout << "nuis_1 : " << endl; nuis_1->Print("v"); }
  //
  _ifile->Close();
  //--------------------------
  */
  //--------------------------
  w->loadSnapshot("nominalGlobs");
  w->loadSnapshot("nominalNuis");
  RooDataSet* asimovData_1 = (RooDataSet*)w->data("asimovData_1");
  if (!asimovData_1)
    {
      asimovData_1 = makeAsimovData(conditionalExpected, obs_nll, 1);
      //asimovData_0 = makeAsimovData2((conditionalExpected ? obs_nll : (RooNLLVar*)NULL), 0., 0.);
    }
  int asimov1_status=global_status;
  
  cout << "asimov1_status = " << asimov1_status << endl;
  cout << "AsimovData_1:: " << endl; asimovData_1->Print("v");
  cout << "AsimovData_1:: isWeighted = " << asimovData_1->isWeighted() << " isNonPoissonWeighted = " << asimovData_1->isNonPoissonWeighted() << endl;
  cout << "AsimovData_1:: numEntries = " << asimovData_1->numEntries() << " sumEntries = " << asimovData_1->sumEntries() << endl;
  
  w->loadSnapshot("nominalGlobs");
  w->loadSnapshot("nominalNuis");

  asimov_1_nll = createNLL(asimovData_1);//(RooNLLVar*)pdf->createNLL(*asimovData_0);
  map_snapshots[asimov_1_nll] = "conditionalGlobs_1";
  map_data_nll[asimovData_1] = asimov_1_nll;
  setMu(1);
  map_muhat[asimov_1_nll] = 1;
  saveSnapshot(asimov_1_nll, 1); //<--------------
  w->loadSnapshot("conditionalNuis_1");
  w->loadSnapshot("conditionalGlobs_1");
  map_nll_muhat[asimov_1_nll] = asimov_1_nll->getVal();

  /*
  //--------------------------

  cout << "Loading conditionalGlobs_0 2" << endl;
  w->loadSnapshot("conditionalGlobs_0");
  mc->GetGlobalObservables()->Print("v");

  cout << "Loading conditionalNuis_0 2" << endl;
  w->loadSnapshot("conditionalNuis_0");
  mc->GetNuisanceParameters()->Print("v");

  //--------------------------

  cout << "Loading nominalGlobs 2" << endl;
  w->loadSnapshot("nominalGlobs");
  mc->GetGlobalObservables()->Print("v");

  cout << "Loading nominalNuis 2" << endl;
  w->loadSnapshot("nominalNuis");
  mc->GetNuisanceParameters()->Print("v");

  //--------------------------
  */

  double one_limit = doOne ? getLimit(asimov_1_nll,  mu_up_p1_approx) :  1;
  int one_status=global_status;

  //--------------------------

  w->loadSnapshot("conditionalNuis_0");
  cout << "doObs = " << doObs << endl;
  double obs_limit = doObs ? getLimit(obs_nll, med_limit) : 0;
  int obs_status=global_status;

  bool hasFailures = false;
  if (obs_status != 0 || med_status != 0 || asimov0_status != 0) hasFailures = true;
  for (map<int, int>::iterator itr=N_status.begin();itr!=N_status.end();itr++)
  {
    if (itr->second != 0) hasFailures = true;
  }
  if (hasFailures)
  {
    cout << "--------------------------------" << endl;
    cout << "Unresolved fit failures detected" << endl;
    cout << "Asimov0:  " << asimov0_status << endl;
    for (map<int, int>::iterator itr=N_status.begin();itr!=N_status.end();itr++)
    {
      cout << "+" << itr->first << "sigma:  " << itr->first << endl;
    }
    cout << "Median:   " << med_status << endl;
    cout << "Observed: " << obs_status << endl;
    cout << "--------------------------------" << endl;
  }

  if (betterBands) cout << "Guess for bands" << endl;
  cout << "+2sigma:  " << mu_up_p2_approx << endl;
  cout << "+1sigma:  " << mu_up_p1_approx << endl;
  cout << "-1sigma:  " << mu_up_n1_approx << endl;
  cout << "-2sigma:  " << mu_up_n2_approx << endl;
  if (betterBands)
  {
    cout << endl;
    cout << "Correct bands" << endl;
    cout << "+2sigma:  " << mu_up_p2 << endl;
    cout << "+1sigma:  " << mu_up_p1 << endl;
    cout << "-1sigma:  " << mu_up_n1 << endl;
    cout << "-2sigma:  " << mu_up_n2 << endl;

  }

  cout << "Median:   " << med_limit << endl;
  cout << "Observed: " << obs_limit << endl;
  cout << endl;
  
  cout << Form("aCLs  [ obs/exp/+2/+1/-1/-2sigma UL at %2.0f %% CL for %20s at %4.1f GeV ] = ",confidenceLevel*100, pname.Data(),xmass) 
       << Form(" %6.02f", obs_limit) 
       << Form(" %6.02f", med_limit)
       << Form(" %6.02f", mu_up_p2)
       << Form(" %6.02f", mu_up_p1)
       << Form(" %6.02f", mu_up_n1)
       << Form(" %6.02f", mu_up_n2)
       << endl;

  cout << Form("CLsOne = %6.02f",one_limit) << endl;
   
  /*
  if (doOne && !doBlind) {
    //TString infile("tautau/results/lhv12_125_combined_AllSYS_model.root");
    TObjArray *arr_infilename = TString(infile).Tokenize("/");
    TObjString* str_infilename = arr_infilename->GetEntries() ? (TObjString*)arr_infilename->At(arr_infilename->GetEntries()-1) : 0;
    TString infilename=str_infilename->String();
    cout << "infile = " << infile << endl;
    cout << "infilename = " << infilename << endl;

    RooWorkspace* wspace = new RooWorkspace("wspace");
    wspace->import(*asimovData_1);
    //
    const RooArgSet *igobs_1 = w->set("conditionalGlobs_1");
    w->loadSnapshot("conditionalGlobs_1");
    if (verbose) igobs_1->Print("v");
    wspace->import(*igobs_1);
    wspace->defineSet("conditionalGlobs_1",*igobs_1,true);
    //
    const RooArgSet *inuis_1 = w->set("conditionalNuis_1");
    w->loadSnapshot("conditionalNuis_1");
    if (verbose) inuis_1->Print("v");
    wspace->import(*inuis_1);
    wspace->defineSet("conditionalNuis_1",*inuis_1,true);
    //
    if (verbose) wspace->Print();
    system("mkdir -vp updated_with_snapshots");
    TString ofname= Form("updated_with_snapshots/%s",infilename.Data());
    wspace->writeToFile(ofname);
    cout << "Saved workspace = wspace to ofname = " << ofname << endl;
  }
  */

  f->Close();
  
  /*
  system(("mkdir -vp root-files/" + folder).c_str());
  
  stringstream fileName;
  fileName << "root-files/" << folder << "/" << mass << ".root";
  TFile fout(fileName.str().c_str(),"recreate");

  TH1D* h_lim = new TH1D("limit","limit",7,0,7);
  h_lim->SetBinContent(1, obs_limit);
  h_lim->SetBinContent(2, med_limit);
  h_lim->SetBinContent(3, mu_up_p2);
  h_lim->SetBinContent(4, mu_up_p1);
  h_lim->SetBinContent(5, mu_up_n1);
  h_lim->SetBinContent(6, mu_up_n2);
  h_lim->SetBinContent(7, global_status);

  h_lim->GetXaxis()->SetBinLabel(1, "Observed");
  h_lim->GetXaxis()->SetBinLabel(2, "Expected");
  h_lim->GetXaxis()->SetBinLabel(3, "+2sigma");
  h_lim->GetXaxis()->SetBinLabel(4, "+1sigma");
  h_lim->GetXaxis()->SetBinLabel(5, "-1sigma");
  h_lim->GetXaxis()->SetBinLabel(6, "-2sigma");
  h_lim->GetXaxis()->SetBinLabel(7, "Global status"); // do something with this later

  TH1D* h_lim_old = new TH1D("limit_old","limit_old",7,0,7); // include also old approximation of bands
  h_lim_old->SetBinContent(1, obs_limit);
  h_lim_old->SetBinContent(2, med_limit);
  h_lim_old->SetBinContent(3, mu_up_p2_approx);
  h_lim_old->SetBinContent(4, mu_up_p1_approx);
  h_lim_old->SetBinContent(5, mu_up_n1_approx);
  h_lim_old->SetBinContent(6, mu_up_n2_approx);
  h_lim_old->SetBinContent(7, global_status);

  h_lim_old->GetXaxis()->SetBinLabel(1, "Observed");
  h_lim_old->GetXaxis()->SetBinLabel(2, "Expected");
  h_lim_old->GetXaxis()->SetBinLabel(3, "+2sigma");
  h_lim_old->GetXaxis()->SetBinLabel(4, "+1sigma");
  h_lim_old->GetXaxis()->SetBinLabel(5, "-1sigma");
  h_lim_old->GetXaxis()->SetBinLabel(6, "-2sigma");
  h_lim_old->GetXaxis()->SetBinLabel(7, "Global status"); 

  fout.Write();
  fout.Close();
  */
  cout << "Finished with " << nrMinimize << " calls to minimize(nll)" << endl;
  timer.Print();
}

double getLimit(RooNLLVar* nll, double initial_guess)
{
  cout << "------------------------" << endl;
  cout << "Getting limit for nll: " << nll->GetName() << endl;
  //get initial guess based on muhat and sigma(muhat)
  firstPOI->setConstant(0);
  global_status=0;

  if (nll == asimov_0_nll) 
  {
    setMu(0);
    firstPOI->setConstant(1);
  }

  double muhat;
  if (map_nll_muhat.find(nll) == map_nll_muhat.end())
  {
    cout << "map_nll_muhat does not contain nll " << endl;
    double nll_val = getNLL(nll);
    muhat = firstPOI->getVal();
    saveSnapshot(nll, muhat);
    map_muhat[nll] = muhat;
    if (muhat < 0 && doTilde)
    {
      setMu(0);
      firstPOI->setConstant(1);
      nll_val = getNLL(nll);
    }

    map_nll_muhat[nll] = nll_val;
  }
  else
  {
    muhat = map_muhat[nll];
    cout << "muhat set from map_muhat to be = " << muhat << endl;
  }

  if (muhat < 0.1 || initial_guess != 0) setMu(initial_guess);
  double qmu,qmuA;
  double sigma_guess = getSigma(asimov_0_nll, firstPOI->getVal(), 0, qmu);
  double sigma_b = sigma_guess;
  double mu_guess = findCrossing(sigma_guess, sigma_b, muhat);
  double pmu = calcPmu(qmu, sigma_b, mu_guess);
  double pb = calcPb(qmu, sigma_b, mu_guess);
  double CLs = calcCLs(qmu, sigma_b, mu_guess);
  double qmu95 = getQmu95(sigma_b, mu_guess);
  setMu(mu_guess);

  cout << "Initial guess:  " << mu_guess << endl;
  cout << "Sigma(obs):     " << sigma_guess << endl;
  cout << "Sigma(mu,0):    " << sigma_b << endl;
  cout << "muhat:          " << muhat << endl;
  cout << "qmu95:          " << qmu95 << endl;
  cout << "qmu:            " << qmu << endl;
  cout << "pmu:            " << pmu << endl;
  cout << "1-pb:           " << pb << endl;
  cout << "CLs:            " << CLs << endl;
  cout << endl;

  int nrDamping = 1;
  map<double, double> guess_to_corr;
  double damping_factor = 1.0;
  //double damping_factor_pre = damping_factor;
  int nrItr = 0;
  double mu_pre = muhat;//mu_guess-10*precision*mu_guess;
  double mu_pre2 = muhat;
  while (fabs(mu_pre-mu_guess) > precision*mu_guess*direction)
  {
    cout << "----------------------" << endl;
    cout << "Starting iteration " << nrItr << " of " << nll->GetName() << endl;
    // do this to avoid comparing multiple minima in the conditional and unconditional fits
    if (nrItr == 0) loadSnapshot(nll, muhat);
    else if (usePredictiveFit) doPredictiveFit(nll, mu_pre2, mu_pre, mu_guess);
    else loadSnapshot(asimov_0_nll, mu_pre);

    sigma_guess=getSigma(nll, mu_guess, muhat, qmu);
    saveSnapshot(nll, mu_guess);


    if (nll != asimov_0_nll)
    {
      if (nrItr == 0) loadSnapshot(asimov_0_nll, map_nll_muhat[asimov_0_nll]);
      else if (usePredictiveFit) 
      {
	if (nrItr == 1) doPredictiveFit(nll, map_nll_muhat[asimov_0_nll], mu_pre, mu_guess);
	else doPredictiveFit(nll, mu_pre2, mu_pre, mu_guess);
      }
      else loadSnapshot(asimov_0_nll, mu_pre);

      sigma_b=getSigma(asimov_0_nll, mu_guess, 0, qmuA);
      saveSnapshot(asimov_0_nll, mu_guess);
    }
    else
    {
      sigma_b=sigma_guess;
      qmuA=qmu;
    }

    double corr = damping_factor*(mu_guess - findCrossing(sigma_guess, sigma_b, muhat));
    for (map<double, double>::iterator itr=guess_to_corr.begin();itr!=guess_to_corr.end();itr++)
    {
      if (fabs(itr->first - (mu_guess-corr)) < direction*mu_guess*0.02 && fabs(corr) > direction*mu_guess*precision) 
      {
	damping_factor *= 0.8;
	cout << "Changing damping factor to " << damping_factor << ", nrDamping = " << nrDamping << endl;
	if (nrDamping++ > 10)
	{
	  nrDamping = 1;
	  damping_factor = 1.0;
	}
	corr *= damping_factor;
	break;
      }
    }

    //subtract off the difference in the new and damped correction
    guess_to_corr[mu_guess] = corr;
    mu_pre2 = mu_pre;
    mu_pre = mu_guess;
    mu_guess -= corr;


    pmu = calcPmu(qmu, sigma_b, mu_pre);
    pb = calcPb(qmu, sigma_b, mu_pre);
    CLs = calcCLs(qmu, sigma_b, mu_pre);
    qmu95 = getQmu95(sigma_b, mu_pre);


    cout << "NLL:            " << nll->GetName() << endl;
    cout << "Previous guess: " << mu_pre << endl;
    cout << "Sigma(obs):     " << sigma_guess << endl;
    cout << "Sigma(mu,0):    " << sigma_b << endl;
    cout << "muhat:          " << muhat << endl;
    cout << "pmu:            " << pmu << endl;
    cout << "1-pb:           " << pb << endl;
    cout << "CLs:            " << CLs << endl;
    cout << "qmu95:          " << qmu95 << endl;
    cout << "qmu:            " << qmu << endl;
    cout << "qmuA0:          " << qmuA << endl;
    cout << "Precision:      " << direction*mu_guess*precision << endl;
    cout << "Correction:    "  << (-corr<0?" ":"") << -corr << endl;
    cout << "New guess:      " << mu_guess << endl;
    cout << endl;

      

    nrItr++;
    if (nrItr > 25)
    {
      cout << "Infinite loop detected in getLimit(). Please intervene." << endl;
      break;
    }
  }


  cout << "Found limit for nll " << nll->GetName() << ": " << mu_guess << endl;
  cout << "Finished in " << nrItr << " iterations." << endl;
  cout << endl;
  return mu_guess;
}


double getSigma(RooNLLVar* nll, double mu, double muhat, double& qmu)
{
  qmu = getQmu(nll, mu);
  if (verbose) cout << "qmu = " << qmu << endl;
  if (mu*direction < muhat) return fabs(mu-muhat)/sqrt(qmu);
  else if (muhat < 0 && doTilde) return sqrt(mu*mu-2*mu*muhat*direction)/sqrt(qmu);
  else return (mu-muhat)*direction/sqrt(qmu);
}

double getQmu(RooNLLVar* nll, double mu)
{
  double nll_muhat = map_nll_muhat[nll];
  bool isConst = firstPOI->isConstant();
  firstPOI->setConstant(1);
  setMu(mu);
  if (verbose) cout << "mu set to be = " << mu << endl;
  double nll_val = getNLL(nll);
  firstPOI->setConstant(isConst);
  if (verbose) cout << "mu = " << mu << " qmu = 2 * (" << nll_val << " - " << nll_muhat << ")" << endl;
  return 2*(nll_val-nll_muhat);
}

void saveSnapshot(RooNLLVar* nll, double mu)
{
  stringstream snapshotName;
  snapshotName << nll->GetName() << "_" << mu;
  //  w->defineSet(snapshotName.str().c_str(), *mc->GetNuisanceParameters(), true);
  w->saveSnapshot(snapshotName.str().c_str(), *mc->GetNuisanceParameters(), true);
  if (verbose) cout << "Saved Snapshot = " << snapshotName.str().c_str() << endl;
}

void loadSnapshot(RooNLLVar* nll, double mu)
{
  stringstream snapshotName;
  snapshotName << nll->GetName() << "_" << mu;
  if (w->loadSnapshot(snapshotName.str().c_str())) {
    if (verbose) cout << "Loaded snapshot = " << snapshotName.str().c_str() << endl;
  } else {
    if (verbose) cout << "No snapshot found with name = " << snapshotName.str().c_str() << endl;
  }
}

void doPredictiveFit(RooNLLVar* nll, double mu1, double mu2, double mu)
{
  if (fabs(mu2-mu) < direction*mu*precision*4)
  {
    loadSnapshot(nll, mu2);
    return;
  }

//extrapolate to mu using mu1 and mu2 assuming nuis scale linear in mu
  const RooArgSet* nuis = mc->GetNuisanceParameters();
  int nrNuis = nuis->getSize();
  double* theta_mu1 = new double[nrNuis];
  double* theta_mu2 = new double[nrNuis];

  TIterator* itr = nuis->createIterator();
  RooRealVar* var;
  int counter = 0;
  loadSnapshot(nll, mu1);
  while ((var == (RooRealVar*)itr->Next()))
  {
    theta_mu1[counter++] = var->getVal();
  }

  itr->Reset();
  counter = 0;
  loadSnapshot(nll, mu2);
  while ((var == (RooRealVar*)itr->Next()))
  {
    theta_mu2[counter++] = var->getVal();
  }

  itr->Reset();
  counter = 0;
  while ((var == (RooRealVar*)itr->Next()))
  {
    double m = (theta_mu2[counter] - theta_mu1[counter])/(mu2-mu1);
    double b = theta_mu2[counter] - m*mu2;
    double theta_extrap = m*mu+b;
    
    var->setVal(theta_extrap);
    counter++;
  }

  delete itr;
  delete theta_mu1;
  delete theta_mu2;
}

RooNLLVar* createNLL(RooDataSet* _data)
{
  RooArgSet nuis = *mc->GetNuisanceParameters();
  RooNLLVar* nll = (RooNLLVar*)mc->GetPdf()->createNLL(*_data, Constrain(nuis));
  if (verbose) cout << "created RooNLLVar with name = " << nll->GetName() << endl;
  return nll;
}

double getNLL(RooNLLVar* nll)
{
  string snapshotName = map_snapshots[nll];
  if (verbose) cout << "GetNLL for nll = " << nll->GetName() << " Snapshotname = " << snapshotName.c_str() << endl;
  if (snapshotName != "") {
    if (w->loadSnapshot(snapshotName.c_str())) {
      if (verbose) cout << "Loaded snapshot = " << snapshotName.c_str() << endl;
    } else {
      if (verbose) cout << "No snapshot found with name = " << snapshotName.c_str() << endl;
    }
  }
  minimize(nll);
  double val = nll->getVal();
  w->loadSnapshot("nominalGlobs");
  return val;
}


double findCrossing(double sigma_obs, double sigma, double muhat)
{
  double mu_guess = muhat + ROOT::Math::gaussian_quantile(1-target_CLs,1)*sigma_obs*direction;
  int nrItr = 0;
  int nrDamping = 1;

  map<double, double> guess_to_corr;
  double damping_factor = 1.0;
  double mu_pre = mu_guess - 10*mu_guess*precision;
  while (fabs(mu_guess-mu_pre) > direction*mu_guess*precision)
  {
    double sigma_obs_extrap = sigma_obs;
    double eps = 0;
    if (extrapolateSigma)
    {
      //map<double, double> map_mu_sigma = map_nll_mu_sigma[nll];
    }

    mu_pre = mu_guess;

    double qmu95 = getQmu95(sigma, mu_guess);
    double qmu;
    qmu = 1./sigma_obs_extrap/sigma_obs_extrap*(mu_guess-muhat)*(mu_guess-muhat);
    if (muhat < 0 && doTilde) qmu = 1./sigma_obs_extrap/sigma_obs_extrap*(mu_guess*mu_guess-2*mu_guess*muhat);

    double dqmu_dmu = 2*(mu_guess-muhat)/sigma_obs_extrap/sigma_obs_extrap - 2*qmu*eps;

    double corr = damping_factor*(qmu-qmu95)/dqmu_dmu;
    for (map<double, double>::iterator itr=guess_to_corr.begin();itr!=guess_to_corr.end();itr++)
    {
      if (fabs(itr->first - mu_guess) < direction*mu_guess*precision) 
      {
	damping_factor *= 0.8;
	if (verbose) cout << "Changing damping factor to " << damping_factor << ", nrDamping = " << nrDamping << endl;
	if (nrDamping++ > 10)
	{
	  nrDamping = 1;
	  damping_factor = 1.0;
	}
	corr *= damping_factor;
	break;
      }
    }
    guess_to_corr[mu_guess] = corr;

    mu_guess = mu_guess - corr;
    nrItr++;
    if (nrItr > 100)
    {
      cout << "Infinite loop detected in findCrossing. Please intervene." << endl;
      exit(1);
    }
    if (verbose) cout << "mu_guess = " << mu_guess << ", mu_pre = " << mu_pre << ", qmu = " << qmu << ", qmu95 = " << qmu95 << ", sigma_obs_extrap = " << sigma_obs_extrap << ", sigma = " << sigma << ", direction*mu*prec = " << direction*mu_guess*precision << endl;
  }

  return mu_guess;
}

void setMu(double mu)
{
  if (mu != mu)
  {
    cout << "ERROR::POI gave nan. Please intervene." << endl;
    exit(1);
  }
  if (mu > 0 && firstPOI->getMax() < mu) firstPOI->setMax(2*mu);
  if (mu < 0 && firstPOI->getMin() > mu) firstPOI->setMin(2*mu);
  firstPOI->setVal(mu);
}


double getQmu95_brute(double sigma, double mu)
{
  double step_size = 0.001;
  double start = step_size;
  if (mu/sigma > 0.2) start = 0;
  for (double qmu=start;qmu<20;qmu+=step_size)
  {
    double CLs = calcCLs(qmu, sigma, mu);

    if (CLs < target_CLs) return qmu;
  }

  return 20;
}

double getQmu95(double sigma, double mu)
{
  double qmu95 = 0;
  //no sane man would venture this far down into |mu/sigma|
  double target_N = ROOT::Math::gaussian_cdf(1-target_CLs,1);
  if (fabs(mu/sigma) < 0.25*target_N)
  {
    qmu95 = 5.83/target_N;
  }
  else
  {
    map<double, double> guess_to_corr;
    double qmu95_guess = pow(ROOT::Math::gaussian_quantile(1-target_CLs,1),2);
    int nrItr = 0;
    int nrDamping = 1;
    double damping_factor = 1.0;
    double qmu95_pre = qmu95_guess - 10*2*qmu95_guess*precision;
    while (fabs(qmu95_guess-qmu95_pre) > 2*qmu95_guess*precision)
    {
      qmu95_pre = qmu95_guess;
      if (verbose)
      {
	cout << "qmu95_guess = " << qmu95_guess << endl;
	cout << "CLs = " << calcCLs(qmu95_guess, sigma, mu) << endl;
	cout << "Derivative = " << calcDerCLs(qmu95_guess, sigma, mu) << endl;
      }

      double corr = damping_factor*(calcCLs(qmu95_guess, sigma, mu)-target_CLs)/calcDerCLs(qmu95_guess, sigma, mu);
      for (map<double, double>::iterator itr=guess_to_corr.begin();itr!=guess_to_corr.end();itr++)
      {
	if (fabs(itr->first - qmu95_guess) < 2*qmu95_guess*precision) 
	{
	  damping_factor *= 0.8;
	  if (verbose) cout << "Changing damping factor to " << damping_factor << ", nrDamping = " << nrDamping << endl;
	  if (nrDamping++ > 10)
	  {
	    nrDamping = 1;
	    damping_factor = 1.0;
	  }
	  corr *= damping_factor;
	}
      }

      guess_to_corr[qmu95_guess] = corr;
      qmu95_guess = qmu95_guess - corr;

      if (verbose)
      {
	cout << "next guess = " << qmu95_guess << endl; 
	cout << "precision = " << 2*qmu95_guess*precision << endl;
	cout << endl;
      }
      nrItr++;
      if (nrItr > 200)
      {
	cout << "Infinite loop detected in getQmu95. Please intervene." << endl;
	exit(1);
      }
    }
    qmu95 = qmu95_guess;
  }

  if (qmu95 != qmu95) 
  {
    qmu95 = getQmu95_brute(sigma, mu);
  }
  if (verbose) cout << "Returning qmu95 = " << qmu95 << endl;

  return qmu95;
}

double calcCLs(double qmu_tilde, double sigma, double mu)
{
  double pmu = calcPmu(qmu_tilde, sigma, mu);
  double pb = calcPb(qmu_tilde, sigma, mu);
  if (verbose)
  {
    cout << "pmu = " << pmu << endl;
    cout << "pb = " << pb << endl;
  }
  if (pb == 1) return 0.5;
  return pmu/(1-pb);
}

double calcPmu(double qmu, double sigma, double mu)
{
  double pmu;
  if (qmu < mu*mu/(sigma*sigma) || !doTilde)
  {
    pmu = 1-ROOT::Math::gaussian_cdf(sqrt(qmu));
  }
  else
  {
    pmu = 1-ROOT::Math::gaussian_cdf((qmu+mu*mu/(sigma*sigma))/(2*fabs(mu/sigma)));
  }
  if (verbose) cout << "for pmu, qmu = " << qmu << ", sigma = " << sigma<< ", mu = " << mu << ", pmu = " << pmu << endl;
  return pmu;
}

double calcPb(double qmu, double sigma, double mu)
{
  if (qmu < mu*mu/(sigma*sigma) || !doTilde)
  {
    return 1-ROOT::Math::gaussian_cdf(fabs(mu/sigma) - sqrt(qmu));
  }
  else
  {
    return 1-ROOT::Math::gaussian_cdf((mu*mu/(sigma*sigma) - qmu)/(2*fabs(mu/sigma)));
  }
}

double calcDerCLs(double qmu, double sigma, double mu)
{
  double dpmu_dq = 0;
  double d1mpb_dq = 0;

  if (qmu < mu*mu/(sigma*sigma))
  {
    double zmu = sqrt(qmu);
    dpmu_dq = -1./(2*sqrt(qmu*2*TMath::Pi()))*exp(-zmu*zmu/2);
  }
  else 
  {
    double zmu = (qmu+mu*mu/(sigma*sigma))/(2*fabs(mu/sigma));
    dpmu_dq = -1./(2*fabs(mu/sigma))*1./(sqrt(2*TMath::Pi()))*exp(-zmu*zmu/2);
  }

  if (qmu < mu*mu/(sigma*sigma))
  {
    double zb = fabs(mu/sigma)-sqrt(qmu);
    d1mpb_dq = -1./sqrt(qmu*2*TMath::Pi())*exp(-zb*zb/2);
  }
  else
  {
    double zb = (mu*mu/(sigma*sigma) - qmu)/(2*fabs(mu/sigma));
    d1mpb_dq = -1./(2*fabs(mu/sigma))*1./(sqrt(2*TMath::Pi()))*exp(-zb*zb/2);
  }

  double pb = calcPb(qmu, sigma, mu);
  return dpmu_dq/(1-pb)-calcCLs(qmu, sigma, mu)/(1-pb)*d1mpb_dq;
}

int minimize(RooNLLVar* nll)
{
  nrMinimize++;
  RooAbsReal* fcn = (RooAbsReal*)nll;
  return minimize(fcn);
}

int minimize(RooAbsReal* fcn)
{
  static int nrItr = 0;
  if (verbose) cout << "Starting minimization. Using these global observables" << endl;
  if (verbose) mc->GetGlobalObservables()->Print("v");


  int printLevel = ROOT::Math::MinimizerOptions::DefaultPrintLevel();
  RooFit::MsgLevel msglevel = RooMsgService::instance().globalKillBelow();
  if (printLevel < 0) RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);

  int strat = ROOT::Math::MinimizerOptions::DefaultStrategy();
  int save_strat = strat;
  RooMinimizer minim(*fcn);
  minim.setStrategy(strat);
  minim.setPrintLevel(printLevel);


  int status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());


//up the strategy
  if (status != 0 && status != 1 && strat < 2)
  {
    strat++;
    cout << "Fit failed with status " << status << ". Retrying with strategy " << strat << endl;
    minim.setStrategy(strat);
    status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
  }

  if (status != 0 && status != 1 && strat < 2)
  {
    strat++;
    cout << "Fit failed with status " << status << ". Retrying with strategy " << strat << endl;
    minim.setStrategy(strat);
    status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
  }

  if (verbose) cout << "status is " << status << endl;

// //switch minuit version and try again
  if (status != 0 && status != 1)
  {
    string minType = ROOT::Math::MinimizerOptions::DefaultMinimizerType();
    string newMinType;
    if (minType == "Minuit2") newMinType = "Minuit";
    else newMinType = "Minuit2";
  
    cout << "Switching minuit type from " << minType << " to " << newMinType << endl;
  
    ROOT::Math::MinimizerOptions::SetDefaultMinimizer(newMinType.c_str());
    strat = ROOT::Math::MinimizerOptions::DefaultStrategy();
    minim.setStrategy(strat);

    status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());


    if (status != 0 && status != 1 && strat < 2)
    {
      strat++;
      cout << "Fit failed with status " << status << ". Retrying with strategy " << strat << endl;
      minim.setStrategy(strat);
      status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
    }

    if (status != 0 && status != 1 && strat < 2)
    {
      strat++;
      cout << "Fit failed with status " << status << ". Retrying with strategy " << strat << endl;
      minim.setStrategy(strat);
      status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
    }

    ROOT::Math::MinimizerOptions::SetDefaultMinimizer(minType.c_str());
  }

  if (status != 0 && status != 1)
  {
    nrItr++;
    if (nrItr > maxRetries)
    {
      nrItr = 0;
      global_status++;
      cout << "WARNING::Fit failure unresolved with status " << status << endl;
      return status;
    }
    else
    {
      if (nrItr == 0) // retry with mu=0 snapshot
      {
	w->loadSnapshot("conditionalNuis_0");
	return minimize(fcn);
      }
      else if (nrItr == 1) // retry with nominal snapshot
      {
	w->loadSnapshot("nominalNuis");
	return minimize(fcn);
      }
    }
  }

  if (printLevel < 0) RooMsgService::instance().setGlobalKillBelow(msglevel);
  ROOT::Math::MinimizerOptions::SetDefaultStrategy(save_strat);


  if (nrItr != 0) cout << "Successful fit" << endl;
  nrItr=0;
  return status;
}


/*
RooDataSet* makeAsimovData2(RooDataSet* conditioningData, double mu_true, double mu_prof, string* mu_str, string* mu_prof_str)
{
  RooNLLVar* conditioningNLL = NULL;
  if (conditioningData)
  {
    conditioningNLL = (RooNLLVar*)mc->GetPdf()->createNLL(*conditioningData);
  }
  return makeAsimovData2(conditioningNLL, mu_true, mu_prof, mu_str, mu_prof_str);
}


RooDataSet* makeAsimovData2(RooNLLVar* conditioningNLL, double mu_true, double mu_prof, string* mu_str, string* mu_prof_str)
{
  if (mu_prof == -999) mu_prof = mu_true;
  bool doTest = 0;

  cout << "Creating asimov data at mu = " << mu_true << ", profiling at mu = " << mu_prof << endl;
  int printLevel = ROOT::Math::MinimizerOptions::DefaultPrintLevel();

  int test = 0;
  if (doTest) cout << "test = " << test++ << endl;

  int _printLevel = 0;

  stringstream muStr;
  muStr << setprecision(5);
  muStr << "_" << mu_true;
  if (mu_str) *mu_str = muStr.str();

  stringstream muStrProf;
  muStrProf << setprecision(5);
  muStrProf << "_" << mu_prof;
  if (mu_prof_str) *mu_prof_str = muStrProf.str();


  if (doTest) cout << "test = " << test++ << endl;
  const RooArgSet* globs = mc->GetGlobalObservables();
  const RooArgSet* nuis = mc->GetNuisanceParameters();
  const RooArgSet* obs = mc->GetObservables();
  const RooArgSet* pois = mc->GetParametersOfInterest();

  TIterator* gItr = globs->createIterator();
  TIterator* nItr = nuis->createIterator();
  RooRealVar* var;

  //cout << "test = " << test++ << endl;
  RooArgSet emptySet;
  RooArgSet params(*nuis);
  params.add(*globs);
  params.add(*pois);
  w->saveSnapshot("initial_params", params);
  
  if (doTest) cout << "test = " << test++ << endl;

//condition the MLEs
  if (conditioningNLL)
  {
    //get the conditional MLEs
    firstPOI->setVal(mu_prof);
    firstPOI->setConstant(1);
    minimize(conditioningNLL);
  }

  if (doTest) cout << "test = " << test++ << endl;
  w->saveSnapshot(("conditionalNuis" +muStrProf.str()).c_str(),*nuis);


//to find the conditional globs, do a fit to the constraint only pdf with the globs floating and the MLEs constant    
  RooArgSet obsCopy = *obs;
  RooArgSet nuisCopy = *nuis;
    
  RooArgSet constraints(*mc->GetPdf()->getAllConstraints(obsCopy, nuisCopy));
  RooRealVar minusOne("minusOne","minusOne",-1);
  constraints.add(minusOne);
  RooProduct constrFunc("constrFunc","constrFunc",constraints);
    
  if (doTest) cout << "test = " << test++ << endl;
  while ((var = (RooRealVar*)gItr->Next()))
  {
    var->setConstant(false);
  }
  gItr->Reset();

  while ((var = (RooRealVar*)nItr->Next()))
  {
    var->setConstant(true);
  }
  nItr->Reset();

  minimize(&constrFunc);

  while ((var = (RooRealVar*)gItr->Next()))
  {
    var->setConstant(true);
  }
  gItr->Reset();

  while ((var = (RooRealVar*)nItr->Next()))
  {
    var->setConstant(false);
  }
  nItr->Reset();

  w->saveSnapshot(("conditionalGlobs"+muStrProf.str()).c_str(),*globs);
  

  if (doTest) cout << "test = " << test++ << endl;
  


//make the asimov data
  const char* weightName="weightVar";
  RooArgSet obsAndWeight;
  obsAndWeight.add(*mc->GetObservables());

  RooRealVar* weightVar = NULL;
  if (!(weightVar = w->var(weightName)))
  {
    w->import(*(new RooRealVar(weightName, weightName, 1,0,10000000)));
    weightVar = w->var(weightName);
  }
  obsAndWeight.add(*w->var(weightName));


  if (doTest) cout << "test = " << test++ << endl;

  RooSimultaneous* simPdf = dynamic_cast<RooSimultaneous*>(mc->GetPdf());
  map<string, RooDataSet*> asimovDataMap;

  //try fix for sim pdf
  RooCategory* channelCat = (RooCategory*)&simPdf->indexCat();
  TIterator* iter = channelCat->typeIterator() ;
  RooCatType* tt = NULL;
  int nrIndices = 0;
  int iFrame=0;
  while((tt=(RooCatType*) iter->Next())) {
    nrIndices++;
  }
  for (int i=0;i<nrIndices;i++){
    channelCat->setIndex(i);
    iFrame++;
    // Get pdf associated with state from simpdf
    RooAbsPdf* pdftmp = simPdf->getPdf(channelCat->getLabel()) ;
	
    // Generate observables defined by the pdf associated with this state
    RooArgSet* obstmp = pdftmp->getObservables(*mc->GetObservables()) ;

    if (_printLevel >= 1)
    {
      obstmp->Print();
      cout << "on type " << channelCat->getLabel() << " " << iFrame << endl;
    }

    RooDataSet* obsDataUnbinned = new RooDataSet(Form("combAsimovData%d",iFrame),Form("combAsimovData%d",iFrame),RooArgSet(obsAndWeight,*channelCat),WeightVar(*weightVar));
    RooRealVar* thisObs = ((RooRealVar*)obstmp->first());
    double expectedEvents = pdftmp->expectedEvents(*obstmp);
    double thisNorm = 0;
    for(int jj=0; jj<thisObs->numBins(); ++jj){
      thisObs->setBin(jj);

      thisNorm=pdftmp->getVal(obstmp)*thisObs->getBinWidth(jj);
      if (thisNorm*expectedEvents > 0 && thisNorm*expectedEvents < pow(10.0, 18)) obsDataUnbinned->add(*mc->GetObservables(), thisNorm*expectedEvents);
    }
    
    if (_printLevel >= 1)
    {
      obsDataUnbinned->Print();
      cout <<"sum entries "<<obsDataUnbinned->sumEntries()<<endl;
    }
    if(obsDataUnbinned->sumEntries()!=obsDataUnbinned->sumEntries()){
      cout << "sum entries is nan"<<endl;
      exit(1);
    }


    asimovDataMap[string(channelCat->getLabel())] = obsDataUnbinned;

    if (_printLevel >= 1)
    {
      cout << "channel: " << channelCat->getLabel() << ", data: ";
      obsDataUnbinned->Print();
      cout << endl;
    }
  }

  if (doTest) cout << "test = " << test++ << endl;
  RooDataSet* asimovData = new RooDataSet(("asimovData"+muStr.str()).c_str(),("asimovData"+muStr.str()).c_str(),RooArgSet(obsAndWeight,*channelCat),Index(*channelCat),Import(asimovDataMap),WeightVar(*weightVar));
  if (w->data(("asimovData"+muStr.str()).c_str()))
  {
    w->import(*asimovData, true);
  }
  else
  {
    w->import(*asimovData);
  }




  w->loadSnapshot("initial_params");
  ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(printLevel);

  if (doTest) cout << "test = " << test++ << endl;
  return asimovData;
  
}
*/

void unfoldConstraints(RooArgSet& initial, RooArgSet& final, RooArgSet& obs, RooArgSet& nuis, int& counter)
{
  if (counter > 50)
  {
    cout << "ERROR::Couldn't unfold constraints!" << endl;
    cout << "Initial: " << endl;
    initial.Print("v");
    cout << endl;
    cout << "Final: " << endl;
    final.Print("v");
    exit(1);
  }
  TIterator* itr = initial.createIterator();
  RooAbsPdf* pdf;
  while ((pdf = (RooAbsPdf*)itr->Next()))
  {
    RooArgSet nuis_tmp = nuis;
    RooArgSet constraint_set(*pdf->getAllConstraints(obs, nuis_tmp, false));
    //if (constraint_set.getSize() > 1)
    //{
    string className(pdf->ClassName());
    if (className != "RooGaussian" && className != "RooLognormal" && className != "RooGamma" && className != "RooPoisson" && className != "RooBifurGauss")
    {
      counter++;
      unfoldConstraints(constraint_set, final, obs, nuis, counter);
    }
    else
    {
      final.add(*pdf);
    }
  }
  delete itr;
}



RooDataSet* makeAsimovData(bool doConditional, RooNLLVar* conditioning_nll, double mu_val, string* mu_str, string* mu_prof_str, double mu_val_profile, bool doFit)
{
  if (mu_val_profile == -999) mu_val_profile = mu_val;


  cout << "Creating asimov data at mu = " << mu_val << ", profiling at mu = " << mu_val_profile << endl;

  //ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");
  //int strat = ROOT::Math::MinimizerOptions::SetDefaultStrategy(0);
  //int printLevel = ROOT::Math::MinimizerOptions::DefaultPrintLevel();
  //ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(-1);
  //RooMinuit::SetMaxIterations(10000);
  //RooMinimizer::SetMaxFunctionCalls(10000);

////////////////////
//make asimov data//
////////////////////
  RooAbsPdf* combPdf = mc->GetPdf();

  int _printLevel = 0;

  stringstream muStr;
  muStr << setprecision(5);
  muStr << "_" << mu_val;
  if (mu_str) *mu_str = muStr.str();

  stringstream muStrProf;
  muStrProf << setprecision(5);
  muStrProf << "_" << mu_val_profile;
  if (mu_prof_str) *mu_prof_str = muStrProf.str();

  RooRealVar* mu = (RooRealVar*)mc->GetParametersOfInterest()->first();//w->var("mu");
  mu->setVal(mu_val);

  RooArgSet mc_obs = *mc->GetObservables();
  RooArgSet mc_globs = *mc->GetGlobalObservables();
  RooArgSet mc_nuis = *mc->GetNuisanceParameters();

//pair the nuisance parameter to the global observable
  RooArgSet mc_nuis_tmp = mc_nuis;
  RooArgList nui_list("ordered_nuis");
  RooArgList glob_list("ordered_globs");
  RooArgSet constraint_set_tmp(*combPdf->getAllConstraints(mc_obs, mc_nuis_tmp, false));
  RooArgSet constraint_set;
  int counter_tmp = 0;
  unfoldConstraints(constraint_set_tmp, constraint_set, mc_obs, mc_nuis_tmp, counter_tmp);

  TIterator* cIter = constraint_set.createIterator();
  RooAbsArg* arg;
  while ((arg = (RooAbsArg*)cIter->Next()))
  {
    RooAbsPdf* pdf = (RooAbsPdf*)arg;
    if (!pdf) continue;
//     cout << "Printing pdf" << endl;
//     pdf->Print();
//     cout << "Done" << endl;
    TIterator* nIter = mc_nuis.createIterator();
    RooRealVar* thisNui = NULL;
    RooAbsArg* nui_arg;
    while ((nui_arg = (RooAbsArg*)nIter->Next()))
    {
      if (pdf->dependsOn(*nui_arg))
      {
	thisNui = (RooRealVar*)nui_arg;
	break;
      }
    }
    delete nIter;

    //RooRealVar* thisNui = (RooRealVar*)pdf->getObservables();


//need this incase the observable isn't fundamental. 
//in this case, see which variable is dependent on the nuisance parameter and use that.
    RooArgSet* components = pdf->getComponents();
//     cout << "\nPrinting components" << endl;
//     components->Print();
//     cout << "Done" << endl;
    components->remove(*pdf);
    if (components->getSize())
    {
      TIterator* itr1 = components->createIterator();
      RooAbsArg* arg1;
      while ((arg1 = (RooAbsArg*)itr1->Next()))
      {
	TIterator* itr2 = components->createIterator();
	RooAbsArg* arg2;
	while ((arg2 = (RooAbsArg*)itr2->Next()))
	{
	  if (arg1 == arg2) continue;
	  if (arg2->dependsOn(*arg1))
	  {
	    components->remove(*arg1);
	  }
	}
	delete itr2;
      }
      delete itr1;
    }
    if (components->getSize() > 1)
    {
      cout << "ERROR::Couldn't isolate proper nuisance parameter" << endl;
      return NULL;
    }
    else if (components->getSize() == 1)
    {
      thisNui = (RooRealVar*)components->first();
    }



    TIterator* gIter = mc_globs.createIterator();
    RooRealVar* thisGlob = NULL;
    RooAbsArg* glob_arg;
    while ((glob_arg = (RooAbsArg*)gIter->Next()))
    {
      if (pdf->dependsOn(*glob_arg))
      {
	thisGlob = (RooRealVar*)glob_arg;
	break;
      }
    }
    delete gIter;

    if (!thisNui || !thisGlob)
    {
      cout << "WARNING::Couldn't find nui or glob for constraint: " << pdf->GetName() << endl;
      //return;
      continue;
    }

    if (_printLevel >= 1) cout << "Pairing nui: " << thisNui->GetName() << ", with glob: " << thisGlob->GetName() << ", from constraint: " << pdf->GetName() << endl;

    nui_list.add(*thisNui);
    glob_list.add(*thisGlob);

//     cout << "\nPrinting Nui/glob" << endl;
//     thisNui->Print();
//     cout << "Done nui" << endl;
//     thisGlob->Print();
//     cout << "Done glob" << endl;
  }
  delete cIter;




//save the snapshots of nominal parameters, but only if they're not already saved
  w->saveSnapshot("tmpGlobs",*mc->GetGlobalObservables());
  w->saveSnapshot("tmpNuis",*mc->GetNuisanceParameters());
  if (!w->loadSnapshot("nominalGlobs"))
  {
    cout << "nominalGlobs doesn't exist. Saving snapshot." << endl;
    w->saveSnapshot("nominalGlobs",*mc->GetGlobalObservables());
  }
  else w->loadSnapshot("tmpGlobs");
  if (!w->loadSnapshot("nominalNuis"))
  {
    cout << "nominalNuis doesn't exist. Saving snapshot." << endl;
    w->saveSnapshot("nominalNuis",*mc->GetNuisanceParameters());
  }
  else w->loadSnapshot("tmpNuis");

  RooArgSet nuiSet_tmp(nui_list);

  mu->setVal(mu_val_profile);
  mu->setConstant(1);
  //int status = 0;
  if (doConditional && doFit)
  {
    minimize(conditioning_nll);
    // cout << "Using globs for minimization" << endl;
    // mc->GetGlobalObservables()->Print("v");
    // cout << "Starting minimization.." << endl;
    // RooAbsReal* nll;
    // if (!(nll = map_data_nll[combData])) nll = combPdf->createNLL(*combData, RooFit::Constrain(nuiSet_tmp));
    // RooMinimizer minim(*nll);
    // minim.setStrategy(0);
    // minim.setPrintLevel(1);
    // status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
    // if (status != 0)
    // {
    //   cout << "Fit failed for mu = " << mu->getVal() << " with status " << status << endl;
    // }
    // cout << "Done" << endl;

    //combPdf->fitTo(*combData,Hesse(false),Minos(false),PrintLevel(0),Extended(), Constrain(nuiSet_tmp));
  }
  mu->setConstant(0);
  mu->setVal(mu_val);
  


//loop over the nui/glob list, grab the corresponding variable from the tmp ws, and set the glob to the value of the nui
  int nrNuis = nui_list.getSize();
  if (nrNuis != glob_list.getSize())
  {
    cout << "ERROR::nui_list.getSize() != glob_list.getSize()!" << endl;
    return NULL;
  }

  for (int i=0;i<nrNuis;i++)
  {
    RooRealVar* nui = (RooRealVar*)nui_list.at(i);
    RooRealVar* glob = (RooRealVar*)glob_list.at(i);

    //cout << "nui: " << nui << ", glob: " << glob << endl;
    if (_printLevel >= 1) cout << "Setting glob: " << glob->GetName() << ", which had previous val: " << glob->getVal() << ", to conditional val: " << nui->getVal() << endl;

    glob->setVal(nui->getVal());
  }

  w->defineSet(("conditionalGlobs"+muStr.str()).c_str(),*mc->GetGlobalObservables(),true);
  w->defineSet(("conditionalNuis"+muStr.str()).c_str(), *mc->GetNuisanceParameters(),true);
  
  //save the snapshots of conditional parameters
  cout << "Saving conditional snapshots" << endl;
  cout << "Glob snapshot name = " << "conditionalGlobs"+muStrProf.str() << endl;
  cout << "Nuis snapshot name = " << "conditionalNuis"+muStrProf.str() << endl;
  w->saveSnapshot(("conditionalGlobs"+muStrProf.str()).c_str(),*mc->GetGlobalObservables(),true);
  w->saveSnapshot(("conditionalNuis" +muStrProf.str()).c_str(),*mc->GetNuisanceParameters(),true);
 
  if (_printLevel >= 1) {
    mc->GetGlobalObservables()->Print("v");
    mc->GetNuisanceParameters()->Print("v");
  }

  if (!doConditional)
  {
    w->loadSnapshot("nominalGlobs");
    w->loadSnapshot("nominalNuis");
  }

  if (_printLevel >= 1) cout << "Making asimov" << endl;
//make the asimov data (snipped from Kyle)
  mu->setVal(mu_val);

  int iFrame=0;

  const char* weightName="weightVar";
  RooArgSet obsAndWeight;
  //cout << "adding obs" << endl;
  obsAndWeight.add(*mc->GetObservables());
  //cout << "adding weight" << endl;

  RooRealVar* weightVar = NULL;
  if (!(weightVar = w->var(weightName)))
  {
    w->import(*(new RooRealVar(weightName, weightName, 1,0,10000000)));
    weightVar = w->var(weightName);
  }
  //cout << "weightVar: " << weightVar << endl;
  obsAndWeight.add(*w->var(weightName));

  //cout << "defining set" << endl;
  w->defineSet("obsAndWeight",obsAndWeight);


  //////////////////////////////////////////////////////
  //////////////////////////////////////////////////////
  //////////////////////////////////////////////////////
  //////////////////////////////////////////////////////
  //////////////////////////////////////////////////////
  // MAKE ASIMOV DATA FOR OBSERVABLES

  // dummy var can just have one bin since it's a dummy
  //if(w->var("ATLAS_dummyX"))  w->var("ATLAS_dummyX")->setBins(1);

  //cout <<" check expectedData by category"<<endl;
  //RooDataSet* simData=NULL;
  RooSimultaneous* simPdf = dynamic_cast<RooSimultaneous*>(mc->GetPdf());

  RooDataSet* asimovData;
  if (!simPdf)
  {
    // Get pdf associated with state from simpdf
    RooAbsPdf* pdftmp = mc->GetPdf();//simPdf->getPdf(channelCat->getLabel()) ;
	
    // Generate observables defined by the pdf associated with this state
    RooArgSet* obstmp = pdftmp->getObservables(*mc->GetObservables()) ;

    if (_printLevel >= 1)
    {
      obstmp->Print();
    }
    cout << "Creating RooDataSet:: " << ("asimovData"+muStr.str()).c_str() << endl;
    asimovData = new RooDataSet(("asimovData"+muStr.str()).c_str(),("asimovData"+muStr.str()).c_str(),RooArgSet(obsAndWeight),WeightVar(*weightVar));

    RooRealVar* thisObs = ((RooRealVar*)obstmp->first());
    double expectedEvents = pdftmp->expectedEvents(*obstmp);
    double thisNorm = 0;
    for(int jj=0; jj<thisObs->numBins(); ++jj){
      thisObs->setBin(jj);

      thisNorm=pdftmp->getVal(obstmp)*thisObs->getBinWidth(jj);
      if (thisNorm*expectedEvents <= 0)
      {
	cout << "WARNING::Detected bin with zero expected events (" << thisNorm*expectedEvents << ") ! Please check your inputs. Obs = " << thisObs->GetName() << ", bin = " << jj << endl;
      }
      if (thisNorm*expectedEvents > 0 && thisNorm*expectedEvents < pow(10.0, 18)) asimovData->add(*mc->GetObservables(), thisNorm*expectedEvents);
    }
    
    if (_printLevel >= 1)
    {
      asimovData->Print();
      cout <<"sum entries "<<asimovData->sumEntries()<<endl;
    }
    if(asimovData->sumEntries()!=asimovData->sumEntries()){
      cout << "sum entries is nan"<<endl;
      exit(1);
    }

    //((RooRealVar*)obstmp->first())->Print();
    //cout << "expected events " << pdftmp->expectedEvents(*obstmp) << endl;
     
    w->import(*asimovData);

    if (_printLevel >= 1)
    {
      asimovData->Print();
      cout << endl;
    }
  }
  else
  {
    map<string, RooDataSet*> asimovDataMap;

    
    //try fix for sim pdf
    RooCategory* channelCat = (RooCategory*)&simPdf->indexCat();//(RooCategory*)w->cat("master_channel");//(RooCategory*) (&simPdf->indexCat());
    //    TIterator* iter = simPdf->indexCat().typeIterator() ;
    TIterator* iter = channelCat->typeIterator() ;
    RooCatType* tt = NULL;
    int nrIndices = 0;
    while((tt=(RooCatType*) iter->Next())) {
      nrIndices++;
    }
    for (int i=0;i<nrIndices;i++){
      channelCat->setIndex(i);
      iFrame++;
      // Get pdf associated with state from simpdf
      RooAbsPdf* pdftmp = simPdf->getPdf(channelCat->getLabel()) ;
	
      // Generate observables defined by the pdf associated with this state
      RooArgSet* obstmp = pdftmp->getObservables(*mc->GetObservables()) ;

      if (_printLevel >= 1)
      {
	obstmp->Print();
	cout << "on type " << channelCat->getLabel() << " " << iFrame << endl;
      }
      cout << "Creating RooDataSet:: " << Form("combAsimovData%d",iFrame) << endl;
      RooDataSet* obsDataUnbinned = new RooDataSet(Form("combAsimovData%d",iFrame),Form("combAsimovData%d",iFrame),RooArgSet(obsAndWeight,*channelCat),WeightVar(*weightVar));
      RooRealVar* thisObs = ((RooRealVar*)obstmp->first());
      double expectedEvents = pdftmp->expectedEvents(*obstmp);
      double thisNorm = 0;
      for(int jj=0; jj<thisObs->numBins(); ++jj){
	thisObs->setBin(jj);
	thisNorm=pdftmp->getVal(obstmp)*thisObs->getBinWidth(jj);
	if (thisNorm*expectedEvents > 0 && thisNorm*expectedEvents < pow(10.0, 18)) obsDataUnbinned->add(*mc->GetObservables(), thisNorm*expectedEvents);
	if (_printLevel >= 1) cout << "jj = " << jj << " thisObs = " << thisObs->getVal() << " pdf = " << pdftmp->getVal(obstmp) << " thisNorm = " << thisNorm << " expectedEvents = " << expectedEvents << endl;
      }
    
      if (_printLevel >= 1)
      {
	obsDataUnbinned->Print("v");
	cout <<"sum entries "<<obsDataUnbinned->sumEntries()<<endl;
      }
      if(obsDataUnbinned->sumEntries()!=obsDataUnbinned->sumEntries()){
	cout << "sum entries is nan"<<endl;
	exit(1);
      }

      // ((RooRealVar*)obstmp->first())->Print();
      // cout << "pdf: " << pdftmp->GetName() << endl;
      // cout << "expected events " << pdftmp->expectedEvents(*obstmp) << endl;
      // cout << "-----" << endl;

      asimovDataMap[string(channelCat->getLabel())] = obsDataUnbinned;//tempData;

      if (_printLevel >= 1)
      {
	cout << "channel: " << channelCat->getLabel() << ", data: ";
	obsDataUnbinned->Print();
	cout << endl;
      }
    }
    cout << "Creating RooDataSet:: " << ("asimovData"+muStr.str()).c_str() << endl;
    asimovData = new RooDataSet(("asimovData"+muStr.str()).c_str(),("asimovData"+muStr.str()).c_str(),RooArgSet(obsAndWeight,*channelCat),Index(*channelCat),Import(asimovDataMap),WeightVar(*weightVar));
    w->import(*asimovData);
  }

//bring us back to nominal for exporting
  //w->loadSnapshot("nominalNuis");
  w->loadSnapshot("nominalGlobs");

  //ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(printLevel);

  return asimovData;
}

int main(int argc, char* argv[]){
  //
  gROOT->SetBatch(kTRUE);
  //
  //Argument variables
  //
  int imode = (argc>1) ? atoi(argv[1]) : 100;
  int isys  = (argc>2) ? atoi(argv[2]) : 0;
  xmass     = (argc>3) ? atof(argv[3]) : 125.0;
  doBlind   = (argc>4) ? atoi(argv[4]) : 0;
  cout << "imode = " << imode << " isys = " << isys << " xmass = " << xmass << " doBlind = " << doBlind << endl;
  //
  //
  if (imode == 100) pname = "ll_mtau_cba_config_vbf";        //"mtau_all";
  if (imode == 101) pname = "ll_mtau_mva_config_vbf";
  if (imode == 102) pname = "ll_etau_cba_config_vbf";
  if (imode == 103) pname = "ll_etau_mva_config_vbf";
  
  if (imode == 200) pname = "etau_sr3_llbin";        //"mtau_all";
  if (imode == 201) pname = "etau_sr3_llbin_noAgg";
  if (imode == 202) pname = "etau_sr3";
  if (imode == 203) pname = "etau_sr3_noAgg";

  if (imode == 300) pname = "etau_sr2_vbf_llbin";        //"mtau_all";
  if (imode == 301) pname = "etau_sr2_vbf_llbin_noAgg";
  if (imode == 302) pname = "etau_sr2_vbf";
  if (imode == 303) pname = "etau_sr2_vbf_noAgg";

  if (imode == 400) pname = "etau_sr3_vbf_llbin";        //"mtau_all";
  if (imode == 401) pname = "etau_sr3_vbf_llbin_noAgg";
  if (imode == 402) pname = "etau_sr3_vbf";
  if (imode == 403) pname = "etau_sr3_vbf_noAgg";

  if (imode == 500) pname = "etau_vbf_llbin";        //"mtau_all";
  if (imode == 501) pname = "etau_vbf_llbin_noAgg";
  if (imode == 502) pname = "etau_vbf";
  if (imode == 503) pname = "etau_vbf_noAgg";
  //if (imode == 103) pname = "mtau_bdt"; //"combinationWS"; // I changed mtau sr3 to combinationsWS for test
  /*
  if (imode == 200) pname = "etau_vbf";   //"etau_all";
  if (imode == 201) pname = "etau_vbf_ddZll";
  if (imode == 202) pname = "etau_sr2";
  if (imode == 203) pname = "etau_bdt";   //"etau_sr3";
  */
  //
  TString sname;
  //
  if (isys==0) sname="AllSys";
  if (isys==1) sname="NoTheory";
  if (isys==2) sname="NoStat";
  if (isys==3) sname="NoSys";
  if (isys==4) sname="NoErr";
  //
  //
  TString fname,wname,mname,dname,aname,gname,nname;
  //
  string thisdir      = "/afs/cern.ch/user/a/atpathak/afswork/public/LFVStat_16Aug2017";
  fname = Form("%s/workspaces/%s_%s.root",thisdir.c_str(),pname.Data(),sname.Data());
  wname = "combined";
  mname = "ModelConfig";
  dname = doBlind ? "asimovData" : "obsData";
  aname = "asimovData_0";
  gname = "conditionalGlobs_0";
  nname = "nominalGlobs";
  //
  cout << " fname = " << fname << endl;
  cout << " wname = " << wname << endl;
  cout << " mname = " << mname << endl;
  cout << " dname = " << dname << endl;
  cout << " aname = " << aname << endl;
  cout << " gname = " << gname << endl;
  cout << " nname = " << nname << endl;

  ROOT::Math::MinimizerOptions::SetDefaultStrategy(1); // usually good enough
  RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL); // lowest message level
  ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(-1);  

  runAsymptoticsCLs(fname,wname,mname,dname,aname,gname,nname);
  return 0;
}
