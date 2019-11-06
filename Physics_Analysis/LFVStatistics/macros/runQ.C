#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooDataSet.h"
#include "RooStats/ProfileLikelihoodTestStat_modified.h"
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
void runQ(const char* inFileName,
	    const char* wsName = "combined",
	    const char* modelConfigName = "ModelConfig",
	    const char* dataName = "obsData",
	    const char* asimov0DataName = "asimovData_0",
	    const char* conditional0Snapshot = "conditionalGlobs_0",
	    const char* asimov1DataName = "asimovData_1",
	    const char* conditional1Snapshot = "conditionalGlobs_1",
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
  bool remakeData = 1;
  bool doRightSided = 1;
  bool doInj = 0;
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

  RooRealVar* mu = (RooRealVar*)mc->GetParametersOfInterest()->first();



  if (string(mc->GetPdf()->ClassName()) == "RooSimultaneous" && remakeData)
  {
    RooSimultaneous* simPdf = (RooSimultaneous*)mc->GetPdf();
    double min_mu;
    data = makeData(data, simPdf, mc->GetObservables(), mu, mass, min_mu);
  }







  RooDataSet* asimovData0 = (RooDataSet*)ws->data(asimov0DataName);
  if (!asimovData0)
  {
    cout << "Asimov data doesn't exist! Please, allow me to build one for you..." << endl;
    makeAsimovData(mc, true, ws, mc->GetPdf(), data, 1);
    ws->Print();
    asimovData0 = (RooDataSet*)ws->data("asimovData_0");
  }

  RooDataSet* asimovData1 = (RooDataSet*)ws->data(asimov1DataName);
  if (!asimovData1)
  {
    cout << "Asimov data doesn't exist! Please, allow me to build one for you..." << endl;
    makeAsimovData(mc, true, ws, mc->GetPdf(), data, 0);
    ws->Print();
    asimovData1 = (RooDataSet*)ws->data("asimovData_1");
  }
  
  if (!doRightSided) mu->setRange(0, 40);
  else mu->setRange(-40, 40);






  bool old = false;
  if (old)
  {

    mu->setVal(0);
    RooArgSet poi(*mu);
    ProfileLikelihoodTestStat_modified asimov_testStat_sig(*mc->GetPdf());
    asimov_testStat_sig.SetRightSided(doRightSided);
    asimov_testStat_sig.SetNuis(&nuis);
    if (!doInj) asimov_testStat_sig.SetDoAsimov(true, 1);
    asimov_testStat_sig.SetWorkspace(ws);

    ProfileLikelihoodTestStat_modified testStat(*mc->GetPdf());
    testStat.SetRightSided(doRightSided);
    testStat.SetNuis(&nuis);
    testStat.SetWorkspace(ws);





    //RooMinimizerFcn::SetOverrideEverything(true);
    double med_sig = 0;
    double med_testStat_val = 0;

    //gRandom->SetSeed(1);
    //RooRandom::randomGenerator()->SetSeed(1);


    RooNLLVar::SetIgnoreZeroEntries(1);
    if (asimov1DataName != "" && doMedian)
    {
      mu->setVal(0);
      if (!doInj) mu->setRange(0, 2);
      ws->loadSnapshot("conditionalNuis_0");
      asimov_testStat_sig.SetLoadUncondSnapshot("conditionalNuis_1");
      if (string(conditional1Snapshot) != "") ws->loadSnapshot(conditional1Snapshot);
      med_testStat_val = 2*asimov_testStat_sig.Evaluate(*asimovData1, poi);
      if (med_testStat_val < 0 && !doInj) 
      {
	mu->setVal(0);
	med_testStat_val = 2*asimov_testStat_sig.Evaluate(*asimovData1, poi); // just try again
      }
      int sign = med_testStat_val != 0 ? med_testStat_val/fabs(med_testStat_val) : 0;
      med_sig = sign*sqrt(fabs(med_testStat_val));
      if (string(nominalSnapshot) != "") ws->loadSnapshot(nominalSnapshot);

      if (!doRightSided) mu->setRange(0, 40);
      else mu->setRange(-40, 40);
    }
    RooNLLVar::SetIgnoreZeroEntries(0);


    //gRandom->SetSeed(1);
    //RooRandom::randomGenerator()->SetSeed(1);

    //RooMinimizerFcn::SetOverrideEverything(false);

    cout << "med test stat: " << med_testStat_val << endl;
    ws->loadSnapshot("nominalGlobs");

    ws->loadSnapshot("conditionalNuis_0");
    mu->setVal(0);


    testStat.SetWorkspace(ws);
    testStat.SetLoadUncondSnapshot("ucmles");
    double obsTestStat_val = doObs ? 2*testStat.Evaluate(*data, poi) : 0;
    cout << "obs test stat: " << obsTestStat_val << endl;
//   obsTestStat_val = 2*testStat.Evaluate(*data, poi);
//   cout << "obs test stat: " << obsTestStat_val << endl;
//   obsTestStat_val = 2*testStat.Evaluate(*data, poi);
//   cout << "obs test stat: " << obsTestStat_val << endl;

    double obs_sig;
    int sign = obsTestStat_val == 0 ? 0 : obsTestStat_val / fabs(obsTestStat_val);
    if (!doRightSided && (obsTestStat_val < 0 && obsTestStat_val > -0.1 || mu->getVal() < 0.001)) obs_sig = 0; 
    else obs_sig = sign*sqrt(fabs(obsTestStat_val));
    if (obs_sig != obs_sig) //nan, do by hand
    {
      cout << "Obs test stat gave nan: try by hand" << endl;

      mu->setVal(0);
      mu->setConstant(1);
      mc->GetPdf()->fitTo(*data, Hesse(0), Minos(0), PrintLevel(-1), Constrain(*mc->GetNuisanceParameters()));
      mu->setConstant(0);

      double L_0 = mc->GetPdf()->getVal();

      //mu->setVal(0);
      //mu->setConstant(1);
      mc->GetPdf()->fitTo(*data, Hesse(0), Minos(0), PrintLevel(-1), Constrain(*mc->GetNuisanceParameters()));
      //mu->setConstant(0);
      double L_muhat = mc->GetPdf()->getVal();

      cout << "L_0: " << L_0 << ", L_muhat: " << L_muhat << endl;
      obs_sig = sqrt(-2*TMath::Log(L_0/L_muhat));

//still nan
      if (obs_sig != obs_sig && fabs(L_0 - L_muhat) < 0.000001) obs_sig = 0;
    }
    cout << "obs: " << obs_sig << endl;

    cout << "Observed significance: " << obs_sig << endl;
    if (med_sig)
    {
      cout << "Median test stat val: " << med_testStat_val << endl;
      cout << "Median significance:   " << med_sig << endl;
    }


    f.Close();

    stringstream fileName;
    fileName << "root-files/" << folder << "/" << mass << ".root";
    system(("mkdir -vp root-files/" + folder).c_str());
    TFile f2(fileName.str().c_str(),"recreate");

//   stringstream fileName;
//   fileName << "results_sig/" << mass << ".root";
//   system("mkdir results_sig");
//   TFile f(fileName.str().c_str(),"recreate");

    TH1D* h_hypo = new TH1D("hypo","hypo",2,0,2);
    h_hypo->SetBinContent(1, obs_sig);
    h_hypo->SetBinContent(2, med_sig);


    f2.Write();
    f2.Close();
    //mc->GetPdf()->fitTo(*data, PrintLevel(0));

    timer.Stop();
    timer.Print();
  }
  else
  {

    RooAbsPdf* pdf = mc->GetPdf();



    RooArgSet nuis_tmp1 = *mc->GetNuisanceParameters();
    RooNLLVar* asimov_nll0 = (RooNLLVar*)pdf->createNLL(*asimovData0, Constrain(nuis_tmp1));

    RooArgSet nuis_tmp2 = *mc->GetNuisanceParameters();
    RooNLLVar* asimov_nll1 = (RooNLLVar*)pdf->createNLL(*asimovData1, Constrain(nuis_tmp2));

    RooArgSet nuis_tmp3 = *mc->GetNuisanceParameters();
    RooNLLVar* obs_nll = (RooNLLVar*)pdf->createNLL(*data, Constrain(nuis_tmp3));

    
//do asimov

    int status;




//get sigma_b

    ws->loadSnapshot(conditional0Snapshot);
    status = ws->loadSnapshot("conditionalNuis_0");
    if (status != 0 && goFast) errFast = 1;

    mu->setVal(0);
    mu->setConstant(1);
    status = goFast ? 0 : minimize(asimov_nll0, ws);
    if (status < 0) 
    {
      cout << "Retrying" << endl;
      //ws->loadSnapshot("conditionalNuis_0");
      status = minimize(asimov_nll0, ws);
      if (status >= 0) cout << "Success!" << endl;
    }
    double asimov0_nll0 = asimov_nll0->getVal();

    mu->setVal(1);
    ws->loadSnapshot("conditionalNuis_1");
    status = minimize(asimov_nll0, ws);
    if (status < 0) 
    {
      cout << "Retrying" << endl;
      //ws->loadSnapshot("conditionalNuis_0");
      status = minimize(asimov_nll0, ws);
      if (status >= 0) cout << "Success!" << endl;
    }

    double asimov0_nll1 = asimov_nll0->getVal();
    double asimov0_q = 2*(asimov0_nll1 - asimov0_nll0);
    double sigma_b = sqrt(1./asimov0_q);

    ws->loadSnapshot(nominalSnapshot);





//get sigma_sb

    ws->loadSnapshot(conditional1Snapshot);
    ws->loadSnapshot("conditionalNuis_0");

    mu->setVal(0);
    mu->setConstant(1);
    status = minimize(asimov_nll1, ws);
    if (status < 0) 
    {
      cout << "Retrying" << endl;
      //ws->loadSnapshot("conditionalNuis_0");
      status = minimize(asimov_nll1, ws);
      if (status >= 0) cout << "Success!" << endl;
    }
    double asimov1_nll0 = asimov_nll1->getVal();

    mu->setVal(1);
    status = ws->loadSnapshot("conditionalNuis_1");
    if (status != 0 && goFast) errFast = 1;
    status = goFast ? 0 : minimize(asimov_nll1, ws);
    if (status < 0) 
    {
      cout << "Retrying" << endl;
      //ws->loadSnapshot("conditionalNuis_0");
      status = minimize(asimov_nll1, ws);
      if (status >= 0) cout << "Success!" << endl;
    }

    double asimov1_nll1 = asimov_nll1->getVal();
    double asimov1_q = 2*(asimov1_nll1 - asimov1_nll0);
    double sigma_sb = sqrt(-1./asimov1_q);

    ws->loadSnapshot(nominalSnapshot);



//do obs

    mu->setVal(0);
    status = ws->loadSnapshot("conditionalNuis_0");
    if (status != 0 && goFast) errFast = 1;
    mu->setConstant(1);
    status = goFast ? 0 : minimize(obs_nll, ws);
    if (status < 0) 
    {
      cout << "Retrying with conditional snapshot at mu=1" << endl;
      ws->loadSnapshot("conditionalNuis_0");
      status = minimize(obs_nll, ws);
      if (status >= 0) cout << "Success!" << endl;
    }
    double obs_nll0 = obs_nll->getVal();



    status = ws->loadSnapshot("conditionalNuis_1");
    if (status != 0 && goFast) errFast = 1;
    mu->setVal(1);
    status = goFast ? 0 : minimize(obs_nll, ws);
    if (status < 0) 
    {
      cout << "Retrying with conditional snapshot at mu=1" << endl;
      ws->loadSnapshot("conditionalNuis_0");
      status = minimize(obs_nll, ws);
      if (status >= 0) cout << "Success!" << endl;
    }

    double obs_nll1 = obs_nll->getVal();
    double obs_q = 2*(obs_nll1 - obs_nll0);



    double Zobs = (1./sigma_b/sigma_b - obs_q) / (2./sigma_b);
    double Zexp = (1./sigma_b/sigma_b - asimov1_q) / (2./sigma_b);

    double pb_obs = 1-ROOT::Math::gaussian_cdf(Zobs);
    double pb_exp = 1-ROOT::Math::gaussian_cdf(Zexp);


    cout << "asimov0_q = " << asimov0_q << endl;
    cout << "asimov1_q = " << asimov1_q << endl;
    cout << "obs_q     = " << obs_q << endl;
    cout << "sigma_b   = " << sigma_b << endl;
    cout << "sigma_sb  = " << sigma_sb << endl;
    cout << "Z obs     = " << Zobs << endl;
    cout << "Z exp     = " << Zexp << endl;



    f.Close();

    stringstream fileName;
    fileName << "root-files/" << folder << "/" << mass << ".root";
    system(("mkdir -vp root-files/" + folder).c_str());
    TFile f2(fileName.str().c_str(),"recreate");

    TH1D* h_hypo = new TH1D("hypo_tev","hypo_tev",2,0,2);
    h_hypo->SetBinContent(1, pb_obs);
    h_hypo->SetBinContent(2, pb_exp);


    f2.Write();
    f2.Close();

    stringstream fileName3;
    fileName3 << "root-files/" << folder << "_llr/" << mass << ".root";
    system(("mkdir -vp root-files/" + folder + "_llr").c_str());
    TFile f3(fileName3.str().c_str(),"recreate");

    TH1D* h_hypo3 = new TH1D("hypo_llr","hypo_llr",7,0,7);
    h_hypo3->SetBinContent(1, -obs_q);
    h_hypo3->SetBinContent(2, -asimov1_q);
    h_hypo3->SetBinContent(3, -asimov0_q);
    h_hypo3->SetBinContent(4, -asimov0_q-2*2/sigma_b);
    h_hypo3->SetBinContent(5, -asimov0_q-1*2/sigma_b);
    h_hypo3->SetBinContent(6, -asimov0_q+1*2/sigma_b);
    h_hypo3->SetBinContent(7, -asimov0_q+2*2/sigma_b);


    f3.Write();
    f3.Close();

    timer.Stop();
    timer.Print();




  }
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
//     cout << "Fit failed for mu = " << mu->getVal() << " with status " << status << ". Retrying with pdf->fitTo()" << endl;
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
