//
// =====================================================================================
//
//       Filename:  printTable_all.C
//
//    Description:  print all tables needed for the notes
//
//        Version:  0.1
//        Created:  2012-10-31 14:48:15
//       Revision:  none
//       Compiler:
//
//         Author:  YOUR NAME ()
//        Company:
//
// =====================================================================================
//

#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooMinimizer.h"
#include "RooNLLVar.h"
#include "RooSimultaneous.h"
#include "RooCategory.h"

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

void setConst(RooArgSet& params, bool flag);
void setVal(RooArgSet& params, double val);
void getErrors(RooWorkspace* w, RooRealVar* par, RooNLLVar* nll, RooArgSet& allParams);

// =====================================================================================

// table mode:
// 0 = rate table / cutflow
// 1 = fractional uncertainties

// mode:
// 0 = "prefit"
// 1 = full + no fit
// 2 = full + mu = 0 fit
// 3 = full + mu = 1 fit
// 4 = full + mu = muhat fit
// 5 = CR refit method for uncertainty propagation / cancellation

// split mode:
// 0 = Signal, Background
// 1 = Signal, WW, WZ/ZZ/Wg(*), ttbar, st, Z+jets, W+jets, Total, Observed
// 2 = Signal, WW, WZZZ, Wg, Wg*, ttbar, st, Z+jets, W+jets, Total, Observed

// =====================================================================================

