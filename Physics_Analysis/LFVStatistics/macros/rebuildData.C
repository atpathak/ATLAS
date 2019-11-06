#include "TFile.h"
#include "TTree.h"

#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooDataSet.h"
#include "RooRealVar.h"

#include <sstream>
#include <string>
#include <iostream>

#include "macros/makeAsimovData.C"

using namespace std;
using namespace RooFit;
using namespace RooStats;

bool match(string s1, string s2);
string getStringNoNumbers(string s, int& nRemoved);
RooDataSet* rebuildData(RooWorkspace* this_w, RooWorkspace* w, RooDataSet* data, string name)
{
  ModelConfig* mc = (ModelConfig*)w->obj("ModelConfig");
  const RooArgSet* globs = mc->GetGlobalObservables();
  const RooArgSet* nuis = mc->GetNuisanceParameters();
  RooArgSet obs_no_cat = *mc->GetObservables();

  if (string(mc->GetPdf()->ClassName()) == "RooSimultaneous")
  {
    RooSimultaneous* simPdf = (RooSimultaneous*)mc->GetPdf();
    RooCategory* cat = (RooCategory*)&simPdf->indexCat();
    obs_no_cat.remove(*cat);
  }

  RooRealVar* weightVar = new RooRealVar("weightVar","weightVar",1);



  ModelConfig* this_mc = (ModelConfig*)this_w->obj("ModelConfig");
  RooArgSet these_globs = *this_mc->GetGlobalObservables();
  RooArgSet these_nuis = *this_mc->GetNuisanceParameters();
  RooArgSet these_obs = *this_mc->GetObservables();
  RooArgSet these_obs_no_cat = these_obs;
  

  if (string(this_mc->GetPdf()->ClassName()) == "RooSimultaneous")
  {
    RooSimultaneous* simPdf = (RooSimultaneous*)this_mc->GetPdf();
    RooCategory* cat = (RooCategory*)&simPdf->indexCat();
    these_obs_no_cat.remove(*cat);
  }

  //this_w->import(these_obs);

  these_globs = *globs;
  these_nuis = *nuis;


  data->Print("v");

  this_w->defineSet("injObs", obs_no_cat, true);
  const RooArgSet* inj_obs = this_w->set("injObs");
  inj_obs->Print("v");
  TIterator* inj_obs_itr = inj_obs->createIterator();
  RooRealVar* this_inj_obs;


  data->Print("v");

  TIterator* these_obs_itr = these_obs_no_cat.createIterator();
  RooRealVar* this_obs;
  RooSimultaneous* simPdf = (RooSimultaneous*)mc->GetPdf();
  RooCategory* cat = (RooCategory*)&simPdf->indexCat();
  TList* datalist = data->split(*cat, true);
  TIterator* dataItr = datalist->MakeIterator();

  while ((this_obs = (RooRealVar*)these_obs_itr->Next()))
  {
    inj_obs_itr->Reset();
    while ((this_inj_obs = (RooRealVar*)inj_obs_itr->Next()))
    {
      if (!(string(this_obs->GetName()).find("ATLAS_H_") != string::npos || string(this_obs->GetName()).find("ZvvH") != string::npos)) continue;;
      if (!match(string(this_obs->GetName()), string(this_inj_obs->GetName()))) continue; 
      if (string(this_obs->GetName()) == string(this_inj_obs->GetName())) continue;

      cout << "Changing observable name from " << this_inj_obs->GetName() << " to " << this_obs->GetName() << endl;
      /*bool status1 =*/ data->changeObservableName(this_inj_obs->GetName(), this_obs->GetName());
      //cout << "status1 = " << status1 << endl;
      //data->Print("v");
      if (string(mc->GetPdf()->ClassName()) == "RooSimultaneous")
      {
	RooAbsData* ds;
	dataItr->Reset();
	while ((ds = (RooAbsData*)dataItr->Next()))
	{
	  cout << "In ds: " << ds->GetName() << ", Changing observable name from " << this_inj_obs->GetName() << " to " << this_obs->GetName() << endl;
	  /*bool status =*/ ds->changeObservableName(this_inj_obs->GetName(), this_obs->GetName());
	  //cout << "status = " << status << endl;
	  //ds->Print("v");
	}
      }
    }
  }
  delete dataItr;
  delete these_obs_itr;




  map<string,RooDataSet*> dataMap;

  int nrDS = datalist->GetEntries();
  

  RooSimultaneous* this_simPdf = (RooSimultaneous*)this_mc->GetPdf();
  RooCategory* this_cat = (RooCategory*)&this_simPdf->indexCat();
  TIterator* catItr = this_cat->typeIterator();
  RooCatType* tt;
  while ((tt = (RooCatType*)catItr->Next()))
  {
    for (int i=0;i<nrDS;i++)
    {
      RooDataSet* ds = (RooDataSet*)datalist->At(i);
      bool isZvvHorZZ = string(ds->GetName()).find("ATLAS_H_") != string::npos || string(ds->GetName()).find("ZvvH") != string::npos;
      if (!match(string(ds->GetName()), string(tt->GetName())) && isZvvHorZZ) 
      {
	//cout << "-----> " << ds->GetName() << " doesn't match " << tt->GetName() << endl;
	continue;
      }
      
      if (string(ds->GetName()) != string(tt->GetName()) && !isZvvHorZZ) continue;
      

      cout << "Renaming ds " << ds->GetName() << " to " << tt->GetName() << endl;
      ds->SetName(tt->GetName());
      dataMap[tt->GetName()] = ds;
    }
  }
  delete datalist;

  cout << "\n\n----\n\n" << endl;
  RooDataSet* rebuilt_data = new RooDataSet(name.c_str(),name.c_str(),RooArgSet(these_obs, *weightVar) ,Index(*this_cat),Import(dataMap),WeightVar(*weightVar));

//   if (string(this_mc->GetPdf()->ClassName()) == "RooSimultaneous")
//   {
//     map<string,RooDataSet*> dataMap;
//     RooSimultaneous* simPdf = (RooSimultaneous*)this_mc->GetPdf();
//     RooCategory* cat = (RooCategory*)&simPdf->indexCat();
//     cat->Print("v");
//     TList* datalist = data->split(*cat, true);
//     int nrDS = datalist->GetEntries();
//     for (int i=0;i<nrDS;i++)
//     {
//       cat->setIndex(i);
//       RooDataSet* ds = (RooDataSet*)datalist->At(i);
//       cout << "Renaming ds " << ds->GetName() << " to " << cat->getLabel() << endl;
//       ds->SetName(cat->getLabel());
//       //ds->Print();
//       dataMap[string(cat->getLabel())] = ds;
//     }

//     RooDataSet* rebuilt_data = new RooDataSet(name.c_str(),name.c_str(),RooArgSet(these_obs, *weightVar) ,Index(*cat),Import(dataMap),WeightVar(*weightVar));

//     delete data;
//     data = rebuilt_data;

//     delete datalist;
//   }
//   else
//   {
//     data->SetName(name.c_str());
//   }

  return rebuilt_data;
}


