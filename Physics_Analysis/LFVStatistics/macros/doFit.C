#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooMinimizer.h"
#include "RooNLLVar.h"
#include "RooSimultaneous.h"
#include "RooCategory.h"
#include "RooPoisson.h"
#include "RooProduct.h"
#include "RooRealSumPdf.h"

#include "RooStats/HistFactory/RooBarlowBeestonLL.h"
#include "RooStats/HistFactory/HistFactorySimultaneous.h"

#include "TFile.h"
#include "Math/MinimizerOptions.h"

#include "macros/printNice.C"
#include "macros/getError.C"

#include <string>
#include <sstream>
#include <vector>

using namespace std;
using namespace RooFit;
using namespace RooStats;
using namespace HistFactory;

void makeAsimovData(ModelConfig* mcInWs, bool doConditional, RooWorkspace* combWS, RooAbsPdf* combPdf, RooDataSet* combData, bool b_only, string snapshotName, int profileMode);
void unfoldConstraints(RooArgSet& initial, RooArgSet& final, RooArgSet& obs, RooArgSet& nuis, int& counter);

void getErrors(RooWorkspace* w, RooRealVar* par, RooNLLVar* nll, RooArgSet& allParams);
void doFit(string inFileName,
     string outFileName,
     const char* wsName = "combined",
     const char* modelConfigName = "ModelConfig",
     const char* dataName = "obsData");


void doFit(int mass,
     string folder,
     const char* wsName = "combined",
     const char* modelConfigName = "ModelConfig",
     const char* dataName = "obsData")
{
  stringstream inFileName;
  inFileName << "workspaces/" << folder << "/" << mass << ".root";

  cout << "Running over workspace: " << inFileName.str() << endl;

  stringstream outFileName;
  outFileName << "fit_params/" << folder;
  system(("mkdir -vp " + outFileName.str()).c_str());

  outFileName << "/" << mass << ".txt";

  doFit(inFileName.str(),outFileName.str(),
  wsName,modelConfigName,dataName);
}

