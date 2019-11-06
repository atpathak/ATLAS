#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooDataSet.h"
#include "RooMinimizerFcn.h"
#include "RooNLLVar.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"
#include "RooCategory.h"
#include "TFile.h"

#include "macros/minimize.C"

using namespace std;
using namespace RooFit;
using namespace RooStats;



void getSandB(const char* inFileName,
		  const char* wsName = "combined",
		  const char* modelConfigName = "ModelConfig",
		  const char* dataName = "obsData")
{
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
  if (!data)
  {
    cout << "ERROR::Dataset: " << dataName << " doesn't exist!" << endl;
    //return;
  }

  RooRealVar* mu = ws->var("SigXsecOverSM");
  RooRealVar* poi = (RooRealVar*)mc->GetParametersOfInterest()->first();

  RooArgSet nuis = *mc->GetNuisanceParameters();
  RooNLLVar* nll = (RooNLLVar*)mc->GetPdf()->createNLL(*data/*, Constrain(nuis)*/);

  mu->setConstant(0);
  poi->setConstant(0);

  //poi->setVal(1);

  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");
  ROOT::Math::MinimizerOptions::SetDefaultStrategy(0);
  ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(1);

  RooArgSet params(*mc->GetNuisanceParameters());
  params.add(*mu);
  params.add(*poi);

//   ws->var("ATLAS_norm_WW0j")->setVal(1.13);
//   ws->var("ATLAS_norm_WW1j")->setVal(0.84);
//   ws->var("ATLAS_norm_Top1j")->setVal(1.03);

  ws->loadSnapshot("nominalNuis");

  ws->saveSnapshot("nominalParams",params);
  //minimize(nll);
  double muhat = poi->getVal();
  ws->saveSnapshot("ucParams",params);
  ws->loadSnapshot("nominalParams");


  if (mu != poi)
  {

    RooSimultaneous* simPdf = (RooSimultaneous*)mc->GetPdf();
    if (!simPdf)
    {
      cout << "ERROR::PDF isn't a sim pdf" << endl;
      exit(1);
    }

    const RooArgSet* obs = mc->GetObservables();

    RooCategory* cat = (RooCategory*)&simPdf->indexCat();
    TIterator* catItr = cat->typeIterator();
    RooCatType* tt;
    vector<double> vec_S0,vec_S2,vec_B;
    vector<string> vec_names;
    while ((tt = (RooCatType*)catItr->Next()))
    {
      mu->setVal(1);
      poi->setVal(1);
      RooAbsPdf* pdf = simPdf->getPdf(tt->GetName());
      double s2PlusB = pdf->expectedEvents(obs);
      poi->setVal(0);
      double s0PlusB = pdf->expectedEvents(obs);

      mu->setVal(0);
      double B = pdf->expectedEvents(obs);
      double S0 = s0PlusB-B;
      double S2 = s2PlusB-B;
      vec_S0.push_back(S0);
      vec_S2.push_back(S2);
      vec_B.push_back(B);
      vec_names.push_back(tt->GetName());
    }
    cout << "----------------------------------" << endl;
    cout << setprecision(5);
    for (int i=0;i<(int)vec_S0.size();i++)
    {
      cout << "Channel " << vec_names[i] << " has S0=" << vec_S0[i] << ", S2=" << vec_S2[i] << ", and B=" << vec_B[i] << endl;
    }
  }
  else
  {
    for (int print=0;print<2;print++)
    {
      if (print == 1) 
      {
	ws->loadSnapshot("ucParams");
	cout << "\n%%%%%%%%%%%%% -- POSTFIT -- %%%%%%%%%%%%%%%" << endl;
      }
      else
      {
	cout << "\n%%%%%%%%%%%%% -- PREFIT  -- %%%%%%%%%%%%%%%" << endl;
      }
      RooSimultaneous* simPdf = (RooSimultaneous*)mc->GetPdf();
      if (!simPdf)
      {
	cout << "ERROR::PDF isn't a sim pdf" << endl;
	exit(1);
      }

      const RooArgSet* obs = mc->GetObservables();

      double S_SR = 0;
      double B_SR = 0;
      double D_SR = 0;

      RooCategory* cat = (RooCategory*)&simPdf->indexCat();

      TList* dataList = data->split(*cat);
      int nrData = dataList->GetEntries();


      TIterator* catItr = cat->typeIterator();
      RooCatType* tt;
      vector<double> vec_S, vec_B, vec_D;
      vector<string> vec_names;
      while ((tt = (RooCatType*)catItr->Next()))
      {
	RooDataSet* thisData = NULL;
	for (int id=0;id<nrData;id++)
	{
	  RooDataSet* subData = (RooDataSet*)dataList->At(id);
	  if (string(subData->GetName()).find(string(tt->GetName())) != string::npos)
	  {
	    thisData = subData;
	    break;
	  }
	}
	if (!thisData)
	{
	  cout << "Couldn't isolate data" << endl;
	  exit(1);
	}

	poi->setVal(1);
	RooAbsPdf* pdf = simPdf->getPdf(tt->GetName());
	double sPlusB = pdf->expectedEvents(obs);

	mu->setVal(0);
	double B = pdf->expectedEvents(obs);
	double S = sPlusB-B;
	double D = thisData->sumEntries();
	vec_S.push_back(S);
	vec_B.push_back(B);
	vec_D.push_back(D);
	vec_names.push_back(tt->GetName());

	if (string(tt->GetName()).find("signalLike") != string::npos)
	{
	  S_SR += S;
	  B_SR += B;
	  D_SR += D;
	}
      }

      cout << "----------------------------------" << endl;
      cout << setprecision(5);
      for (int i=0;i<(int)vec_S.size();i++)
      {
	cout << "Channel " << vec_names[i] << " has S=" << vec_S[i] << ", B=" << vec_B[i] << ", S+B=" << vec_S[i]+vec_B[i] << ", and D = " << vec_D[i] << endl;
      }
      cout << "S(SR) = " << S_SR << ", B(SR) = " << B_SR << ", D(SR) = " << D_SR << ", naive muhat = " << (D_SR - B_SR) / S_SR << endl;
    }
  }
  cout << "muhat = " << muhat << endl;
}
