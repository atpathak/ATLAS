#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooDataSet.h"
#include "RooMinimizerFcn.h"
#include "RooNLLVar.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"
#include "RooCategory.h"
#include "TFile.h"

#include "macros/makeAsimovData.C"
#include "macros/minimize.C"
#include "macros/findSigma.C"

double subtractError(double err12, double err1)
{
  return sqrt(err12*err12-err1*err1);
}

void getMuhat(const char* inFileName,
  const char* wsName = "combined",
  const char* modelConfigName = "ModelConfig",
  const char* dataName = "obsData")
{
//0 = standard
//1 = cb
//2 = breakdown
  int mode = 2;

//0 = do observed error
//1 = do expected mu=1 error
  bool doAsimov = 0;

  TFile* file = new TFile(inFileName);
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

  ws->loadSnapshot("nominalNuis");

  //RooRealVar* mu = ws->var("SigXsecOverSM");
  RooRealVar* poi = (RooRealVar*)mc->GetParametersOfInterest()->first();

  RooDataSet* asimovData = (RooDataSet*)ws->data("asimovData_1");//makeAsimovData(mc, 0, ws, mc->GetPdf(), data, 1);
  //RooDataSet* asimovData = (RooDataSet*)ws->data("asimovData_1");
  //ws->loadSnapshot("conditionalGlobs_1");
  mc->GetGlobalObservables()->Print("v");
  if (doAsimov) data = asimovData;
  RooNLLVar* nll = (RooNLLVar*)mc->GetPdf()->createNLL(*data, Constrain(*mc->GetNuisanceParameters()));

  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");
  ROOT::Math::MinimizerOptions::SetDefaultStrategy(0);
  ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(-1);

  poi->setConstant(0);

  minimize(nll);

  //double muhat = poi->getVal();

  RooArgSet nuisAndPOI(*mc->GetNuisanceParameters(),*mc->GetParametersOfInterest());
  ws->saveSnapshot("tmp_shot",nuisAndPOI);


  double nll_val_true = nll->getVal();
  double muhat = poi->getVal();
  double err_guess = poi->getError();

  if (string(inFileName).find("_cb") != string::npos) mode = 1;

  if (mode == 0)
  {
//find non-mc stat sys + stat err
    const RooArgSet* nuis = mc->GetNuisanceParameters();
    TIterator* nitr = nuis->createIterator();
    RooRealVar* var;
    while ((var = (RooRealVar*)nitr->Next()))
    {
      if (string(var->GetName()).find("gamma") != string::npos) var->setConstant(1); // if it's a gamma, set const
    }


    double sys_stat_err_hi = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, 1, 0.01);
    double sys_stat_err_lo = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, -1, 0.01);





//find stat-only

    //minimize(nll);
    ws->loadSnapshot("tmp_shot");

    nitr->Reset();
    while ((var = (RooRealVar*)nitr->Next()))
    {
      if (string(var->GetName()).find("norm") != string::npos) continue;
      var->setConstant(1);
    }


    double stat_err_hi = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, 1, 0.01);
    double stat_err_lo = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, -1, 0.01);





//find alpha sys+stat

    //minimize(nll);
    ws->loadSnapshot("tmp_shot");

    nitr->Reset();
    while ((var = (RooRealVar*)nitr->Next()))
    {
      if (string(var->GetName()).find("norm") != string::npos) continue;
      var->setConstant(1);
    }

    vector<string> sysNames;
    //sysNames.push_back("alpha_Fake_Rate");
    sysNames.push_back("alpha_pdf_qqbar_ACCEPT");
    sysNames.push_back("alpha_QCDscale_VV_ACCEPT");
//     sysNames.push_back("alpha_ATLAS_WW_MTSHAPE");
    sysNames.push_back("alpha_ATLAS_alpha_ww_model");

    for (int i=0;i<(int)sysNames.size();i++)
    {
      if (ws->var(sysNames[i].c_str())) ws->var(sysNames[i].c_str())->setConstant(0);
    }

    double asys_stat_err_hi = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, 1, 0.01);
    double asys_stat_err_lo = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, -1, 0.01);

    cout << "asys_stat_err_hi = " << asys_stat_err_hi << endl;
    cout << "asys_stat_err_lo = " << asys_stat_err_lo << endl;
    cout << "STAT : +" << stat_err_hi << " / -" << stat_err_lo << endl;


//find mc-stat + data stat
    ws->loadSnapshot("tmp_shot");

    nitr->Reset();
    while ((var = (RooRealVar*)nitr->Next()))
    {
      if (string(var->GetName()).find("norm") != string::npos) continue;
      if (string(var->GetName()).find("gamma") != string::npos) continue;
      var->setConstant(1);
    }

    double mc_stat_err_hi = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, 1, 0.01);
    double mc_stat_err_lo = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, -1, 0.01);


