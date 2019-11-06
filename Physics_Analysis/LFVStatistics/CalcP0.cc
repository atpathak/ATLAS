//
// P0 calculation code extended with multiple profiling customization [Author: Sw.Banerjee, UW-Madison]
//
#define USING_MINUIT2
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include "TStyle.h"
#include "TROOT.h"
#include "TPluginManager.h"
#include "TSystem.h"
#include "TFile.h"
#include "TGaxis.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TGraphAsymmErrors.h"
#include "TLine.h"
#include "TSpline.h"
#include "RooAbsData.h"
#include "RooDataHist.h"
#include "RooCategory.h"
#include "RooDataSet.h"
#include "RooAbsPdf.h"
#include "RooSimultaneous.h"
#include "RooProdPdf.h"
#include "RooNLLVar.h"
#include "RooProfileLL.h"
#include "RooFitResult.h"
#include "RooPlot.h"
#include "RooRandom.h"
#include "RooMinuit.h"
#include "TRandom3.h"
#include "RooWorkspace.h"
#include "RooStats/RooStatsUtils.h"
#include "RooStats/ModelConfig.h"
#include "TStopwatch.h"
#include "RooStats/ModelConfig.h"
#include "RooStats/ProfileLikelihoodTestStat.h"
#include "RooGlobalFunc.h"
#include "RooMinimizerFcn.h" 
#include "TMinuitMinimizer.h"
#include "TMinuit.h"
#include "RooRealVar.h"
#include "RooProdPdf.h"
#include "RooWorkspace.h"
#include "RooDataSet.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TH1.h"
#include "RooPlot.h"
#include "RooMsgService.h"
#include "RooStats/NumberCountingUtils.h"
#include "RooStats/HybridCalculator.h"
#include "RooStats/ToyMCSampler.h"
#include "RooStats/HypoTestPlot.h"
#include "RooStats/AsymptoticCalculator.h"
#include "RooStats/ProfileLikelihoodCalculator.h"
#include "RooStats/ProfileLikelihoodTestStat.h"
#include "RooStats/SimpleLikelihoodRatioTestStat.h"
#include "RooStats/RatioOfProfiledLikelihoodsTestStat.h"
#include "RooStats/MaxLikelihoodEstimateTestStat.h"
#include "RooMinimizer.h"
#include "Math/MinimizerOptions.h"
#include "RooStats/ProfileLikelihoodTestStat.h"
/*
#include "AsymPow.h"
#include "HZZ2L2QRooPdfs.h"
#include "HZZ4LRooPdfs.h"
#include "VerticalInterpPdf.h"
*/
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "RooPlot.h"
#include "TROOT.h"
#include "THStack.h"
#include "TString.h"
#include "TH1.h"
#include "TRandom.h"
#include "RooRealVar.h"
#include "RooExponential.h"
#include "RooPlot.h"
#include "RooGaussian.h"
#include "RooPolynomial.h"
#include "RooAddPdf.h"
#include "RooAbsData.h"
#include "RooAbsRealLValue.h"
#include "RooAbsPdf.h"
#include "RooMinuit.h"
#include "RooCategory.h"
#include "RooDataSet.h"
#include "RooArgSet.h"
#include "RooDataHist.h"
#include "RooNLLVar.h"
#include "RooSimultaneous.h"
#include "RooExponential.h"
#include "RooGlobalFunc.h"
#include "RooCBShape.h"
#include "RooFormula.h"
#include "RooRandom.h"
#include "RooFitResult.h"
#include "RooProdPdf.h"
#include "RooProfileLL.h"
#include "RooLognormal.h"
#include "RooPoisson.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;
using namespace RooFit;
using namespace RooStats;
#include <sys/stat.h>
double _mu_hat_saved = 1;
bool LoadSnapshot(RooWorkspace* combWS, const char* snapshotname, int severity=0, const char* newname="", const RooArgSet*pars=0) {
  bool foundSnapshot=false;
  foundSnapshot=combWS->loadSnapshot(snapshotname);
  if ( foundSnapshot) cout << "Found snapshot with name = " << snapshotname << endl;
  if (!foundSnapshot) {
    cout << "No Snapshot found with name = " << snapshotname;
    if (severity==0)   {cout << " ... proceeding anyway ..." << endl;} // <-------- THIS IS THE DEFAULT
    if (severity==1)   {cout << " ... cannot proceed . aborting ... " << endl; abort();}
    if (severity==-1)  {cout << " ... saving " << newname << " as " << snapshotname << endl; combWS->saveSnapshot(snapshotname,RooArgList(*pars));}
  }
  return foundSnapshot;
}
RooDataSet* makeData(RooDataSet* orig, RooSimultaneous* simPdf, const RooArgSet* observables, RooRealVar* firstPOI, double mass, double& mu_min);
int makeAsimovData(ModelConfig* mcInWs, bool doConditional, RooWorkspace* combWS, RooAbsPdf* combPdf, RooDataSet* combData,double muval, double profileMu);
void unfoldConstraints(RooArgSet& initial, RooArgSet& final, RooArgSet& obs, RooArgSet& nuis, int& counter);
int minimize(RooNLLVar* nll, RooWorkspace* wspace, int& npar, vector<double>& corr_ij, vector<TString>& parnames,
	     vector<double>& fitval, vector<double>& fiterr, vector<double>& fiterrlo, vector<double>& fiterrhi, TH2D* &h2Dcorrelation,TString h2Dcorrelation_name, 
	     double&edm, int&covQual) {
  //  wspace->var("alpha_ATLAS_ANA_EMB_MFS")->setVal(-0.224); <-- DO NOT DO THIS UNLESS YOU REALLY WANT TO [BUT THEN IF YOU REALLY WANT TO, BE MY GUEST]

  int printLevel = ROOT::Math::MinimizerOptions::DefaultPrintLevel();
  RooFit::MsgLevel msglevel = RooMsgService::instance().globalKillBelow();
  if (printLevel < 0) RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);

  int strat = ROOT::Math::MinimizerOptions::DefaultStrategy();
  RooMinimizer minim(*nll);
  minim.setStrategy(strat);
  minim.optimizeConst(2);
  minim.setPrintLevel(printLevel);
  minim.setProfile(1);

  cout << ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str() << " " << ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str() << endl;
  int status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());


  if (status != 0 && status != 1 && strat < 2)
  {
    strat++;
    cout << "Fit failed with status " << status << ". Retrying with strategy " << strat << endl;
    minim.setStrategy(strat);
    status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
  }

  if (status != 0 && status != 1 && strat < 2) // WELL IF YOU START WITH 1 THEN THIS IS OUTSIDE YOUR EVENT HORIZON, BUT THEN YOU MAY HAVE STARTED AT 0
  {
    strat++;
    cout << "Fit failed with status " << status << ". Retrying with strategy " << strat << endl;
    minim.setStrategy(strat);
    status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
  }

  if (printLevel < 0) RooMsgService::instance().setGlobalKillBelow(msglevel);

  if (status != 0 && status != 1)
  {
    cout << "Fit failed with status " << status << endl;
  }

//  if (const_test)
//  {
//    for (int i=0;i<nrConst;i++)
//    {
//      RooRealVar* const_var = wspace->var(const_vars[i].c_str());
//      const_var->setConstant(0);
//    }
//  }

  int HessStatus= minim.hesse();
  RooFitResult* fit = minim.save();
  //
  edm=fit->edm();
  covQual=fit->covQual();
  const RooArgList& fitpars = fit->floatParsFinal();
  //  cout << "fitpars:: size = " << fitpars.getSize() << endl; fitpars.Print("v") ;
  //
  parnames.clear();
  fitval.clear();
  fiterr.clear();
  fiterrlo.clear();
  fiterrhi.clear();
  for (int ipar=0;ipar<fitpars.getSize();++ipar){
    RooRealVar*v = (RooRealVar*)fitpars.at(ipar);
    TString vname=v->GetName();
    /*
    cout << "ipar = " << Form("%2.2d",ipar) << " " << Form("%-50s",vname.Data()) << " " ;
    cout << Form("%10.5f",v->getVal()) << " +- " << Form("%10.5f",v->getError()) 
	 << " - " << Form("%10.5f",fabs(v->getErrorLo())) 
	 << " + " << Form("%10.5f",v->getErrorHi()) << " " ;
    if (v->getError()>0) {
      cout << Form("%+10.3f",v->getVal() / v->getError()) ; 
    } else { 
      cout << " 0 ";
    } 
    cout << endl;// v->Print();
    */
    //    if (vname.Contains("gamma_stat")) continue;
    parnames.push_back(vname);
    fitval.push_back(v->getVal());
    fiterr.push_back(v->getError());
    fiterrlo.push_back(v->getErrorLo());
    fiterrhi.push_back(v->getErrorHi());
  }
  //
  npar = parnames.size();
  //
  corr_ij.clear();
  for (int i=0;i<npar;++i) {
    RooRealVar* ivar=wspace->var(parnames[i]);
    for (int j=0;j<npar;++j) {
      RooRealVar* jvar=wspace->var(parnames[j]);
      corr_ij.push_back(fit->correlation(*ivar,*jvar));
      // cout << i << " " << j << " " << parnames[i] << " " << parnames[j] << " " << corr_ij[i*npar+j] << " " << corr_ij.back() << endl;
    }
  }
  //
  //  TDirectory* pwd=gDirectory;
  TH2D* h2Dcorrelation_tmp = (TH2D*) fit->correlationHist(Form("%s_tmp",h2Dcorrelation_name.Data()));
  h2Dcorrelation=(TH2D*)h2Dcorrelation_tmp->Clone(h2Dcorrelation_name);
  h2Dcorrelation->SetDirectory(0);
  h2Dcorrelation->Print();
  //  gDirectory=pwd;
  if (h2Dcorrelation_tmp) {delete h2Dcorrelation_tmp; h2Dcorrelation_tmp=0;}
  //
  return status;
}

