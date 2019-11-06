#include <string>
#include <sstream>
#include <vector>

#include "TFile.h"
#include "Math/MinimizerOptions.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "TCanvas.h"
#include "RooPlot.h"
#include "RooHist.h"
#include "RooSimultaneous.h"
#include "RooCategory.h"
#include "TLine.h"

#include "macros/minimize.C"
#include "macros/drawPlot.C"

using namespace RooFit ;

void drawPlot_res(int mass = 125,
     string folder = "test",
     const char* wsName = "combined",
     const char* modelConfigName = "ModelConfig",
     const char* dataName = "obsData")
{
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

  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");
  ROOT::Math::MinimizerOptions::SetDefaultStrategy(0);
  ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(-1);
  RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);

  RooRealVar* firstPOI = (RooRealVar*)mc->GetParametersOfInterest()->first();
  firstPOI->setConstant(0);
  firstPOI->setRange(-5., 5.);

  RooSimultaneous* simPdf = (RooSimultaneous*)(mc->GetPdf());
  RooRealVar* obs = (RooRealVar*)mc->GetObservables()->first();  

  RooArgSet* allparams = simPdf->getParameters(*data);
  RooNLLVar* fNll = (RooNLLVar*)simPdf->createNLL(*data, CloneData(kFALSE), RooFit::Constrain(*mc->GetNuisanceParameters()),RooFit::GlobalObservables(*mc->GetGlobalObservables()));
  minimize(fNll);

  RooCategory* channelCat = (RooCategory*) (&simPdf->indexCat());
  TIterator* iter = channelCat->typeIterator() ;
  RooCatType* tt = NULL;
  while(tt=(RooCatType*) iter->Next())
  {
    string channelName(tt->GetName());
    cout << "on type " << tt->GetName() << " " << endl;
    channelCat->setLabel(tt->GetName());
    
    RooAbsPdf* pdftmp = simPdf->getPdf(tt->GetName()) ;
    RooArgSet* obstmp = pdftmp->getObservables(*mc->GetObservables()) ;
    obs = ((RooRealVar*)obstmp->first());

    RooPlot* frame1 = obs->frame();

    cout <<Form("%s==%s::%s",channelCat->GetName(),channelCat->GetName(),tt->GetName())<<endl;
    cout << tt->GetName() << " " << channelCat->getLabel() <<endl;
    data->plotOn(frame1,MarkerSize(1),Cut(Form("%s==%s::%s",channelCat->GetName(),channelCat->GetName(),tt->GetName())),DataError(RooAbsData::Poisson));
    // Double_t normCount = data->sumEntries(Form("%s==%s::%s",channelCat->GetName(),channelCat->GetName(),tt->GetName())) ;
    double normCount = pdftmp->expectedEvents(*obs);
    cout << "expected events ( ): " << normCount << endl;
    pdftmp->plotOn(frame1,Normalization(normCount,RooAbsReal::NumEvent)) ;
    //pdftmp->plotOn(frame1/*,Normalization(normCount,RooAbsReal::NumEvent)*/) ;

    cout << "chi^2 = " << frame1->chiSquare() << endl ;

    RooHist* hresid = frame1->residHist() ;
    RooHist* hpull = frame1->pullHist() ;
    RooPlot* frame2 = obs->frame(Title("Residual Distribution")) ;
    frame2->addPlotable(hresid,"P") ;
    RooPlot* frame3 = obs->frame(Title("Pull Distribution")) ;
    frame3->addPlotable(hpull,"P") ;


    TCanvas* c1 = new TCanvas("canvas","canvas",800,400) ;
    c1->Divide(2) ;
    c1->cd(1); frame1->Draw() ;
    c1->cd(2); frame2->Draw() ;

    TLine l;
    l.SetLineStyle(2);
    l.DrawLine(obs->getMin(),0,obs->getMax(),0);

    string saveName = Form("%s_%s",tt->GetName(),obs->GetName());
  
    save(saveName, "eps", c1);
    save(saveName, "pdf", c1);
    save(saveName, "C", c1);
  }  
}
