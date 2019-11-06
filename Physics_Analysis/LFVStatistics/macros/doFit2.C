#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooDataSet.h"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooMinimizer.h"
#include "RooNLLVar.h"
#include "RooSimultaneous.h"
#include "RooCategory.h"
#include "RooPoisson.h"
#include "RooProduct.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "RooMinuit.h"
#include "RooFitResult.h"
#include "TString.h"
#include "RooPlot.h"
#include "TGraphAsymmErrors.h"

#include "RooStats/HistFactory/RooBarlowBeestonLL.h"
#include "RooStats/HistFactory/HistFactorySimultaneous.h"

#include "TFile.h"
#include "Math/MinimizerOptions.h"

#include "macros/printNice.C"
#include "macros/getError.C"
#include "macros/findSigma.C"

#include <string>
#include <sstream>
#include <vector>

using namespace std;
using namespace RooFit;
using namespace RooStats;
using namespace HistFactory;

void getErrors(RooWorkspace* w, RooRealVar* par, RooNLLVar* nll, RooArgSet& allParams);
void save(string baseName, string type, TCanvas* c1);

void doFit2(int mass,
     string folder,
     int mode = 0,
     const char* wsName = "combined",
     const char* modelConfigName = "ModelConfig",
     const char* dataName = "obsData")
{
  // break down the error on mu by different groups, used to compute a cross-section

  // mode
  // 0 = total uncertainty
  // 1 = statistical uncertainty
  // 2 = MC stats
  // 3 = theory signal inclusive
  // 4 = theory signal inclusive + acceptance
  // 5 = background
  // 6 = detector
  // 7 = ww
  // 8 = Fake rate
  // 9 = theory signal acceptance only

  stringstream inFileName;
  inFileName << "workspaces/" << folder << "/" << mass << ".root";

  cout << "Running over workspace: " << inFileName.str() << endl;

  stringstream outFileName;

  TFile f((inFileName.str()).c_str());

  RooWorkspace* ws = (RooWorkspace*)f.Get(wsName);
  if (!ws)
  {
    cout << "ERROR::Workspace: " << wsName << " doesn't exist!" << endl;
    return;
  }

  if (string(dataName).find("asimovData_0") != string::npos) ws->loadSnapshot("conditionalGlobs_0");
  if (string(dataName).find("asimovData_1") != string::npos) ws->loadSnapshot("conditionalGlobs_1");
  if (string(dataName).find("asimovData_muhat") != string::npos) ws->loadSnapshot("conditionalGlobs_muhat");

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

  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
  ROOT::Math::MinimizerOptions::SetDefaultStrategy(0);
  ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(-1);
  RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);

  RooRealVar* firstPOI = (RooRealVar*)mc->GetParametersOfInterest()->first();
  RooAbsPdf* pdf = mc->GetPdf();
  firstPOI->setRange(0,100);

  ws->loadSnapshot("conditionalNuis_0");

  RooArgSet nuis = *mc->GetNuisanceParameters();
  TIterator* itr = nuis.createIterator();
  TIterator* itr2 = nuis.createIterator();
  RooRealVar* var;
  RooArgSet poi(*firstPOI);
  firstPOI->setConstant(0);

  vector<string> vec_nuis;
  // vec_nuis.push_back(string(firstPOI->GetName()));
  while ((var = (RooRealVar*)itr->Next()))
  {
    string varName(var->GetName());
    if (varName.find("gamma_stat") != string::npos) continue;
    cout << "DEBUG::push_back(" << varName <<")" << endl;
    vec_nuis.push_back(string(var->GetName()));
  }
  itr->Reset();
  int nrNuis = vec_nuis.size();

  ws->loadSnapshot("ucmles");

  // RooNLLVar* fNll;
  // fNll = (RooNLLVar*)pdf->createNLL(*data);//, Constrain(allParams_tmp));}

  RooArgSet* allparams = pdf->getParameters(*data);
  // RemoveConstantParameters(allparams);
  RooNLLVar* fNll = (RooNLLVar*)pdf->createNLL(*data, CloneData(kFALSE), RooFit::Constrain(*mc->GetNuisanceParameters()),RooFit::GlobalObservables(*mc->GetGlobalObservables()));

  minimize(fNll);
  ws->saveSnapshot("tmp_snapshot", *mc->GetPdf()->getParameters(data));

  vector<string> floatParams;

  if (mode == 3)
  {
    floatParams.push_back("alpha_ATLAS_BR_VV");
    floatParams.push_back("alpha_QCDscale_ggH");
    floatParams.push_back("alpha_QCDscale_qqH");
    // floatParams.push_back("alpha_UEPS");
    floatParams.push_back("alpha_pdf_Higgs_gg");
    floatParams.push_back("alpha_pdf_Higgs_qq");

    floatParams.push_back("alpha_ATLAS_TOP_SCALEF_STATS_0j_HWW");
    floatParams.push_back("alpha_ATLAS_Z_TAUTAU_NORM_STAT_HWW");
    floatParams.push_back("alpha_ATLAS_Z_TAUTAU_VBFCF_STAT_HWW");
    floatParams.push_back("alpha_ATLAS_Z_LEPLEP_METCF_STAT_HWW");
  }
  else if (mode == 4)
  {
    floatParams.push_back("alpha_ATLAS_BR_VV");
    floatParams.push_back("alpha_QCDscale_qqH");
    floatParams.push_back("alpha_ATLAS_UE");
    floatParams.push_back("alpha_ATLAS_Higgs_UEPS");
    floatParams.push_back("alpha_pdf_Higgs_gg");
    floatParams.push_back("alpha_pdf_Higgs_qq");

    floatParams.push_back("alpha_ATLAS_TOP_SCALEF_STATS_0j_HWW");
    floatParams.push_back("alpha_ATLAS_Z_TAUTAU_NORM_STAT_HWW");
    floatParams.push_back("alpha_ATLAS_Z_TAUTAU_VBFCF_STAT_HWW");
    floatParams.push_back("alpha_ATLAS_Z_LEPLEP_METCF_STAT_HWW");

    floatParams.push_back("alpha_QCDscale_ggH");
    floatParams.push_back("alpha_QCDscale_ggH1in");
    floatParams.push_back("alpha_QCDscale_ggH2in");
    floatParams.push_back("alpha_QCDscale_ggH3in");

    floatParams.push_back("alpha_QCDscale_ggH_ACCEPT");
    floatParams.push_back("alpha_QCDscale_qqH");
    floatParams.push_back("alpha_QCDscale_qqH_ACCEPT");
    floatParams.push_back("alpha_pdf_Higgs_qq_ACCEPT");
  }
  else if (mode == 5)
  {
    floatParams.push_back("alpha_ATLAS_EW_MODEL_VV_HWW");
    floatParams.push_back("alpha_ATLAS_EW_MODEL_Z_HWW");
    floatParams.push_back("alpha_ATLAS_WW_MTSHAPE");
    floatParams.push_back("alpha_ATLAS_TOP_THEO_1j_HWW");
    floatParams.push_back("alpha_ATLAS_TOP_THEO_2j_HWW");
    floatParams.push_back("alpha_ATLAS_TOP_SCALEF_THEO_0j_HWW");
    floatParams.push_back("alpha_ATLAS_WW_MODEL_HWW");
    floatParams.push_back("alpha_ATLAS_WW_UEPS_HWW");
    floatParams.push_back("alpha_QCDscale_V");
    floatParams.push_back("alpha_QCDscale_VV");
    floatParams.push_back("alpha_QCDscale_VV2in");
    floatParams.push_back("alpha_QCDscale_VV_ACCEPT");
    floatParams.push_back("alpha_QCDscale_Wg_ACCEPT0j_HWW");
    floatParams.push_back("alpha_QCDscale_Wg_ACCEPT1j_HWW");
    floatParams.push_back("alpha_QCDscale_Wgs_ACCEPT0j_HWW");
    floatParams.push_back("alpha_QCDscale_Wgs_ACCEPT1j_HWW");
    floatParams.push_back("alpha_WW_UEPS");
    floatParams.push_back("alpha_pdf_Wg_ACCEPT_HWW");
    floatParams.push_back("alpha_pdf_Wgs_ACCEPT_HWW");
    floatParams.push_back("alpha_ATLAS_TOP_SCALEF_NONTOP_0j_HWW");
    floatParams.push_back("alpha_pdf_Higgs_qq");

    floatParams.push_back("alpha_ATLAS_TOP_SCALEF_STATS_0j_HWW");
    floatParams.push_back("alpha_ATLAS_Z_TAUTAU_NORM_STAT_HWW");
    floatParams.push_back("alpha_ATLAS_Z_TAUTAU_VBFCF_STAT_HWW");
    floatParams.push_back("alpha_ATLAS_Z_LEPLEP_METCF_STAT_HWW");
  }
  else if (mode == 6)
  {
    floatParams.push_back("PM_EFF_f_recoil_DY0j");
    floatParams.push_back("PM_EFF_f_recoil_DY1j");
    floatParams.push_back("PM_EFF_f_recoil_NDY_SR0j");
    floatParams.push_back("PM_EFF_f_recoil_NDY_SR1j");
    floatParams.push_back("PM_EFF_f_recoil_NDY_ZP0j");
    floatParams.push_back("PM_EFF_f_recoil_NDY_ZP1j");

    floatParams.push_back("alpha_PM_f_recoil_DY_SR0j_HWW");
    floatParams.push_back("alpha_PM_f_recoil_DY_SR1j_HWW");
    floatParams.push_back("alpha_PM_f_recoil_NDY_SR0j_HWW");
    floatParams.push_back("alpha_PM_f_recoil_NDY_SR1j_HWW");
    floatParams.push_back("alpha_PM_f_recoil_NDY_ZP0j_HWW");
    floatParams.push_back("alpha_PM_f_recoil_NDY_ZP1j_HWW");
    floatParams.push_back("alpha_PM_theta_SR0j");
    floatParams.push_back("alpha_PM_theta_SR1j");

    floatParams.push_back("alpha_ATLAS_BTag_BEFF");
    floatParams.push_back("alpha_ATLAS_BTag_CEFF");
    floatParams.push_back("alpha_ATLAS_BTag_LEFF");

    floatParams.push_back("alpha_ATLAS_EL_EFF");
    floatParams.push_back("alpha_ATLAS_EL_RES");
    floatParams.push_back("alpha_ATLAS_EL_ESCALE");
    floatParams.push_back("alpha_ATLAS_ID_RES");
    floatParams.push_back("alpha_ATLAS_ISO");
    floatParams.push_back("alpha_ATLAS_JER");
    floatParams.push_back("alpha_ATLAS_JES_2012_Detector1");
    floatParams.push_back("alpha_ATLAS_JES_2012_Eta_StatMethod");
    floatParams.push_back("alpha_ATLAS_JES_2012_Modelling1");
    floatParams.push_back("alpha_ATLAS_JES_2012_PilePt");
    floatParams.push_back("alpha_ATLAS_JES_2012_PileRho_HWW");
    floatParams.push_back("alpha_ATLAS_JES_BJET");
    floatParams.push_back("alpha_ATLAS_JES_CLOSEBY");
    floatParams.push_back("alpha_ATLAS_JES_Eta_Modelling");
    floatParams.push_back("alpha_ATLAS_JES_FlavComp_HWW_WW");
    floatParams.push_back("alpha_ATLAS_JES_FlavComp_HWW_other");
    floatParams.push_back("alpha_ATLAS_JES_FlavComp_HWW_tt");
    floatParams.push_back("alpha_ATLAS_JES_FlavResp");
    floatParams.push_back("alpha_ATLAS_JES_HighPt");
    floatParams.push_back("alpha_ATLAS_JES_MU");
    floatParams.push_back("alpha_ATLAS_JES_NPV");
    floatParams.push_back("alpha_ATLAS_MET_RESOSOFT");
    floatParams.push_back("alpha_ATLAS_MET_SCALESOFT");

    floatParams.push_back("alpha_ATLAS_TRACKMET_RESOSOFT");
    floatParams.push_back("alpha_ATLAS_TRACKMET_SCALESOFT");

    floatParams.push_back("alpha_ATLAS_MU_EFF");
    floatParams.push_back("alpha_ATLAS_MU_ESCALE");
    floatParams.push_back("alpha_ATLAS_MU_ID_RES");
    floatParams.push_back("alpha_ATLAS_MU_MS_RES");
    floatParams.push_back("alpha_ATLAS_MU_RESCALE_lvlv_2012");
    floatParams.push_back("alpha_ATLAS_TRIGGER_HWW");
    floatParams.push_back("alpha_ATLAS_Z_TAUTAU_NORM_SYS_HWW");
    floatParams.push_back("alpha_ATLAS_TOP_SCALEF_TOPO_CR_0j");
    floatParams.push_back("alpha_ATLAS_TOP_SCALEF_TOPO_SR_0j");

    floatParams.push_back("alpha_ATLAS_TOP_SCALEF_STATS_0j_HWW");
    floatParams.push_back("alpha_ATLAS_Z_TAUTAU_VBFCF_STAT_HWW");
    floatParams.push_back("alpha_ATLAS_Z_LEPLEP_METCF_STAT_HWW");
    floatParams.push_back("alpha_ATLAS_Z_TAUTAU_NORM_STAT_HWW");
  }
  else if (mode == 7)
  {
    floatParams.push_back("alpha_ATLAS_WW_MTSHAPE");
    floatParams.push_back("alpha_ATLAS_WW_MODEL_HWW");
    floatParams.push_back("alpha_ATLAS_WW_UEPS_HWW");
    floatParams.push_back("alpha_QCDscale_VV_ACCEPT");
    floatParams.push_back("alpha_pdf_Higgs_qq");
    floatParams.push_back("alpha_pdf_Higgs_qq_ACCEPT");

    floatParams.push_back("alpha_ATLAS_TOP_SCALEF_STATS_0j_HWW");
    floatParams.push_back("alpha_ATLAS_Z_TAUTAU_VBFCF_STAT_HWW");
    floatParams.push_back("alpha_ATLAS_Z_LEPLEP_METCF_STAT_HWW");
    floatParams.push_back("alpha_ATLAS_Z_TAUTAU_NORM_STAT_HWW");
  }
  else if (mode == 8)
  {
    floatParams.push_back("alpha_FakeRate_EL_HWW");
    floatParams.push_back("alpha_FakeRate_MU_HWW");

    floatParams.push_back("alpha_ATLAS_TOP_SCALEF_STATS_0j_HWW");
    floatParams.push_back("alpha_ATLAS_Z_TAUTAU_VBFCF_STAT_HWW");
    floatParams.push_back("alpha_ATLAS_Z_LEPLEP_METCF_STAT_HWW");
    floatParams.push_back("alpha_ATLAS_Z_TAUTAU_NORM_STAT_HWW");
  }
  else if (mode == 9)
  {
    floatParams.push_back("alpha_LUMI_2012");

    floatParams.push_back("alpha_ATLAS_TOP_SCALEF_STATS_0j_HWW");
    floatParams.push_back("alpha_ATLAS_Z_TAUTAU_VBFCF_STAT_HWW");
    floatParams.push_back("alpha_ATLAS_Z_LEPLEP_METCF_STAT_HWW");
    floatParams.push_back("alpha_ATLAS_Z_TAUTAU_NORM_STAT_HWW");
  }

  int nrFloat = floatParams.size();

  if (mode == 1)
  {
    while ((var = (RooRealVar*)itr->Next()))
    {
      if (string(var->GetName()).find("STAT") == string::npos && string(var->GetName()).find("norm") == string::npos && string(var->GetName()).find("PM_EFF") == string::npos)
      {
        var->setConstant(1);
        cout << "DEBUG::Set " << var->GetName() << " constant. Value is " << var->getVal() << endl;
      }
    }
    itr->Reset();
  }
  else if (mode == 2)
  {
    while ((var = (RooRealVar*)itr->Next()))
    {
      if (string(var->GetName()).find("gamma") == string::npos && string(var->GetName()).find("norm") == string::npos && string(var->GetName()).find("STAT") == string::npos && string(var->GetName()).find("PM_EFF") == string::npos)
      {
        var->setConstant(1);
        cout << "DEBUG::Set " << var->GetName() << " constant. Value is " << var->getVal() << endl;
      }
    }
    itr->Reset();
  }
  else if (mode != 0)
  {
    while ((var = (RooRealVar*)itr->Next()))
    {
      if (string(var->GetName()).find("alpha") != string::npos || string(var->GetName()).find("gamma") != string::npos)
      {
        var->setConstant(1);
        cout << "DEBUG::Set " << var->GetName() << " constant. Value is " << var->getVal() << endl;
      }
    }
    itr->Reset();

    for (int i=0;i<nrFloat;i++)
    {
      RooRealVar* nuip = (RooRealVar*)nuis.find(floatParams[i].c_str());
      if (nuip)
      {
        nuip->setConstant(0);
        cout << "DEBUG::Set " << nuip->GetName() << " floating. Value is " << nuip->getVal() << endl;
      }
    }
  }

  // check with findSigma
  // minimize(fNll);
  // double nll_hat = fNll->getVal();
  // cout << "DEBUG::findSigma" << endl;
  // ws->saveSnapshot("tmp_snapshot2", *mc->GetPdf()->getParameters(data));
  // double fs_errup   = findSigma(fNll, nll_hat, firstPOI, 1.5  , 0.7, +1, 0.005);
  // ws->loadSnapshot("tmp_snapshot2");
  // double fs_errdown = findSigma(fNll, nll_hat, firstPOI, -0.3, 0.7, -1, 0.005);
  // // ws->loadSnapshot("tmp_snapshot2");
  // cout << "DEBUG::findSigma returns: muhat = " << firstPOI->getVal() << " +" << fabs(fs_errup) << " /  -" << fabs(fs_errdown) << endl;

  RooArgSet* minosSet = new RooArgSet(*firstPOI);
  RooMinuit m(*fNll);
  m.minos(*minosSet);
  RooFitResult* r = m.save();

  cout << "=====================================================================================" << endl;

  r->Print("v") ;

  cout << "=====================================================================================" << endl;

  cout << "DEBUG::Mode was " << mode << endl;

}

void getErrors(RooWorkspace* w, RooRealVar* par, RooNLLVar* nll, RooArgSet& allParams)
{
  static int errNr = 0;
  errNr++;
  stringstream shNameStr;
  shNameStr << "tmp_snapshot_" << errNr;
  string shName = shNameStr.str();
  double errhi, errlo;
  map<string, double> plls1 = pllVals();
  w->saveSnapshot(shName.c_str(),allParams);
  getError(1, nll, par, errhi);
  map<string, double> plls2 = pllVals();
  //printDeltaPll(plls1, plls2);

  w->loadSnapshot(shName.c_str());
  getError(-1, nll, par, errlo);
  w->loadSnapshot(shName.c_str());

  par->setAsymError(errlo, errhi);
}

void save(string baseName, string type, TCanvas* c1)
{
  system(("mkdir -vp " + type + "-files").c_str());
  stringstream saveName;
  saveName << type << "-files/" << baseName << "." << type;
  c1->SaveAs(saveName.str().c_str());
}