void doFit(string inFileName,
     string outFileName,
     const char* wsName,
     const char* modelConfigName,
     const char* dataName)
{
  bool minos = 0;

  TFile f(inFileName.c_str());
  RooWorkspace* ws = (RooWorkspace*)f.Get(wsName);
  if (!ws)
  {
    cout << "ERROR::Workspace: " << wsName << " doesn't exist!" << endl;
    return;
  }
  if (string(dataName).find("asimovData_0") != string::npos) ws->loadSnapshot("conditionalGlobs_0");
  if (string(dataName).find("asimovData_1") != string::npos) ws->loadSnapshot("conditionalGlobs_1");
  if (string(dataName).find("asimovData_muhat") != string::npos) ws->loadSnapshot("conditionalGlobs_muhat");
  // ws->loadSnapshot("conditionalGlobs_inj");

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
  // RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
  // RooNLLVar::SetIgnoreZeroEntries(1);
  RooRealVar* firstPOI = (RooRealVar*)mc->GetParametersOfInterest()->first();
  RooAbsPdf* pdf = mc->GetPdf();
  firstPOI->setRange(0,100);

  // TIterator* nItr = mc->GetNuisanceParameters()->createIterator();
  // RooRealVar* thisNui;
  // while ((thisNui = (RooRealVar*)nItr->Next()))
  // {
  //   string poisName = string(thisNui->GetName()) + "_constraint";
  //   if (poisName.find("gamma_stat") == string::npos) continue;
  //   RooPoisson* pois = (RooPoisson*)ws->pdf(poisName.c_str());
  //   if (!pois)
  //   {
  //     cout << "ERROR::Couldn't fine corresponding poisson for var " << thisNui->GetName() << endl;
  //     exit(1);
  //   }
  //   
  //   string prodName = string(thisNui->GetName())+"_poisMean";
  //   RooProduct* prod = (RooProduct*)ws->function(prodName.c_str());
  //   pois->setNoRounding(true);
  //   pois->Print();
  //   prod->Print();
  //   // thisNui->setVal(0.5);
  // 
  // }

  vector<string> minosParNames;
  // minosParNames.push_back("ATLAS_norm_WW0j");
  // minosParNames.push_back("ATLAS_epsilon");
  // minosParNames.push_back("SigXsecOverSM");
  // minosParNames.push_back("alpha_Fake_Rate");
  // minosParNames.push_back("alpha_ATLAS_JES");
  // minosParNames.push_back("ATLAS_B_EFF");
  // minosParNames.push_back("ATLAS_L_EFF");
  // minosParNames.push_back("ATLAS_JES");
  // minosParNames.push_back("ATLAS_JER");
  // minosParNames.push_back("ATLAS_METE");
  // minosParNames.push_back("QCDscale_ttbar");
  // minosParNames.push_back("alpha_ATLAS_multijet_norm_zh");
  int nrMinos = minosParNames.size();
  vector<RooRealVar*> minosPars;
  for (int i=0;i<nrMinos;i++)
  {
    RooRealVar* var = ws->var(minosParNames[i].c_str());
    if (!var)
    {
      cout << "ERROR::Var doesn't exist: " << minosParNames[i] << endl;
    }
    minosPars.push_back(var);
  }

  // if (pdf->canBeExtended())
  // {
  //   firstPOI->setVal(1);
  //   double s_b = pdf->expectedEvents(*data->get());
  //   firstPOI->setVal(0);
  //   double b = pdf->expectedEvents(*data->get());
  //   cout << "S: " << s_b-b << ", b: " << b << ", ws = " << wsName << endl;
  // }

  // firstPOI->setVal(0.521197);
  // RooArgSet poi(*firstPOI);
  // if (conditionalSnapshot != "") ws->loadSnapshot(conditionalSnapshot.c_str());
  // ProfileLikelihoodTestStat testStat(*mc->GetPdf());
  // double val = 2*testStat.Evaluate(*data, poi);
  // cout << "test stat: " << val << endl;
  // return;

  ws->loadSnapshot("conditionalNuis_0");

  // mc->GetGlobalObservables()->Print("v");
  // mc->GetNuisanceParameters()->Print("v");

  RooArgSet nuis = *mc->GetNuisanceParameters();
  // nuis.add(*combWS->var("lumi"));
  TIterator* itr = nuis.createIterator();
  RooRealVar* var;
  RooArgSet poi(*firstPOI);
  firstPOI->setConstant(0);

  // ProfileLikelihoodTestStat testStat(*mc->GetPdf());
  // testStat.SetOneSided(true);
  // testStat.SetNuis(&nuis);
  // cout << "Test stat: " << testStat.Evaluate(*data, poi) << endl;

  int nrCols = 3;
  int nrNuis = nuis.getSize();
  string* header = new string[nrCols];
  header[0] = "Best Fit";
  header[1] = "ErrorHi";
  header[2] = "ErrorLo";
  string* header2 = new string[nrCols+1];
  header2[0] = "Prefit";
  header2[1] = "mu=0";
  header2[2] = "mu=1";
  string* firstCol0 = new string[nrNuis+1];
  string* firstCol1 = new string[nrNuis+1];
  string* firstCol_un = new string[nrNuis+2];
  string* firstCol_all = new string[nrNuis+1];
  double** matrix0 = new double*[nrNuis];
  double** matrix1 = new double*[nrNuis];
  double** matrix_un = new double*[nrNuis+1];

  firstCol0[0] = "Parameter";
  firstCol1[0] = "Parameter";
  firstCol_un[0] = "Parameter";
  firstCol_all[0] = "Parameter";

  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");
  ROOT::Math::MinimizerOptions::SetDefaultStrategy(0);
  ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(1);
  // RooMinuit::SetMaxIterations(10000);
  // RooMinimizer::SetMaxFunctionCalls(10000);

  ofstream outFile(outFileName.c_str());

  cout << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  cout << "%%  UNCONDITIONAL MU  %%" << endl;
  cout << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;

  // int counter = 0;
  // while ((var = (RooRealVar*)itr->Next()))
  // {
  //   var->setConstant(1);
  //   counter++;
  // }
  // if ((var = ws->var("ATLAS_norm_Top1j"))) var->setConstant(0);
  // if ((var = ws->var("ATLAS_norm_WW0j"))) var->setConstant(0);
  // if ((var = ws->var("ATLAS_norm_WW1j"))) var->setConstant(0);

  double errhi, errlo;
  RooArgSet allParams(*mc->GetNuisanceParameters(), *firstPOI);
  RooArgSet allParams_tmp = allParams;

  bool doBarlow = 0;

  RooNLLVar* fNll;
  cout << "Class = " << pdf->ClassName() << endl;
  if (string(pdf->ClassName()) == "RooSimultaneous" && doBarlow)
  {
    cout << "Creating barlow" << endl;
    RooSimultaneous* simPdf = (RooSimultaneous*)pdf;
    HistFactorySimultaneous* model_hf = new HistFactorySimultaneous( *simPdf );
    fNll = (RooNLLVar*)model_hf->createNLL(*data, RooFit::Constrain(*mc->GetNuisanceParameters()),RooFit::GlobalObservables(*mc->GetGlobalObservables()));
  }
  else
  {
    fNll = (RooNLLVar*)pdf->createNLL(*data,RooFit::Constrain(*mc->GetNuisanceParameters()),RooFit::GlobalObservables(*mc->GetGlobalObservables()));//, Constrain(allParams_tmp));
  }

  // if (string(pdf->ClassName()) == "RooSimultaneous")
  // {
  //   RooSimultaneous* simPdf = (RooSimultaneous*)pdf;
  //   RooCategory* cat = (RooCategory*)&simPdf->indexCat();
  //   TList* dataList = data->split(*cat);
  //
  //   TIterator* catItr = cat->typeIterator();
  //   RooCatType* type;
  //   while ((type = (RooCatType*)catItr->Next()))
  //   {
  //     RooDataSet* thisData = NULL;
  //     int nrData = dataList->GetEntries();
  //     for (int id=0;id<nrData;id++)
  //     {
  //       if (string(((RooDataSet*)dataList->At(id))->GetName()) == string(type->GetName()))
  //       {
  //         thisData = (RooDataSet*)dataList->At(id);
  //         break;
  //       }
  //     }
  //     string pdfName(type->GetName());
  //     RooAbsPdf* thisPdf = simPdf->getPdf(pdfName.c_str());
  //
  //     RooArgSet emptySet;
  //     nll_map[pdfName] = (RooNLLVar*)thisPdf->createNLL(*thisData, Constrain(emptySet));
  //   }
  // }

  // printNlls();
  ws->loadSnapshot("ucmles");
  minimize(fNll,minos);
  double LLun = fNll->getVal();
  // double poi_val = firstPOI->getVal();
  // makeAsimovData(mc, 1, ws, mc->GetPdf(), data, 0, "conditionalNuis_muhat", 2); // s+b, profile at muhat

  // cout << "poi val = " << poi_val << endl;
  // return;
  
  for (int i=0;i<nrMinos;i++)
  {
    getErrors(ws, minosPars[i], fNll, allParams);
  }
  // pdf->fitTo(*data, PrintLevel(0), Extended());
  // double L_muhat = pdf->getVal();

  int counter = 0;
  while ((var = (RooRealVar*)itr->Next()))
  {
    // cout << "var: " << var << endl;
    // cout << "var name: " << var->GetName() << endl;

    stringstream name;
    name << counter << " " << var->GetName();
    firstCol_un[counter+1] = name.str();
    matrix_un[counter] = new double[nrCols];
    matrix_un[counter][0] = var->getVal();
    matrix_un[counter][1] = var->getErrorHi();
    matrix_un[counter][2] = var->getErrorLo();
    counter++;
  }

  matrix_un[counter] = new double[nrCols];
  // cout << "Out loop" << endl;
  firstCol_un[counter+1] = "mu";
  // cout << "Out loop 2" << endl;
  // cout << "Counter: " << counter << ", nr nuis: " << nrNuis << endl;
  matrix_un[counter][0] = firstPOI->getVal();
  header2[3] = "muhat=";
  stringstream muStr;
  muStr << setprecision(3);
  muStr << firstPOI->getVal();
  header2[3] += muStr.str();
  // cout << "Out loop 3" << endl;
  matrix_un[counter][1] = firstPOI->getErrorHi();
  matrix_un[counter][2] = firstPOI->getErrorLo();
  // cout << "Out loop 4" << endl;

  cout << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  cout << "%%  MU=0   %%%%%%%%%%%%%" << endl;
  cout << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;

  firstPOI->setVal(0);
  firstPOI->setConstant(1);

  // minimize(fNll);
  minimize(fNll,minos);
  double LL0 = fNll->getVal();
  for (int i=0;i<nrMinos;i++)
  {
    getErrors(ws, minosPars[i], fNll, allParams);
  }
  // return;

  // pdf->fitTo(*data, PrintLevel(0), Extended());
  // double L_mu1 = pdf->getVal();

  counter = 0;
  itr->Reset();
  while ((var = (RooRealVar*)itr->Next()))
  {
    stringstream name;
    name << counter << " " << var->GetName();
    firstCol0[counter+1] = name.str();
    matrix0[counter] = new double[nrCols];
    matrix0[counter][0] = var->getVal();
    matrix0[counter][1] = var->getErrorHi();
    matrix0[counter][2] = var->getErrorLo();
    counter++;
  }
  cout << endl << endl;

  cout << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  cout << "%%  MU=1   %%%%%%%%%%%%%" << endl;
  cout << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;

  firstPOI->setVal(1);
  firstPOI->setConstant(1);

  // minimize(fNll);
  minimize(fNll,minos);
  double LL1 = fNll->getVal();
  for (int i=0;i<nrMinos;i++)
  {
    getErrors(ws, minosPars[i], fNll, allParams);
  }

  delete fNll;

  // pdf->fitTo(*data, PrintLevel(0), Extended());
  // double L_mu0 = pdf->getVal();

  counter = 0;
  itr->Reset();
  double** matrix_all = new double*[nrNuis];
  while ((var = (RooRealVar*)itr->Next()))
  {
    stringstream name;
    name << counter << " " << var->GetName();
    firstCol1[counter+1] = name.str();
    firstCol_all[counter+1] = var->GetName();
    matrix1[counter] = new double[nrCols];
    matrix1[counter][0] = var->getVal();
    matrix1[counter][1] = var->getErrorHi();
    matrix1[counter][2] = var->getErrorLo();

    matrix_all[counter] = new double[nrCols+1];
    if (name.str().find("gamma_stat") != string::npos || name.str().find("_norm_") != string::npos) matrix_all[counter][0] = 1;
    else matrix_all[counter][0] = 0;
    matrix_all[counter][1] = matrix0[counter][0];
    matrix_all[counter][2] = matrix1[counter][0];
    matrix_all[counter][3] = matrix_un[counter][0];

    counter++;
    // cout << var->GetName() << ": " << var->getVal() << endl;
  }
  cout << endl << endl;

  outFile << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  outFile << "%%  UNCONDITIONAL MU  %%" << endl;
  outFile << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  printNice(firstCol_un, matrix_un, NULL, header, nrNuis+1, nrCols, 3, outFile);

  outFile << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  outFile << "%%  MU = 0            %%" << endl;
  outFile << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  printNice(firstCol0, matrix0, NULL, header, nrNuis, nrCols, 3, outFile);

  outFile << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  outFile << "%%  MU = 1            %%" << endl;
  outFile << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  printNice(firstCol1, matrix1, NULL, header, nrNuis, nrCols, 3, outFile);

  outFile << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  outFile << "%%  SUMMARY           %%" << endl;
  outFile << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  printNice(firstCol_all, matrix_all, NULL, header2, nrNuis, nrCols+1, 3, outFile);
  outFile << " LL value " << LLun << " delta LL0 " << LL0-LLun << " delta LL1 " << LL1-LLun << endl;  

  cout << endl;
  cout << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  cout << "%%  UNCONDITIONAL MU  %%" << endl;
  cout << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  printNice(firstCol_un, matrix_un, NULL, header, nrNuis+1, nrCols, 3, cout);

  cout << endl;
  cout << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  cout << "%%  MU = 0            %%" << endl;
  cout << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  printNice(firstCol0, matrix0, NULL, header, nrNuis, nrCols, 3, cout);

  cout << endl;
  cout << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  cout << "%%  MU = 1            %%" << endl;
  cout << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  printNice(firstCol1, matrix1, NULL, header, nrNuis, nrCols, 3, cout);

  cout << endl;
  cout << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  cout << "%%  SUMMARY           %%" << endl;
  cout << "%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  printNice(firstCol_all, matrix_all, NULL, header2, nrNuis, nrCols+1, 3, cout);

  cout << " LL value " << LLun << " delta LL0 " << LL0-LLun << " delta LL1 " << LL1-LLun << endl;  
}

