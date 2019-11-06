#include "RooDataSet.h"
#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooSimultaneous.h"
#include "RooCategory.h"
#include "RooProdPdf.h"
#include "RooRealVar.h"
#include "RooStats/HistFactory/FlexibleInterpVar.h"
#include "RooStats/HistFactory/PiecewiseInterpolation.h"
#include "RooStats/HistFactory/Measurement.h"
#include "RooStats/HistFactory/MakeModelAndMeasurementsFast.h"

#include "TFile.h"

#include <iostream>
#include <set>
#include <utility>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>

#include "macros/makeAsimovData.C"

using namespace std;
using namespace RooFit;
using namespace RooStats;
using namespace HistFactory;

// void unfoldConstraints(RooArgSet& initial, RooArgSet& final, RooArgSet& obs, RooArgSet& nuis, int& counter);
void setup(ModelConfig* mcInWs);

void splitws() {

}

void splitws(string inFolderName, double mass, string channel) {
  cout << "Splitting workspace in " << channel << endl;

  int flatInterpCode = 4;
  int shapeInterpCode = 4;

  bool do2011 = 0;

  if (inFolderName.find("2011") != string::npos) do2011 = 1;

  bool conditionalAsimov = 0;
  bool doData = 1;
  if (inFolderName.find("_blind_") != string::npos) {
    conditionalAsimov = 0;
  }
  else {
    conditionalAsimov = 1;
  }

  set<string> channelNames;

  if (channel == "01j") {
    channelNames.insert("em_signalLike1_0j"+string(!do2011?"_2012":""));
    channelNames.insert("em_signalLike2_0j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike1_0j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike2_0j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_AfrecSR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_ASR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_AfrecSR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_ASR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CfrecZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CfrecZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_mainControl_0j"+string(!do2011?"_2012":""));

    channelNames.insert("em_signalLike1_1j"+string(!do2011?"_2012":""));
    channelNames.insert("em_signalLike2_1j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike1_1j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike2_1j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_AfrecSR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_ASR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_AfrecSR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_ASR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CfrecZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CfrecZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_mainControl_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_topbox_1j"+string(!do2011?"_2012":""));
  }
  else if (channel == "0j") {
    channelNames.insert("em_signalLike1_0j"+string(!do2011?"_2012":""));
    channelNames.insert("em_signalLike2_0j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike1_0j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike2_0j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_AfrecSR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_ASR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_AfrecSR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_ASR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CfrecZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CfrecZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_mainControl_0j"+string(!do2011?"_2012":""));
  }
  else if (channel == "1j") {
    channelNames.insert("em_signalLike1_1j"+string(!do2011?"_2012":""));
    channelNames.insert("em_signalLike2_1j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike1_1j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike2_1j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_AfrecSR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_ASR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_AfrecSR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_ASR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CfrecZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CfrecZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_mainControl_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_topbox_1j"+string(!do2011?"_2012":""));
  }
  else if (channel == "OF01j") {
    channelNames.insert("em_signalLike1_0j"+string(!do2011?"_2012":""));
    channelNames.insert("em_signalLike2_0j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike1_0j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike2_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_mainControl_0j"+string(!do2011?"_2012":""));

    channelNames.insert("em_signalLike1_1j"+string(!do2011?"_2012":""));
    channelNames.insert("em_signalLike2_1j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike1_1j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike2_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_mainControl_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_topbox_1j"+string(!do2011?"_2012":""));
  }
  else if (channel == "OF0j") {
    channelNames.insert("em_signalLike1_0j"+string(!do2011?"_2012":""));
    channelNames.insert("em_signalLike2_0j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike1_0j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike2_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_mainControl_0j"+string(!do2011?"_2012":""));
  }
  else if (channel == "OF1j") {
    channelNames.insert("em_signalLike1_1j"+string(!do2011?"_2012":""));
    channelNames.insert("em_signalLike2_1j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike1_1j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike2_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_mainControl_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_topbox_1j"+string(!do2011?"_2012":""));
  }
  else if (channel == "SF01j") {
    channelNames.insert("SF_AfrecSR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_ASR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_AfrecSR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_ASR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CfrecZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CfrecZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_mainControl_0j"+string(!do2011?"_2012":""));

    channelNames.insert("SF_AfrecSR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_ASR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_AfrecSR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_ASR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CfrecZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CfrecZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_mainControl_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_topbox_1j"+string(!do2011?"_2012":""));
  }
  else if (channel == "SF0j") {
    channelNames.insert("SF_AfrecSR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_ASR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_AfrecSR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_ASR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CfrecZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CfrecZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_mainControl_0j"+string(!do2011?"_2012":""));
  }
  else if (channel == "SF1j") {
    channelNames.insert("SF_AfrecSR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_ASR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_AfrecSR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_ASR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CfrecZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CfrecZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_mainControl_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_topbox_1j"+string(!do2011?"_2012":""));
  }
  else if (channel == "2j") {
    channelNames.insert("em_signalLike1_2j"+string(!do2011?"_2012":""));
    channelNames.insert("ee_signalLike1_2j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_topbox_2j"+string(!do2011?"_2012":""));
  }
  else if (channel == "OF2j") {
    channelNames.insert("em_signalLike1_2j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_topbox_2j"+string(!do2011?"_2012":""));
  }
  else if (channel == "SF2j") {
    channelNames.insert("ee_signalLike1_2j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_topbox_2j"+string(!do2011?"_2012":""));
  }
  else if (channel == "OF") {
    channelNames.insert("em_signalLike1_0j"+string(!do2011?"_2012":""));
    channelNames.insert("em_signalLike2_0j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike1_0j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike2_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_mainControl_0j"+string(!do2011?"_2012":""));

    channelNames.insert("em_signalLike1_1j"+string(!do2011?"_2012":""));
    channelNames.insert("em_signalLike2_1j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike1_1j"+string(!do2011?"_2012":""));
    channelNames.insert("me_signalLike2_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_mainControl_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_topbox_1j"+string(!do2011?"_2012":""));

    channelNames.insert("em_signalLike1_2j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_topbox_2j"+string(!do2011?"_2012":""));
  }
  else if (channel == "SF") {
    channelNames.insert("SF_AfrecSR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_ASR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_AfrecSR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_ASR_0j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CfrecZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CfrecZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CZpeak_0j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_mainControl_0j"+string(!do2011?"_2012":""));

    channelNames.insert("SF_AfrecSR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_ASR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_AfrecSR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_ASR_1j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CfrecZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_CZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CfrecZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_CZpeak_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_mainControl_1j"+string(!do2011?"_2012":""));
    channelNames.insert("OF_topbox_1j"+string(!do2011?"_2012":""));

    channelNames.insert("ee_signalLike1_2j"+string(!do2011?"_2012":""));
    channelNames.insert("SF_topbox_2j"+string(!do2011?"_2012":""));
  }
  else {
    cout << "Channel " << channel << " not defined. Please check!" << endl;
    exit(1);
  }

  // bool fix = 1;
  stringstream inFileName;

  inFileName << "workspaces/" << inFolderName << "/" << mass << ".root";
  TFile f(inFileName.str().c_str());
  
  RooWorkspace* w = (RooWorkspace*)f.Get("combWS");
  if (!w) w = (RooWorkspace*)f.Get("combined");
  
  RooDataSet* data = (RooDataSet*)w->data("combData");
  if (!data) data = (RooDataSet*)w->data("obsData");
  
  ModelConfig* mc = (ModelConfig*)w->obj("ModelConfig");
  
  RooRealVar* weightVar = w->var("weightVar");
  
  RooRealVar* mu = (RooRealVar*)mc->GetParametersOfInterest()->first();
  if (!mu) mu = w->var("SigXsecOverSM");

  const RooArgSet* mc_obs = mc->GetObservables();
  const RooArgSet* mc_nuis = mc->GetNuisanceParameters();
  const RooArgSet* mc_globs = mc->GetGlobalObservables();
  const RooArgSet* mc_poi = mc->GetParametersOfInterest();

  RooArgSet nuis = *mc_nuis;
  RooArgSet antiNuis = *mc_nuis;

  RooArgSet globs = *mc_globs;
  RooArgSet antiGlobs = *mc_globs;

  RooArgSet allParams;

  RooSimultaneous* simPdf = (RooSimultaneous*)mc->GetPdf();
  RooCategory* cat = (RooCategory*)&simPdf->indexCat();

  RooArgSet nuis_tmp = nuis;
  RooArgSet fullConstraints = *simPdf->getAllConstraints(*mc_obs,nuis_tmp,false);

  vector<string> foundChannels;
  vector<string> skippedChannels;  

  cout << "Getting constraints" << endl;
  map<string, RooDataSet*> data_map;
  map<string, RooAbsPdf*> pdf_map;
  RooCategory* decCat = new RooCategory("dec_channel","dec_channel");
  // int i = 0;
  TIterator* catItr = cat->typeIterator();
  RooCatType* type;
  RooArgSet allConstraints;
  while ((type = (RooCatType*)catItr->Next())) {
    RooAbsPdf* pdf =  simPdf->getPdf(type->GetName());

    string typeName(type->GetName());
    if (channelNames.size() && channelNames.find(typeName) == channelNames.end())  {
      skippedChannels.push_back(typeName);
      continue;
    }
    cout << "On channel " << type->GetName() << endl;
    foundChannels.push_back(typeName);

    decCat->defineType(type->GetName());
    // pdf->getParameters(*data)->Print("v");

    RooArgSet nuis_tmp1 = nuis;
    RooArgSet nuis_tmp2 = nuis;
    RooArgSet* constraints = pdf->getAllConstraints(*mc_obs, nuis_tmp1, true);
    constraints->Print();
    allConstraints.add(*constraints);
  }

  catItr->Reset();

  while ((type = (RooCatType*)catItr->Next())) {
    RooAbsPdf* pdf =  simPdf->getPdf(type->GetName());

    string typeName(type->GetName());
    cout << "Considering type " << typeName << endl;
    if (channelNames.size() && channelNames.find(typeName) == channelNames.end()) continue;
    cout << "On channel " << type->GetName() << endl;

    RooArgSet nuis_tmp1 = nuis;
    RooArgSet nuis_tmp2 = nuis;
    RooArgSet* constraints = pdf->getAllConstraints(*mc_obs, nuis_tmp1, true);

    cout << "Adding pdf to map: " << typeName << " = " << pdf->GetName() << endl;
    pdf_map[typeName] = pdf;

    RooProdPdf prod("prod","prod",*constraints);

    RooArgSet* params = pdf->getParameters(*data);
    antiNuis.remove(*params);
    antiGlobs.remove(*params);

    allParams.add(*params);
    // cout << type->GetName() << endl;
  }
  // return;

  RooArgSet decNuis;
  TIterator* nuiItr = mc_nuis->createIterator();
  TIterator* parItr = allParams.createIterator();
  RooAbsArg* nui, *par;
  while ((par = (RooAbsArg*)parItr->Next())) {
    nuiItr->Reset();
    while ((nui = (RooAbsArg*)nuiItr->Next())) {
      if (par == nui) decNuis.add(*nui);
    }
  }

  RooArgSet decGlobs;
  TIterator* globItr = mc_globs->createIterator();
  parItr->Reset();
  RooAbsArg* glob;
  while ((par = (RooAbsArg*)parItr->Next())) {
    globItr->Reset();
    while ((glob = (RooAbsArg*)globItr->Next())) {
      if (par == glob) decGlobs.add(*glob);
    }
  }

  // antiNuis.Print();

  // nuis.Print();
  // globs.Print();

  // i = 0;
  TList* datalist = data->split(*cat, true);
  TIterator* dataItr = datalist->MakeIterator();
  RooAbsData* ds;
  while ((ds = (RooAbsData*)dataItr->Next())) {
    string typeName(ds->GetName());
    if (channelNames.size() && channelNames.find(typeName) == channelNames.end()) continue;

    cout << "Adding dataset to map: " << ds->GetName() << endl;
    data_map[string(ds->GetName())] = (RooDataSet*)ds;

    cout << ds->GetName() << endl;
  }

  RooSimultaneous* decPdf = new RooSimultaneous("decPdf","decPdf",pdf_map,*decCat); 
  RooArgSet decObs = *decPdf->getObservables(data);
  // decObs.add(*(RooAbsArg*)weightVar);
  decObs.add(*(RooAbsArg*)decCat);
  decObs.Print();

  nuis.remove(antiNuis);
  globs.remove(antiGlobs);
  // nuis.Print("v");

  RooDataSet* decData = new RooDataSet("obsData","obsData",RooArgSet(decObs,*(RooAbsArg*)weightVar),Index(*decCat),Import(data_map),WeightVar(*weightVar));

  decData->Print();

  RooArgSet poi(*(RooAbsArg*)mu);
  RooWorkspace decWS("combined");
  ModelConfig decMC("ModelConfig",&decWS);
  decMC.SetPdf(*decPdf);
  decMC.SetObservables(decObs);
  decMC.SetNuisanceParameters(decNuis);
  decMC.SetGlobalObservables(decGlobs);
  decMC.SetParametersOfInterest(poi);

  decMC.Print();
  decWS.import(*decPdf);
  decWS.import(decMC);
  decWS.import(*decData);
  // decWS.Print();

  ModelConfig* mcInWs = (ModelConfig*)decWS.obj("ModelConfig");
  decPdf = (RooSimultaneous*)mcInWs->GetPdf();

  // setup(mcInWs);
  // return;

  mcInWs->GetNuisanceParameters()->Print("v");
  mcInWs->GetGlobalObservables()->Print("v");
  // decData->tree()->Scan("*");

  // Make asimov data
  RooArgSet funcs = decWS.allFunctions();
  TIterator* it = funcs.createIterator();
  TObject* tempObj = 0;
  while((tempObj=it->Next()))
  {
    FlexibleInterpVar* flex = dynamic_cast<FlexibleInterpVar*>(tempObj);
    if(flex) {
      flex->setAllInterpCodes(flatInterpCode);
    }
    PiecewiseInterpolation* piece = dynamic_cast<PiecewiseInterpolation*>(tempObj);
    if(piece) {
      piece->setAllInterpCodes(shapeInterpCode);
    }
  }

  RooDataSet* dataInWs = (RooDataSet*)decWS.data("obsData");
  makeAsimovData(mcInWs, conditionalAsimov && doData, &decWS, mcInWs->GetPdf(), dataInWs, 0);
  makeAsimovData(mcInWs, conditionalAsimov && doData, &decWS, mcInWs->GetPdf(), dataInWs, 1);
  makeAsimovData(mcInWs, conditionalAsimov && doData, &decWS, mcInWs->GetPdf(), dataInWs, 2);

  system(("mkdir -vp workspaces/"+inFolderName+"_"+channel).c_str());
  stringstream outFileName;
  outFileName << "workspaces/" << inFolderName << "_" << channel << "/" << mass << ".root";
  cout << "Exporting" << endl;

  decWS.writeToFile(outFileName.str().c_str());

  cout << "\nIncluded the following channels: " << endl;
  for (int i=0;i<(int)foundChannels.size();i++) {
    cout << "-> " << foundChannels[i] << endl;
  }

  cout << "\nSkipping the following channels: " << endl;
  
  for (int i=0;i<(int)skippedChannels.size();i++) {
    cout << "-> " << skippedChannels[i] << endl;
  }

  cout << "Done" << endl;

  // decPdf->fitTo(*decData, Hesse(0), Minos(0), PrintLevel(0));
}

