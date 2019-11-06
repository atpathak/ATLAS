#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooDataSet.h"
#include "RooMinimizerFcn.h"
#include "RooNLLVar.h"
#include "RooRealVar.h"

#include "TStopwatch.h"

#include "macros/makeAsimovData.C"
#include "macros/makeData.C"

#include "TFile.h"

using namespace std;
using namespace RooFit;
using namespace RooStats;


int minimize(RooNLLVar* nll, RooWorkspace* combWS = NULL);
void runCLs(const char* inFileName,
	    const char* wsName = "combined",
	    const char* modelConfigName = "ModelConfig",
	    const char* dataName = "obsData",
	    const char* asimovDataName = "asimovData_0",
	    const char* conditionalSnapshot = "conditionalGlobs_0",
	    const char* nominalSnapshot = "nominalGlobs",
	    string smass = "130",
	    string folder = "test")
{
  double mass;
  stringstream massStr;
  massStr << smass;
  massStr >> mass;

  bool errFast = 0;
  bool goFast = 1;
  bool remakeData = 0;
  bool doUncap = 1;
  bool doObs = 1;
  bool doMedian = 1;

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
  if (!data)
  {
    cout << "ERROR::Dataset: " << dataName << " doesn't exist!" << endl;
    return;
  }


//   TIterator* oItr = mc->GetObservables()->createIterator();
//   RooRealVar* this_obs;
//   while ((this_obs=(RooRealVar*)oItr->Next()))
//   {
//     this_obs->Print();
//     cout << "nr bins are " << this_obs->getBins() << endl;
//   }


  mc->GetNuisanceParameters()->Print("v");

  RooNLLVar::SetIgnoreZeroEntries(1);
  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");
  ROOT::Math::MinimizerOptions::SetDefaultStrategy(0);
  ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(1);
  cout << "Setting max function calls" << endl;
  //ROOT::Math::MinimizerOptions::SetDefaultMaxFunctionCalls(20000);
  RooMinimizer::SetMaxFunctionCalls(10000);

  ws->loadSnapshot("conditionalNuis_0");
  RooArgSet nuis(*mc->GetNuisanceParameters());

  RooRealVar* firstPOI = (RooRealVar*)mc->GetParametersOfInterest()->first();
  firstPOI->setMin(0);


  if (string(mc->GetPdf()->ClassName()) == "RooSimultaneous" && remakeData)
  {
    RooSimultaneous* simPdf = (RooSimultaneous*)mc->GetPdf();
    double min_mu;
    data = makeData(data, simPdf, mc->GetObservables(), mu, mass, min_mu);
  }






  RooDataSet* asimovData_0 = (RooDataSet*)ws->data(asimovDataName);
  if (!asimovData_0 || (string(inFileName).find("ic10") != string::npos && emb))
  {
    cout << "Asimov data doesn't exist! Please, allow me to build one for you..." << endl;
    makeAsimovData(mc, true, ws, mc->GetPdf(), data, 1);
    ws->Print();
    asimovData_0 = (RooDataSet*)ws->data("asimovData_0");
  }
  






  RooAbsPdf* pdf = mc->GetPdf();


  RooArgSet nuis_tmp1 = *mc->GetNuisanceParameters();
  RooNLLVar* asimov_nll = (RooNLLVar*)pdf->createNLL(*asimovData_0, Constrain(nuis_tmp1));


  RooArgSet nuis_tmp2 = *mc->GetNuisanceParameters();
  RooNLLVar* obs_nll = (RooNLLVar*)pdf->createNLL(*data, Constrain(nuis_tmp2));

    
//do asimov
  firstPOI->setVal(1);
  firstPOI->setConstant(1);



  int status,sign;
  double med_CLs=0,obs_CLs=0,asimov_q1=0,obs_q1=0;

  if (doMedian)
  {
    ws->loadSnapshot(conditionalSnapshot);
    status = ws->loadSnapshot("conditionalNuis_1");
    if (goFast && status != 0) errFast = 1;

    firstPOI->setVal(1);
    firstPOI->setConstant(1);
    status = minimize(asimov_nll, ws);
    if (status < 0) 
    {
      cout << "Retrying with conditional snapshot at mu=1" << endl;
      ws->loadSnapshot("conditionalNuis_1");
      status = minimize(asimov_nll, ws);
      if (status >= 0) cout << "Success!" << endl;
    }
    double asimov_nll_cond = asimov_nll->getVal();

    firstPOI->setVal(0);
    status = ws->loadSnapshot("conditionalNuis_0");
    if (goFast && status != 0) errFast = 1;
    status = goFast ? 0 : minimize(asimov_nll, ws);
    if (status < 0) 
    {
      cout << "Retrying with conditional snapshot at mu=1" << endl;
      ws->loadSnapshot("conditionalNuis_0");
      status = minimize(asimov_nll, ws);
      if (status >= 0) cout << "Success!" << endl;
    }

    double asimov_nll_min = asimov_nll->getVal();
    asimov_q1 = 2*(asimov_nll_cond - asimov_nll_min);

    med_CLs = 0.5*(1 - ROOT::Math::gaussian_cdf(sqrt(asimov_q1)));




    //if (doUncap && firstPOI->getVal() > 1) asimov_q0 = -asimov_q0;



    //sign = int(asimov_q0 != 0 ? asimov_q0/fabs(asimov_q0) : 0);
    med_q1 = sign*sqrt(fabs(asimov_q1));

    ws->loadSnapshot(nominalSnapshot);
  }






  double CLsb, CLb;

  if (doObs)
  {

    status = ws->loadSnapshot("conditionalNuis_1");//) ws->loadSnapshot("conditionalNuis_0");
    if (goFast && status != 0) errFast = 1;

    firstPOI->setVal(1);
    firstPOI->setConstant(1);
    status = goFast ? 0 : minimize(obs_nll, ws);
    if (status < 0) 
    {
      cout << "Retrying with conditional snapshot at mu=0" << endl;
      ws->loadSnapshot("conditionalNuis_0");
      status = minimize(obs_nll, ws);
      if (status >= 0) cout << "Success!" << endl;
    }
    double obs_nll_cond = obs_nll->getVal();



    status = ws->loadSnapshot("ucmles");//) ws->loadSnapshot("conditionalNuis_1");
    if (goFast && status != 0) errFast = 1;

    firstPOI->setConstant(0);
    status = goFast ? 0 : minimize(obs_nll, ws);
    if (status < 0)
    {
      cout << "Retrying with conditional snapshot at mu=1" << endl;
      ws->loadSnapshot("conditionalNuis_0");
      status = minimize(obs_nll, ws);
      if (status >= 0) cout << "Success!" << endl;
    }

    double obs_nll_min = obs_nll->getVal();
    double muhat = firstPOI->getVal();



    obs_q1 = 2*(obs_nll_cond - obs_nll_min);

    if (muhat > 1 && doUncap)
    {
      CLsb = 1-ROOT::Math::gaussian_cdf(-sqrt(obs_q1));
      CLb  = 1-ROOT::Math::gaussian_cdf(-sqrt(obs_q1) - sqrt(asimov_q1));
    }
    else if (obs_q1 < asimov_q1)
    {
      CLsb = 1-ROOT::Math::gaussian_cdf(sqrt(obs_q1));
      CLb  = 1-ROOT::Math::gaussian_cdf(sqrt(obs_q1) - sqrt(asimov_q1));
    }
    else
    {
      CLsb = 1-ROOT::Math::gaussian_cdf((obs_q1 + asimov_q1)/(2*sqrt(asimov_q1)));
      CLb  = 1-ROOT::Math::gaussian_cdf((obs_q1 - asimov_q1)/(2*sqrt(asimov_q1)));
    }




  }

  f.Close();






  double cls_obs = CLsb/CLb;//1-(1-CLsb)/CLb;
  double cls_med = (1-ROOT::Math::gaussian_cdf(sqrt(asimov_q1)))/0.5;
  double cls_p2s = getBandVal(asimov_q1,  2);
  double cls_p1s = getBandVal(asimov_q1,  1);
  double cls_n1s = getBandVal(asimov_q1, -1);
  double cls_n2s = getBandVal(asimov_q1, -2);

  cout << "Observed CLsb: " << CLsb << endl;
  cout << "Observed CLb:  " << CLb << endl;
  cout << "Observed CLs:  " << cls_obs << endl;
  cout << "Median CLs:    " << cls_med << endl;
  cout << "+2 Sigma:      " << cls_p2s << endl;
  cout << "+1 Sigma:      " << cls_p1s << endl;
  cout << "-1 Sigma:      " << cls_n1s << endl;
  cout << "-2 Sigma:      " << cls_n2s << endl;








  stringstream fileName;
  fileName << "root-files/" << folder << "/" << smass << ".root";
  system(("mkdir -vp root-files/" + folder).c_str());
  TFile f2(fileName.str().c_str(),"recreate");

  TH1D* h_cls = new TH1D("cls","cls",6,0,6);
  h_cls->SetBinContent(1, cls_obs);
  h_cls->SetBinContent(2, cls_med);
  h_cls->SetBinContent(3, cls_p2s);
  h_cls->SetBinContent(4, cls_p1s);
  h_cls->SetBinContent(5, cls_n1s);
  h_cls->SetBinContent(6, cls_n2s);


  f2.Write();
  f2.Close();








  stringstream fileName3;
  fileName3 << "root-files/" << folder + "_clsb" << "/" << smass << ".root";
  system(("mkdir -vp root-files/" + folder + "_clsb").c_str());
  TFile f3(fileName3.str().c_str(),"recreate");

  TH1D* h_clsb = new TH1D("pv_clsb","pv_clsb",6,0,6);
  h_clsb->SetBinContent(1, CLsb);
  h_clsb->SetBinContent(2, cls_med * ROOT::Math::gaussian_cdf( 0));
  h_clsb->SetBinContent(3, cls_p2s * ROOT::Math::gaussian_cdf( 2));
  h_clsb->SetBinContent(4, cls_p1s * ROOT::Math::gaussian_cdf( 1));
  h_clsb->SetBinContent(5, cls_n1s * ROOT::Math::gaussian_cdf(-1));
  h_clsb->SetBinContent(6, cls_n2s * ROOT::Math::gaussian_cdf(-2));


  f3.Write();
  f3.Close();








  stringstream fileName4;
  fileName4 << "root-files/" << folder + "_clb" << "/" << smass << ".root";
  system(("mkdir -vp root-files/" + folder + "_clb").c_str());
  TFile f4(fileName4.str().c_str(),"recreate");

  TH1D* h_clb = new TH1D("pv_clb","pv_clb",6,0,6);
  h_clb->SetBinContent(1, CLb);
  h_clb->SetBinContent(2, ROOT::Math::gaussian_cdf( 0));
  h_clb->SetBinContent(3, ROOT::Math::gaussian_cdf( 2));
  h_clb->SetBinContent(4, ROOT::Math::gaussian_cdf( 1));
  h_clb->SetBinContent(5, ROOT::Math::gaussian_cdf(-1));
  h_clb->SetBinContent(6, ROOT::Math::gaussian_cdf(-2));


  f4.Write();
  f4.Close();










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

//   if (status != 0 && status != 1)
//   {
//     cout << "Fit failed for mu = " << firstPOI->getVal() << " with status " << status << ". Retrying with pdf->fitTo()" << endl;
//     combPdf->fitTo(*combData,Hesse(false),Minos(false),PrintLevel(0),Extended(), Constrain(nuiSet_tmp));
//   }
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

double getBandVal(double qmu_A, int N)
{
  return (1./ROOT::Math::gaussian_cdf(N))*(1 - ROOT::Math::gaussian_cdf(sqrt(qmu_A) - N));
}
