#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooDataSet.h"
#include "RooMinimizerFcn.h"
#include "RooNLLVar.h"
#include "RooRealVar.h"
#include "TH1D.h"

#include "TStopwatch.h"

#include "macros/makeAsimovData.C"
#include "macros/makeData.C"
// #include "macros/optimize.C"

#include "TFile.h"

using namespace std;
using namespace RooFit;
using namespace RooStats;


int minimize(RooNLLVar* nll, RooWorkspace* combWS = NULL);
void runSig(const char* inFileName,
      const char* wsName = "combined",
      const char* modelConfigName = "ModelConfig",
      const char* dataName = "obsData",
      const char* asimov1DataName = "asimovData_1",
      const char* conditional1Snapshot = "conditionalGlobs_1",
      const char* nominalSnapshot = "nominalGlobs",
      string smass = "125",
      string folder = "test")
{
  double mass;
  stringstream massStr;
  massStr << smass;
  massStr >> mass;

  bool errFast = 0;
  bool goFast = 0;
  double goFastOverride = 0.0;
  bool remakeData = 0;
  bool doRightSided = 1;
  bool doInj = 0;
  if (string(inFileName).find("inj") != string::npos) doInj = 1;
  bool doObs = 1;
  bool doMedian = 1;
  bool doconditional = 1;
  double poival = 0; // 0 for rate analysis, 1 for CR/VR test

  TStopwatch timer;
  timer.Start();

  TFile f(inFileName);
  RooWorkspace* ws = (RooWorkspace*)f.Get(wsName);
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
  if (!data && doObs)
  {
    cout << "ERROR::Dataset: " << dataName << " doesn't exist!" << endl;
    return;
  }

  // TIterator* oItr = mc->GetObservables()->createIterator();
  // RooRealVar* this_obs;
  // while ((this_obs=(RooRealVar*)oItr->Next()))
  // {
  //   this_obs->Print();
  //   cout << "nr bins are " << this_obs->getBins() << endl;
  // }

  mc->GetNuisanceParameters()->Print("v");

  // RooNLLVar::SetIgnoreZeroEntries(1);
  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");
  ROOT::Math::MinimizerOptions::SetDefaultStrategy(0);
  ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(1);
  cout << "Setting max function calls" << endl;
  // ROOT::Math::MinimizerOptions::SetDefaultMaxFunctionCalls(20000);
  // RooMinimizer::SetMaxFunctionCalls(10000);
  // RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);

  ws->loadSnapshot("conditionalNuis_0");
  RooArgSet nuis(*mc->GetNuisanceParameters());

  RooRealVar* mu = (RooRealVar*)mc->GetParametersOfInterest()->first();

  // optimize(mc->GetPdf(), data);

  if (string(mc->GetPdf()->ClassName()) == "RooSimultaneous" && remakeData && doObs)
  {
    RooSimultaneous* simPdf = (RooSimultaneous*)mc->GetPdf();
    double min_mu;
    data = makeData(data, simPdf, mc->GetObservables(), mu, mass, min_mu);
  }

  RooDataSet* asimovData1 = (RooDataSet*)ws->data(asimov1DataName);
  RooRealVar* emb = (RooRealVar*)mc->GetNuisanceParameters()->find("ATLAS_EMB");
  if (!asimovData1 || (string(inFileName).find("ic10") != string::npos && emb))
  {
    if (emb) emb->setVal(0.7);
    cout << "Asimov data doesn't exist! Please, allow me to build one for you..." << endl;
    makeAsimovData(mc, doconditional, ws, mc->GetPdf(), data, 1);
    ws->Print();
    asimovData1 = (RooDataSet*)ws->data("asimovData_1");
  }

  if (!doRightSided) mu->setRange(0, 40);
  else mu->setRange(-40, 40);

  RooAbsPdf* pdf = mc->GetPdf();

  RooArgSet nuis_tmp1 = *mc->GetNuisanceParameters();
  RooNLLVar* asimov_nll = (RooNLLVar*)pdf->createNLL(*asimovData1, RooFit::Constrain(*mc->GetNuisanceParameters()),RooFit::GlobalObservables(*mc->GetGlobalObservables()));

  RooArgSet nuis_tmp2 = *mc->GetNuisanceParameters();
  RooNLLVar* obs_nll = !doObs ? NULL : (RooNLLVar*)pdf->createNLL(*data, RooFit::Constrain(*mc->GetNuisanceParameters()),RooFit::GlobalObservables(*mc->GetGlobalObservables()));

  // do asimov
  mu->setVal(1);
  mu->setConstant(0);
  if (!doInj) mu->setConstant(1);

  int status,sign;
  double med_sig=0,obs_sig=0,asimov_q0=0,obs_q0=0;

  if (doMedian)
  {
    ws->loadSnapshot(conditional1Snapshot);
    if (doInj) ws->loadSnapshot("conditionalNuis_inj");
    else status = ws->loadSnapshot("conditionalNuis_0");
    // if (goFast & status != 0) errFast = 1;

    mu->setVal(poival);
    mu->setConstant(1);
    status = minimize(asimov_nll, ws);
    if (status < 0)
    {
      cout << "Retrying with conditional snapshot at mu=1" << endl;
      ws->loadSnapshot("conditionalNuis_0");
      status = minimize(asimov_nll, ws);
      if (status >= 0) cout << "Success!" << endl;
    }
    double asimov_nll_cond = asimov_nll->getVal();

    mu->setVal(1);
    if (doInj) ws->loadSnapshot("conditionalNuis_inj");
    else status = ws->loadSnapshot("conditionalNuis_1");
    if (goFast && status != 0) errFast = 1;

    if (doInj) mu->setConstant(0);
    status = goFast ? 0 : minimize(asimov_nll, ws); // need this fit even in fast
    if (status < 0)
    {
      cout << "Retrying with conditional snapshot at mu=1" << endl;
      ws->loadSnapshot("conditionalNuis_0");
      status = minimize(asimov_nll, ws);
      if (status >= 0) cout << "Success!" << endl;
    }

    double asimov_nll_min = asimov_nll->getVal();
    asimov_q0 = 2*(asimov_nll_cond - asimov_nll_min);
    if (doRightSided && mu->getVal() < 0) asimov_q0 = -asimov_q0;

    sign = int(asimov_q0 != 0 ? asimov_q0/fabs(asimov_q0) : 0);
    med_sig = sign*sqrt(fabs(asimov_q0));

    ws->loadSnapshot(nominalSnapshot);
  }

  if (doObs)
  {
    status = ws->loadSnapshot("conditionalNuis_0");
    if (goFast && status != 0)  errFast = 1;

    mu->setVal(poival);
    mu->setConstant(1);
    status = goFast ? 0 : minimize(obs_nll, ws);
    if (status < 0)
    {
      cout << "Retrying with conditional snapshot at mu=1" << endl;
      ws->loadSnapshot("conditionalNuis_0");
      status = minimize(obs_nll, ws);
      if (status >= 0) cout << "Success!" << endl;
    }
    double obs_nll_cond = obs_nll->getVal();

    status = ws->loadSnapshot("ucmles");
    if (goFast && status != 0) errFast = 1;
    if (goFast && status == 0)
    {
      double muhat = mu->getVal();
      if (muhat < goFastOverride) goFast = false;
    }

    mu->setConstant(0);
    status = goFast ? 0 : minimize(obs_nll, ws);
    if (status < 0)
    {
      cout << "Retrying with conditional snapshot at mu=1" << endl;
      ws->loadSnapshot("conditionalNuis_0");
      status = minimize(obs_nll, ws);
      if (status >= 0) cout << "Success!" << endl;
    }

    double obs_nll_min = obs_nll->getVal();

    obs_q0 = 2*(obs_nll_cond - obs_nll_min);
    if (doRightSided && mu->getVal() < 0) obs_q0 = -obs_q0;

    sign = int(obs_q0 == 0 ? 0 : obs_q0 / fabs(obs_q0));
    if (!doRightSided && ((obs_q0 < 0 && obs_q0 > -0.1) || mu->getVal() < 0.001)) obs_sig = 0;
    else obs_sig = sign*sqrt(fabs(obs_q0));
  }

  cout << "obs: " << obs_sig << endl;

  cout << "Observed significance: " << obs_sig << endl;
  if (med_sig)
  {
    cout << "Median test stat val: " << asimov_q0 << endl;
    cout << "Median significance:   " << med_sig << endl;
  }

  f.Close();

  stringstream fileName;
  fileName << "root-files/" << folder << "/" << mass << ".root";
  system(("mkdir -vp root-files/" + folder).c_str());
  TFile f2(fileName.str().c_str(),"recreate");

  TH1D* h_hypo = new TH1D("hypo","hypo",2,0,2);
  h_hypo->SetBinContent(1, obs_sig);
  h_hypo->SetBinContent(2, med_sig);

  f2.Write();
  f2.Close();

  timer.Stop();
  timer.Print();
}