void setup(ModelConfig* mcInWs) {
  RooAbsPdf* combPdf = mcInWs->GetPdf();

  RooArgSet mc_obs = *mcInWs->GetObservables();
  RooArgSet mc_globs = *mcInWs->GetGlobalObservables();
  RooArgSet mc_nuis = *mcInWs->GetNuisanceParameters();

  // pair the nuisance parameter to the global observable
  RooArgSet mc_nuis_tmp = mc_nuis;
  RooArgList nui_list;
  RooArgList glob_list;
  RooArgSet constraint_set_tmp(*combPdf->getAllConstraints(mc_obs, mc_nuis_tmp, false));
  RooArgSet constraint_set;
  int counter_tmp = 0;
  unfoldConstraints(constraint_set_tmp, constraint_set, mc_obs, mc_nuis_tmp, counter_tmp);

  TIterator* cIter = constraint_set.createIterator();
  RooAbsArg* arg;
  while ((arg = (RooAbsArg*)cIter->Next())) {
    RooAbsPdf* pdf = (RooAbsPdf*)arg;
    if (!pdf) continue;

    // pdf->Print();

    TIterator* nIter = mc_nuis.createIterator();
    RooRealVar* thisNui = NULL;
    RooAbsArg* nui_arg;
    while ((nui_arg = (RooAbsArg*)nIter->Next())) {
      if (pdf->dependsOn(*nui_arg)) {
        thisNui = (RooRealVar*)nui_arg;
        break;
      }
    }
    delete nIter;

    // need this incase the observable isn't fundamental. 
    // in this case, see which variable is dependent on the nuisance parameter and use that.
    RooArgSet* components = pdf->getComponents();
    // components->Print();
    components->remove(*pdf);
    if (components->getSize()) {
      TIterator* itr1 = components->createIterator();
      RooAbsArg* arg1;
      while ((arg1 = (RooAbsArg*)itr1->Next())) {
        TIterator* itr2 = components->createIterator();
        RooAbsArg* arg2;
        while ((arg2 = (RooAbsArg*)itr2->Next())) {
          if (arg1 == arg2) continue;
          if (arg2->dependsOn(*arg1)) {
            components->remove(*arg1);
          }
        }
        delete itr2;
      }
      delete itr1;
    }

    if (components->getSize() > 1) {
      cout << "ERROR::Couldn't isolate proper nuisance parameter" << endl;
      return;
    }
    else if (components->getSize() == 1) {
      thisNui = (RooRealVar*)components->first();
    }

    TIterator* gIter = mc_globs.createIterator();
    RooRealVar* thisGlob = NULL;
    RooAbsArg* glob_arg;
    while ((glob_arg = (RooAbsArg*)gIter->Next())) {
      if (pdf->dependsOn(*glob_arg)) {
        thisGlob = (RooRealVar*)glob_arg;
        break;
      }
    }
    delete gIter;

    if (!thisNui || !thisGlob) {
      cout << "WARNING::Couldn't find nui or glob for constraint: " << pdf->GetName() << endl;
      //return;
      continue;
    }

    // cout << "Pairing nui: " << thisNui->GetName() << ", with glob: " << thisGlob->GetName() << ", from constraint: " << pdf->GetName() << endl;

    nui_list.add(*thisNui);
    glob_list.add(*thisGlob);

    if (string(pdf->ClassName()) == "RooPoisson")  {
      double minVal = max(0.0, thisGlob->getVal() - 8*sqrt(thisGlob->getVal()));
      double maxVal = max(10.0, thisGlob->getVal() + 8*sqrt(thisGlob->getVal()));
      thisNui->setRange(minVal, maxVal);
      thisGlob->setRange(minVal, maxVal);
    }
    else if (string(pdf->ClassName()) == "RooGaussian") {
      thisNui->setRange(-7, 7);
      thisGlob->setRange(-10, 10);
    }

    // thisNui->Print();
    // thisGlob->Print();
  }
  delete cIter;

}