//find total err

    ws->loadSnapshot("tmp_shot");
    nitr->Reset();
    while ((var = (RooRealVar*)nitr->Next()))
    {
      var->setConstant(0);
    }

    double err_hi = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, 1, 0.01);
    double err_lo = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, -1, 0.01);


//compute components
    double sys_err_hi = subtractError(sys_stat_err_hi, stat_err_hi);
    double sys_err_lo = subtractError(sys_stat_err_lo, stat_err_lo);

    double mc_err_hi = subtractError(mc_stat_err_hi, stat_err_hi);
    double mc_err_lo = subtractError(mc_stat_err_lo, stat_err_lo);

    double asys_err_hi = subtractError(asys_stat_err_hi, stat_err_hi);
    double asys_err_lo = subtractError(asys_stat_err_lo, stat_err_lo);


    cout << "muhat: " << muhat << endl;
    cout << "ASYS : +" << asys_err_hi << " / -" << asys_err_lo << endl;
    cout << "SYS  : +" << sys_err_hi << " / -" << sys_err_lo << endl;
    cout << "STAT : +" << stat_err_hi << " / -" << stat_err_lo << endl;
    cout << "MC   : +" << mc_err_hi << " / -" << mc_err_lo << endl;
    cout << "TOT  : +" << err_hi << " / -" << err_lo << endl;
  }
  else if (mode == 1)
  {

//find sys + stat err
    const RooArgSet* nuis = mc->GetNuisanceParameters();
    TIterator* nitr = nuis->createIterator();
    RooRealVar* var;
    while ((var = (RooRealVar*)nitr->Next()))
    {
      string varName(var->GetName());
      if (string(var->GetName()).find("norm") != string::npos) continue;
    }


    double sys_stat_err_hi = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, 1, 0.01);
    double sys_stat_err_lo = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, -1, 0.01);

//find non-mc stat sys (2011) + stat err
    ws->loadSnapshot("tmp_shot");
    nitr->Reset();
    while ((var = (RooRealVar*)nitr->Next()))
    {
      string varName(var->GetName());
      if (string(var->GetName()).find("norm") != string::npos) continue;
      if (varName.find("gamma") != string::npos) var->setConstant(1); // if it's a gamma, set const
      if (varName.find("_lvlv_2012") != string::npos) var->setConstant(1); // if it's 2012, set const
      if (varName.find("_lvlv") == string::npos) var->setConstant(1); // if it's corr, set const
    }


    double sys11_stat_err_hi = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, 1, 0.01);
    double sys11_stat_err_lo = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, -1, 0.01);



//find non-mc stat sys (2012) + stat err
    ws->loadSnapshot("tmp_shot");
    nitr->Reset();
    while ((var = (RooRealVar*)nitr->Next()))
    {
      string varName(var->GetName());
      if (string(var->GetName()).find("norm") != string::npos) continue;
      if (string(var->GetName()).find("gamma") != string::npos) var->setConstant(1); // if it's a gamma, set const
      if (varName.find("_lvlv") != string::npos && varName.find("_lvlv_2012") == string::npos) var->setConstant(1); // if it's 2011, set const
      if (varName.find("_lvlv") == string::npos) var->setConstant(1); // if it's corr, set const
    }


    double sys12_stat_err_hi = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, 1, 0.01);
    double sys12_stat_err_lo = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, -1, 0.01);



//find non-mc stat sys (corr) + stat err
    ws->loadSnapshot("tmp_shot");
    nitr->Reset();
    while ((var = (RooRealVar*)nitr->Next()))
    {
      string varName(var->GetName());
      if (string(var->GetName()).find("norm") != string::npos) continue;
      if (string(var->GetName()).find("gamma") != string::npos) var->setConstant(1); // if it's a gamma, set const
      if (varName.find("_lvlv") != string::npos) var->setConstant(1); // if it's 2011 or 2012 only, set const
    }


    double syscorr_stat_err_hi = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, 1, 0.01);
    double syscorr_stat_err_lo = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, -1, 0.01);





//find stat-only

    //minimize(nll);
    ws->loadSnapshot("tmp_shot");

    nitr->Reset();
    while ((var = (RooRealVar*)nitr->Next()))
    {
      if (string(var->GetName()).find("norm") != string::npos) continue;
      var->setConstant(1);
    }


    double stat_err_hi = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, 1, 0.01);
    double stat_err_lo = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, -1, 0.01);


    ws->loadSnapshot("tmp_shot");


//find mc-stat + data stat

    ws->loadSnapshot("tmp_shot");
    nitr->Reset();
    while ((var = (RooRealVar*)nitr->Next()))
    {
      if (string(var->GetName()).find("norm") != string::npos) continue;
      if (string(var->GetName()).find("gamma") != string::npos) continue;
      var->setConstant(1);
    }

    double mc_stat_err_hi = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, 1, 0.01);
    double mc_stat_err_lo = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, -1, 0.01);