void makeAsimovData(ModelConfig* mcInWs, bool doConditional, RooWorkspace* combWS, RooAbsPdf* combPdf, RooDataSet* combData, bool b_only, string snapshotName, int profileMode)
{
  ////////////////////
  //make asimov data//
  ////////////////////

  stringstream muStr;
  muStr << "_" << !b_only;
  if (profileMode == 1)
  {
    muStr << "_paz";
  }
  else if (profileMode == 2)
  {
    muStr << "_pamh";
  }

  RooRealVar* mu = combWS->var("mu");
  mu->setVal(!b_only);

  RooArgSet mc_obs = *mcInWs->GetObservables();
  RooArgSet mc_globs = *mcInWs->GetGlobalObservables();
  RooArgSet mc_nuis = *mcInWs->GetNuisanceParameters();

  // pair the nuisance parameter to the global observable
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

    pdf->Print();

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

    // RooRealVar* thisNui = (RooRealVar*)pdf->getObservables();

    // need this incase the observable isn't fundamental.
    // in this case, see which variable is dependent on the nuisance parameter and use that.
    RooArgSet* components = pdf->getComponents();
    components->Print();
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
      return;
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
      // return;
      continue;
    }

    cout << "Pairing nui: " << thisNui->GetName() << ", with glob: " << thisGlob->GetName() << ", from constraint: " << pdf->GetName() << endl;

    if (string(pdf->ClassName()) == "RooGaussian")
    {
      if ((thisNui->getMin() == -7 && thisNui->getMax() == 7) || (thisNui->getMin() == -5 && thisNui->getMax() == 5))
      {
        thisNui->setRange(-5, 5);
      }
    }

    nui_list.add(*thisNui);
    glob_list.add(*thisGlob);
    thisNui->Print();
    thisGlob->Print();
  }
  delete cIter;

  // save the snapshots of nominal parameters
  combWS->saveSnapshot("nominalGlobs",glob_list);
  // combWS->saveSnapshot("nominalNuis", nui_list);

  RooArgSet nuiSet_tmp(nui_list);

  mu->setVal(!b_only);
  // if (b_only) mu->setVal(pow(10., -5));
  mu->setConstant(1);

  // do a fit to set the nuis to the conditional MLEs

  if (doConditional)
  {
    combWS->loadSnapshot(snapshotName.c_str());

    if (profileMode == 0) // p at !bonly
    {
      mu->setVal(!b_only);
    }
    else if (profileMode == 1)
    {
      mu->setVal(0);
    }
    else if (profileMode == 2)
    {
      mu->setConstant(0);
    }
    else
    {
      cout << "ERROR::Unrecognized profile mode: " << profileMode << endl;
      exit(1);
    }

    // minimize(_nll);

    mu->setVal(!b_only);
    mu->setConstant(1);
  }
  mu->setConstant(0);

  // loop over the nui/glob list, grab the corresponding variable from the tmp ws, and set the glob to the value of the nui
  int nrNuis = nui_list.getSize();
  if (nrNuis != glob_list.getSize())
  {
    cout << "ERROR::nui_list.getSize() != glob_list.getSize()!" << endl;
    return;
  }

  for (int i=0;i<nrNuis;i++)
  {
    RooRealVar* nui = (RooRealVar*)nui_list.at(i);
    RooRealVar* glob = (RooRealVar*)glob_list.at(i);

    cout << "nui: " << nui << ", glob: " << glob << endl;
    cout << "Setting glob: " << glob->GetName() << ", which had previous val: " << glob->getVal() << ", to conditional val: " << nui->getVal() << endl;

    glob->setVal(nui->getVal());
  }

  // save the snapshots of conditional parameters
  cout << "Saving conditional snapshots" << endl;
  combWS->saveSnapshot(("conditionalGlobs"+muStr.str()).c_str(),glob_list);
  combWS->saveSnapshot(("conditionalNuis" +muStr.str()).c_str(), mc_nuis);

  if (!doConditional)
  {
    combWS->loadSnapshot("nominalGlobs");
    combWS->loadSnapshot("nominalNuis");
  }

  cout << "Making asimov" << endl;
  // make the asimov data (snipped from Kyle)
  mu->setVal(!b_only);
  ModelConfig* mc = mcInWs;

  int iFrame=0;

  const char* weightName="weightVar";
  RooArgSet obsAndWeight;
  cout << "adding obs" << endl;
  obsAndWeight.add(*mc->GetObservables());
  cout << "adding weight" << endl;

  RooRealVar* weightVar = NULL;
  if (!(weightVar = combWS->var(weightName)))
  {
    combWS->import(*new RooRealVar(weightName, weightName, 1,0,1000000), Silence());
    weightVar = combWS->var(weightName);
  }
  cout << "weightVar: " << weightVar << endl;
  obsAndWeight.add(*combWS->var(weightName));

  cout << "defining set" << endl;
  combWS->defineSet("obsAndWeight",obsAndWeight);

  //////////////////////////////////////////////////////
  //////////////////////////////////////////////////////
  //////////////////////////////////////////////////////
  //////////////////////////////////////////////////////
  //////////////////////////////////////////////////////
  // MAKE ASIMOV DATA FOR OBSERVABLES

  // dummy var can just have one bin since it's a dummy
  if(combWS->var("ATLAS_dummyX"))  combWS->var("ATLAS_dummyX")->setBins(1);

  cout <<" check expectedData by category"<<endl;
  // RooDataSet* simData=NULL;
  RooSimultaneous* simPdf = dynamic_cast<RooSimultaneous*>(mc->GetPdf());

  map<string, RooDataSet*> asimovDataMap;
  int _printLevel = 0;

  // try fix for sim pdf
  RooCategory* channelCat = (RooCategory*)combWS->cat("master_channel"); // (RooCategory*) (&simPdf->indexCat());
  // TIterator* iter = simPdf->indexCat().typeIterator() ;
  TIterator* iter = channelCat->typeIterator() ;
  RooCatType* tt = NULL;
  int nrIndices = 0;
  while((tt=(RooCatType*) iter->Next()))
  {
    nrIndices++;
  }

  TList* dataList = combData->split(*channelCat);
  int nrData = dataList->GetEntries();

  for (int i=0;i<nrIndices;i++){
    channelCat->setIndex(i);
    iFrame++;
    // Get pdf associated with state from simpdf
    RooAbsPdf* pdftmp = simPdf->getPdf(channelCat->getLabel()) ;

    RooDataSet* obsds = NULL;
    for (int j=0;j<nrData;j++)
    {
      RooDataSet* ds = (RooDataSet*)dataList->At(j);
      if (string(ds->GetName()) == string(channelCat->getLabel()))
      {
        obsds = ds;
        break;
      }
    }

    // Generate observables defined by the pdf associated with this state
    RooArgSet* obstmp = pdftmp->getObservables(*mc->GetObservables()) ;

    if (_printLevel >= 1)
    {
      obstmp->Print();
    }
    cout << "on type " << channelCat->getLabel() << " " << iFrame << endl;

    RooDataSet* obsDataUnbinned = new RooDataSet(Form("combAsimovData%d",iFrame),Form("combAsimovData%d",iFrame),RooArgSet(obsAndWeight,*channelCat),WeightVar(*weightVar));
    RooRealVar* thisObs = ((RooRealVar*)obstmp->first());
    double mu_save = mu->getVal();
    mu->setVal(1);
    double expectedSB = pdftmp->expectedEvents(*obstmp);
    mu->setVal(0);
    double expectedB = pdftmp->expectedEvents(*obstmp);
    mu->setVal(mu_save);
    double expectedEvents = pdftmp->expectedEvents(*obstmp);
    double thisNorm = 0;
    cout << "nr bins = " << thisObs->numBins() << endl;
    for(int jj=0; jj<thisObs->numBins(); ++jj){
      thisObs->setBin(jj);
      // cout << "obs bin = " << jj << ", obs val = " << thisObs->getVal() << endl;
      obsds->get(jj);
      // cout << "INFO::Observed events in bin " << jj << " = " << obsds->weight() << endl;

      mu->setVal(1);
      double SB = expectedSB*pdftmp->getVal(obstmp)*thisObs->getBinWidth(jj);
      mu->setVal(0);
      double B = expectedB*pdftmp->getVal(obstmp)*thisObs->getBinWidth(jj);
      double S = SB-B;
      mu->setVal(mu_save);
      thisNorm=pdftmp->getVal(obstmp)*thisObs->getBinWidth(jj);

      if ((B < 0 && S > 0) || S/B > 5)
      {
        cout << "WARNING::WALL::Bin " << jj << " of obs " << thisObs->GetName() << " (val=" << thisObs->getVal() << ") " << " has S=" << S << " and B=" << B << " -> muhat > " << -B/S << endl;
      }

      // cout << "thisNorm*expectedEvents: " << thisNorm*expectedEvents << endl;
      if (thisNorm*expectedEvents <= 0)
      {
        cout << "_WARNING_::Expected events <= 0 for bin " << jj << " of observable " << thisObs->GetName() << " in category " << channelCat->getLabel() << ": " << thisNorm*expectedEvents << endl;

        if (!obsds)
        {
          cout << "ERROR::Couldn't isolate ds" << endl;
          exit(1);
        }
        double nrObsEntries = obsds->weight();
        if (nrObsEntries != 0)
        {
          cout << "ERROR::Expected events <= 0 for bin " << jj << " of observable " << thisObs->GetName() << " in category " << channelCat->getLabel() << ": " << thisNorm*expectedEvents << " but obs entries = " << nrObsEntries << endl;
        }
      }

      if (thisNorm*expectedEvents > 0 && thisNorm*expectedEvents < pow(10., 9))
      {
        // cout << "INFO::Adding events for bin " << jj << " of observable " << thisObs->GetName() << " in cat " << channelCat->getLabel() << ": " << thisNorm*expectedEvents << endl;
        obsDataUnbinned->add(*mc->GetObservables(), thisNorm*expectedEvents);
      }
    }

    if (_printLevel >= 1)
    {
      obsDataUnbinned->Print();
      cout <<"sum entries "<<obsDataUnbinned->sumEntries()<<endl;
    }
    if(obsDataUnbinned->sumEntries()!=obsDataUnbinned->sumEntries())
    {
      cout << "sum entries is nan"<<endl;
      exit(1);
    }

    ((RooRealVar*)obstmp->first())->Print();
    cout << "expected events " << pdftmp->expectedEvents(*obstmp) << endl;

    asimovDataMap[string(channelCat->getLabel())] = obsDataUnbinned;//tempData;

    if (_printLevel >= 1)
    {
      cout << "channel: " << channelCat->getLabel() << ", data: ";
      obsDataUnbinned->Print();
      cout << endl;
    }
  }

  RooDataSet* asimovData = new RooDataSet(("asimovData"+muStr.str()).c_str(),("asimovData"+muStr.str()).c_str(),RooArgSet(obsAndWeight,*channelCat),Index(*channelCat),Import(asimovDataMap),WeightVar(*weightVar), Silence());
  combWS->import(*asimovData, Silence());

  // bring us back to nominal for exporting
  combWS->loadSnapshot("nominalNuis");
  combWS->loadSnapshot("nominalGlobs");
}

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
    if (constraint_set.getSize() > 1)
    {
      // string className(pdf->ClassName());
      // if (className != "RooGaussian" && className != "RooLognormal" && className != "RooGamma" && className != "RooPoisson" && className != "RooBifurGauss")
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