void printTable_all(double mass,
    string version,
    int tablemode = 0,
    int mode = 5,
    int splitmode = 0,
    bool verbose = 0,
    const char* wsName = "combined",
    const char* modelConfigName = "ModelConfig",
    const char* dataName = "obsData") {

  bool do2011    = 0;

  bool doggf     = 1;
  bool dovbf     = 1;
  bool dowh      = 1;
  bool dozh      = 1;
  bool doww      = 1;
  bool dowwew    = 1;
  bool dowzzz    = 1;
  bool dowzzzew  = 1;
  bool dozjets   = 1;
  bool dozjetsew = 1;
  bool doWjets   = 1;
  bool dottbar   = 1;
  bool dost      = 1;
  bool doWg      = 1;
  bool doWgs     = 1;

  bool vbfmode   = 1;  
  bool doTopCR   = 0 && vbfmode;  
  bool doSF      = 0;
  bool doOF      = 1;  

  bool splitww   = 1;
  bool splittop  = 1;
  bool splitew   = 0;

  stringstream outFileName;
  outFileName << "table_" << tablemode << "_" << mode << "_" << splitmode << "_" << version << ".tex";

  // =====================================================================================

  // WW and top normalisation factors from CAF
  bool useCAFvalues = 1;

  double ww0j_sf      = 1.16;
  double ww1j_sf      = 1.03;
  double top1j_sf     = 1.04;
  double top2j_sf     = 0.59;
  double zdy0j_sf     = 0.77;
  double zdy1j_sf     = 0.93;
  double DY0j_eff     = 0.27;
  double DY1j_eff     = 0.48;
  double NDY0j_eff    = 0.74;
  double NDY1j_eff    = 0.81;
  double DY0j_effunc  = 0.40;
  double DY1j_effunc  = 0.275;
  double NDY0j_effunc = 0.034;
  double NDY1j_effunc = 0.054;

  if (do2011) {
    // NFs
    ww0j_sf  = 1.025;
    ww1j_sf  = 0.749;
    top1j_sf = 1.034;
    top2j_sf = 0.74;
    zdy0j_sf = 1.4;		// SF numbers from Olivier, email 23 Feb
    zdy1j_sf = 1.9;
    DY0j_eff = 0.14;
    DY1j_eff = 0.42;
    NDY0j_eff = 0.71;
    NDY1j_eff = 0.96;
    DY0j_effunc = 0.07;
    DY1j_effunc = 0.10;
    NDY0j_effunc = 0.06;
    NDY1j_effunc = 0.09;

    // flags
    splitww = 0;
  }

  if (verbose) cout << "DEBUG::WW  0j norm = " << ww0j_sf  << endl;
  if (verbose) cout << "DEBUG::WW  1j norm = " << ww1j_sf  << endl;
  if (verbose) cout << "DEBUG::Top 1j norm = " << top1j_sf << endl;
  if (verbose) cout << "DEBUG::Top 2j norm = " << top2j_sf << endl;

  // =====================================================================================

  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
  ROOT::Math::MinimizerOptions::SetDefaultStrategy(0);
  ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(1);
  //RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);

  int nSigFig   = 5;
  int sign      = 1;
  double cutoff = 0.001;

  // =====================================================================================

  // Load file and grab workspace
  if (verbose) cout << "DEBUG::Loading workspace, ModelConfig and dataset" << endl;

  stringstream inFileName;
  inFileName << "workspaces/" << version << "/" << mass << ".root";

  TFile f(inFileName.str().c_str());
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

  if (verbose) cout << "DEBUG::Loaded ModelConfig " << modelConfigName << "  from workspace " << wsName << " in file " << inFileName.str() << endl;
  if (verbose) cout << "DEBUG::Dataset is " << dataName << endl;

  // =====================================================================================

  RooRealVar* firstPOI = (RooRealVar*)mc->GetParametersOfInterest()->first();
  firstPOI->setRange(-100,100);

  const RooArgSet* nuis = mc->GetNuisanceParameters();
  RooArgSet sysNuis;
  TIterator* itr = nuis->createIterator();
  RooRealVar* var;
  while ((var = (RooRealVar*)itr->Next()))
  {
    if (string(var->GetName()).find("norm") != string::npos) continue;
    sysNuis.add(*var);
  }

  // build a nll based on crs only
  RooNLLVar* cr_nll = NULL;
  if (mode == 5)
  {
    vector<string> cr_ids;
    cr_ids.push_back("mainControl");
    cr_ids.push_back("topbox");
    int nrIds = cr_ids.size();

    RooCategory* crCat = new RooCategory("crCat","crCat");
    RooSimultaneous* simPdf = (RooSimultaneous*)mc->GetPdf();
    RooCategory* cat = (RooCategory*)&simPdf->indexCat();

    TList* dataList = data->split(*cat);
    int nrData = dataList->GetEntries();

    TIterator* catItr = cat->typeIterator();
    RooCatType* tt;

    RooArgSet obs;
    map<string, RooAbsPdf*> pdfMap;
    map<string, RooDataSet*> dataMap;
    while ((tt = (RooCatType*)catItr->Next()))
    {
      bool skip = true;
      for (int i=0;i<nrIds;i++)
      {
        if (string(tt->GetName()).find(cr_ids[i]) != string::npos)
        {
          skip = false;
          break;
        }
      }
      if (skip) continue;
      cout << "Adding cat to pdf: " << tt->GetName() << endl;

      RooAbsPdf* pdf = simPdf->getPdf(tt->GetName());
      pdfMap[tt->GetName()] = pdf;
      crCat->defineType(tt->GetName());

      RooRealVar* observable = (RooRealVar*)pdf->getObservables(*mc->GetObservables())->first();
      obs.add(*observable);

      for (int i=0;i<nrData;i++)
      {
        RooDataSet* subData = (RooDataSet*)dataList->At(i);
        if (string(subData->GetName()).find(string(tt->GetName())) != string::npos)
        {
          dataMap[tt->GetName()] = subData;
          cout << "Adding data to map: " << subData->GetName() << endl;
          break;
        }
      }
    }

    obs.add(*crCat);
    obs.add(*ws->var("weightVar"));
    obs.Print("v");
    crCat->Print("v");

    RooDataSet* cr_data = new RooDataSet("cr_data","cr_data",obs,Index(*crCat),Import(dataMap),WeightVar(*ws->var("weightVar")));
    RooSimultaneous* cr_pdf = new RooSimultaneous("cr_pdf","cr_pdf",pdfMap,*crCat);
    cr_pdf->Print();
    cr_nll = (RooNLLVar*)cr_pdf->createNLL(*cr_data,RooFit::Constrain(*mc->GetNuisanceParameters()),RooFit::GlobalObservables(*mc->GetGlobalObservables()));//,Constrain(nuis));

    // setConst(sysNuis, 1);
    // firstPOI->setVal(1);
    // firstPOI->setConstant(1);
    // minimize(cr_nll);
    // setConst(sysNuis, 0);
    // return;
  }

  if (verbose) cout << "DEBUG::Loaded ModelConfig " << modelConfigName << "  from workspace " << wsName << " in file " << inFileName.str() << endl;
  if (verbose) cout << "DEBUG::Dataset is " << dataName << endl;

  // =====================================================================================

  RooAbsPdf* pdf = mc->GetPdf();

  itr->Reset();
  vector<string> vec_nuis;
  while ((var = (RooRealVar*)itr->Next()))
  {
    string varName(var->GetName());
    if (varName.find("gamma_stat") != string::npos) continue;
    vec_nuis.push_back(string(var->GetName()));
  }
  itr->Reset();
  int nrNuis = vec_nuis.size();

  // do a b-only fit and set errors

  firstPOI->setVal(0);
  firstPOI->setConstant(1);

  // load the snapshot if it exists
  // ws->loadSnapshot("conditionalNuis_0");

  if (mode == 0 || mode >= 2)
  {
    if (mode == 0 || mode == 2 || mode == 5)
    {
      firstPOI->setVal(0);
      firstPOI->setConstant(1);
    }
    if (mode == 3)
    {
      firstPOI->setVal(1);
      firstPOI->setConstant(1);
    }
    if (mode == 4)
    {
      firstPOI->setConstant(0);
    }
    RooArgSet allParams(*mc->GetNuisanceParameters(), *firstPOI);
    RooArgSet allParams_tmp = allParams;
    RooNLLVar* nll = (RooNLLVar*)mc->GetPdf()->createNLL(*data, RooFit::Constrain(*mc->GetNuisanceParameters()),RooFit::GlobalObservables(*mc->GetGlobalObservables()));
    minimize(nll);
  }

  // =====================================================================================

  // Get the stats only errors for the normalisation factors

  map<string, double> error_map;
  if (mode == 0 || mode == 5)
  {
    while ((var = (RooRealVar*)itr->Next()))
    {
      string varName = var->GetName();
      var->setConstant(1);
    }

    itr->Reset();

    while ((var = (RooRealVar*)itr->Next()))
    {
      string varName = var->GetName();
      if (varName.find("norm") != string::npos)
      {
        ws->var(varName.c_str())->setConstant(0);

        firstPOI->setVal(1);
        firstPOI->setConstant(1);

        RooDataSet* SplusB_data = (RooDataSet*)ws->data("asimovData_1");
        if (!SplusB_data)
        {
          cout << "ERROR::Dataset: " << "asimovData_1" << " doesn't exist!" << endl;
          return;
        }

        RooArgSet allParams2(*mc->GetNuisanceParameters(), *firstPOI);
        RooArgSet allParams2_tmp = allParams2;
        RooNLLVar* sbnll = (RooNLLVar*)mc->GetPdf()->createNLL(*SplusB_data, RooFit::Constrain(*mc->GetNuisanceParameters()),RooFit::GlobalObservables(*mc->GetGlobalObservables()));
        minimize(sbnll);

        double sbnll_hat = sbnll->getVal();
        double var_hat = var->getVal();
        double var_err = var->getError();
        var->setConstant(1);
        double err = findSigma(sbnll, sbnll_hat, var, var_hat+var_err,var_hat,1,0.005);
        var->setError(err);
        error_map[var->GetName()] = err;

        if (verbose) cout << "DEBUG::Error is " << var->getError() << endl;

        ws->var(varName.c_str())->setConstant(1);
      }
    }

    itr->Reset();

    while ((var = (RooRealVar*)itr->Next()))
    {
      string varName = var->GetName();
      if (varName.find("norm") != string::npos) var->setConstant(0);
      if (string(var->GetName()).find("norm") != string::npos) continue; // keep norms at their conditional value

      if (string(var->GetName()).find("norm") != string::npos ||
          string(var->GetName()).find("gamma_stat") != string::npos ||
         (string(var->GetName()).find("recoil") != string::npos && string(var->GetName()).find("alpha") == string::npos))
      {
        var->setVal(1);
      }
      else
      {
        var->setVal(0);
      }
    }

    itr->Reset();

    bool isConst = firstPOI->isConstant();
    double poiVal = firstPOI->getVal();
    firstPOI->setVal(1);
    // setConst(sysNuis,1);
    // setVal(sampleNorms, 1);
    if (mode==5) minimize(cr_nll);
    // setConst(sysNuis,0);
    // setVal(sampleNorms, 0);
    // setVal(idNorms, 1);
    firstPOI->setVal(poiVal);
    firstPOI->setConstant(isConst);

    while ((var = (RooRealVar*)itr->Next()))
    {
      string varName = var->GetName();
      var->setConstant(0);
    }

    itr->Reset();

    while ((var = (RooRealVar*)itr->Next()))
    {
      if (string(var->GetName()).find("norm") != string::npos) continue; // keep norms at their conditional value

      if (/*string(var->GetName()).find("norm") != string::npos ||*/
          string(var->GetName()).find("gamma_stat") != string::npos ||
         (string(var->GetName()).find("recoil") != string::npos && string(var->GetName()).find("alpha") == string::npos))
      {
        var->setVal(1);
      }
      else
      {
        var->setVal(0);
        var->setError(1);
      }
      // getErrors(ws, var, nll, allParams);
    }

    itr->Reset();
    // return;
  }

  if (mode == 0 || mode == 1 || mode == 5) firstPOI->setVal(1);

  // =====================================================================================

  // Define the sample names
  if (verbose) cout << "DEBUG::Define the sample names" << endl;

  stringstream ggfName;
  ggfName << "ggf" << mass;

  stringstream vbfName;
  vbfName << "vbf" << mass;

  stringstream whName;
  whName << "wh" << mass;

  stringstream zhName;
  zhName << "zh" << mass;

  vector<string> sampleNames;
  if (doggf) sampleNames.push_back(ggfName.str());
  if (dovbf) sampleNames.push_back(vbfName.str());
  if (mass <= 300)
  {
    if (dowh) sampleNames.push_back(whName.str());
    if (dozh) sampleNames.push_back(zhName.str());
  }
  if (dottbar)  sampleNames.push_back("ttbar");
  if (dost)     sampleNames.push_back("st");
  if (doww)
  {
    if (!splitww)
    {
      sampleNames.push_back("ww");
    }
    else
    {
      sampleNames.push_back("ggww");
      sampleNames.push_back("qqww");
    }
  }
  if (dowwew)   sampleNames.push_back("wwew");
  if (dowzzz)   sampleNames.push_back("wzzz");
  if (dowzzzew) sampleNames.push_back("wzzzew");
  if (doWg)     sampleNames.push_back("wg");
  if (doWgs)    sampleNames.push_back("wgs");
  if (doWjets)  sampleNames.push_back("wjets");
  if (dozjets){
    sampleNames.push_back("zleplep");
    sampleNames.push_back("ztautau");
  }
  if (dozjetsew){
    sampleNames.push_back("zleplepew");
    sampleNames.push_back("ztautauew");
  }

  int nrSamples = sampleNames.size();

  // =====================================================================================

  // Define regions
  if (verbose) cout << "DEBUG::Collect the regions" << endl;

  vector<vector<string> > ids;

  if (mode == 0 || mode == 5)
  {
    // vector<string> id1;
    // id1.push_back("em_signalLike1_0j");
    // id1.push_back("");
    // ids.push_back(id1);

    // vector<string> id2;
    // id2.push_back("em_signalLike2_0j");
    // id2.push_back("");
    // ids.push_back(id2);

    // vector<string> id3;
    // id3.push_back("me_signalLike1_0j");
    // id3.push_back("");
    // ids.push_back(id3);

    // vector<string> id4;
    // id4.push_back("me_signalLike2_0j");
    // id4.push_back("");
    // ids.push_back(id4);

    // vector<string> id5;
    // id5.push_back("em_signalLike1_1j");
    // id5.push_back("");
    // ids.push_back(id5);

    // vector<string> id6;
    // id6.push_back("em_signalLike2_1j");
    // id6.push_back("");
    // ids.push_back(id6);

    // vector<string> id7;
    // id7.push_back("me_signalLike1_1j");
    // id7.push_back("");
    // ids.push_back(id7);

    // vector<string> id8;
    // id8.push_back("me_signalLike2_1j");
    // id8.push_back("");
    // ids.push_back(id8);

    // vector<string> id9;
    // id9.push_back("SF_AfrecSR_0j");
    // id9.push_back("");
    // ids.push_back(id9);

    // vector<string> id10;
    // id10.push_back("SF_AfrecSR_1j");
    // id10.push_back("");
    // ids.push_back(id10);

//     vector<string> id11;
//     id11.push_back("ee_signalLike1_2j");
//     id11.push_back("");
//     ids.push_back(id11);
// 
//     vector<string> id12;
//     id12.push_back("em_signalLike1_2j");
//     id12.push_back("");
//     ids.push_back(id12);

    vector<string> id13;
    id13.push_back("0j");
    id13.push_back("");
    if(!vbfmode) ids.push_back(id13);

    vector<string> id14;
    id14.push_back("1j");
    id14.push_back("");
    if(!vbfmode) ids.push_back(id14);

    vector<string> id15;
    id15.push_back("ee_signalLike_2j");
    id15.push_back("");
    if(vbfmode && doSF && !doTopCR) ids.push_back(id15);

    vector<string> id16;
    id16.push_back("em_signalLike_2j");
    id16.push_back("");
    if(vbfmode && doOF && !doTopCR) ids.push_back(id16);

    vector<string> id17;
    id17.push_back("SF_topbox_2j");
    id17.push_back("");
    if(doTopCR) ids.push_back(id17);


  }
  else
  {
    vector<string> regionNames;
    vector<string> chanNames;
    vector<string> jetNames;
    vector<string> periodNames;

    if(!vbfmode) regionNames.push_back("signalLike1");
    if(!vbfmode) regionNames.push_back("signalLike2");
    if(!vbfmode) regionNames.push_back("ASR");
    if(!vbfmode) regionNames.push_back("AfrecSR");
    if(vbfmode && !doTopCR) regionNames.push_back("signalLike");
    if(doTopCR) regionNames.push_back("topbox");

    int nrRegions = regionNames.size();

    if((vbfmode && doOF && !doTopCR) || !vbfmode) chanNames.push_back("em");
    if(vbfmode && doSF && !doTopCR) chanNames.push_back("ee");
    if(!vbfmode) chanNames.push_back("me");
    if(!vbfmode || doTopCR) chanNames.push_back("SF");

    int nrChans = chanNames.size();

    if(!vbfmode) jetNames.push_back("0j");
    if(!vbfmode) jetNames.push_back("1j");
    jetNames.push_back("2j");

    int nrJets = jetNames.size();

    for (int ic=0;ic<nrChans;ic++)
    {
      for (int ij=0;ij<nrJets;ij++)
      {
        for (int ir=0;ir<nrRegions;ir++)
        {
          if (jetNames[ij] == "2j" && regionNames[ir] == "mainControl") continue;
          if (jetNames[ij] == "0j" && regionNames[ir] == "topbox") continue;
          vector<string> id;
          id.push_back(chanNames[ic]+"_"+regionNames[ir]+"_"+jetNames[ij]);
          ids.push_back(id);
        }
      }
    }
  }

  int nrIds = ids.size();

  // =====================================================================================

  // Use nice names for printing the tables
  if (verbose) cout << "DEBUG::Collect the samples" << endl;

  vector<string> sampleNamesNice;
  vector<vector<string> > sampleIDs;

  // if (splitmode == 0 && tablemode == 0)
  if (tablemode == 0)
  {
    vector<string> Sig_ids;
    if (!vbfmode && doggf) Sig_ids.push_back(ggfName.str());
    if (dovbf) Sig_ids.push_back(vbfName.str());
    if (dowh)  Sig_ids.push_back(whName.str());
    if (dozh)  Sig_ids.push_back(zhName.str());
    sampleIDs.push_back(Sig_ids);
    sampleNamesNice.push_back("Signal");
  }

  if (splitmode != 0)
  {
    // if (doggf) {
    //   vector<string> ggf_ids;
    //   ggf_ids.push_back(ggfName.str());
    //   sampleIDs.push_back(ggf_ids);
    //   sampleNamesNice.push_back("ggf");
    // }

    // if (dovbf) {
    //   vector<string> vbf_ids;
    //   vbf_ids.push_back("vbf125");
    //   sampleIDs.push_back(vbf_ids);
    //   sampleNamesNice.push_back("vbf");
    // }

    // if (dowh) {
    //   vector<string> wh_ids;
    //   wh_ids.push_back("wh125");
    //   sampleIDs.push_back(wh_ids);
    //   sampleNamesNice.push_back("wh");
    // }

    // if (dozh) {
    //   vector<string> zh_ids;
    //   zh_ids.push_back("zh125");
    //   sampleIDs.push_back(zh_ids);
    //   sampleNamesNice.push_back("zh");
    // }

    if (vbfmode && doggf) {
      vector<string> ggf_ids;
      ggf_ids.push_back(ggfName.str());
      sampleIDs.push_back(ggf_ids);
      sampleNamesNice.push_back(ggfName.str());
    }

    if (doww) {
      vector<string> WW_ids;
      if (!splitww)
      {
        WW_ids.push_back("ww");
      }
      else
      {
        WW_ids.push_back("ggww");
        WW_ids.push_back("qqww");
      }
      if (dowwew && !splitew) {
        WW_ids.push_back("wwew");
      }
      sampleIDs.push_back(WW_ids);
      sampleNamesNice.push_back("WW");
    }

    if (dowwew && splitew) {
      vector<string> WWew_ids;
      WWew_ids.push_back("wwew");
      sampleIDs.push_back(WWew_ids);
      sampleNamesNice.push_back("WW (ew)");
    }

    if (dowzzz || doWg || doWgs) {
      vector<string> WZZZ_ids;
      if (dowzzz) WZZZ_ids.push_back("wzzz");
      if (dowzzzew && !splitew) {
        WZZZ_ids.push_back("wzzzew");
      }
      if (splitmode!=2) {
        if (doWg)   WZZZ_ids.push_back("wg");
        if (doWgs)  WZZZ_ids.push_back("wgs");
        sampleIDs.push_back(WZZZ_ids);
        sampleNamesNice.push_back("WZ/ZZ/Wg(*)");
      }
      else{
        sampleIDs.push_back(WZZZ_ids);
        sampleNamesNice.push_back("WZ/ZZ");
        if (doWg) {
          vector<string> Wg_ids;
          Wg_ids.push_back("wg");
          sampleIDs.push_back(Wg_ids);
          sampleNamesNice.push_back("Wg");
        }
        if (doWgs) {
          vector<string> Wgs_ids;
          Wgs_ids.push_back("wgs");
          sampleIDs.push_back(Wgs_ids);
          sampleNamesNice.push_back("Wg(*)");
        }
      }
    }

    if (dowzzzew && splitew) {
      vector<string> WZZZew_ids;
      WZZZew_ids.push_back("wzzzew");
      sampleIDs.push_back(WZZZew_ids);
      sampleNamesNice.push_back("WZ/ZZ (ew)");
    }

    if ((dottbar || dost) && !splittop) {
      vector<string> top_ids;
      if (dottbar) top_ids.push_back("ttbar");
      if (dost) top_ids.push_back("st");
      sampleIDs.push_back(top_ids);
      sampleNamesNice.push_back("top");
    }

    if (dost && splittop) {
      vector<string> st_ids;
      st_ids.push_back("st");
      sampleIDs.push_back(st_ids);
      sampleNamesNice.push_back("st");
    }

    if (dottbar && splittop) {
      vector<string> ttbar_ids;
      ttbar_ids.push_back("ttbar");
      sampleIDs.push_back(ttbar_ids);
      sampleNamesNice.push_back("ttbar");
    }

    //if (dost && splittop) {
    // vector<string> st_ids;
    //  st_ids.push_back("st");
    //  sampleIDs.push_back(st_ids);
    //  sampleNamesNice.push_back("st");
    // }

    if (dozjets) {
      vector<string> zjets_ids;
      zjets_ids.push_back("zleplep");
      zjets_ids.push_back("ztautau");
      if (dozjetsew && splitew) {
        zjets_ids.push_back("zleplepew");
        zjets_ids.push_back("ztautauew");
      }
      sampleIDs.push_back(zjets_ids);
      sampleNamesNice.push_back("Z+jets");
    }

    if (dozjetsew && splitew) {
      vector<string> zjetsew_ids;
      zjetsew_ids.push_back("zleplepew");
      zjetsew_ids.push_back("ztautauew");
      sampleIDs.push_back(zjetsew_ids);
      sampleNamesNice.push_back("Z+jets (ew)");
    }

    if (doWjets) {
      vector<string> wjets_ids;
      wjets_ids.push_back("wjets");
      sampleIDs.push_back(wjets_ids);
      sampleNamesNice.push_back("W+jets");
    }
  }
  else if (splitmode == 0)
  {
    vector<string> bkg_ids;
    if (doggf && vbfmode) bkg_ids.push_back(ggfName.str());
    if (doww)
    {
      if (!splitww)
      {
        bkg_ids.push_back("ww");
      }
      else
      {
        bkg_ids.push_back("ggww");
        bkg_ids.push_back("qqww");
      }
    }
    if (dowwew)   bkg_ids.push_back("wwew");
    if (dowzzz)   bkg_ids.push_back("wzzz");
    if (dowzzzew) bkg_ids.push_back("wzzzew");
    if (doWgs)    bkg_ids.push_back("wg");
    if (doWgs)    bkg_ids.push_back("wgs");
    if (dottbar)  bkg_ids.push_back("ttbar");
    if (dost)     bkg_ids.push_back("st");
    if (doWjets)  bkg_ids.push_back("wjets");
    if (dozjets) {
      bkg_ids.push_back("zleplep");
      bkg_ids.push_back("ztautau");
    }
    if (dozjetsew)
    {
      bkg_ids.push_back("zleplepew");
      bkg_ids.push_back("ztautauew"); 
    }
    sampleIDs.push_back(bkg_ids);
    sampleNamesNice.push_back("Background");
  }

  sampleNamesNice.push_back("Total");
  sampleNamesNice.push_back("Observed");

  int nrSampleIDs = sampleIDs.size();
  int nrSamplesNice = sampleNamesNice.size();

  // =====================================================================================

  // Grab the pdfs
  if (verbose) cout << "DEBUG::Collect the PDFs" << endl;

  RooSimultaneous* simPdf = (RooSimultaneous*)pdf;
  RooCategory* cat = (RooCategory*)&simPdf->indexCat();

  TList* dataList = data->split(*cat);

  const RooArgSet* observables = mc->GetObservables();

  vector<string> channelNames;

  vector<double> totals;
  vector<double> totals_errhi;
  vector<double> totals_errlo;

  vector<vector<double> > inds;
  vector<vector<double> > inds_errhi;
  vector<vector<double> > inds_errlo;
  vector<vector<RooAbsPdf*> > pdfSets;

  for (int id=0;id<nrIds;id++)
  {
    string channelName = "";
    int nrId2 = ids[id].size();
    for (int idd=0;idd<nrId2;idd++)
    {
      channelName += ids[id][idd];
    }
    channelNames.push_back(channelName);

    vector<RooAbsPdf*> pdfs;

    TIterator* catItr = cat->typeIterator();
    RooCatType* type;
    while ((type = (RooCatType*)catItr->Next()))
    {
      string pdfName(type->GetName());

      bool skip = false;
      for (int idd=0;idd<nrId2;idd++)
      {
        if (pdfName.find(ids[id][idd]) == string::npos) skip = true;
      }

      if (mode == 0 || mode == 5)
      {
        if (pdfName.find("mainControl") != string::npos) skip = true;
        if (pdfName.find("topbox") != string::npos && !doTopCR) skip = true;
        if (pdfName.find("Zpeak") != string::npos) skip = true;
        if (pdfName.find("ASR") != string::npos) skip = true;
        if (pdfName.find("OF") != string::npos && (pdfName.find("0j") != string::npos || pdfName.find("1j") != string::npos)) skip = true;
      }

      if (skip) continue;

      RooAbsPdf* subPdf = simPdf->getPdf(pdfName.c_str());
      cout << "Getting PDF " << subPdf->GetName() << endl;
      pdfs.push_back(subPdf);
    }
    pdfSets.push_back(pdfs);
  }

  int nrSets = pdfSets.size();
  int nrChannels = channelNames.size();

  // =====================================================================================

  // Initialise the tables depending on mode
  if (verbose) cout << "DEBUG::Initialise the table" << endl;
  if (verbose) cout << "DEBUG::nrSets      = " << nrSets      << endl;
  if (verbose) cout << "DEBUG::nrChannels  = " << nrChannels  << endl;
  if (verbose) cout << "DEBUG::nrSampleIDs = " << nrSampleIDs << endl;
  if (verbose) cout << "DEBUG::nrSamples   = " << nrSamples   << endl;
  if (verbose) cout << "DEBUG::nrNuis      = " << nrNuis      << endl;

  string* firstCol = new string[((tablemode==0)?nrChannels:nrNuis)+1];
  firstCol[0] = (tablemode==0)?"Region":"Source";
  string* header = new string[tablemode==0?nrSampleIDs+2:(splitmode!=0?nrSampleIDs*nrSets+1:nrSets*2)];
  double** matrix = new double*[(tablemode==0)?nrChannels:nrNuis];
  double** matrixErr = new double*[(tablemode==0)?nrChannels:nrNuis];

  if (tablemode == 0)
  {
    for (int ic=0;ic<nrChannels;ic++)
    {
      matrix[ic] = new double[nrSampleIDs+2];
      matrixErr[ic] = new double[nrSampleIDs+2];
      for (int isam=0;isam<nrSampleIDs+2;isam++)
      {
        matrix[ic][isam] = 0;
        matrixErr[ic][isam] = 0;
      }
      firstCol[ic+1] = channelNames[ic];
    }

    for (int is=0;is<nrSamplesNice;is++) header[is] = sampleNamesNice[is];

   }
   else if (tablemode == 1)
   {
    for (int i=0;i<nrNuis;i++) firstCol[i+1] = vec_nuis[i];
    if (splitmode == 0)
    {
      for (int in=0;in<nrNuis;in++)
      {
        matrix[in] = new double[nrSets*2];
        matrixErr[in] = new double[nrSets*2];
        for (int i=0;i<nrSets*2;i++)
        {
          matrix[in][i] = 0;
          matrixErr[in][i] = 0;
        }
      }

      for (int is=0;is<nrSets*2;is++)
      {
        if (is < nrSets)
        {
          header[is] = "Signal " + channelNames[is];
        }
        else
        {
          header[is] = "Background " + channelNames[is-nrSets];
        }
      }
    }
    else
    {
      for (int in=0;in<nrNuis;in++)
      {
        matrix[in]    = new double[nrSampleIDs*nrSets+1];
        matrixErr[in] = new double[nrSampleIDs*nrSets+1];
        for (int i=0;i<nrSampleIDs*nrSets;i++)
        {
          matrix[in][i] = 0;
          matrixErr[in][i] = 0;
        }
      }

      for (int isam=0;isam<nrSampleIDs;isam++)
      {
        for (int is=0;is<nrSets;is++)
        {
          header[isam+is+isam*(nrSets-1)] = sampleNamesNice[isam] + " " + channelNames[is];
          if (verbose) cout << "DEBUG::" << isam+is+isam*(nrSets-1) << ":" << header[isam+is+isam*(nrSets-1)] << endl;
        }
      }
    }
  }

  // =====================================================================================
  // =====================================================================================


  RooArgSet sampleNorms;
  for (int i=0;i<nrSamples;i++)
  {
    sampleNorms.add(*ws->var(("ATLAS_sampleNorm_"+sampleNames[i]).c_str()));
  }

  if (tablemode == 0)
  {
    if (verbose) cout << "DEBUG::tablemode is 0" << endl;
    // Compute each individual background
    if (verbose) cout << "DEBUG::Computing individual backgrounds" << endl;

    for (int i=0;i<nrSamples;i++)
    {
      ws->var(("ATLAS_sampleNorm_"+sampleNames[i]).c_str())->setVal(0);
      sampleNorms.add(*ws->var(("ATLAS_sampleNorm_"+sampleNames[i]).c_str()));
      if (verbose) cout << "DEBUG::Set " << "ATLAS_sampleNorm_"+sampleNames[i] << " to 0." << endl;
    }

    for (int isam=0;isam<nrSampleIDs;isam++)
    {
      bool isWW = true;
      bool isTop = true;
      bool isDYleplep = true;

      if (verbose) cout << "DEBUG::Computing samples: " << endl;

      vector<string> idNames = sampleIDs[isam];
      int nrIDs = idNames.size();

      RooArgSet idNorms;
      for (int iid=0;iid<nrIDs;iid++)
      {
        isWW = isWW && (idNames[iid] == "ww" || idNames[iid] == "ggww" || idNames[iid] == "qqww");
        isTop = isTop && (idNames[iid] == "ttbar" || idNames[iid] == "st");
        isDYleplep = isDYleplep && (idNames[iid] == "zleplep" || idNames[iid] == "ztautau");

        if (verbose) cout << "DEBUG::--> " << idNames[iid] << endl;
        ws->var(("ATLAS_sampleNorm_"+idNames[iid]).c_str())->setVal(1);
        idNorms.add(*ws->var(("ATLAS_sampleNorm_"+idNames[iid]).c_str()));
      }

      for (int i=0;i<nrSets;i++)
      {
        bool first = true;
        double total = 0;
        int nrPdfs = pdfSets[i].size();
        itr->Reset();

        double sf = 1;

        for (int j=0;j<nrPdfs;j++)
        {
          string pdfName = pdfSets[i][j]->GetName();
          if (isWW  && pdfName.find("0j") != string::npos && (pdfName.find("signalLike") != string::npos || pdfName.find("SF_AfrecSR") != string::npos)) sf = ww0j_sf;
          if (isWW  && pdfName.find("1j") != string::npos && (pdfName.find("signalLike") != string::npos || pdfName.find("SF_AfrecSR") != string::npos)) sf = ww1j_sf;
          if (isTop && pdfName.find("1j") != string::npos && (pdfName.find("signalLike") != string::npos || pdfName.find("SF_AfrecSR") != string::npos || pdfName.find("mainControl") != string::npos)) sf = top1j_sf;
          if (isTop && pdfName.find("2j") != string::npos && (pdfName.find("signalLike") != string::npos || pdfName.find("mainControl") != string::npos)) sf = top2j_sf;
          if (isDYleplep  && pdfName.find("0j") != string::npos && pdfName.find("SF_AfrecSR") != string::npos) sf = zdy0j_sf;
          if (isDYleplep  && pdfName.find("1j") != string::npos && pdfName.find("SF_AfrecSR") != string::npos) sf = zdy1j_sf;

          // // Adding fRecoil efficiency if needed
          // if ( pdfName.find("SF_AfrecSR") != string::npos && (pdfName.find("0j") != string::npos || pdfName.find("1j") != string::npos) ) { //same-flavor 0/1j channels
          //   float eff = 1;
          //   if (isDYleplep)
          //     eff = (pdfName.find("0j") != string::npos) ? DY0j_eff: DY1j_eff ;//var->getVal();
          //   else
          //     eff = (pdfName.find("0j") != string::npos) ? NDY0j_eff: NDY1j_eff ;//var->getVal();
          //   sf *= eff;
          //   std::cout << " DEBUGOLIVIER : adding efficiency=" << eff << " for " << ((isDYleplep) ? "DY": "nonDY") << " in " << pdfName.c_str() << std::endl;
          // }//end of if same-flavor 0/1j channels
        }

        while ((var = (RooRealVar*)itr->Next()))
        {
          // if (string(var->GetName()).find("gamma_stat") != string::npos) continue;

          double nui_err = var->getError();
          if (error_map.find(string(var->GetName())) != error_map.end()) nui_err = error_map[var->GetName()];
          double nomVal = var->getVal();
          double err = 0;
          for (int j=0;j<nrPdfs;j++)
          {
            RooAbsPdf* subPdf = pdfSets[i][j];

            // set some paramters to CAF values
            if (useCAFvalues)
            {
              ws->var("ATLAS_norm_SF_MUSR_DY0j")->setVal(zdy0j_sf);
              ws->var("ATLAS_norm_SF_MUSR_DY1j")->setVal(zdy1j_sf);
              ws->var("ATLAS_norm_Top1j")->setVal(top1j_sf);
              ws->var("ATLAS_norm_Top2j")->setVal(top2j_sf);
              ws->var("ATLAS_norm_WW0j")->setVal(ww0j_sf);
              ws->var("ATLAS_norm_WW1j")->setVal(ww1j_sf);
              ws->var("PM_EFF_f_recoil_DY0j")->setVal(DY0j_eff);
              ws->var("PM_EFF_f_recoil_DY1j")->setVal(DY1j_eff);
              ws->var("PM_EFF_f_recoil_NDY_SR0j")->setVal(NDY0j_eff);
              ws->var("PM_EFF_f_recoil_NDY_SR1j")->setVal(NDY1j_eff);
            }

            // if (!first) continue; // just taking up time
            const RooArgSet* obs = subPdf->getObservables(*observables);
            double thisB = subPdf->expectedEvents(*obs);//*sf;
            if (first)
            {
              std::cout << " DEBUGOLIVIER : adding thisB=" << thisB << " to total=" << total << "-->" << total+thisB << " for " << ((isDYleplep) ? "DY": "nonDY") << " in " << pdfSets[i][j]->GetName() << std::endl;
              total += thisB;
            }
            var->setVal(nomVal+nui_err);

            if (mode == 5 && error_map.find(string(var->GetName())) == error_map.end())
            {
              bool isConst = firstPOI->isConstant();
              setConst(sysNuis,1);
              setVal(sampleNorms, 1);
              minimize(cr_nll);
              setConst(sysNuis,0);
              setVal(sampleNorms, 0);
              setVal(idNorms, 1);
              firstPOI->setConstant(isConst);
            }

            // cout << "========================================" << endl;
            // ws->var("ATLAS_norm_WW0j")->Print();
            // cout << "========================================" << endl;

            // double p1sVar = subPdf->expectedEvents(*obs)*sf - thisB;
            double p1sVar = subPdf->expectedEvents(*obs) - thisB;
            err += p1sVar;

            var->setVal(nomVal);

            if (mode == 5 && error_map.find(string(var->GetName())) == error_map.end())
            {
              bool isConst = firstPOI->isConstant();
              setConst(sysNuis,1);
              setVal(sampleNorms, 1);
              minimize(cr_nll);
              setConst(sysNuis,0);
              setVal(sampleNorms, 0);
              setVal(idNorms, 1);
              firstPOI->setConstant(isConst);
            }
          }

          matrixErr[i][isam] += err*err;
          first = false;
        } //end of loop over vars
        if (total < pow(10., -9)) total = 0;
        matrix[i][isam] = total;
        matrixErr[i][isam] = sqrt(matrixErr[i][isam]);
      } //end of loop over nrSets

      for (int iid=0;iid<nrIDs;iid++)
      {
        if (verbose) cout << "DEBUG::--> " << idNames[iid] << endl;
        ws->var(("ATLAS_sampleNorm_"+idNames[iid]).c_str())->setVal(0);
      }
    }

    for (int i=0;i<nrSamples;i++)
    {
      if (verbose) cout << "DEBUG::sample is " << sampleNames[i] << endl;
      ws->var(("ATLAS_sampleNorm_"+sampleNames[i]).c_str())->setVal(1);
    }

    if (mode == 0 || mode == 5) firstPOI->setVal(0);

    // =====================================================================================

    // get total background uncertainties
    if (verbose) cout << "DEBUG::Computing total bgs" << endl;
    for (int i=0;i<nrSets;i++)
    {
      bool first = true;
      double total = 0;

      vector<double> dummyVec;
      inds.push_back(dummyVec);
      inds_errhi.push_back(dummyVec);

      int nrPdfs = pdfSets[i].size();
      itr->Reset();
      while ((var = (RooRealVar*)itr->Next()))
      {
        // if (string(var->GetName()).find("gamma_stat") != string::npos) continue;

        double nui_err = var->getError();
        if (error_map.find(string(var->GetName())) != error_map.end()) nui_err = error_map[var->GetName()];
        double nomVal = var->getVal();
        double err = 0;
        for (int j=0;j<nrPdfs;j++)
        {
          RooAbsPdf* subPdf = pdfSets[i][j];

          string pdfName = pdfSets[i][j]->GetName();
          bool is0j=(pdfName.find("0j") != string::npos);
          bool is1j=(pdfName.find("1j") != string::npos);

          for (int j=0;j<nrSamples;j++) {
            if (verbose) cout << "DEBUG::sample is " << sampleNames[j] << endl;
            // if (pdfName.find("SF_AfrecSR") == string::npos)
            ws->var(("ATLAS_sampleNorm_"+sampleNames[j]).c_str())->setVal(1);
            // else
            // ws->var(("ATLAS_sampleNorm_"+sampleNames[i]).c_str())->setVal( (is0j) ? NDY0j_eff: NDY1j_eff );
          }

          // Computing process scale factor
          // if (pdfName.find("signalLike") != string::npos || pdfName.find("SF_AfrecSR") != string::npos) {
          //   float sfww  = (is0j) ? ww0j_sf : ((is1j) ? ww1j_sf : 1);
          //   float sftop = 0;//(is1j) ? top1j_sf : ((!is0j) ? top2j_sf : 1);
          //   float sfzdy = 0;//(is0j) ? zdy0j_sf : ((is1j) ? zdy1j_sf : 1);
          //   if (pdfName.find("SF_AfrecSR") != string::npos) {
          //     if (is0j) {
          //       sfww *= NDY0j_eff;
          //       sftop *= NDY0j_eff;
          //       sfzdy *= DY0j_eff;
          //     } else {
          //       sfww *= NDY1j_eff;
          //       sftop *= NDY1j_eff;
          //       sfzdy *= DY1j_eff;
          //     }
          //   }
          //      // ws->var("ATLAS_sampleNorm_qqww")->setVal(sfww);
          //      // ws->var("ATLAS_sampleNorm_ggww")->setVal(sfww);
          //      // ws->var("ATLAS_sampleNorm_ttbar")->setVal(sftop);
          //      // ws->var("ATLAS_sampleNorm_st")->setVal(sftop);
          //      // ws->var("ATLAS_sampleNorm_zleplep")->setVal(sfzdy);
          // }

          // if (!first) continue;
          const RooArgSet* obs = subPdf->getObservables(*observables);
          double thisB = subPdf->expectedEvents(*obs);
          if (first) total += thisB;

          var->setVal(nomVal+nui_err);
          double p1sVar = subPdf->expectedEvents(*obs) - thisB;
          err += p1sVar;

          var->setVal(nomVal);
        }

        matrixErr[i][nrSampleIDs] += err*err;

        first = false;
      }
      matrix[i][nrSampleIDs] = total;
      matrixErr[i][nrSampleIDs] = sqrt(matrixErr[i][nrSampleIDs]);
    }

    TIterator* dataItr = dataList->MakeIterator();
    RooDataSet* thisData;

    for (int i=0;i<nrSets;i++)
    {
      int nrPdfs = pdfSets[i].size();
      dataItr->Reset();
      while ((thisData = (RooDataSet*)dataItr->Next()))
      {
        cout << thisData->GetName() << endl;
        for (int j=0;j<nrPdfs;j++)
        {
          if (string(pdfSets[i][j]->GetName()).find(string(thisData->GetName())) != string::npos)
          {
            cout << "Adding " << thisData->sumEntries() << " to pdfSet " << pdfSets[i][j]->GetName() << endl;
            matrix[i][nrSampleIDs+1] += thisData->sumEntries();
          }
        }
      }
    }
  }

  // =====================================================================================
  // =====================================================================================

  else if (tablemode == 1)
  {
    if (verbose) cout << "DEBUG::tablemode is 1" << endl;
    if (splitmode != 0)
    {
      if (verbose) cout << "DEBUG::splitmode is " << splitmode << endl;
      for (int in=0;in<nrNuis;in++)
      {
        for (int i=0;i<nrSamples;i++)
        {
          ws->var(("ATLAS_sampleNorm_"+sampleNames[i]).c_str())->setVal(0);
          if (verbose) cout << "DEBUG::Set " << "ATLAS_sampleNorm_"+sampleNames[i] << " to 0." << endl;
        }

        for (int isam=0;isam<nrSampleIDs;isam++)
        {
          bool isWW = true;
          bool isTop = true;

          if (verbose) cout << "DEBUG::Computing samples: " << endl;

          vector<string> idNames = sampleIDs[isam];
          int nrIDs = idNames.size();

          RooArgSet idNorms;
          for (int iid=0;iid<nrIDs;iid++)
          {
            isWW = isWW && (idNames[iid] == "ww" || idNames[iid] == "ggww" || idNames[iid] == "qqww");
            isTop = isTop && (idNames[iid] == "ttbar" || idNames[iid] == "st");

            if (verbose) cout << "DEBUG::--> " << idNames[iid] << endl;
            ws->var(("ATLAS_sampleNorm_"+idNames[iid]).c_str())->setVal(1);
            idNorms.add(*ws->var(("ATLAS_sampleNorm_"+idNames[iid]).c_str()));
          }

          for (int i=0;i<nrSets;i++)
          {
            double S = 0, B = 0;
            double p1sS = 0, p1sB = 0;
            double Serr = 0, Berr=0;

            int nrPdfs = pdfSets[i].size();

            RooRealVar* nui = (RooRealVar*)mc->GetNuisanceParameters()->find(vec_nuis[in].c_str());
            if (!nui)
            {
              cout << "ERROR::Couldn't find nuisance parameter: " << vec_nuis[in] << endl;
              exit(1);
            }

            double nui_err = nui->getError();
            if (error_map.find(string(nui->GetName())) != error_map.end()) nui_err = error_map[nui->GetName()];
            double nomVal = nui->getVal();

            if (verbose) cout << "DEBUG::" << nui->GetName() << " = " << nomVal << " +/- " << nui_err << endl;

            for (int j=0;j<nrPdfs;j++)
            {
              RooAbsPdf* subPdf = pdfSets[i][j];
              const RooArgSet* obs = subPdf->getObservables(*observables);

              // get nominal
              nui->setVal(nomVal);

              if (mode == 5 && error_map.find(string(nui->GetName())) == error_map.end())
              {
                bool isConst = firstPOI->isConstant();
                setConst(sysNuis,1);
                setVal(sampleNorms, 1);
                minimize(cr_nll);
                setConst(sysNuis,0);
                setVal(sampleNorms, 0);
                setVal(idNorms, 1);
                firstPOI->setConstant(isConst);
              }

              firstPOI->setVal(1);
              double thisSplusB = subPdf->expectedEvents(*obs);

              firstPOI->setVal(0);
              double thisB = subPdf->expectedEvents(*obs);
              double thisS = thisSplusB-thisB;

              if (verbose) cout << "DEBUG::pdf " << subPdf->GetName() << " has background " << thisB << endl;
              if (verbose) cout << "DEBUG::pdf " << subPdf->GetName() << " has signal     " << thisS << endl;

              // get +1s
              nui->setVal(nomVal + sign*nui_err);

              if (mode == 5 && error_map.find(string(nui->GetName())) == error_map.end())
              {
                bool isConst = firstPOI->isConstant();
                setConst(sysNuis,1);
                setVal(sampleNorms, 1);
                minimize(cr_nll);
                setConst(sysNuis,0);
                setVal(sampleNorms, 0);
                setVal(idNorms, 1);
                firstPOI->setConstant(isConst);
              }

              firstPOI->setVal(1);
              double thisP1sSplusB = subPdf->expectedEvents(*obs);

              firstPOI->setVal(0);
              double thisP1sB = subPdf->expectedEvents(*obs);
              double thisP1sS = thisP1sSplusB - thisP1sB;

              if (verbose) cout << "DEBUG::pdf " << subPdf->GetName() << " has background " << thisB << " for +1 sigma variation" << endl;
              if (verbose) cout << "DEBUG::pdf " << subPdf->GetName() << " has signal     " << thisS << " for +1 sigma variation" << endl;

              nui->setVal(nomVal);

              S += thisS;
              B += thisB;
              p1sS += thisP1sS;
              p1sB += thisP1sB;
            }

            Serr = fabs(p1sS-S);
            Berr = fabs(p1sB-B);

            if (verbose) cout << "DEBUG::channel is          " << channelNames[i] << endl;
            if (verbose) cout << "DEBUG::signal is           " << S << endl;
            if (verbose) cout << "DEBUG::background is       " << B << endl;
            if (verbose) cout << "DEBUG::signal error is     " << Serr << endl;
            if (verbose) cout << "DEBUG::background error is " << Berr << endl;

            matrixErr[in][i] = Serr;
            matrixErr[in][nrSets+i] = Berr;

            double relErrS = Serr/S;
            double relErrB = Berr/B;

            if (relErrS < cutoff) relErrS = 0;
            if (relErrB < cutoff) relErrB = 0;

            matrix[in][isam+i+isam*(nrSets-1)] = relErrB;
          }

          for (int iid=0;iid<nrIDs;iid++)
          {
            if (verbose) cout << "DEBUG::--> " << idNames[iid] << endl;
            ws->var(("ATLAS_sampleNorm_"+idNames[iid]).c_str())->setVal(0);
          }
        }

        for (int i=0;i<nrSamples;i++)
        {
          if (verbose) cout << "DEBUG::sample is " << sampleNames[i] << endl;
          ws->var(("ATLAS_sampleNorm_"+sampleNames[i]).c_str())->setVal(1);
        }
      }
    }

    // =====================================================================================
    // =====================================================================================

    else if (splitmode == 0)
    {
      if (verbose) cout << "DEBUG::splitmode is " << splitmode << endl;
      for (int in=0;in<nrNuis;in++)
      {
        for (int i=0;i<nrSets;i++)
        {
          double S = 0, B = 0;
          double p1sS = 0, p1sB = 0;
          double Serr = 0, Berr=0;

          int nrPdfs = pdfSets[i].size();

          RooRealVar* nui = (RooRealVar*)mc->GetNuisanceParameters()->find(vec_nuis[in].c_str());
          if (!nui)
          {
            cout << "ERROR::Couldn't find nuisance parameter: " << vec_nuis[in] << endl;
            exit(1);
          }

          double nui_err = nui->getError();
          if (error_map.find(string(nui->GetName())) != error_map.end()) nui_err = error_map[nui->GetName()];
          double nomVal = nui->getVal();

          if (verbose) if (string(nui->GetName()).find("norm") != string::npos) cout << "DEBUG::" << nui->GetName() << " has error " << nui->getError() << endl;

          for (int j=0;j<nrPdfs;j++)
          {
            RooAbsPdf* subPdf = pdfSets[i][j];
            const RooArgSet* obs = subPdf->getObservables(*observables);

            // get nominal
            nui->setVal(nomVal);

          if (mode == 5 && error_map.find(string(nui->GetName())) == error_map.end())
          {
            bool isConst = firstPOI->isConstant();
            setConst(sysNuis,1);
            //setVal(sampleNorms, 1);
            minimize(cr_nll);
            setConst(sysNuis,0);
            //setVal(sampleNorms, 0);
            //setVal(idNorms, 1);
            firstPOI->setConstant(isConst);
          }

            firstPOI->setVal(1);

            if (verbose) cout << "DEBUG::" << subPdf->GetName() << " : " << subPdf->expectedEvents(*obs) << endl;


            double thisSplusB = subPdf->expectedEvents(*obs);

            firstPOI->setVal(0);
            double thisB = subPdf->expectedEvents(*obs);
            double thisS = thisSplusB-thisB;

            if (verbose) cout << "DEBUG::pdf " << subPdf->GetName() << " has background " << thisB << endl;
            if (verbose) cout << "DEBUG::pdf " << subPdf->GetName() << " has signal     " << thisS << endl;

            // get +1s
            nui->setVal(nomVal + sign*nui_err);
            if (verbose) cout << "Nui " << nui->GetName() << " nomVal = " << nomVal << " (" << sign << ") " << nui_err << endl;

            firstPOI->setVal(1);
            double thisP1sSplusB = subPdf->expectedEvents(*obs);

            firstPOI->setVal(0);
            double thisP1sB = subPdf->expectedEvents(*obs);
            double thisP1sS = thisP1sSplusB - thisP1sB;

            if (verbose) cout << "DEBUG::pdf " << subPdf->GetName() << " has background " << thisP1sB << " for +1 sigma variation" << endl;
            if (verbose) cout << "DEBUG::pdf " << subPdf->GetName() << " has signal     " << thisP1sS << " for +1 sigma variation" << endl;

            nui->setVal(nomVal);

            if (mode == 5 && error_map.find(string(nui->GetName())) == error_map.end())
            {
              bool isConst = firstPOI->isConstant();
              setConst(sysNuis,1);
              //setVal(sampleNorms, 1);
              minimize(cr_nll);
              setConst(sysNuis,0);
              //setVal(sampleNorms, 0);
              //setVal(idNorms, 1);
              firstPOI->setConstant(isConst);
            }

            S += thisS;
            B += thisB;
            p1sS += thisP1sS;
            p1sB += thisP1sB;
          }

          Serr = fabs(p1sS-S);
          Berr = fabs(p1sB-B);

          if (in == 0)
          {
            if (verbose) cout << "DEBUG::channel is          " << channelNames[i] << endl;
            if (verbose) cout << "DEBUG::signal is           " << S << endl;
            if (verbose) cout << "DEBUG::background is       " << B << endl;
            if (verbose) cout << "DEBUG::signal error is     " << Serr << endl;
            if (verbose) cout << "DEBUG::background error is " << Berr << endl;
          }

          matrixErr[in][i] = Serr;
          matrixErr[in][nrSets+i] = Berr;

          double relErrS = Serr/S;
          double relErrB = Berr/B;

          if (relErrS < cutoff) relErrS = 0;
          if (relErrB < cutoff) relErrB = 0;

          matrix[in][i] = relErrS;
          matrix[in][nrSets+i] = relErrB;
        }
      }
    }
  }

  // =====================================================================================
  // =====================================================================================

  // Print table
  if (verbose) cout << "DEBUG::Printing table" << endl;

  cout << "\\begin{slide}\n"
       << "\\heading{}\n"
       << "\\begin{center}\n"
       << "\\scalebox{0.7}{\n"
       << "\\begin{tabular}{c";
  for (int i=0;i<(tablemode==0?nrSamplesNice+2:(splitmode!=0?nrSampleIDs*nrSets+1:nrSets*2+1));i++) cout << "|c";
  cout << "}\n";

  if (mode == 0 || mode == 5) printNice(firstCol, matrix, tablemode==0?matrixErr:NULL, header, tablemode==0?nrChannels:nrNuis, tablemode==0?nrSamplesNice:(splitmode!=0?nrSampleIDs*nrSets:nrSets*2), nSigFig, cout);
  else printNice(firstCol, matrix, NULL, header, tablemode==0?nrChannels:nrNuis, tablemode==0?nrSamplesNice:(splitmode!=0?nrSampleIDs*nrSets:nrSets*2), nSigFig, cout);

  cout << "\\end{tabular}\n"
       << "}\n"
       << "\\end{center}\n"
       << "\\end{slide}\n\n";

  ofstream outFile(outFileName.str().c_str());

  outFile << "\\begin{slide}\n"
       << "\\heading{}\n"
       << "\\begin{center}\n"
       << "\\scalebox{0.7}{\n"
       << "\\begin{tabular}{c";
  for (int i=0;i<(tablemode==0?nrSamplesNice+2:(splitmode!=0?nrSampleIDs*nrSets+1:nrSets*2+1));i++) outFile << "|c";
  outFile << "}\n";

  if (mode == 0 || mode == 5) printNice(firstCol, matrix, tablemode==0?matrixErr:NULL, header, tablemode==0?nrChannels:nrNuis, tablemode==0?nrSamplesNice:(splitmode!=0?nrSampleIDs*nrSets:nrSets*2), nSigFig, outFile);
  else printNice(firstCol, matrix, NULL, header, tablemode==0?nrChannels:nrNuis, tablemode==0?nrSamplesNice:(splitmode!=0?nrSampleIDs*nrSets:nrSets*2), nSigFig, outFile);

  outFile << "\\end{tabular}\n"
       << "}\n"
       << "\\end{center}\n"
       << "\\end{slide}\n\n";

  // =====================================================================================

  if (verbose) cout << "DEBUG::Done. Check output in " << outFileName.str() << endl;

}

