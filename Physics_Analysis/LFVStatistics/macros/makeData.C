
#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooNLLVar.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "TStopwatch.h"
#include "RooSimultaneous.h"
#include "RooCategory.h"

#include "RooMinimizer.h"
#include "Math/MinimizerOptions.h"
//#include "Minuit2/Minuit2Minimizer.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>

using namespace std;
using namespace RooFit;
using namespace RooStats;



//put very small data entries in a binned dataset
RooDataSet* makeData(RooDataSet* orig, RooSimultaneous* simPdf, const RooArgSet* observables, RooRealVar* firstPOI, double mass, double& mu_min)
{

  double max_soverb = 0;

  mu_min = -10e9;


  map<string, RooDataSet*> data_map;
  firstPOI->setVal(0);
  RooCategory* cat = (RooCategory*)&simPdf->indexCat();
  TList* datalist = orig->split(*(RooAbsCategory*)cat, true);
  TIterator* dataItr = datalist->MakeIterator();
  RooAbsData* ds;
  RooRealVar* weightVar = new RooRealVar("weightVar","weightVar",1);
  while ((ds = (RooAbsData*)dataItr->Next()))
  {
    string typeName(ds->GetName());
    cat->setLabel(typeName.c_str());
    RooAbsPdf* pdf = simPdf->getPdf(typeName.c_str());
    cout << "pdf: " << pdf << endl;
    RooArgSet* obs = pdf->getObservables(observables);
    cout << "obs: " << obs << endl;

    RooArgSet obsAndWeight(*obs, *weightVar);
    obsAndWeight.add(*cat);
    stringstream datasetName;
    datasetName << "newData_" << typeName;
    RooDataSet* thisData = new RooDataSet(datasetName.str().c_str(),datasetName.str().c_str(), obsAndWeight, WeightVar(*weightVar));

    RooRealVar* firstObs = (RooRealVar*)obs->first();
    //int ibin = 0;
    int nrEntries = ds->numEntries();

    firstPOI->setVal(0);
    double expB=pdf->expectedEvents(*firstObs);
    firstPOI->setVal(1);
    double expSB=pdf->expectedEvents(*firstObs);


    firstPOI->setVal(0);
    vector<double> vec_b;
    for (int ib=0;ib<nrEntries;ib++)
    {
      const RooArgSet* event = ds->get(ib);
      const RooRealVar* thisObs = (RooRealVar*)event->find(firstObs->GetName());
      firstObs->setVal(thisObs->getVal());

      vec_b.push_back(expB*pdf->getVal(obs));
    }

    firstPOI->setVal(1);
    vector<double> vec_s;
    for (int ib=0;ib<nrEntries;ib++)
    {
      const RooArgSet* event = ds->get(ib);
      const RooRealVar* thisObs = (RooRealVar*)event->find(firstObs->GetName());
      firstObs->setVal(thisObs->getVal());

      vec_s.push_back(expSB*pdf->getVal(obs)-vec_b[ib]);
    }

    for (int ib=0;ib<nrEntries;ib++)
    {
      const RooArgSet* event = ds->get(ib);
      const RooRealVar* thisObs = (RooRealVar*)event->find(firstObs->GetName());
      firstObs->setVal(thisObs->getVal());

      //firstPOI->setVal(0);
      double b = vec_b[ib];//expB*pdf->getVal(obs);
      //firstPOI->setVal(1);
      double s = vec_s[ib];//expSB*pdf->getVal(obs) - b;
      //cout << "s = " << s << ", b = " << b << endl;

      if (s > 0)
      {
	mu_min = max(mu_min, -b/s);
	double soverb = s/b;
	if (soverb > max_soverb)
	{
	  max_soverb = soverb;
	  cout << "Found new max s/b: " << soverb << " in pdf " << pdf->GetName() << " at m = " << thisObs->getVal() << endl;
	}
      }

      
      
      //cout << "expected s = " << s << ", b = " << b << endl;
      //cout << "nexp = " << nexp << endl;
//       if (s < 0) 
//       {
// 	cout << "expecting negative s at m=" << firstObs->getVal() << endl;
// 	continue;
//       }
      if (b == 0 && s != 0)
      {
	cout << "Expecting non-zero signal and zero bg at m=" << firstObs->getVal() << " in pdf " << pdf->GetName() << endl;
      }
      if (s+b <= 0) 
      {
	cout << "expecting zero" << endl;
	continue;
      }


      double weight = ds->weight();
      if ((typeName.find("ATLAS_H_4mu") != string::npos || 
	   typeName.find("ATLAS_H_4e") != string::npos ||
	   typeName.find("ATLAS_H_2mu2e") != string::npos ||
	   typeName.find("ATLAS_H_2e2mu") != string::npos) && fabs(firstObs->getVal() - mass) < 10 && weight == 0)
      {
	cout << "adding event: " << firstObs->getVal() << endl;
	thisData->add(*event, pow(10., -9.));
      }
      else if (typeName.find("em_signalLike_2j") != string::npos && weight == 0)
      {
        cout << "adding event: " << firstObs->getVal() << endl;
        thisData->add(*event, pow(10., -9.));
      }
      else
      {
        thisData->add(*event, weight);
      }

      //else
      //{
	//weight = max(pow(10.0, -9), weight);
	//thisData->add(*event, weight);
      //}



    }



    data_map[string(ds->GetName())] = (RooDataSet*)thisData;
  }

  
  RooDataSet* newData = new RooDataSet("newData","newData",RooArgSet(*observables, *weightVar), 
				       Index(*cat), Import(data_map), WeightVar(*weightVar));

  orig->Print();
  newData->Print();
  //newData->tree()->Scan("*");
  return newData;

}
