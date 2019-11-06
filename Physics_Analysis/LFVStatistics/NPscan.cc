#include "TROOT.h"
#include "TStyle.h"
#include <string>
#include <vector>

#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"

#include "Math/MinimizerOptions.h"
#include "TStopwatch.h"

#include "TStyle.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TGraph.h"
#include "TLine.h"
#include "TH1.h"
#include "TLatex.h"
#include "TString.h"

#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooDataSet.h"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooNLLVar.h"
#include "RooFitResult.h"

//#include "macros/log.C"
//#include "macros/parseString.C"
//#include "macros/minimize.C"
//#include "macros/findSigma.C"

#include "log.h"
#include "parseString.h"
#include "minimize.h"
#include "findSigma.h"

using namespace std;
using namespace RooFit;
using namespace RooStats;

/*
// ____________________________________________________________________________|__________
void runPulls() {
    // for compiling only
}

// ____________________________________________________________________________|__________
// compute pulls of the nuisance parameters and store them in text files. norm and syst parameters will be split among different files
void runPulls(
*/
int main(int argc, char* argv[]){
  //
  // Argument variables
  //
  TString inFileName      = (argc>1) ? argv[1] : "";
  TString wsName          = (argc>2) ? argv[2] : "combined";
  TString modelConfigName = (argc>3) ? argv[3] : "ModelConfig";
  TString dataName        = (argc>4) ? argv[4] : "obsData";
  TString poiName         = (argc>5) ? argv[5] : "SigXsecOverSM";
  TString folder          = (argc>6) ? argv[6] : "test";
  TString variable        = (argc>7) ? argv[7] :  "";
  double  NPlow           = (argc>8) ? atof(argv[8]) : -3.0;
  double  NPhig           = (argc>9) ? atof(argv[9]) :  3.1;

  //  double  inival          = (argc>8) ? atof(argv[8]) : -999;
  //
  string thisdir  = "/afs/cern.ch/user/a/atpathak/afswork/public/LFVStat_16Aug2017";
  //
  double precision = 0.01;
  bool useMinos = 0;
  string loglevel = "DEBUG"; // "INFO"; // "DEBUG";
  //{
    TStopwatch timer_pulls;
    timer_pulls.Start();

    // DEBUG OUTPUT
    // - ERROR
    // - WARNING
    // - INFO
    // - DEBUG
    LOG::ReportingLevel() = LOG::FromString(loglevel);

    // some settings
    ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");
    ROOT::Math::MinimizerOptions::SetDefaultStrategy(1);
    if (loglevel == "DEBUG") {
        ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(1);
    } else {
        ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(-1);
        RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
    }
    // thanks to https://wiki.physik.uzh.ch/lhcb/root:controloutputroofit for this list
    RooMsgService::instance().getStream(0).removeTopic(RooFit::Eval);
    RooMsgService::instance().getStream(1).removeTopic(RooFit::Eval);
    RooMsgService::instance().getStream(1).removeTopic(RooFit::Generation);
    RooMsgService::instance().getStream(1).removeTopic(RooFit::Minimization);
    RooMsgService::instance().getStream(1).removeTopic(RooFit::Plotting);
    RooMsgService::instance().getStream(1).removeTopic(RooFit::Fitting);
    RooMsgService::instance().getStream(1).removeTopic(RooFit::Integration);
    RooMsgService::instance().getStream(1).removeTopic(RooFit::LinkStateMgmt);
    RooMsgService::instance().getStream(1).removeTopic(RooFit::Caching);
    RooMsgService::instance().getStream(1).removeTopic(RooFit::Optimization);
    RooMsgService::instance().getStream(1).removeTopic(RooFit::ObjectHandling);
    RooMsgService::instance().getStream(1).removeTopic(RooFit::InputArguments);
    RooMsgService::instance().getStream(1).removeTopic(RooFit::Tracing);
    RooMsgService::instance().getStream(1).removeTopic(RooFit::Contents);
    RooMsgService::instance().getStream(1).removeTopic(RooFit::DataHandling);
    RooMsgService::instance().getStream(1).removeTopic(RooFit::NumIntegration);
    //
    // loading the workspace etc.
    LOG(logINFO) << "Running over workspace: " << inFileName;
    system(("mkdir -vp " + thisdir + "/" + "root-files/" + string(folder) + "_scans").c_str());
    vector<string> parsed = parseString(poiName.Data(), ",");

    system(Form("/bin/ls -l %s",inFileName.Data()));
    //
    if (gSystem->AccessPathName(inFileName)) {
      LOG(logERROR) << "Filename: " << inFileName << " doesn't exist!";
      exit(1);
    };
    //
    TFile* file =  TFile::Open(inFileName);

    RooWorkspace* ws = (RooWorkspace*)file->Get(wsName);
    if (!ws) {
        LOG(logERROR) << "Workspace: " << wsName << " doesn't exist!";
        exit(1);
    }

    ModelConfig* mc = (ModelConfig*)ws->obj(modelConfigName);
    if (!mc) {
        LOG(logERROR) << "ModelConfig: " << modelConfigName << " doesn't exist!";
        exit(1);
    }

    RooDataSet* data = (RooDataSet*)ws->data(dataName);
    if (!data) {
        LOG(logERROR) << "Dataset: " << dataName << " doesn't exist!";
        exit(1);
    }

    vector<RooRealVar*> pois;
    for (int i = 0; i < parsed.size(); i++) {
        RooRealVar* poi = (RooRealVar*)ws->var(parsed[i].c_str());
        LOG(logINFO) << "Getting POI " << poi->GetName();
        if (!poi) {
            LOG(logERROR) << "POI: " << poi->GetName() << " doesn't exist!";
            exit(1);
        }
        poi->setVal(0);
        poi->setRange(-50.,50.);
        poi->setConstant(0);
        pois.push_back(poi);
    }

    RooArgSet* nuis = (RooArgSet*)mc->GetNuisanceParameters();
    if (!nuis) {
        LOG(logERROR) << "Nuisance parameter set doesn't exist!";
        exit(1);
    }
    TIterator* itr = nuis->createIterator();
    RooRealVar* var;

    RooArgSet* globs = (RooArgSet*)mc->GetGlobalObservables();
    if (!globs) {
        LOG(logERROR) << "GetGlobal observables don't exist!";
        exit(1);
    }

    // collect nuisance parameters
    vector<string> vec_nuis;

    while ((var = (RooRealVar*)itr->Next())) {
        string varName(var->GetName());

//        // skipping MC stat
//        if (varName.find("gamma_stat") != string::npos) {
//            LOG(logDEBUG) << "Skipping " << varName;
//            continue;
//        }

        // skipping lvlv pacman efficiencies
        if (varName.find("PM_EFF_f_recoil") != string::npos && varName.find("alpha") == string::npos) {
            LOG(logDEBUG) << "Skipping " << varName;
            continue;
        }

        // all remaining nuisance parameters
        LOG(logDEBUG) << "pushed back " << varName;
        vec_nuis.push_back(string(var->GetName()));
    }

    itr->Reset();
    int nrNuis = vec_nuis.size();

    // create nll and do unconditional fit
    //    ws->var("alpha_ATLAS_JES_2012_Modelling1")->setVal(-0.5);
    /*
    if (TString(folder) == "embsmooth1tescrand_ajet_125_0") {
      ws->var("alpha_ATLAS_ANA_LH12_0jet_QCD")->setVal(0.3);    cout << "Variable :: alpha_ATLAS_ANA_LH12_0jet_QCD   initialized to value =  0.3" << endl;
      ws->var("alpha_ATLAS_ANA_LH12_1jet_QCD")->setVal(0.3);    cout << "Variable :: alpha_ATLAS_ANA_LH12_1jet_QCD   initialized to value =  0.3" << endl;
    }

    if (TString(folder) == "embsmooth7tescrand_0jet_125_0") {
      ws->var("alpha_ATLAS_TES_EOP_2012")->setVal(1.0);         cout << "Variable :: alpha_ATLAS_TES_EOP_2012        initialized to value =  1.0" << endl;
      //      ws->var("alpha_ATLAS_JVF_2012")->setVal(-0.5);            cout << "Variable :: alpha_ATLAS_JVF_2012            initialized to value = -0.5" << endl;
    }
    
    if (TString(folder) == "embsmooth7tescrand_ajet_125_0") {
      ws->var("alpha_ATLAS_JES_2012_Modelling1")->setVal(-0.5); cout << "Variable :: alpha_ATLAS_JES_2012_Modelling1 initialized to value = -0.5" << endl;
      ws->var("alpha_ATLAS_ANA_LH12_1jet_QCD")->setVal(0.3);    cout << "Variable :: alpha_ATLAS_ANA_LH12_1jet_QCD   initialized to value =  0.3" << endl;
    }

    if (TString(folder) == "embsmoothcrand_ajet_125_0") {
      ws->var("alpha_ATLAS_ANA_LH12_0jet_QCD")->setVal(0.3);    cout << "Variable :: alpha_ATLAS_ANA_LH12_0jet_QCD   initialized to value =  0.3" << endl;
      ws->var("alpha_ATLAS_ANA_LH12_1jet_QCD")->setVal(0.3);    cout << "Variable :: alpha_ATLAS_ANA_LH12_1jet_QCD   initialized to value =  0.3" << endl;
      ws->var("alpha_ATLAS_JVF_2012")->setVal(-0.5);            cout << "Variable :: alpha_ATLAS_JVF_2012            initialized to value = -0.5" << endl;
    }

    if (TString(folder) == "embsofia1tescrand_0jet_125_0") { 
      ws->var("alpha_ATLAS_TES_FAKE_2012")->setVal(0.25);      cout << "Variable :: alpha_ATLAS_TES_FAKE_2012        initialized to value =  0.25" << endl;
    }

    if (TString(folder) == "embsofia1tescrand_1jet_125_0") {
      ws->var("alpha_ATLAS_JVF_2012")->setVal(0.25);           cout << "Variable :: alpha_ATLAS_JVF_2012             initialized to value =  0.25" << endl;
      ws->var("alpha_ATLAS_ANA_LH12_1jet_QCD")->setVal(-0.2);  cout << "Variable :: alpha_ATLAS_ANA_LH12_1jet_QCD    initialized to value = -0.2" << endl;
    }

    if (TString(folder) == "embsofia1tescrand_ajet_125_0") { 
      ws->var("alpha_ATLAS_ANA_EMB_ISOL")->setVal(0.25);       cout << "Variable :: alpha_ATLAS_ANA_EMB_ISOL         initialized to value =  0.25" << endl;
    }

    if (TString(folder) == "embsofia7tescrand_0jet_125_0") {
      ws->var("alpha_ATLAS_TES_BIAS_2012")->setVal(1.0);      cout << "Variable :: alpha_ATLAS_TES_BIAS_2012        initialized to value =  1.0" << endl;
    }

    if (TString(folder) == "embsofia7tescrand_1jet_125_0") {
      ws->var("alpha_ATLAS_JVF_2012")->setVal(0.2);            cout << "Variable :: alpha_ATLAS_JVF_2012             initialized to value =  0.2" << endl;
      ws->var("alpha_ATLAS_TES_FAKE_2012")->setVal(0.1);       cout << "Variable :: alpha_ATLAS_TES_FAKE_2012        initialized to value =  0.1" << endl;
    }

    if (TString(folder) == "embsofia7tescrand_ajet_125_0") {
      ws->var("alpha_ATLAS_JER_2012")->setVal(0.2);            cout << "Variable :: alpha_ATLAS_JER_2012             initialized to value =  0.2" << endl;
      ws->var("alpha_ATLAS_TES_FAKE_2012")->setVal(0.1);       cout << "Variable :: alpha_ATLAS_TES_FAKE_2012        initialized to value =  0.1" << endl;
      ws->var("alpha_ATLAS_TES_BIAS_2012")->setVal(1.0);      cout << "Variable :: alpha_ATLAS_TES_BIAS_2012        initialized to value =  1.0" << endl;
    }

    if (TString(folder) == "embsofiacrand_0jet_125_0") { 
      ws->var("alpha_ATLAS_TES_FAKE_2012")->setVal(0.25);      cout << "Variable :: alpha_ATLAS_TES_FAKE_2012        initialized to value =  0.25" << endl;
    }

    if (TString(folder) == "embsofiacrand_1jet_125_0") { 
      ws->var("alpha_ATLAS_TES_FAKE_2012")->setVal(0.25);      cout << "Variable :: alpha_ATLAS_TES_FAKE_2012        initialized to value =  0.25" << endl;
    }

    if (TString(folder) == "embsofiacrand_ajet_125_0") { 
      ws->var("alpha_ATLAS_JVF_2012")->setVal(-0.2);           cout << "Variable :: alpha_ATLAS_JVF_2012             initialized to value = -0.2" << endl;
      ws->var("alpha_ATLAS_JES_FlavComp_TAU_G")->setVal(-0.5); cout << "Variable :: alpha_ATLAS_JES_FlavComp_TAU_G   initialized to value = -0.5" << endl;
      ws->var("alpha_ATLAS_ANA_LH12_1jet_QCD")->setVal(0.2) ;  cout << "Variable :: alpha_ATLAS_ANA_LH12_1jet_QCD    initialized to value =  0.2" << endl;
    }
    */

    if (dataName == "obsData")           ws->loadSnapshot("nominalGlobs");
    if (dataName == "asimovData_1_pamh") ws->loadSnapshot("conditionalGlobs_1_pamh");

    ws->loadSnapshot("unconditional");

    for (int i = 0; i < pois.size(); i++) {
    	pois[i]->setConstant(0);
        pois[i]->setRange(-50., 50.);
    }



    int numCPU = sysconf( _SC_NPROCESSORS_ONLN );
    if (useMinos) numCPU = 1; // Minos doesn't like our NLL with multiple cores
    numCPU = 1; // LXBATCH  like our NLL with multiple cores
    RooNLLVar* nll = (RooNLLVar*)mc->GetPdf()->createNLL(*data, Constrain(*nuis), GlobalObservables(*globs), Offset(1), NumCPU(numCPU, RooFit::Hybrid));

    RooFitResult* fitresult = minimize(nll);

    double nll_hat = nll->getVal();
    vector<double> pois_hat;
    for (int i = 0; i < pois.size(); i++) {
        pois_hat.push_back(pois[i]->getVal());
    }

    // set all nuisance parameters floating and save snapshot
    while ((var = (RooRealVar*)itr->Next())) {
        var->setConstant(0);
    }
    itr->Reset();
    for (int i = 0; i < pois.size(); i++) {
        pois[i]->setConstant(0);
    }
    ws->saveSnapshot("tmp_snapshot", *mc->GetPdf()->getParameters(data));

    LOG(logDEBUG) << "Made unconditional snapshot";

    for (int in = 0; in <= nrNuis; in++) {
        ws->loadSnapshot("tmp_snapshot");

        RooRealVar* nuip = in < nrNuis ? (RooRealVar*)nuis->find(vec_nuis[in].c_str()) : pois[0];
        string nuipName(nuip->GetName());
	//cout << "nuipName : " << nuipName.c_str() << "string(variable) : " << string(variable) << endl; 
        if (variable != NULL && nuipName != string(variable)) continue;
	//cout << " ok upto this point " << endl;
        // find all unconstrained NFs etc.
        bool isNorm = 0;
        // if (nuipName.find("ATLAS_norm") != string::npos) isNorm = 1;
        // if (nuipName.find("atlas_nbkg_") != string::npos) isNorm = true;
        // if (nuipName.find("slope_") != string::npos) isNorm = true;
        // if (nuipName.find("p0_") != string::npos) isNorm = true;
        // if (nuipName.find("p1_") != string::npos) isNorm = true;
        // if (nuipName.find("p2_") != string::npos) isNorm = true;
        // if (nuipName.find("p3_") != string::npos) isNorm = true;
        // if (nuipName.find("ATLAS_Hbb_norm_") != string::npos) isNorm = true;
        // if (nuipName.find("ATLAS_PM_EFF_") != string::npos) isNorm = true;
        if (nuipName.find("scale_") != string::npos && nuipName.find("QCDscale_") == string::npos) isNorm = true;

        // setting all parameters but the one NF to look at constant at the best fit value when computing the error for the NF
        if (isNorm) {
            LOG(logDEBUG) << nuipName << " is unconstrained.";
            while ((var = (RooRealVar*)itr->Next())) {
                string varName(var->GetName());
                if (varName.find(nuipName) == string::npos) var->setConstant(1);
                LOG(logDEBUG) << varName << " is constant -> " << var->isConstant();
            }
            itr->Reset();
        }

        double nuip_hat = nuip->getVal();
        nuip->setConstant(0);

        ws->saveSnapshot("tmp_snapshot2", *mc->GetPdf()->getParameters(data));

        LOG(logDEBUG) << "Computing error for var " << nuip->GetName() << " at " << nuip->getVal();

        double nuip_errup;
        double nuip_errdown;

        if (useMinos) {
            RooArgSet* minosSet = new RooArgSet(*nuip);
            minimize(nll, minosSet);
            nuip_errup = nuip->getErrorHi();
            nuip_errdown = nuip->getErrorLo();
        } else {
            ws->loadSnapshot("tmp_snapshot2");
            nuip_errup = findSigma(nll, nll_hat, nuip, nuip_hat+fabs(nuip->getErrorHi()), nuip_hat, +1, precision);
            ws->loadSnapshot("tmp_snapshot2");
            nuip_errdown = findSigma(nll, nll_hat, nuip, nuip_hat-fabs(nuip->getErrorLo()), nuip_hat, -1, precision);
        }

        LOG(logINFO) << nuip->GetName() << ":: FitVal = " << nuip_hat << " +" << fabs(nuip_errup) << " /  -" << fabs(nuip_errdown);

	//
        bool isConst = nuip->isConstant();
	nuip->setConstant(true);

	//do the scan based on the error
	bool above = false;
	vector<double> nuip_vals;
	vector<double> q_vals;
	const int nrPoints = 50 ;//40;
	//
	//	for (int inuis = 0; inuis < vec_nuis.size(); inuis++) {
	//	  cout << "inuis = " << inuis << " name = " << vec_nuis[inuis] << endl;
	//	}
	

	double Edm;
	double FCN;
	int MigradStatus;
	int HesseStatus;
	double poi_val;
	double poi_err; 
	int poi_isconst;
	vector<double> NP_val;
	vector<double> NP_err;
	vector<int> NP_isconst;

	NP_val.clear();
	NP_err.clear();
	NP_isconst.clear();

	for (int inuis = 0; inuis < vec_nuis.size(); inuis++) {
	  NP_val.push_back(0);
	  NP_err.push_back(0);
	  NP_isconst.push_back(0);
	}

	double qtheta;
	double nll_val;
	//

	gROOT->SetStyle("Plain"); 
	gROOT->ForceStyle();
	gStyle->SetOptStat(0);
	gStyle->SetOptTitle(0);
	gStyle->SetPadBottomMargin(.15);
	gStyle->SetPadRightMargin(.025);
	gStyle->SetPadLeftMargin(.15);
	gStyle->SetPadTopMargin(.025);
	gStyle->SetLabelFont(62,"X");
	gStyle->SetLabelFont(62,"Y");
	gStyle->SetLabelSize(0.04,"X");
	gStyle->SetLabelSize(0.04,"Y");
	gStyle->SetTitleXOffset(1.25);
	gStyle->SetTitleYOffset(1.25);
	gStyle->SetLabelOffset(0.01,"X");
	gStyle->SetLabelOffset(0.01,"Y");
	gStyle->SetNdivisions(5,"X");
	gStyle->SetTextFont(62);
	gStyle->SetTextSize(0.05);
	
	// store result in root file
        stringstream fileName;
        fileName <<  thisdir << "/" << "root-files/" << folder << "_scans/" << nuipName  << "_" << Form("%3.1f",NPlow) << "_" << Form("%3.1f",NPhig)  << ".root";
	cout << "OutFile: " << fileName.str().c_str() << endl;
        TFile* fout = TFile::Open(fileName.str().c_str(), "recreate");
	
	// Initialize the tree
	TTree *tree = new TTree("ScanQmu","ScanQmu");


	tree->Branch("nll_hat", &nll_hat , "nll_hat/D");
	tree->Branch("nll_val", &nll_val , "nll_val/D");
	tree->Branch("qtheta",  &qtheta  , "qtheta/D");

	tree->Branch("Edm",          &Edm ,          "Edm/D");
	tree->Branch("FCN",          &FCN ,          "FCN/D");
	tree->Branch("MigradStatus", &MigradStatus , "MigradStatus/I");
	tree->Branch("HesseStatus",  &HesseStatus ,  "HesseStatus/I");

	tree->Branch(Form("%s_val",    poiName.Data()),  &poi_val,     Form("%s_val/D",poiName.Data()));
	tree->Branch(Form("%s_err",    poiName.Data()),  &poi_err,     Form("%s_err/D",poiName.Data()));
	tree->Branch(Form("%s_isconst",poiName.Data()),  &poi_isconst, Form("%s_isconst/I",poiName.Data()));

	for (int inuis = 0; inuis < vec_nuis.size(); inuis++) {
	  tree->Branch(Form("%s_val",     vec_nuis[inuis].c_str()),  &NP_val[inuis],     Form("%s_val/D",    vec_nuis[inuis].c_str()));
	  tree->Branch(Form("%s_err",     vec_nuis[inuis].c_str()),  &NP_err[inuis],     Form("%s_err/D",    vec_nuis[inuis].c_str()));
	  tree->Branch(Form("%s_isconst", vec_nuis[inuis].c_str()),  &NP_isconst[inuis], Form("%s_isconst/I",vec_nuis[inuis].c_str()));
	}

	double step_size = 2* 1.1 * (fabs(nuip_errup) + fabs(nuip_errdown)) / nrPoints;
	for (double nuip_val = nuip_hat +fabs(nuip_errdown)*1.1* NPlow; nuip_val < nuip_hat + fabs(nuip_errup)*1.1* NPhig; nuip_val += step_size)
	  {
	    ws->loadSnapshot("tmp_snapshot2");
	    nuip->setVal(nuip_val);
	    nuip->setConstant(true);
	    RooFitResult* fit = minimize(MigradStatus, HesseStatus,nll);

	    Edm = fit->edm();
	    FCN = fit->minNll() ;
	    nll_val = nll->getVal();

	    if (nuip_val > nuip_hat && !above)
	      {
		nuip_vals.push_back(nuip_hat);
		q_vals.push_back(0);
		above=true;
	      }
	    
	    nuip_vals.push_back(nuip_val);
	    qtheta = 2*(nll_val - nll_hat);
	    q_vals.push_back(qtheta);
	    LOG(logINFO) << nuip->GetName() << ":: ScanQmu:: q(" << nuip_val << ") = " << qtheta ;

	    poi_val = ws->var(poiName)->getVal();
	    poi_err = ws->var(poiName)->getError();
	    poi_isconst = ws->var(poiName)->isConstant();
	    //	    cout << "poi_val = " << poi_val << " poi_err = " << poi_err << " poi_isconst = " << poi_isconst << endl;

	    NP_val.clear();
	    NP_err.clear();
	    NP_isconst.clear();

	    for (int inuis = 0; inuis < vec_nuis.size(); inuis++) {
	      NP_val.push_back(ws->var(vec_nuis[inuis].c_str())->getVal());
	      NP_err.push_back(ws->var(vec_nuis[inuis].c_str())->getError());
	      NP_isconst.push_back(ws->var(vec_nuis[inuis].c_str())->isConstant());

	      //	      cout << "inuis = " << inuis << " " << vec_nuis[inuis].c_str() 
	      //		   << " val = " << NP_val[inuis] << " err = " << NP_err[inuis] << " isconst = " <<  NP_isconst[inuis] << endl;
	    }	    

	    tree->Fill();

	  }

	/*
	  for (double nuip_val = -1; nuip_val <= 1; nuip_val +=1)
	  {
	  ws->loadSnapshot("tmp_snapshot2");
	  nuip->setVal(nuip_val);
	  nuip->setConstant(true);
	  RooFitResult* fit = minimize(MigradStatus, HesseStatus,nll);
	  
	  Edm = fit->edm();
	  FCN = fit->minNll() ;
	  
	  nll_val = nll->getVal();
	  
	  qtheta = 2*(nll_val - nll_hat);
	  
	  LOG(logINFO) << nuip->GetName() << ":: ScanQmu:: q(" << nuip_val << ") = " << qtheta ;
	  
	  poi_val = ws->var(poiName)->getVal();
	  poi_err = ws->var(poiName)->getError();
	  poi_isconst = ws->var(poiName)->isConstant();
	  //	    cout << "poi_val = " << poi_val << " poi_err = " << poi_err << " poi_isconst = " << poi_isconst << endl;
	  
	  NP_val.clear();
	  NP_err.clear();
	  NP_isconst.clear();
	  
	  for (int inuis = 0; inuis < vec_nuis.size(); inuis++) {
	  NP_val.push_back(ws->var(vec_nuis[inuis].c_str())->getVal());
	  NP_err.push_back(ws->var(vec_nuis[inuis].c_str())->getError());
	  NP_isconst.push_back(ws->var(vec_nuis[inuis].c_str())->isConstant());
	  //	      cout << "inuis = " << inuis << " " << vec_nuis[inuis].c_str() 
	  //		   << " val = " << NP_val[inuis] << " err = " << NP_err[inuis] << " isconst = " <<  NP_isconst[inuis] << endl;
	  
	  }	    
	  
	  tree->Fill();
	  
	  }
	*/

	
	int nrVals = nuip_vals.size();
	double* nuip_ary = new double[nrVals];
	double* q_ary = new double[nrVals];
	for (int i=0;i<nrVals;i++)
	  {
	    nuip_ary[i] = nuip_vals[i];
	    q_ary[i] = q_vals[i];
	  }




	
	TCanvas* c1 = new TCanvas("c1","c1",800,800);

	TGraph* graph = new TGraph(nrVals, nuip_ary, q_ary); 
	graph->SetMarkerSize(1.2);
	stringstream titleName;
	titleName << "q_{#theta}, #mu = ";
	titleName << "#hat{#mu}";
	graph->SetTitle("");
	graph->GetYaxis()->SetTitle(titleName.str().c_str());
	TString tmpname_1 = folder; tmpname_1.ReplaceAll("125_","");
	TString tmpname_2 = nuipName; tmpname_2.ReplaceAll("alpha_",""); tmpname_2.ReplaceAll("ATLAS_",""); tmpname_2.ReplaceAll("gamma_",""); 
	graph->GetXaxis()->SetTitle(Form("%s : %s",tmpname_1.Data(),tmpname_2.Data()));
	//	graph->GetXaxis()->SetLimits(nuip_hat - fabs(nuip_errdown)*1.2, nuip_hat + fabs(nuip_errup)*1.2);
	graph->GetXaxis()->SetLimits(-2.5,2.5);//nuip_hat - fabs(nuip_errdown)*1.2, nuip_hat + fabs(nuip_errup)*1.2);
	graph->SetMaximum(1.25);
	graph->SetMinimum(-.25);
	graph->Draw("alp");
	gPad->Modified();
	
	TLine l;
	l.SetLineColor(kRed);
	l.DrawLine(nuip_hat-fabs(nuip_errdown), 1, nuip_hat+fabs(nuip_errup),   1);
	l.DrawLine(nuip_hat-fabs(nuip_errdown), 1, nuip_hat-fabs(nuip_errdown), 0);
	l.DrawLine(nuip_hat+fabs(nuip_errup),   1, nuip_hat+fabs(nuip_errup),   0);
	l.SetLineColor(kBlue) ;
	//	l.DrawLine(nuip_hat-fabs(nuip_errdown), 0, nuip_hat+fabs(nuip_errup), 0);
	l.DrawLine(-2.5, 0, 2.5, 0);
	
	TLatex text;
	//text.SetTextSize(0.05);
	text.SetTextSize(1.5);
	text.SetTextAlign(21);
	text.SetTextColor(4);
	text.DrawLatex(nuip_hat,1.05,Form("%5.2f + %5.2f - %5.2f",nuip_hat,fabs(nuip_errup),fabs(nuip_errdown)));

	stringstream saveName;
	saveName << thisdir << "/" << "scans/" << folder;
	system(("mkdir -vp " + saveName.str()).c_str());
	saveName << "/" << nuipName << "_muhat";
	saveName << "_" << Form("%3.1f",NPlow) << "_" << Form("%3.1f",NPhig) ;
	c1->Update();
	c1->SaveAs((saveName.str()+".pdf").c_str());
	c1->SaveAs((saveName.str()+".png").c_str());
	c1->SaveAs((saveName.str()+".eps").c_str());

	nuip->setConstant(isConst);
	
        // fix theta at the MLE value +/- postfit uncertainty and minimize again to estimate the change in the POI
        ws->loadSnapshot("tmp_snapshot2");
        nuip->setVal(nuip_hat+fabs(nuip_errup));
        nuip->setConstant(1);
        minimize(nll);
        vector<double> pois_up;
        for (int i = 0; i < pois.size(); i++) {
            pois_up.push_back(pois[i]->getVal());
        }

        ws->loadSnapshot("tmp_snapshot2");
        nuip->setVal(nuip_hat-fabs(nuip_errdown));
        nuip->setConstant(1);
        minimize(nll);
        vector<double> pois_down;
        for (int i = 0; i < pois.size(); i++) {
            pois_down.push_back(pois[i]->getVal());
        }

        // fix theta at the MLE value +/- postfit uncertainty and minimize again to estimate the change in the POI
        ws->loadSnapshot("tmp_snapshot2");
        nuip->setVal(nuip_hat+1.0);
        nuip->setConstant(1);
        minimize(nll);
        vector<double> pois_nom_up;
        for (int i = 0; i < pois.size(); i++) {
            pois_nom_up.push_back(pois[i]->getVal());
        }

        ws->loadSnapshot("tmp_snapshot2");
        nuip->setVal(nuip_hat-1.0);
        nuip->setConstant(1);
        minimize(nll);
        vector<double> pois_nom_down;
        for (int i = 0; i < pois.size(); i++) {
            pois_nom_down.push_back(pois[i]->getVal());
        }

        for (int i = 0; i < pois.size(); i++) {
	  LOG(logINFO) << nuip->GetName() << ":: Variation of " << pois[i]->GetName() << " from " << pois_hat[i] << " up  = " << pois_up[i] << " (" << pois_nom_up[i] << ") / down = " << pois_down[i] << " (" << pois_nom_down[i] << ")";
        }


        TH1D* h_out = new TH1D(nuipName.c_str(), nuipName.c_str(), 3 + 5 * pois.size(), 0, 3 + 5 * pois.size());

        h_out->SetBinContent(1, nuip_hat);
        h_out->SetBinContent(2, fabs(nuip_errup));
        h_out->SetBinContent(3, fabs(nuip_errdown));

        h_out->GetXaxis()->SetBinLabel(1, "nuip_hat");
        h_out->GetXaxis()->SetBinLabel(2, "nuip_up");
        h_out->GetXaxis()->SetBinLabel(3, "nuip_down");

        int bin = 4;
        for (int i = 0; i < pois.size(); i++) {
            h_out->SetBinContent(bin, pois_hat[i]);
            h_out->SetBinContent(bin+1, pois_up[i]);
            h_out->SetBinContent(bin+2, pois_down[i]);
            h_out->SetBinContent(bin+3, pois_nom_up[i]);
            h_out->SetBinContent(bin+4, pois_nom_down[i]);

            h_out->GetXaxis()->SetBinLabel(bin, pois[i]->GetName());
            h_out->GetXaxis()->SetBinLabel(bin+1, "poi_up");
            h_out->GetXaxis()->SetBinLabel(bin+2, "poi_down");
            h_out->GetXaxis()->SetBinLabel(bin+3, "poi_nom_up");
            h_out->GetXaxis()->SetBinLabel(bin+4, "poi_nom_down");

            bin += 5;
        }

	h_out->Write();
	graph->Write();
	tree->Write();
        fout->Close();
    }

    timer_pulls.Stop();
    timer_pulls.Print();
}