//match strings based on string minus the numbers in the string
bool match(string s1, string s2)
{
  bool is2011_1 = s1.find("2011") != string::npos;
  bool is2011_2 = s2.find("2011") != string::npos;
  bool is2012_1 = s1.find("2012") != string::npos;
  bool is2012_2 = s2.find("2012") != string::npos;
  int nRemoved1=0, nRemoved2=0;
  bool match1 = getStringNoNumbers(s1, nRemoved1) == getStringNoNumbers(s2, nRemoved2);
  bool match2 = is2011_1 == is2011_2 && is2012_1 == is2012_2;
  if (nRemoved1 < 3 || nRemoved2 < 3)
  {
    return s1 == s2 && match2;
  }
  else if (nRemoved1 >= 3 && nRemoved2 >= 3)
  {
    return match1 && match2;
  }
  else return false;
}

string getStringNoNumbers(string s, int& nRemoved)
{
  string s2 = "";
  int nrChars = s.size();
  vector<string> chars;
  for (int i=0;i<nrChars;i++)
  {
    if (s[i] == '0' ||
	s[i] == '1' ||
	s[i] == '2' ||
	s[i] == '3' ||
	s[i] == '4' ||
	s[i] == '5' ||
	s[i] == '6' ||
	s[i] == '7' ||
	s[i] == '8' ||
	s[i] == '9' ||
	s[i] == '.') 
    {
      nRemoved++;
      continue;
    }
    s2 += s[i];
  }
  return s2;
}