int main(int argc, char* argv[]){
  //
  // Argument variables
  //
  int imode    = (argc>1) ? atoi(argv[1]) : 10; 
  int isys     = (argc>2) ? atoi(argv[2]) :  0;
  double xmass = (argc>3) ? atof(argv[3]) : 125;
  int  doBlind = (argc>4) ? atoi(argv[4]) :  0; // YEAH GO AHEAD BY TURNING THE KNOB, AS LONG AS YOU ARE EXPECTING WITH ASIMOV YOU MUST BE > = 0 [Try 0 or 1, for example]
  int uncapped     = 1;
  //
  string thisdir      = "/afs/cern.ch/user/a/atpathak/afswork/public/LFVStat_16Aug2017";
  //
  // JUSTICE IS BLIND :: BUT THEN IF YOU ARE UNJUST ALREADY, YOU ARE MOST PROBABLY BLIND AS WELL : GET SOME CONTACTS BEFORE YOU TOUCH THIS FLAG !
  int doexp_1_only = 0; // INTERNAL FLAG : CHANGE AND PERIL AT YOUR OWN RISK ; IT IS CONTROLLED VIA THE EXTERNALLY FLAV DOBLIND
  int doobs_not    = 1; // INTERNAL FLAG : CHANGE AND PERIL AT YOUR OWN RISK ; IT IS CONTROLLED VIA THE EXTERNALLY FLAV DOBLIND
  int hasSnapshot  = 0; // WELL THIS IS THE UNTOLD PART OF THE OTHER STORY 
  //
  if (doBlind>0) doexp_1_only = 1;
  if (doBlind<0) doobs_not    = 0;
  //
  TString pname; // Particuliers Nom
  TString fname; // Input File Name 
  TString sname; // Part of Input Filename [systematics]
  TString wname; // Workspace Name
  TString mname; // ModelConfig Name
  TString dname; // Dataset Name
  //
  RooWorkspace* ws(0);
  RooDataSet* data(0);
  ModelConfig* mc(0);
  RooAbsPdf* pdf(0);
  //
  //
  //
  wname = "combined";
  mname = "ModelConfig";
  dname =  (doBlind>0) ? "asimovData" : "obsData"; 
  //
  int save_imode = imode;
  if (save_imode>=1100 && save_imode<= 1449){
    imode = save_imode - 1000;
  }
  //
  if (imode == 100) pname = "comb_mtau_cba";
  if (imode == 101) pname = "mtau_sr1";
  if (imode == 102) pname = "mtau_sr2";
  //if (imode == 103) pname = "mtau_sr3";
  if (imode == 103) pname = "combinationWS"; // I changed mtau sr3 to combinationsWS for test 
  //
  if (imode == 200) pname = "etau_all";
  if (imode == 201) pname = "etau_sr1";
  if (imode == 202) pname = "etau_sr2";
  if (imode == 203) pname = "etau_sr3";
  //
  if (save_imode>=1100 && save_imode<= 1449){
    imode = save_imode;
  }
  //
  if (isys==0) sname="AllSys";
  if (isys==1) sname="NoTheory";
  if (isys==2) sname="NoGammaStat";
  if (isys==3) sname="NoSys";
  if (isys==4) sname="NoErr";
  //
  //
  if (imode >=100 && imode <= 449)  {
    TString sname_tmp = sname;
    sname="AllSys"; // TEMPORARY OVERRIDE
    //sname="NoSys"; // 
    fname = (doBlind<0) ? 
      Form("updated_withasimov_re/%s_%s_%d.root",pname.Data(),sname.Data(),0) :
      Form("workspaces/%s_%s.root",pname.Data(),sname.Data());
    sname = sname_tmp; // PERMANENT MISSION RESTORED
  }
  //
  TString temp_string=fname;
  fname=Form("%s/%s",thisdir.c_str(),temp_string.Data());
  // 
  if (doobs_not == 0) hasSnapshot=1;
  cout << "hasSnapshot = " << hasSnapshot << " doBlind = " << doBlind << " doexp_1_only = " << doexp_1_only  << " doobs_not = " << doobs_not << endl;
  cout << "Reading workspace = " << wname << " modelconfig = " << mname << " and data = " << dname << " from file = " << fname << " uncapped = " <<  uncapped << endl;
  //
  TString pname_tmp = pname;
  if (isys>0) {
    pname = Form("%s_%s",pname_tmp.Data(),sname.Data());
  }
  //
  TFile *_file = TFile::Open(fname);
  ws = (RooWorkspace*)_file->Get(wname);
  data = (RooDataSet*)ws->data(dname);
  mc = (ModelConfig*) ws->obj(mname);
  pdf = mc->GetPdf();
  //  ws->Print();
  cout << "Number of ParametersOfInterest =  " << mc->GetParametersOfInterest()->getSize() << endl;
  RooRealVar* firstPOI = (RooRealVar*) mc->GetParametersOfInterest()->first();
  if (uncapped) { 
    firstPOI->setRange(-50, 50);//setMin(-10); // HAD REST BEEN SIGNAL, FLOOR WOULD FLY OUT OF THE CEILING
  } else {
    firstPOI->setRange(0, 50);//setMin(0);
  }
  RooArgSet poi(*firstPOI); 
  //
  if (imode==72 || (imode==80 && xmass <200)) {
    if (string(pdf->ClassName()) == "RooSimultaneous") {
      RooSimultaneous* simPdf = (RooSimultaneous*)pdf;
      double min_mu;
      data = makeData(data, simPdf, mc->GetObservables(), firstPOI, xmass, min_mu);
    }
  }
  //
  //  RooAbsData::defaultStorageType = RooAbsData::Vector;
  data->convertToVectorStore() ;
  if (data) {
    cout << "Data:: " << data->GetName() << endl; data->Print("v");
    cout << "Data:: isWeighted = " << data->isWeighted() << " isNonPoissonWeighted = " << data->isNonPoissonWeighted() << endl;
    cout << "Data:: numEntries = " << data->numEntries() << " sumEntries = " << data->sumEntries() << endl;
  } else {
    cout << "data not found" << endl;
    return -1;
  }
  //
  if (pdf) {
    cout << "pdf:: " << endl; pdf->Print();
  } else {
    cout << "pdf not found" << endl;
    return -1;
  }
  //
  const RooArgSet* nuisPars = mc->GetNuisanceParameters(); 
  if (nuisPars) {
    cout << "nuisPars:: " << nuisPars->getSize() << endl; 
    TIterator* itr = nuisPars->createIterator() ;
    RooRealVar* arg;
    while ((arg=(RooRealVar*)itr->Next())) {
      if (!arg) continue;
      TString nname=arg->GetName();
      if (isys==2) { // NoGammaStat
	if (nname.Contains("gamma_")) arg->setConstant(1);
	//      if (nname.Contains("ATLAS_norm")) arg->setConstant(1);
	//	if (nname.Contains("alpha_")) arg->setConstant(1);
      }
    }
    if (isys==2) nuisPars->Print("v");
    if (isys!=0 && isys!=2 && isys!=3) {
      cout << "DO SOMETHING ELSE IN SPARE TIME" << endl;
      exit(1);
    }
  }
  //
#ifdef USING_MINUIT2
  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");
  ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(1);
#else
  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
  ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(-1);
  RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL); // LOWEST MESSAGE LEVEL