//find total err

    ws->loadSnapshot("tmp_shot");
    nitr->Reset();
    while ((var = (RooRealVar*)nitr->Next()))
    {
      var->setConstant(0);
    }

    double err_hi = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, 1, 0.01);
    double err_lo = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, -1, 0.01);


//compute components
    double sys_err_hi = subtractError(sys_stat_err_hi, stat_err_hi);
    double sys_err_lo = subtractError(sys_stat_err_lo, stat_err_lo);

    double sys11_err_hi = subtractError(sys11_stat_err_hi, stat_err_hi);
    double sys11_err_lo = subtractError(sys11_stat_err_lo, stat_err_lo);

    double sys12_err_hi = subtractError(sys12_stat_err_hi, stat_err_hi);
    double sys12_err_lo = subtractError(sys12_stat_err_lo, stat_err_lo);

    double syscorr_err_hi = subtractError(syscorr_stat_err_hi, stat_err_hi);
    double syscorr_err_lo = subtractError(syscorr_stat_err_lo, stat_err_lo);

    double mc_err_hi = subtractError(mc_stat_err_hi, stat_err_hi);
    double mc_err_lo = subtractError(mc_stat_err_lo, stat_err_lo);



    cout << "muhat: " << muhat << endl;
    cout << "SYS 11   : +" << sys11_err_hi << " / -" << sys11_err_lo << endl;
    cout << "SYS 12   : +" << sys12_err_hi << " / -" << sys12_err_lo << endl;
    cout << "SYS CORR : +" << syscorr_err_hi << " / -" << syscorr_err_lo << endl;
    cout << "SYS TOT  : +" << sys_err_hi << " / -" << sys_err_lo << endl;
    cout << "STAT     : +" << stat_err_hi << " / -" << stat_err_lo << endl;
    cout << "MC       : +" << mc_err_hi << " / -" << mc_err_lo << endl;
    cout << "TOT      : +" << err_hi << " / -" << err_lo << endl;
  }
  else if (mode == 2)
  {

//find non-mc stat sys + stat err
    const RooArgSet* nuis = mc->GetNuisanceParameters();
    TIterator* nitr = nuis->createIterator();
    RooRealVar* var;
    while ((var = (RooRealVar*)nitr->Next()))
    {
      if (string(var->GetName()).find("norm") != string::npos) continue;
      var->setConstant(1);
    }


    double stat_err_hi = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, 1, 0.01);
    double stat_err_lo = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, -1, 0.01);



//find total err

    ws->loadSnapshot("tmp_shot");

    double err_hi = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, 1, 0.01);
    double err_lo = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, -1, 0.01);


//find alpha sys+stat

    //minimize(nll);
    ws->loadSnapshot("tmp_shot");

    nitr->Reset();
    while ((var = (RooRealVar*)nitr->Next()))
    {
      if (string(var->GetName()).find("norm") != string::npos) continue;
      var->setConstant(1);
    }
    ws->saveSnapshot("tmp_shot2",nuisAndPOI);

    set<pair<double, string> > set_sys;

    double quad_hi = 0;
    double quad_lo = 0;
    map<string,double> ind_err_hi;
    map<string,double> ind_err_lo;
    nitr->Reset();
    while ((var = (RooRealVar*)nitr->Next()))
    {
      if (string(var->GetName()).find("norm") != string::npos) continue;
      if (string(var->GetName()).find("gamma") != string::npos) continue;


      string varName(var->GetName());

      ws->loadSnapshot("tmp_shot2");
      var->setConstant(0);

      ind_err_hi[varName] = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, 1, 0.01);
      ind_err_lo[varName] = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, -1, 0.01);

      ind_err_hi[varName] = subtractError(ind_err_hi[varName], stat_err_hi);
      ind_err_lo[varName] = subtractError(ind_err_lo[varName], stat_err_lo);

      if (ind_err_hi[varName] != ind_err_hi[varName] || ind_err_hi[varName] < 0.001) ind_err_hi[varName]=0;
      if (ind_err_lo[varName] != ind_err_lo[varName] || ind_err_lo[varName] < 0.001) ind_err_lo[varName]=0;

//       if (fabs(ind_err_hi[varName]-stat_err_hi) < 0.0001)
//       {
//  ind_err_hi[varName] = 0;
//       }
//       else
//       {
//  ind_err_hi[varName] = subtractError(ind_err_hi[varName], stat_err_hi);
//       }

//       if (fabs(ind_err_lo[varName]-stat_err_lo) < 0.0001)
//       {
//  ind_err_lo[varName] = 0;
//       }
//       else
//       {
//  ind_err_lo[varName] = subtractError(ind_err_lo[varName], stat_err_lo);
//       }