void getErrors(RooWorkspace* w, RooRealVar* par, RooNLLVar* nll, RooArgSet& allParams)
{
  return;
  cout << "-------------------------------------------------" << endl;
  cout << "Getting error for parameter: " << par->GetName() << endl;
  static int errNr = 0;
  errNr++;
  stringstream shNameStr;
  shNameStr << "tmp_snapshot_" << errNr;
  string shName = shNameStr.str();
  double errhi, errlo;
  w->saveSnapshot(shName.c_str(),allParams);
  getError(1, nll, par, errhi);
  w->loadSnapshot(shName.c_str());
  getError(-1, nll, par, errlo);
  par->setAsymError(errlo, errhi);
  w->loadSnapshot(shName.c_str());
  cout << "Err = +" << errhi << "  -" << errlo << endl;
  cout << endl << endl;
}

void setConst(RooArgSet& params, bool flag)
{
  TIterator* itr = params.createIterator();
  RooRealVar* param;
  while ((param = (RooRealVar*)itr->Next()))
  {
    param->setConstant(flag);
  }
  delete itr;
}

void setVal(RooArgSet& params, double val)
{
  TIterator* itr = params.createIterator();
  RooRealVar* param;
  while ((param = (RooRealVar*)itr->Next()))
  {
    param->setVal(val);
  }
  delete itr;
}