#endif
  //  ROOT::Math::MinimizerOptions::SetDefaultTolerance(.001); // DO NOT PLAY WITH SUCH STUFF, OK?
  ROOT::Math::MinimizerOptions::SetDefaultStrategy(1); // USUALLY GOOD ENOUGH

  // https://wiki.physik.uzh.ch/lhcb/root:controloutputroofit
  // RooMsgService::instance().Print() ;

  RooMsgService::instance().getStream(0).removeTopic(RooFit::Eval);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Eval);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Generation);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Minimization);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Plotting);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Fitting);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Integration);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::LinkStateMgmt);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Caching);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Optimization);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::ObjectHandling);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::InputArguments);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Tracing);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Contents);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::DataHandling);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::NumIntegration);

  //  cout << "Setting max function calls" << endl;
  //  RooMinimizer::SetMaxFunctionCalls(20000); // THIS DOESNT WORK STILL
  //  ROOT::Math::MinimizerOptions::SetDefaultMaxFunctionCalls(100000); // OK, FINALLY THIS IS A FUNCTION OF NUMBER OF FLOATING FIT PAREMETERS IN TRUNK
  //
  //
  TStopwatch timer;
  timer.Start();
  //
  int status_0=-1;
  int status_1_paz=-1;
  int status_1=-1;
  int status_1_pamh=-1;
  ROOT::Math::MinimizerOptions::SetDefaultStrategy(1); 
  if (doexp_1_only ==1 ) { 
    if (hasSnapshot==0 || !LoadSnapshot(ws,"conditionalGlobs_1"))     {
      status_1      = makeAsimovData(mc, 0, ws, pdf, data, 1.,1.0) ;//mu=1, profile at mu=1.0;
      cout << "makeAsimovData:: status_1 = " << status_1 << endl;
    }
  } else {
    if (!LoadSnapshot(ws,"conditionalGlobs_0"))  {
      status_0      = makeAsimovData(mc, 1, ws, pdf, data, 0.,0.0) ;//mu=0, profile at 0
      cout << "makeAsimovData:: status_0 = " << status_0 << endl;
    }
    if (!LoadSnapshot(ws,"conditionalGlobs_1_paz"))  {
      status_1_paz  = makeAsimovData(mc, 1, ws, pdf, data, 1.,0.0) ;//mu=1, profile at mu=0;
      cout << "makeAsimovData:: status_1_paz = " << status_1_paz << endl;
    }
    if (!LoadSnapshot(ws,"conditionalGlobs_1"))     {
      status_1      = makeAsimovData(mc, 1, ws, pdf, data, 1.,1.0) ;//mu=1, profile at mu=1.0;
      cout << "makeAsimovData:: status_1 = " << status_1 << endl;
    }
    if (!LoadSnapshot(ws,"conditionalGlobs_1_pamh")) {
      status_1_pamh = makeAsimovData(mc, 1, ws, pdf, data, 1.,2.0) ;//mu=1, profile at mu=\hat{mu}
      cout << "makeAsimovData:: status_1_pamh = " << status_1_pamh << endl;
    }
  }
  ROOT::Math::MinimizerOptions::SetDefaultStrategy(1);
  //
  TString str[4]               = { "obs",      "exp_paz",    "exp_1",      "exp_pamh"};
  TString descriptivestring[4] = { "observed", "expected_0", "expected_1", "expected_muhat"};
  double q0[4],p0[4],sig[4];
  for (int i=0;i<4;i+=1){
    if (doobs_not == 1 && i == 0 ) continue;
    if (doobs_not == 0 && i != 0 ) continue;
    if (doexp_1_only == 1 && i != 2) continue;
    //    if (i==1 || i== 3) continue;
    cout << endl;
    q0[i]=0; 
    p0[i]=0;
    sig[i]=-999; // AS IF ZERO IS NOT SELF OBVIOUS, LETS GO ALL THE WAY TO NEGATIVE INFINITY, BUT OF COURSE THATS NOT POSSIBLE, HUMANELY AT LEAST!
    RooDataSet *thisdata=0;
    bool foundSnapshot=false;
    switch (i) {
    case 0:
      thisdata=data;
      foundSnapshot=LoadSnapshot(ws,"nominalGlobs");
      //LoadSnapshot(ws,"nominalNuis");
      foundSnapshot=LoadSnapshot(ws,"conditionalNuis_0"); 
      break;
    case 1:
      thisdata=(RooDataSet*)ws->data("asimovData_1_paz");
      foundSnapshot=LoadSnapshot(ws,"conditionalGlobs_1_paz");
      foundSnapshot=LoadSnapshot(ws,"conditionalNuis_1_paz");
      break;
    case 2:
      thisdata=(RooDataSet*)ws->data("asimovData_1");
      foundSnapshot=LoadSnapshot(ws,"conditionalGlobs_1");
      foundSnapshot=LoadSnapshot(ws,"conditionalNuis_1"); 
      break;
    case 3:
      thisdata=(RooDataSet*)ws->data("asimovData_1_pamh");
      foundSnapshot=LoadSnapshot(ws,"conditionalGlobs_1_pamh");
      foundSnapshot=LoadSnapshot(ws,"conditionalNuis_1_pamh"); 
      break;
    }
    if (thisdata) {
      //      RooAbsData::defaultStorageType = RooAbsData::Vector;
      thisdata->convertToVectorStore() ;
    } else {
      cout << "thisdata not defined ... continue" << endl;
      continue;
    }
    if (!foundSnapshot) continue;
    //
    /*
      ProfileLikelihoodTestStat testStat(*pdf);
      if (uncapped) {
      testStat.SetOneSidedDiscovery();
      testStat.SetSigned();
      }
      testStat.SetPrintLevel(1);
      //    q0[i]  = 2*testStat.Evaluate(*thisdata,poi);
      */
    
    //    RooRealVar *nuis_EMB=ws->var("ATLAS_EMB");    if (nuis_EMB) nuis_EMB->setVal(0.7);
    // SILENCE ...
    //    RooFit::MsgLevel msglevel = RooMsgService::instance().globalKillBelow();
    //    RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
    //
    int npar=0;
    vector<double> corr_ij;
    vector<TString> parnames;
    vector<double> fitval;
    vector<double> fiterr;
    vector<double> fiterrlo;
    vector<double> fiterrhi;
    TH2D* h2Dcorrelation_cond=0;
    TH2D* h2Dcorrelation_uncond=0;
    double cond_edm ;
    double uncond_edm;
    int cond_covQual ;
    int uncond_covQual;
    //
    int ntrial=0;
  tryAgain: // this is a statement label
    ++ntrial;
    
    // Conditional
    firstPOI->setVal(0);
    firstPOI->setConstant(1);   
    cout << "Data:: " << endl; thisdata->Print("v");
    cout << "Data:: isWeighted = " << thisdata->isWeighted() << " isNonPoissonWeighted = " << thisdata->isNonPoissonWeighted() << endl;
    cout << "Data:: numEntries = " << thisdata->numEntries() << " sumEntries = " << thisdata->sumEntries() << endl;
    pdf->attachDataSet(*thisdata);
    RooArgSet* allParams = pdf->getParameters(thisdata);  RooStats::RemoveConstantParameters(allParams);
    cout << str[i] << " " << descriptivestring[i] << " allParams : " << endl;// allParams->Print("v");
    int numCPU = sysconf( _SC_NPROCESSORS_ONLN );
    numCPU = 1; // LXBATCH  like our NLL with multiple cores
    RooNLLVar* this_nll = (RooNLLVar*) pdf->createNLL(*thisdata,RooFit::Constrain(*mc->GetNuisanceParameters()), RooFit::GlobalObservables(*mc->GetGlobalObservables()), Offset(true), NumCPU(numCPU, RooFit::Hybrid));//, Extended(true), Verbose(kFALSE));
    //    this_nll->Print("V");
    if (h2Dcorrelation_cond) {delete h2Dcorrelation_cond; h2Dcorrelation_cond=0;}
    int status_cond = minimize(this_nll, ws, npar, corr_ij, parnames, fitval, fiterr, fiterrlo, fiterrhi,h2Dcorrelation_cond,"h2Dcorrelation_cond", cond_edm, cond_covQual);
    if (status_cond < 0) {
      cout << "Retry with conditional snapshot at mu=0" << endl;
      LoadSnapshot(ws,"conditionalNuis_0");// FOR THOSE ALIENS WHO DONT KNOW THE MEANING OF SIGNAL
      if (h2Dcorrelation_cond) {delete h2Dcorrelation_cond; h2Dcorrelation_cond=0;}
      status_cond   = minimize(this_nll, ws, npar, corr_ij, parnames, fitval, fiterr, fiterrlo, fiterrhi,h2Dcorrelation_cond,"h2Dcorrelation_cond", cond_edm, cond_covQual);
      if (status_cond >= 0) cout << "Retry with conditional snapshot at mu=0 :: Success!" << endl;
    }
    this_nll->Print();
    double cond_pll = this_nll->getVal();
    cout << "status_cond = " << status_cond 
	 << " cond_covQual = " << cond_covQual
	 << " cond_edm = " << Form("%10.6f",cond_edm) 
	 << " cond_nll = " << Form("%20.10f",cond_pll) << endl << endl;
    h2Dcorrelation_cond->Print();
    
    // Unconditional
    if (i==0) {
      LoadSnapshot(ws,"conditionalNuis_1_pamh");// 
      firstPOI->setVal(_mu_hat_saved);// Observed Initialization
    } else {
      firstPOI->setVal(1); //  Asimov Initialization
    }
    firstPOI->setConstant(0);
    if (h2Dcorrelation_uncond) {delete h2Dcorrelation_uncond; h2Dcorrelation_uncond=0;}
    int status_uncond = minimize(this_nll, ws, npar, corr_ij, parnames, fitval, fiterr, fiterrlo, fiterrhi,h2Dcorrelation_uncond,"h2Dcorrelation_uncond", uncond_edm, uncond_covQual);
    if (status_uncond < 0) {
      cout << "Retrying with conditional snapshot at mu=0" << endl;
      LoadSnapshot(ws,"conditionalNuis_0"); // FOR THOSE ALIENS WHO DONT KNOW THE MEANING OF SIGNAL
      if (h2Dcorrelation_uncond) {delete h2Dcorrelation_uncond; h2Dcorrelation_uncond=0;}
      status_uncond = minimize(this_nll, ws, npar, corr_ij, parnames, fitval, fiterr, fiterrlo, fiterrhi,h2Dcorrelation_uncond,"h2Dcorrelation_uncond",  uncond_edm, uncond_covQual);
      if (status_uncond >= 0) cout << "Success!" << endl;
    }
    this_nll->Print();
    double uncond_pll = this_nll->getVal();
    cout << "status_uncond = " << status_uncond 
	 << " uncond_covQual = " << uncond_covQual
	 << " uncond_edm = " << Form("%10.6f",uncond_edm) 
	 << " uncond_nll = " << Form("%20.10f",uncond_pll) << endl << endl;
    h2Dcorrelation_uncond->Print();
    //
    cout << "ntrial = " << ntrial << " status_cond = " << status_cond << " status_uncond = " << status_uncond << endl;
    delete this_nll; this_nll = 0; // OH MAN : WHEN WILL MULTI-PROCESSING SLAVES LEARN TO LISTEN TO ITS MASTER ! KILL, DIE, RESURRECT !!!
    if (status_cond <0 || status_uncond <0) {
      if (ntrial==1) {
	cout << "goto tryAgain ... with the other Minuit " << endl;
#ifdef USING_MINUIT2
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
	ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(-1);
#else
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");
	ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(1);
#endif
        goto tryAgain; // this is the goto statement
      } else { // WELL, WE COULD DO FIX THIS IF WE REALLY WANTED TO, BUT FIRST LETS SEE WHY THE NEED ARISES ... MAYBE PROBLEM IS ELSEWHERE
        cout << "ntrial = " << ntrial << " status_cond = " << status_cond << " status_uncond = " << status_uncond << "  still non-zero ... continuing  " << endl;
#ifdef USING_MINUIT2
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");
	ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(1); 
#else
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
	ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(-1); 
#endif
      }
    }
    
    /*
      RooMinimizer cond_minim(*cond_nll);
      cond_minim.setPrintLevel(RooMinimizer::None);
      cond_minim.optimizeConst(2); 
      cond_minim.setStrategy(0);
      int cond_fitstatus = cond_minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
      cout << "CONDITIONAL FIT:: fitstatus = " << cond_fitstatus << " cond_nll = " << Form("%20.10f",cond_nll->getVal()) << " ";
      if (nuis_EMB) cout << " nuis_EMB = " << nuis_EMB->getVal() ;
      cout << endl;
      if (cond_fitstatus!=0 || std::isnan(cond_nll->getVal()) || std::isinf(cond_nll->getVal())) {
      cout << "Retrying with Recan " << endl;
      cond_minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(),"Scan");
      cond_fitstatus = cond_minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
      cout << "CONDITIONAL FIT:: fitstatus = " << cond_fitstatus << " cond_nll = " << Form("%20.10f",cond_nll->getVal()) << " ";
      if (nuis_EMB) cout << " nuis_EMB = " << nuis_EMB->getVal() ;
      cout << endl;
      }
      if (cond_fitstatus!=0 || std::isnan(cond_nll->getVal()) || std::isinf(cond_nll->getVal())) {
      cout << "Retrying with Strategy = 1 " << endl;
      cond_minim.setStrategy(1);
      cond_fitstatus = cond_minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
      cout << "CONDITIONAL FIT:: fitstatus = " << cond_fitstatus << " cond_nll = " << Form("%20.10f",cond_nll->getVal()) << " ";
      if (nuis_EMB) cout << " nuis_EMB = " << nuis_EMB->getVal() ;
      cout << endl;
      }
      if (cond_fitstatus!=0 || std::isnan(cond_nll->getVal()) || std::isinf(cond_nll->getVal())) {
      cout << "Retrying with Minuit MigradImproved  " << endl;
      cond_minim.setStrategy(1);
      cond_fitstatus = cond_minim.minimize("Minuit","migradimproved");
      cout << "CONDITIONAL FIT:: fitstatus = " << cond_fitstatus << " cond_nll = " << Form("%20.10f",cond_nll->getVal()) << " ";
      if (nuis_EMB) cout << " nuis_EMB = " << nuis_EMB->getVal() ;
      cout << endl;
      }
      if (cond_fitstatus!=0 || std::isnan(cond_nll->getVal()) || std::isinf(cond_nll->getVal())) {
      cout << "Retrying with Strategy = 2 " << endl;
      cond_minim.setStrategy(2);
      cond_fitstatus = cond_minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
      cout << "CONDITIONAL FIT:: fitstatus = " << cond_fitstatus << " cond_nll = " << Form("%20.10f",cond_nll->getVal()) << " ";
      if (nuis_EMB) cout << " nuis_EMB = " << nuis_EMB->getVal() ;
      cout << endl;
      }
      double cond_pll = cond_nll->getVal();
      cout << "cond_nll = " << Form("%20.10f",cond_pll) << endl;
      cout << "Printing nuisance parameters : " << endl;    mc->GetNuisanceParameters()->Print("v");
      
      // UnConditional
      foundSnapshot=ws->loadSnapshot("conditionalNuis_0"); 
      firstPOI->setConstant(0);
      RooNLLVar* uncond_nll = cond_nll;//(RooNLLVar*) pdf->createNLL(*thisdata, Constrain(_nuis), Extended(true), Verbose(kFALSE));
      RooMinimizer uncond_minim(*uncond_nll);
      uncond_minim.setPrintLevel(RooMinimizer::None);
      //    uncond_minim.optimizeConst(2);
      uncond_minim.setStrategy(0);
      int uncond_fitstatus = uncond_minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
      cout << "UNCONDITIONAL FIT:: fitstatus = " << uncond_fitstatus << " uncond_nll = " << Form("%20.10f",uncond_nll->getVal()) << " ";
      if (nuis_EMB) cout << " nuis_EMB = " << nuis_EMB->getVal() ;
      cout << endl;
      if (uncond_fitstatus!=0 || std::isnan(uncond_nll->getVal()) || std::isinf(uncond_nll->getVal())) {
      cout << "Retrying with Recan " << endl;
      uncond_minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(),"Scan");
      uncond_fitstatus = uncond_minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
      cout << "UNCONDITIONAL FIT:: fitstatus = " << uncond_fitstatus << " uncond_nll = " << Form("%20.10f",uncond_nll->getVal()) << " ";
      if (nuis_EMB) cout << " nuis_EMB = " << nuis_EMB->getVal() ;
      cout << endl;
      }
      if (uncond_fitstatus!=0 || std::isnan(uncond_nll->getVal()) || std::isinf(uncond_nll->getVal())) {
      cout << "Retrying with Strategy = 1 " << endl;
      uncond_minim.setStrategy(1);
      uncond_fitstatus = uncond_minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
      cout << "UNCONDITIONAL FIT:: fitstatus = " << uncond_fitstatus << " uncond_nll = " << Form("%20.10f",uncond_nll->getVal()) << " ";
      if (nuis_EMB) cout << " nuis_EMB = " << nuis_EMB->getVal() ;
      cout << endl;
      }
      if (uncond_fitstatus!=0 || std::isnan(uncond_nll->getVal()) || std::isinf(uncond_nll->getVal())) {
      cout << "Retrying with Minuit MigradImproved  " << endl;
      uncond_minim.setStrategy(1);
      uncond_fitstatus = uncond_minim.minimize("Minuit","migradimproved");
      cout << "UNCONDITIONAL FIT:: fitstatus = " << uncond_fitstatus << " uncond_nll = " << Form("%20.10f",uncond_nll->getVal()) << " ";
      if (nuis_EMB) cout << " nuis_EMB = " << nuis_EMB->getVal() ;
      cout << endl;
      }
      if (uncond_fitstatus!=0 || std::isnan(uncond_nll->getVal()) || std::isinf(uncond_nll->getVal())) {
      cout << "Retrying with Strategy = 2 " << endl;
      uncond_minim.setStrategy(2);
      uncond_fitstatus = uncond_minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
      cout << "UNCONDITIONAL FIT:: fitstatus = " << uncond_fitstatus << " uncond_nll = " << Form("%20.10f",uncond_nll->getVal()) << " ";
      if (nuis_EMB) cout << " nuis_EMB = " << nuis_EMB->getVal() ;
      cout << endl;
      }
      double uncond_pll = uncond_nll->getVal();
      cout << "uncond_nll = " << Form("%20.10f",uncond_pll) << endl;
      cout << "Printing nuisance parameters : " << endl;    mc->GetNuisanceParameters()->Print("v");
    */
    RooRealVar* firstPOI_fitted = (RooRealVar*)firstPOI->Clone("firstPOI_fitted");
    firstPOI_fitted->Print();
    double POIFitVal = firstPOI->getVal(); 
    double POIFitErr = firstPOI->getError(); 
    double POIFitErrLo = firstPOI->getErrorLo();
    double POIFitErrHi = firstPOI->getErrorHi();
    cout << str[i].Data() << ":: POIFitVal = " << POIFitVal << " POIFitErr = " << POIFitErr << " POIFitErrLo = " << POIFitErrLo << " POIFitErrHi = " << POIFitErrHi << endl;
    //        if (i==0) {
    for (int ipar=0;ipar<npar;++ipar){
      cout << "ipar = " << Form("%2.2d",ipar) << " " << Form("%-50s",parnames[ipar].Data()) << " " ;
      cout << Form("%10.5f",fitval[ipar]) << " +- " << Form("%10.5f",fiterr[ipar]) 
	   << " + " << Form("%10.5f",fiterrhi[ipar]) 
	   << " - " << Form("%10.5f",fabs(fiterrlo[ipar])) 
	   << " " ;
      if (fiterr[ipar]>0) {
	if (parnames[ipar].BeginsWith("alpha_")) { 
	  cout << Form("%+10.3f",fitval[ipar] / fiterr[ipar]) ; 
	} else {
	  cout << Form("%+10.3f",(fitval[ipar] -1)/ fiterr[ipar]) ; 
	}
      } else { 
	cout << Form("%+10.3f",0) ; 
      } 
      cout << endl;
    }
//    //
//    if (str[i] == "obs") {
//      RooArgSet nuisAndPoi(*nuisPars);
//      nuisAndPoi.add(*firstPOI);
//      //
//      RooArgSet* nuisAndPoi_save=new RooArgSet();
//      nuisAndPoi.snapshot(*nuisAndPoi_save,kFALSE);
////      RooRealVar mhpoint("mhpoint","m_{H}",xmass);
////      nuisAndPoi_save->addOwned(mhpoint);
////      RooRealVar nllminvar("nllmin","NLL",uncond_pll);
////      nuisAndPoi_save->addOwned(nllminvar);
//      //
//      ws->saveSnapshot("unconditional",RooArgList(*nuisAndPoi_save)); // OH WHAT A FUNDAMENTAL TRICK TO KEEP IN SLEEVES ... BUT IF YOU DONT FIX YOURSELF, YOU CANT BE FIXED !!!
//    }
    //
    /*
      int h2d_nx = h2Dcorrelation_uncond->GetNbinsX();
      for (int ipar=0;ipar<h2d_nx;++ipar) {
      cout << ipar << " " << h2Dcorrelation_uncond->GetXaxis()->GetBinLabel(ipar+1) << endl;
      }
    */
    
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    gStyle->SetLabelFont(62,"X");
    gStyle->SetLabelFont(62,"Y");
    gStyle->SetLabelSize(0.033,"X");
    gStyle->SetLabelSize(0.033,"Y");
    gStyle->SetPalette(1);
    gStyle->SetPaintTextFormat(".0f");
    //  // put tick marks on top and RHS of plots
    //  gStyle->SetPadTickX(0);
    //  gStyle->SetPadTickY(0);
    //  gStyle->SetPadTickX(0);//not for x-axis
    gStyle->SetNdivisions(5,"X");
    
    //
    int napar=0;
    int ngpar=0;
    for (int ipar=0;ipar<npar;ipar++) {
      TString parname=parnames[ipar];
      if (parname=="SigXsecOverSM") continue;
      if (parname.BeginsWith("alpha_")) ++napar;
      if (parname.BeginsWith("gamma_")) ++ngpar;
      if (parname.BeginsWith("ATLAS_norm_")) ++ngpar;
    }
    cout << "npar = " << npar << " napar = " << napar << " ngpar = " << ngpar << endl;
    //
    //
    //
    TH2F *ah_fitval = new TH2F ("ah_fitval", "ah_fitval", 1, -2.4, 4.4, napar, 0 , napar);
    TGraphAsymmErrors *ag_fitval = new TGraphAsymmErrors(napar);
    ag_fitval->SetTitle("ag_fitval");
    ag_fitval->SetMarkerColor(6);
    ag_fitval->SetMarkerStyle(25);
    ag_fitval->SetMarkerSize(0.5); 
    ag_fitval->SetLineWidth(2);
    ag_fitval->SetLineColor(6);
    int iplot=-1;
    for (int ipar=0;ipar<npar;ipar++) {
      TString parname=parnames[ipar];
      if (parname=="SigXsecOverSM") continue;
      if (parname.BeginsWith("gamma_")) continue;
      if (parname.BeginsWith("ATLAS_norm_")) continue;
      parname.ReplaceAll("ATLAS_","");
      parname.ReplaceAll("alpha_","");
      parname.ReplaceAll("gamma_","");
      ++iplot;
      //  cout << "iplot = " << iplot << " bin = " << napar-iplot << " yval = " << napar-iplot-0.5 << " " << parname << endl;
      ah_fitval->GetYaxis()->SetBinLabel(napar-iplot,parname);
      ag_fitval->SetPoint(iplot,fitval[ipar], napar-iplot-0.5);
      ag_fitval->SetPointEXlow (iplot,-fiterrlo[ipar]);
      ag_fitval->SetPointEXhigh(iplot, fiterrhi[ipar]);
      ag_fitval->SetPointEYlow (iplot,0);
      ag_fitval->SetPointEYhigh(iplot,0);
    }
    TCanvas*ac1=new TCanvas("ac1","ac1",400,600);
    ac1->cd();
    //
    ac1->SetTopMargin(0.075);
    ac1->SetRightMargin(0.025);
    ac1->SetLeftMargin(.275);
    ac1->SetBottomMargin(.075);
    //
    ah_fitval->GetXaxis()->SetRangeUser(-2.4,4.4);
    ah_fitval->SetMaximum(napar-0.5);
    ah_fitval->SetMinimum(0.5);
    //
    ah_fitval->LabelsOption("v","Y");
    ah_fitval->GetYaxis()->SetTickLength(0);
    ah_fitval->GetXaxis()->SetTicks("-");
    ah_fitval->GetXaxis()->SetLabelOffset(-.05);
    ah_fitval->GetXaxis()->SetTitleOffset(-1);
    ah_fitval->GetXaxis()->SetTitle("Fitted Value of Nuisance Parameters");
    ah_fitval->Draw();
    //
    ag_fitval->Draw("P SAME");
    //    cout << "ag_fitval->GetN = " << ag_fitval->GetN() << " npar = " << npar << " napar = " << napar << " iplot = " << iplot << endl;
    for (int ipar=0;ipar<napar;ipar++) {
      //
      // cout << "ipar = " << ipar << " label = " << ah_fitval->GetYaxis()->GetBinLabel(ipar+1) << endl;
      //
      int ivar=-1;
      for (ivar=0;ivar<npar;++ivar) {
	TString parname = parnames[ivar] ;
	parname.ReplaceAll("ATLAS_","");
	parname.ReplaceAll("alpha_","");
	parname.ReplaceAll("gamma_","");
	TString tmpname=ah_fitval->GetYaxis()->GetBinLabel(ipar+1);
	//	cout << ivar << " " << tmpname << " " << parname << endl;
	if (parname == tmpname) break;
      }
      //
      TLatex t; 
      int fitval_col=1;
      if (fitval[ivar] > .25 || fitval[ivar] < -.25) fitval_col=2;
      int fiterr_col=1;
      if (-fiterrlo[ivar] <= .5 || fiterrhi[ivar] <= .5) fiterr_col=2;
      t.SetTextColor(1) ; 
      t.SetTextSize(0.025) ; 
      t.SetTextAlign(12); 
      TString str1=Form("#color[%d]{% 3.1f}", fitval_col, fitval[ivar]);
      TString str2=Form("#color[%d]{+%3.1f}",fiterr_col, fabs(fiterrhi[ivar]));
      TString str3=Form("#color[%d]{-%3.1f}",fiterr_col, fabs(fiterrlo[ivar]));
      //      cout << ivar << " " <<  parnames[ivar]  << " " << ipar+0.5 << " " << str1 << " " << str2 << " " << str3 << endl;
      t.DrawLatex(2,ipar+0.5,str1);
      t.DrawLatex(2.75,ipar+0.5,str2);
      t.DrawLatex(3.50,ipar+0.5,str3);
    }
    gPad->SetGrid(0,1);
    TLine* aline=new TLine();
    aline->SetLineColor(4); 
    aline->SetLineWidth(2); 
    aline->DrawLine(0,0,0,napar);
    //
    TString poi_str=Form("#splitline{Unconditional Fit Status = %d, #mu = %5.2f + %5.2f - %5.2f}{Strategy = %d, EDM = %10.6f, NLL = %20.10f}", 
			 status_uncond, POIFitVal, POIFitErrHi, fabs(POIFitErrLo),  
			 1, uncond_edm, uncond_pll);
    TLatex poi_ltx; 
    poi_ltx.SetTextColor(1) ; 
    poi_ltx.SetTextSize(0.025) ; 
    poi_ltx.SetTextAlign(22); 
    poi_ltx.DrawLatex(0,napar+1.5,poi_str);
    //
    system(Form("mkdir -vp %s/corrfig_re",thisdir.c_str()));
    ac1->SaveAs(Form("%s/corrfig_re/apulls_muhat_%s_%5.1f_%d_%s.eps",thisdir.c_str(),pname.Data(),xmass,doBlind,descriptivestring[i].Data()));  
    ac1->SaveAs(Form("%s/corrfig_re/apulls_muhat_%s_%5.1f_%d_%s.pdf",thisdir.c_str(),pname.Data(),xmass,doBlind,descriptivestring[i].Data()));  
    ac1->SaveAs(Form("%s/corrfig_re/apulls_muhat_%s_%5.1f_%d_%s.png",thisdir.c_str(),pname.Data(),xmass,doBlind,descriptivestring[i].Data()));  
    delete ac1;   ac1=0;
    delete ah_fitval; ah_fitval=0;
    delete ag_fitval; ag_fitval=0;
    //
    //
    //
    TH2F *gh_fitval = new TH2F ("gh_fitval", "gh_fitval", 1, 0.5, 2.5, ngpar, 0 , ngpar);
    TGraphAsymmErrors *gg_fitval = new TGraphAsymmErrors(ngpar);
    gg_fitval->SetTitle("gg_fitval");
    gg_fitval->SetMarkerColor(6);
    gg_fitval->SetMarkerStyle(25);
    gg_fitval->SetMarkerSize(0.5); 
    gg_fitval->SetLineWidth(2);
    gg_fitval->SetLineColor(6);
    iplot=-1;
    for (int ipar=0;ipar<npar;ipar++) {
      TString parname=parnames[ipar];
      if (parname=="SigXsecOverSM") continue;
      if (parname.BeginsWith("alpha_")) continue;
      parname.ReplaceAll("ATLAS_","");
      parname.ReplaceAll("alpha_","");
      parname.ReplaceAll("gamma_","");
      ++iplot;
      //      cout << "iplot = " << iplot << " bin = " << ngpar-iplot << " yval = " << ngpar-iplot-0.5 << " " << parname << endl;
      gh_fitval->GetYaxis()->SetBinLabel(ngpar-iplot,parname);
      gg_fitval->SetPoint(iplot,fitval[ipar], ngpar-iplot-0.5);
      gg_fitval->SetPointEXlow (iplot,-fiterrlo[ipar]);
      gg_fitval->SetPointEXhigh(iplot, fiterrhi[ipar]);
      gg_fitval->SetPointEYlow (iplot,0);
      gg_fitval->SetPointEYhigh(iplot,0);
    }
    TCanvas*gc1=new TCanvas("gc1","gc1",400,600);
    gc1->cd();
    //
    gc1->SetTopMargin(0.075);
    gc1->SetRightMargin(0.025);
    gc1->SetLeftMargin(.275);
    gc1->SetBottomMargin(.075);
    //
    gh_fitval->GetXaxis()->SetRangeUser(0.5,2.5);
    gh_fitval->SetMaximum(ngpar-0.5);
    gh_fitval->SetMinimum(0.5);
    //
    gh_fitval->LabelsOption("v","Y");
    gh_fitval->GetYaxis()->SetTickLength(0);
    gh_fitval->GetXaxis()->SetTicks("-");
    gh_fitval->GetXaxis()->SetLabelOffset(-.05);
    gh_fitval->GetXaxis()->SetTitleOffset(-1);
    gh_fitval->GetXaxis()->SetTitle("Fitted Value of Nuisance Parameters");
    gh_fitval->Draw();
    //
    gg_fitval->Draw("P SAME");
    //    cout << "gg_fitval->GetN = " << gg_fitval->GetN() << " npar = " << npar << " ngpar = " << ngpar << " iplot = " << iplot << endl;
    for (int ipar=0;ipar<ngpar;ipar++) {
      //
      //      cout << "ipar = " << ipar << " label = " << gh_fitval->GetYaxis()->GetBinLabel(ipar+1) << endl;
      //
      int ivar=-1;
      for (ivar=0;ivar<npar;++ivar) {
	TString parname = parnames[ivar] ;
	parname.ReplaceAll("ATLAS_","");
	parname.ReplaceAll("alpha_","");
	parname.ReplaceAll("gamma_","");
	TString tmpname=gh_fitval->GetYaxis()->GetBinLabel(ipar+1);
	//	cout << ivar << " " << tmpname << " " << parname << endl;
	if (parname == tmpname) break;
      }
      //
      TLatex t; 
      int fitval_col=1;
      if (fitval[ivar] > 1.5 || fitval[ivar] < 0.5) fitval_col=2;
      int fiterr_col=1;
      if (-fiterrlo[ivar] >= .5 || fiterrhi[ivar] >= .5) fiterr_col=2;
      t.SetTextColor(1) ; 
      t.SetTextSize(0.025) ; 
      t.SetTextAlign(12); 
      TString str1=Form("#color[%d]{% 4.2f}", fitval_col, fitval[ivar]);
      TString str2=Form("#color[%d]{+%4.2f}",fiterr_col, fabs(fiterrhi[ivar]));
      TString str3=Form("#color[%d]{-%4.2f}",fiterr_col, fabs(fiterrlo[ivar]));
      //      cout << ivar << " " <<  parnames[ivar]  << " " << ipar+0.5 << " " << str1 << " " << str2 << " " << str3 << endl;
      t.DrawLatex(1.75,ipar+0.5,str1);
      t.DrawLatex(2.00,ipar+0.5,str2);
      t.DrawLatex(2.25,ipar+0.5,str3);
    }
    gPad->SetGrid(0,1);
    TLine* line_2=new TLine();
    line_2->SetLineColor(4); 
    line_2->SetLineWidth(2); 
    line_2->DrawLine(1,0,1,ngpar);
    //
    //    poi_ltx.DrawLatex(0,napar+1.5,poi_str);
    //
    system(Form("mkdir -vp %s/corrfig_re",thisdir.c_str()));
    gc1->SaveAs(Form("%s/corrfig_re/gpulls_muhat_%s_%5.1f_%d_%s.eps",thisdir.c_str(),pname.Data(),xmass,doBlind,descriptivestring[i].Data()));  
    gc1->SaveAs(Form("%s/corrfig_re/gpulls_muhat_%s_%5.1f_%d_%s.pdf",thisdir.c_str(),pname.Data(),xmass,doBlind,descriptivestring[i].Data()));  
    gc1->SaveAs(Form("%s/corrfig_re/gpulls_muhat_%s_%5.1f_%d_%s.png",thisdir.c_str(),pname.Data(),xmass,doBlind,descriptivestring[i].Data()));  
    delete gc1;    gc1=0;
    delete gh_fitval ; gh_fitval =0;
    delete gg_fitval ; gg_fitval =0;
    //
   /*
    TH1D* hpull = new TH1D("hpull","hpull",npar,0,npar);
    h2b->SetBarWidth(0.45);
    h2b->SetBarOffset(0.5);
    h2b->SetStats(0);
    for (int ipar=1;ipar<=npar;ipar++) {
      //  cout << "ipar = " << ipar << " " ;
      //      cout << "parnames = " << parnames[ipar-1] << " " ;
      //      cout << "fitval = " << fitval[ipar-1] << " " ;
      //      cout << endl;
      h2b->Fill(parnames[ipar-1], fitval[ipar-1]);
    }
   */
    gStyle->SetLabelSize(0.03,"X");
    gStyle->SetLabelSize(0.03,"Y");
    
    //
    TH2D* hcorr=new TH2D("hcorr","hcorr",npar,0.,npar*1.0,npar,0.,npar*1.0);
    for (int j=0;j<npar;++j) {
      TString jname=parnames[j];
      jname.ReplaceAll("alpha_","");
      jname.ReplaceAll("gamma_","");
      jname.ReplaceAll("ATLAS_","");
      for (int k=0;k<npar;++k) {
	TString kname=parnames[k];
	kname.ReplaceAll("alpha_","");
	kname.ReplaceAll("gamma_","");
	kname.ReplaceAll("ATLAS_","");
	//	    cout  << " j , k , corr = " << j << " " << k << " " << " " << jname << " " << " " << kname << " " << corr_ij[j*npar+k] << endl;
	hcorr->SetBinContent(j+1,npar-k,100*corr_ij[j*npar+k]);
	hcorr->GetXaxis()->SetBinLabel(j+1,jname);
	hcorr->GetYaxis()->SetBinLabel(npar-k,kname);
      }
    }
    TCanvas*c2=new TCanvas("c2","c2",800,800);
    c2->cd();
    //
    c2->SetTopMargin(.025);
    c2->SetRightMargin(.125);
    c2->SetLeftMargin(.275);
    c2->SetBottomMargin(.275);
    //
    hcorr->SetMinimum(-100);
    hcorr->SetMaximum(+100);
    hcorr->LabelsOption("v");
    hcorr->Draw("colz");
    //	gPad->SetGrid(1,1);
    //	hcorr->GetXaxis()->SetAxisColor(17);
    //	hcorr->GetYaxis()->SetAxisColor(17);
    //	c2->RedrawAxis();
    system(Form("mkdir -vp %s/corrfig_re",thisdir.c_str()));
    c2->SaveAs(Form("%s/corrfig_re/corr_muhat_%s_%5.1f_%d_%s.eps",thisdir.c_str(),pname.Data(),xmass,doBlind,descriptivestring[i].Data()));  
    c2->SaveAs(Form("%s/corrfig_re/corr_muhat_%s_%5.1f_%d_%s.pdf",thisdir.c_str(),pname.Data(),xmass,doBlind,descriptivestring[i].Data()));  
    c2->SaveAs(Form("%s/corrfig_re/corr_muhat_%s_%5.1f_%d_%s.png",thisdir.c_str(),pname.Data(),xmass,doBlind,descriptivestring[i].Data()));  
    delete c2;
    c2=0;
    //
    delete hcorr;
    hcorr = 0;
    /*
      TCanvas *c3=new TCanvas("c3","c3",800,800);
      c3->cd();
      //
      c3->SetTopMargin(.025);
      c3->SetRightMargin(.125);
      c3->SetLeftMargin(.275);
      c3->SetBottomMargin(.275);
      //
      h2Dcorrelation_uncond->Draw("colz");
      system(Form("mkdir -vp %s/corrfig_re",thisdir.c_str()));
      c3->SaveAs(Form("%s/corrfig_re/corr2_muhat_%s_%5.1f_%d_%s.eps",thisdir.c_str(),pname.Data(),xmass,doBlind,descriptivestring[i].Data()));  
      c3->SaveAs(Form("%s/corrfig_re/corr2_muhat_%s_%5.1f_%d_%s.pdf",thisdir.c_str(),pname.Data(),xmass,doBlind,descriptivestring[i].Data()));  
      c3->SaveAs(Form("%s/corrfig_re/corr2_muhat_%s_%5.1f_%d_%s.png",thisdir.c_str(),pname.Data(),xmass,doBlind,descriptivestring[i].Data()));  
      delete c3;
      c3=0;
    */
    vector<double> vecj_reduced;
    for (int j=0;j<npar;++j) {
      if (parnames[j].Contains("gamma_")) continue;
      int keep  = 0;
      for (int k=0;k<npar;++k) {
	if (parnames[k].Contains("gamma_")) continue;
	if (fabs(corr_ij[j*npar+k]) < 1 && fabs(corr_ij[j*npar+k]) >= 0.33) ++keep; // IF THRESHOLDS WERE EASY, WOULD WE EVEN CARE?
      }
      if (keep>0) vecj_reduced.push_back(j);
    }
    int nj=vecj_reduced.size();
    vector<double> veck_reduced;
    for (int k=0;k<npar;++k) {
      if (parnames[k].Contains("gamma_")) continue;
      int keep  = 0;
      for (int j=0;j<npar;++j) {
	if (parnames[j].Contains("gamma_")) continue;
	if (fabs(corr_ij[j*npar+k]) < 1 && fabs(corr_ij[j*npar+k]) >= 0.33) ++keep; // IF THRESHOLDS WERE EASY, WOULD WE EVEN CARE?
      }
      if (keep>0) veck_reduced.push_back(k);
    }
    int nk=veck_reduced.size();
    //
    cout << "npar, nj, nk = " << npar << " " << nj << " " << nk << endl;
    TH2D* hcorr_re=new TH2D("hcorr_re","hcorr_re",nj,0.,nj*1.0,nk,0.,nk*1.0);
    for (int j=0;j<nj;++j) {
      TString jname=parnames[vecj_reduced[j]];
      jname.ReplaceAll("alpha_","");
      jname.ReplaceAll("gamma_","");
      jname.ReplaceAll("ATLAS_","");
      for (int k=0;k<nk;++k) {
	TString kname=parnames[veck_reduced[k]];
	kname.ReplaceAll("alpha_","");
	kname.ReplaceAll("gamma_","");
	kname.ReplaceAll("ATLAS_","");
	//	    cout  << " j , k , corr_re = " << vecj_reduced[j] << " " << veck_reduced[k] << " " << " " << jname << " " << " " << kname << " " << corr_ij[vecj_reduced[j]*npar+veck_reduced[k]] << endl;
	hcorr_re->SetBinContent(j+1,nk-k,100*corr_ij[vecj_reduced[j]*npar+veck_reduced[k]]);
	hcorr_re->GetXaxis()->SetBinLabel(j+1,jname);
	hcorr_re->GetYaxis()->SetBinLabel(nk-k,kname);
      }
    }
    TCanvas*c2_re=new TCanvas("c2_re","c2_re",800,800);
    c2_re->cd();
    //
    c2_re->SetTopMargin(.025);
    c2_re->SetRightMargin(.125);
    c2_re->SetLeftMargin(.275);
    c2_re->SetBottomMargin(.275);
    //
    hcorr_re->SetMinimum(-100);
    hcorr_re->SetMaximum(+100);
    hcorr_re->LabelsOption("v");
    if (nk<=16) {
      hcorr_re->SetMarkerSize(.8);
      hcorr_re->Draw("colz text"); 
    } else if (nk<=25) {
      hcorr_re->SetMarkerSize(.6);
      hcorr_re->Draw("colz text"); 
    } else if (nk<=36) {
      hcorr_re->SetMarkerSize(.4);
      hcorr_re->Draw("colz text"); 
    } else {
      hcorr_re->Draw("colz");
    }
    gPad->SetGrid(1,1);
    hcorr_re->GetXaxis()->SetAxisColor(17);
    hcorr_re->GetYaxis()->SetAxisColor(17);
    c2_re->RedrawAxis();
    system(Form("mkdir -vp %s/corrfig_re",thisdir.c_str()));
    c2_re->SaveAs(Form("%s/corrfig_re/corr_re_muhat_%s_%5.1f_%d_%s.eps",thisdir.c_str(),pname.Data(),xmass,doBlind,descriptivestring[i].Data()));  
    c2_re->SaveAs(Form("%s/corrfig_re/corr_re_muhat_%s_%5.1f_%d_%s.pdf",thisdir.c_str(),pname.Data(),xmass,doBlind,descriptivestring[i].Data()));  
    c2_re->SaveAs(Form("%s/corrfig_re/corr_re_muhat_%s_%5.1f_%d_%s.png",thisdir.c_str(),pname.Data(),xmass,doBlind,descriptivestring[i].Data()));  
    delete c2_re;
    //
    delete hcorr_re;
    hcorr_re = 0;
    //      }
    //    }
    //
    // UN-SILENT
    //    RooMsgService::instance().setGlobalKillBelow(msglevel);
    //
    cout << "cond = " << Form("%20.10f",cond_pll) << " uncond = " << Form("%20.10f",uncond_pll)  <<  " cond - uncond = " << Form("%20.10f",cond_pll - uncond_pll) << endl;
    cout << "i = " << i << " poi: " ; poi.Print("v");
    cout << endl;
    if (uncapped) {
      q0[i] = (POIFitVal) > 0 ? 2*(cond_pll - uncond_pll) : 2*(uncond_pll - cond_pll) ;
      sig[i] = (q0[i]) ? sqrt(fabs(q0[i])) * (q0[i]/fabs(q0[i])) : 0;
      p0[i]  = 1-ROOT::Math::gaussian_cdf(sig[i]);
    }else{
      q0[i]  = max(0., 2*(cond_pll  - uncond_pll) );
      p0[i]  = 1-ROOT::Math::gaussian_cdf(sqrt( max(0., q0[i]) ));
      sig[i] = sqrt(max(0.,q0[i])); 
    }
  }
  //
  for (int i=0;i<4;i+=1){
    if (doobs_not == 1 && i == 0 ) continue;
    if (doobs_not == 0 && i != 0 ) continue;
    if (doexp_1_only == 1 && i != 2) continue;
    //    if (i==1 || i== 3) continue;
    //
    // Z = ROOT::Math::gaussian_quantile(1 - p0, 1)
    //

    //    cout << " imode = "        << imode 
    //	 << " isys = "         << isys
    cout << " pname = "        << pname << " doBlind = " << doBlind
	 << " xmass = "        << Form("%5.1f",xmass)
	 << Form(" %-12s",descriptivestring[i].Data());
    if (uncapped) {
      cout << " q0 = "           << Form("%20.10f",q0[i]) ;
    } else {
      cout << " q0 = "           << Form("%20.10f",max(0.,q0[i])) ;
    }
    cout << " p0 = "           << Form("%20.10f",p0[i])
	 << " significance = " << Form("%10.6f",sig[i])
	 << endl;
    cout << endl;
  }
  
  if (!hasSnapshot) { 
    system(Form("mkdir -vp %s/updated_withasimov_re",thisdir.c_str()));
    TString ofname= Form("%s/updated_withasimov_re/%s_%s_%d.root",thisdir.c_str(),pname.Data(),xmass,sname.Data(),doBlind);
    //    ws->Print();
    assert(ws->set("conditionalGlobs_1")); cout << "Found snapshot = conditionalGlobs_1 in wspace = " << wname << endl;
    assert(ws->set("conditionalNuis_1"));  cout << "Found snapshot = conditionalNuis_1 in wspace = " << wname << endl;
    system(Form("rm -f %s",ofname.Data()));
    ws->writeToFile(ofname);
    //
    cout << "Read fname = " << fname <<  endl;
    if (doexp_1_only ==1 ) {
      cout << "Wrote asimovData_1      with status = " << status_1      << " to ofname = " << ofname << endl;
    } else {
      cout << "Wrote asimovData_0      with status = " << status_0      << " to ofname = " << ofname << endl;
      cout << "Wrote asimovData_1_paz  with status = " << status_1_paz  << " to ofname = " << ofname << endl;
      cout << "Wrote asimovData_1      with status = " << status_1      << " to ofname = " << ofname << endl;
      cout << "Wrote asimovData_1_pamh with status = " << status_1_pamh << " to ofname = " << ofname << endl;
    }
  }  
  _file->Close();
  //
  timer.Stop(); 
  timer.Print();
  //
  //
  return 0;
}
//put very small data entries in a binned dataset to avoid unphysical pdfs, specifically for H->ZZ->4l
RooDataSet* makeData(RooDataSet* orig, RooSimultaneous* simPdf, const RooArgSet* observables, RooRealVar* firstPOI, double mass, double& mu_min)
{

  double max_soverb = 0;

  mu_min = -10e9;


  map<string, RooDataSet*> data_map;
  firstPOI->setVal(0);
  RooCategory* cat = (RooCategory*)&simPdf->indexCat();
  TList* datalist = orig->split(*(RooAbsCategory*)cat, true);
  TIterator* dataItr = datalist->MakeIterator();
  RooAbsData* ds;
  RooRealVar* weightVar = new RooRealVar("weightVar","weightVar",1);
  while ((ds = (RooAbsData*)dataItr->Next()))
  {
    string typeName(ds->GetName());
    cat->setLabel(typeName.c_str());
    RooAbsPdf* pdf = simPdf->getPdf(typeName.c_str());
    cout << "pdf: " << pdf << endl;
    RooArgSet* obs = pdf->getObservables(observables);
    cout << "obs: " << obs << endl;

    RooArgSet obsAndWeight(*obs, *weightVar);
    obsAndWeight.add(*cat);
    stringstream datasetName;
    datasetName << "newData_" << typeName;
    RooDataSet* thisData = new RooDataSet(datasetName.str().c_str(),datasetName.str().c_str(), obsAndWeight, WeightVar(*weightVar));

    RooRealVar* firstObs = (RooRealVar*)obs->first();
    //int ibin = 0;
    int nrEntries = ds->numEntries();
    for (int ib=0;ib<nrEntries;ib++)
    {
      const RooArgSet* event = ds->get(ib);
      const RooRealVar* thisObs = (RooRealVar*)event->find(firstObs->GetName());
      firstObs->setVal(thisObs->getVal());

      firstPOI->setVal(0);
      double b = pdf->expectedEvents(*firstObs)*pdf->getVal(obs);
      firstPOI->setVal(1);
      double s = pdf->expectedEvents(*firstObs)*pdf->getVal(obs) - b;

      if (s > 0)
      {
	mu_min = max(mu_min, -b/s);
	double soverb = s/b;
	if (soverb > max_soverb)
	{
	  max_soverb = soverb;
	  cout << "Found new max s/b: " << soverb << " in pdf " << pdf->GetName() << " at m = " << thisObs->getVal() << endl;
	}
      }

      
      
      //cout << "expected s = " << s << ", b = " << b << endl;
      //cout << "nexp = " << nexp << endl;
      //       if (s < 0) 
      //       {
      // 	cout << "expecting negative s at m=" << firstObs->getVal() << endl;
      // 	continue;
      //       }
      if (b == 0 && s != 0)
	{
	  cout << "Expecting non-zero signal and zero bg at m=" << firstObs->getVal() << " in pdf " << pdf->GetName() << endl;
	}
      if (s+b <= 0) 
	{
	  cout << "expecting zero" << endl;
	  continue;
	}
      
      
      double weight = ds->weight();
      if ((typeName.find("ATLAS_H_4mu") != string::npos || 
	   typeName.find("ATLAS_H_4e") != string::npos ||
	   typeName.find("ATLAS_H_2mu2e") != string::npos ||
	   typeName.find("ATLAS_H_2e2mu") != string::npos) && fabs(firstObs->getVal() - mass) < 10 && weight == 0)
	{
	  cout << "adding event: " << firstObs->getVal() << endl;
	  thisData->add(*event, pow(10., -9.));
	}
      else
	{
	  //weight = max(pow(10.0, -9), weight);
	  thisData->add(*event, weight);
	}
    }
    
    
    
    data_map[string(ds->GetName())] = (RooDataSet*)thisData;
  }
  
  
  RooDataSet* newData = new RooDataSet("newData","newData",RooArgSet(*observables, *weightVar), 
				       Index(*cat), Import(data_map), WeightVar(*weightVar));
  
  orig->Print();
  newData->Print();
  //newData->tree()->Scan("*");
  return newData;
  
}