//       ind_err_hi[varName] = subtractError(ind_err_hi[varName], stat_err_hi);
//       ind_err_lo[varName] = subtractError(ind_err_lo[varName], stat_err_lo);

      set_sys.insert(make_pair(sqrt((1+ind_err_hi[varName])*(1+ind_err_lo[varName]))-1,varName));

      quad_hi += ind_err_hi[varName]*ind_err_hi[varName];
      quad_lo += ind_err_lo[varName]*ind_err_lo[varName];

      var->setConstant(1);
    }
    quad_hi = sqrt(quad_hi);
    quad_lo = sqrt(quad_lo);









//find alpha data stat and cr stat errors

    //minimize(nll);
    ws->loadSnapshot("tmp_shot");

    nitr->Reset();
    while ((var = (RooRealVar*)nitr->Next()))
    {
      var->setConstant(1);
    }
    ws->saveSnapshot("tmp_shot3",nuisAndPOI);

    double sr_err_hi = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, 1, 0.01);
    double sr_err_lo = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, -1, 0.01);





    set<pair<double, string> > set_stat;

    map<string,double> cr_err_hi;
    map<string,double> cr_err_lo;
    nitr->Reset();
    while ((var = (RooRealVar*)nitr->Next()))
    {
      if (string(var->GetName()).find("norm") == string::npos) continue;

      string varName(var->GetName());

      ws->loadSnapshot("tmp_shot3");
      var->setConstant(0);

      cr_err_hi[varName] = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, 1, 0.01);
      cr_err_lo[varName] = findSigma(nll, nll_val_true, poi, muhat+err_guess, muhat, -1, 0.01);

      cr_err_hi[varName] = subtractError(cr_err_hi[varName], sr_err_hi);
      cr_err_lo[varName] = subtractError(cr_err_lo[varName], sr_err_lo);

      if (cr_err_hi[varName] != cr_err_hi[varName] || cr_err_hi[varName] < 0.001) cr_err_hi[varName]=0;
      if (cr_err_lo[varName] != cr_err_lo[varName] || cr_err_lo[varName] < 0.001) cr_err_lo[varName]=0;

      set_stat.insert(make_pair(sqrt((1+cr_err_hi[varName])*(1+cr_err_lo[varName]))-1,varName));

      var->setConstant(1);
    }



//compute components
    double sys_err_hi = subtractError(err_hi, stat_err_hi);
    double sys_err_lo = subtractError(err_lo, stat_err_lo);




    set<string> set_sysNames;
    cout << setprecision(3);
    cout << "\nSYSTEMATIC:" << endl;
    for (set<pair<double, string> >::reverse_iterator itr=set_sys.rbegin();itr!=set_sys.rend();itr++)
    {
      string varName(itr->second);
      cout << varName << " : +" << ind_err_hi[varName] << " / -" << ind_err_lo[varName] << endl;
      set_sysNames.insert(varName);
    }

    cout << "\nSTATISTICAL:" << endl;
    for (set<pair<double, string> >::reverse_iterator itr=set_stat.rbegin();itr!=set_stat.rend();itr++)
    {
      string varName(itr->second);
      cout << varName << " : +" << cr_err_hi[varName] << " / -" << cr_err_lo[varName] << endl;
      set_sysNames.insert(varName);
    }

    int counter = 0;
    ofstream outFile1("sys_errors.txt");
    ofstream outFile2("sys_names.txt");
    for (set<string>::iterator itr=set_sysNames.begin();itr!=set_sysNames.end();itr++)
    {
      if (itr->find("norm") != string::npos)
      {
        outFile1 << counter << " " << cr_err_hi[*itr] << " " << -cr_err_lo[*itr] << "\n";
      }
      else
      {
        outFile1 << counter << " " << ind_err_hi[*itr] << " " << -ind_err_lo[*itr] << "\n";
      }
      outFile2 << counter << " " << *itr << "\n";
      counter++;
    }

    outFile1.close();
    outFile2.close();

    cout << "muhat: " << muhat << endl;
    cout << "CTRL    : +" << quad_hi << " / -" << quad_lo << endl;
    cout << "SYS     : +" << sys_err_hi << " / -" << sys_err_lo << endl;
    cout << "SR STAT : +" << sr_err_hi << " / -" << sr_err_lo << endl;
    cout << "STAT    : +" << stat_err_hi << " / -" << stat_err_lo << endl;
    cout << "TOT     : +" << err_hi << " / -" << err_lo << endl;
  }

  //cout << "muhat = " << muhat << " +" << err_hi_stat << " / -" << err_lo_stat << " (stat) +" << sys_err_hi << " / -" << sys_err_lo << " (sys) -> +" << err_hi << " / -" << err_lo << " (tot)" << endl;
}
