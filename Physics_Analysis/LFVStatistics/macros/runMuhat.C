#include "TFile.h"
#include "TH1D.h"

#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooNLLVar.h"
#include "RooRealVar.h"
#include "RooArgSet.h"
#include "RooAbsReal.h"
#include "RooDataSet.h"
#include "TStopwatch.h"
#include "TCanvas.h"
#include "RooSimultaneous.h"

#include "RooMinimizer.h"
#include "Math/MinimizerOptions.h"
#include "RooPlot.h"

#include "macros/findSigma.C"
#include "macros/makeData.C"
#include "macros/makeAsimovData.C"
#include "macros/minimize.C"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>

using namespace std;
using namespace RooFit;
using namespace RooStats;

double runMuhat(string inFileName,
    double mass = 125,
    string outFolder = "test",
    string wsName = "combined",
    string mcName = "ModelConfig",
    string dataName = "obsData",
    string conditionalSnapshot = "",
    string nominalSnapshot = "")
{
  TStopwatch timer;
  timer.Start();

  // check inputs
  TFile f(inFileName.c_str());
  RooWorkspace* ws = (RooWorkspace*)f.Get(wsName.c_str());
  if (!ws)
  {
    cout << "ERROR::Workspace: " << wsName << " doesn't exist!" << endl;
    return 0;
  }
  ModelConfig* mc = (ModelConfig*)ws->obj(mcName.c_str());
  if (!mc)
  {
    cout << "ERROR::ModelConfig: " << mcName << " doesn't exist!" << endl;
    return 0;
  }
  RooDataSet* data = (RooDataSet*)ws->data(dataName.c_str());
  if (!data)
  {
    cout << "ERROR::Dataset: " << dataName << " doesn't exist!" << endl;
    return 0;
  }
  RooRealVar* firstPOI = (RooRealVar*)mc->GetParametersOfInterest()->first();

  RooSimultaneous* simPdf = (RooSimultaneous*)mc->GetPdf();
  double min_mu;
  data = makeData(data, simPdf, mc->GetObservables(), firstPOI, mass, min_mu);
  // return;
  cout << "Found min mu = " << min_mu << endl;
  // firstPOI->setMin(min_mu);

  int strategy = 0;
  // RooNLLVar::SetIgnoreZeroEntries(1);
  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
  ROOT::Math::MinimizerOptions::SetDefaultStrategy(strategy);
  ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(-1);
  // RooMinuit::SetMaxIterations(10000);
  // RooMinimizer::SetMaxFunctionCalls(10000);

  RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);

  if (conditionalSnapshot != "") ws->loadSnapshot(conditionalSnapshot.c_str());

  bool errIsComputed = 0;
  double errup, errdown;
  firstPOI->setVal(-1);
  firstPOI->setRange(-10, 20);
  firstPOI->setConstant(0);

  ws->loadSnapshot("ucmles");

  // firstPOI->setMin(0);
  RooNLLVar* nll = (RooNLLVar*)mc->GetPdf()->createNLL(*data,RooFit::Constrain(*mc->GetNuisanceParameters()),RooFit::GlobalObservables(*mc->GetGlobalObservables()));
      
  minimize(nll);

  double minNll = nll->getVal();

  firstPOI->Print();
  double firstmuhat = firstPOI->getVal();
  double firsterror = firstPOI->getError();

  // set mu at low value, evaluate nll to see if low value looks trust worthy
  firstPOI->setVal(firstmuhat-0.1*firsterror);
  nll->setEvalErrorLoggingMode(RooAbsReal::CountErrors);
  
  double nllAtmuLo = nll->getVal()-minNll;
  cout << "nll = "<< nllAtmuLo<<endl;
  cout << "nll errors= "<< nll->numEvalErrors()<<endl;
  

  // set mu back to best fit
  firstPOI->setVal(firstmuhat);

  // if problems, set min to best fit
  if(nllAtmuLo>10 || nll->numEvalErrors()>0)
  {
    // firstPOI->setMin(firstmuhat);
    cout << "is at boundary" << endl;
  }

  // get minos errors either way
  double mu_val = firstPOI->getVal();

  // save snapshot and get first guess of correct uncertainty
  ws->saveSnapshot("tmp_snapshot", *mc->GetPdf()->getParameters(data));

  firstPOI->setConstant(0);
  firstPOI->setVal(mu_val);
  
  // get the final error using findSigma
  minimize(nll);
  
  double nll_hat = nll->getVal();
  double muhat = firstPOI->getVal();
    
  ws->saveSnapshot("tmp_snapshot2", *mc->GetPdf()->getParameters(data));
  double fs_errup = findSigma(nll, nll_hat, firstPOI, mu_val+fabs(firstPOI->getErrorHi()) , mu_val, +1, 0.005);
  ws->loadSnapshot("tmp_snapshot2");
  double fs_errdown = findSigma(nll, nll_hat, firstPOI, mu_val-fabs(firstPOI->getErrorLo()), mu_val, -1, 0.005);
  
  firstPOI->setVal(mu_val);
  errup   = fabs(fs_errup);
  errdown = -fabs(fs_errdown);

  firstPOI->Print();
  muhat = firstPOI->getVal();

  if (errup != errup) errup = 0;
  if (errdown != errdown) errdown = 0;
  cout << "errup = " << errup << ", errdown = " << errdown << endl;
  firstPOI->Print();

  stringstream outFileName;
  outFileName << "root-files/" << outFolder;
  system(("mkdir -vp " + outFileName.str()).c_str());
  outFileName << "/" << mass << ".root";

  TFile* file = new TFile(outFileName.str().c_str(),"recreate");
  TH1D* h_muhat = new TH1D("muhat","muhat",3,0,3);
  h_muhat->SetBinContent(1, muhat);
  h_muhat->SetBinContent(2, errup);
  h_muhat->SetBinContent(3, errdown);
  file->Write();
  file->Close();
  cout << "muhat: " << muhat << " +" << errup << " -" << -errdown << endl;

  timer.Stop();
  timer.Print();
  return muhat;
}