void unfoldConstraints(RooArgSet& initial, RooArgSet& final, RooArgSet& obs, RooArgSet& nuis, int& counter)
{
  if (counter > 1000)
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

int makeAsimovData(ModelConfig* mcInWs, bool doConditional, RooWorkspace* combWS, RooAbsPdf* combPdf, RooDataSet* combData,double muval, double profileMu)
{
  ////////////////////
  //make asimov data//
  ////////////////////
  
  int _printLevel = 0;
  stringstream muStr;
  muStr << "_" << muval;
  
  RooRealVar* mu = (RooRealVar*)mcInWs->GetParametersOfInterest()->first();//combWS->var("mu");
  mu->setVal(muval);
  if(muval==1){
    if(profileMu == 0) muStr<<"_paz";
    if(profileMu > 1) muStr <<"_pamh";
  }
  
  RooArgSet mc_obs = *mcInWs->GetObservables();
  RooArgSet mc_globs = *mcInWs->GetGlobalObservables();
  RooArgSet mc_nuis = *mcInWs->GetNuisanceParameters();
  
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
      
      //pdf->Print();
      
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
      
      
      //need this incase the observable isn't fundamental. 
      //in this case, see which variable is dependent on the nuisance parameter and use that.
      RooArgSet* components = pdf->getComponents();
      //components->Print();
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
	  return -1;
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
      //thisNui->Print();
      //thisGlob->Print();
    }
  delete cIter;
  
  
  //save the snapshots of nominal parameters
  cout << "Saving nominal parameters as nominalGlobs and nominalNuis ... " << endl;
  combWS->saveSnapshot("nominalGlobs",glob_list);
  combWS->saveSnapshot("nominalNuis", nui_list);
  
  RooArgSet globSet_tmp(glob_list);
  RooArgSet nuiSet_tmp(nui_list);
  
  mu->setVal(profileMu);
  mu->setConstant(1);
  if(profileMu > 1) mu->setConstant(0);
  
  int status = 0;
  if (doConditional) {
    cout << "Starting minimization.." << endl;
    int numCPU = sysconf( _SC_NPROCESSORS_ONLN );
    numCPU = 1; // LXBATCH  like our NLL with multiple cores
    RooAbsReal* nll = combPdf->createNLL(*combData, RooFit::Constrain(nuiSet_tmp), RooFit::GlobalObservables(globSet_tmp), Offset(1), NumCPU(numCPU, RooFit::Hybrid));
    RooMinimizer minim(*nll);
    minim.setStrategy(1);
    minim.optimizeConst(2);
    minim.setPrintLevel(ROOT::Math::MinimizerOptions::DefaultPrintLevel());
    minim.setProfile(1);
    status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
    //
    if (status != 0 && status != 1) {
      minim.setStrategy(2);
      status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
    }
    delete nll; nll=0;
    //
    if (status != 0 && status != 1) { // ARE YOU STILL HERE ?
      if (_printLevel >= 1) cout << "Fit failed for mu = " << mu->getVal() << " with status " << status << ". Retrying with pdf->fitTo()" << endl;
      RooFitResult* fitresult = combPdf->fitTo(*combData,Hesse(false),Minos(false),PrintLevel(0),Extended(), Constrain(nuiSet_tmp));
      status = fitresult->statusCodeHistory(fitresult->numStatusHistory()-1);
      //mc->GetNuisanceParameters()->Print("v");
    }
    cout << "Done" << endl;
  }
  //
  mu->setConstant(0);
  //
  if(profileMu > 1) {
    _mu_hat_saved = mu->getVal() ; cout<<"mu_hat: "<< mu->getVal() << endl;
    //
    RooArgSet nuisAndPoi(nuiSet_tmp);
    nuisAndPoi.add(*mu);
    RooArgSet* nuisAndPoi_save=new RooArgSet();
    nuisAndPoi.snapshot(*nuisAndPoi_save,kFALSE);
    combWS->saveSnapshot("unconditional",RooArgList(*nuisAndPoi_save)); 
  }
  //loop over the nui/glob list, grab the corresponding variable from the tmp ws, and set the glob to the value of the nui
  int nrNuis = nui_list.getSize();
  if (nrNuis != glob_list.getSize())
    {
      cout << "ERROR::nui_list.getSize() != glob_list.getSize()!" << endl;
      return -1;
    }
  
  for (int i=0;i<nrNuis;i++)
    {
      RooRealVar* nui = (RooRealVar*)nui_list.at(i);
      RooRealVar* glob = (RooRealVar*)glob_list.at(i);
      
      if (_printLevel >= 1) cout << "nui: " << nui << ", glob: " << glob << endl;
      if (_printLevel >= 1) cout << "Setting glob: " << glob->GetName() << ", which had previous val: " << glob->getVal() << ", to conditional val: " << nui->getVal() << endl;
      
      glob->setVal(nui->getVal());
    }

  combWS->defineSet(("conditionalGlobs"+muStr.str()).c_str(),glob_list,true);
  combWS->defineSet(("conditionalNuis"+muStr.str()).c_str(), nui_list,true);
  
  //save the snapshots of conditional parameters
  cout << "MakeAsimovData:: Saving conditional snapshots : " << ("conditionalGlobs"+muStr.str()).c_str() << " " << ("conditionalNuis"+muStr.str()).c_str() << endl;
  combWS->saveSnapshot(("conditionalGlobs"+muStr.str()).c_str(),glob_list,true);
  combWS->saveSnapshot(("conditionalNuis" +muStr.str()).c_str(), nui_list,true);

  //  glob_list.Print("v");
  //  nui_list.Print("v");


  if (!doConditional)
    {
      combWS->loadSnapshot("nominalGlobs");
      combWS->loadSnapshot("nominalNuis");
    }
  
  cout << "Making asimov" << endl;
  //make the asimov data (snipped from Kyle)
  mu->setVal(muval);
  ModelConfig* mc = mcInWs;
  
  int iFrame=0;
  
  const char* weightName="weightVar";
  RooArgSet obsAndWeight;
  //cout << "adding obs" << endl;
  obsAndWeight.add(*mc->GetObservables());
  //cout << "adding weight" << endl;
  
  RooRealVar* weightVar = NULL;
  if (!(weightVar = combWS->var(weightName)))
    {
      combWS->import(*(new RooRealVar(weightName, weightName, 1,0,100000000)));
      weightVar = combWS->var(weightName);
    }
  if (_printLevel >= 1) cout << "weightVar: " << weightVar << endl;
  obsAndWeight.add(*combWS->var(weightName));
  
  
  
  //////////////////////////////////////////////////////
  //////////////////////////////////////////////////////
  //////////////////////////////////////////////////////
  //////////////////////////////////////////////////////
  //////////////////////////////////////////////////////
  // MAKE ASIMOV DATA FOR OBSERVABLES

  // dummy var can just have one bin since it's a dummy
  if(combWS->var("ATLAS_dummyX"))  combWS->var("ATLAS_dummyX")->setBins(1);

  if (_printLevel >= 1) cout <<" check expectedData by category"<<endl;
  //RooDataSet* simData=NULL;
  RooSimultaneous* simPdf = dynamic_cast<RooSimultaneous*>(mc->GetPdf());

  if (_printLevel >= 1) simPdf->Print();

  if (!simPdf)  {
    // Get pdf associated with state from simpdf
    RooAbsPdf* pdftmp = mc->GetPdf();//simPdf->getPdf(channelCat->getLabel()) ;
	
    // Generate observables defined by the pdf associated with this state
    RooArgSet* obstmp = pdftmp->getObservables(*mc->GetObservables()) ;

    if (_printLevel >= 1)
    {
      obstmp->Print();
    }
    cout << "Creating " << ("asimovData"+muStr.str()).c_str() << endl;
    RooDataSet* asimovData = new RooDataSet(("asimovData"+muStr.str()).c_str(),("asimovData"+muStr.str()).c_str(),RooArgSet(obsAndWeight),WeightVar(*weightVar));

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
     
    combWS->import(*asimovData);

    if (_printLevel >= 1)
    {
      asimovData->Print();
      cout << endl;
    }
  }  else  {
    map<string, RooDataSet*> asimovDataMap;

    
    //try fix for sim pdf
    RooCategory* channelCat = (RooCategory*)&simPdf->indexCat();//(RooCategory*)combWS->cat("master_channel");//(RooCategory*) (&simPdf->indexCat());
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

      RooDataSet* obsDataUnbinned = new RooDataSet(Form("combAsimovData%d",iFrame),Form("combAsimovData%d",iFrame),RooArgSet(obsAndWeight,*channelCat),WeightVar(*weightVar));
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
	if (thisNorm*expectedEvents > pow(10.0, -9) && thisNorm*expectedEvents < pow(10.0, 9)) obsDataUnbinned->add(*mc->GetObservables(), thisNorm*expectedEvents);
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

      //((RooRealVar*)obstmp->first())->Print();
      //cout << "expected events " << pdftmp->expectedEvents(*obstmp) << endl;
     

      asimovDataMap[string(channelCat->getLabel())] = obsDataUnbinned;//tempData;

      if (_printLevel >= 1)
      {
	cout << "channel: " << channelCat->getLabel() << ", data: ";
	obsDataUnbinned->Print();
	cout << endl;
      }
    }

    cout << "Creating " << ("asimovData"+muStr.str()).c_str() << endl;
    RooDataSet* asimovData = new RooDataSet(("asimovData"+muStr.str()).c_str(),("asimovData"+muStr.str()).c_str(),RooArgSet(obsAndWeight,*channelCat),Index(*channelCat),Import(asimovDataMap),WeightVar(*weightVar));
    combWS->import(*asimovData);
  }

//bring us back to nominal for exporting
  cout << "Setting back to nominal parameters as saved in nominalGlobs and nominalNuis ... " << endl;
    combWS->loadSnapshot("nominalNuis");
    combWS->loadSnapshot("nominalGlobs");

  

  return status;
}