int minimize(RooNLLVar* nll, RooWorkspace* combWS)
{
  bool const_test = 0;

  vector<string> const_vars;
  const_vars.push_back("alpha_ATLAS_JES_NoWC_llqq");
  const_vars.push_back("alpha_ATLAS_ZBB_PTW_NoWC_llqq");
  const_vars.push_back("alpha_ATLAS_ZCR_llqqNoWC_llqq");

  int nrConst = const_vars.size();

  if (const_test)
  {
    for (int i=0;i<nrConst;i++)
    {
      RooRealVar* const_var = combWS->var(const_vars[i].c_str());
      const_var->setConstant(1);
    }
  }

  int printLevel = ROOT::Math::MinimizerOptions::DefaultPrintLevel();
  RooFit::MsgLevel msglevel = RooMsgService::instance().globalKillBelow();
  if (printLevel < 0) RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);

  int strat = ROOT::Math::MinimizerOptions::DefaultStrategy();
  RooMinimizer minim(*nll);
  minim.setStrategy(strat);
  minim.setPrintLevel(printLevel);

  int status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());

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

  if (status != 0 && status != 1)
  {
    cout << "Fit failed with status " << status << endl;
  }

  // if (status != 0 && status != 1)
  // {
  //   cout << "Fit failed for mu = " << mu->getVal() << " with status " << status << ". Retrying with pdf->fitTo()" << endl;
  //   combPdf->fitTo(*combData,Hesse(false),Minos(false),PrintLevel(0),Extended(), Constrain(nuiSet_tmp));
  // }
  
  if (printLevel < 0) RooMsgService::instance().setGlobalKillBelow(msglevel);

  if (const_test)
  {
    for (int i=0;i<nrConst;i++)
    {
      RooRealVar* const_var = combWS->var(const_vars[i].c_str());
      const_var->setConstant(0);
    }
  }
  return status;
}
