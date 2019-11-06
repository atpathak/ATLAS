#include "TROOT.h"
#include "TStyle.h"
#include <string>
#include <vector>

#include "TFile.h"
#include "TH1D.h"
#include "Math/MinimizerOptions.h"
#include "TStopwatch.h"
#include "TXMLEngine.h"

#include "TStyle.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TH2.h"
#include "TH1.h"

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

struct sort_np_by_corr {
  TString name ;
  double corr ;
  bool operator < (const sort_np_by_corr & other) const {
    return fabs(corr) > fabs(other.corr); // for sorting in decreasing order in correlation, so that first element has largest correlation
  }
};

struct settings {
    string inFileName;
    string wsName;
    string modelConfigName;
    string dataName;
    string poiName;
    string xmlName;
    string technique;
    string catecory2eval;
    double precision;
    double corrCutoff;
    bool useMinos;
    string folder;
    string loglevel;
};

void setParams(RooArgSet* nuis, list<string> tmp_list, string technique, RooFitResult* fitresult, RooRealVar* poi, double corrCutoff);
list<string> addParams(settings* config, string catecory2eval);
void writeTmpXML (string systName, settings* config);

// ____________________________________________________________________________|__________
//void runBreakdown() {
    // for compiling only
//}

// ____________________________________________________________________________|__________
// Compute ranking of systematics specified in xml
//void runBreakdown(
int main(int argc, char* argv[]){
  //
  // Argument variables
  //
  string inFileName      = argv[1];
  string wsName          = argv[2];; //"combWS",
  string modelConfigName = argv[3];; // "ModelConfig",
  string dataName        = argv[4];; // "combData",
  string poiName         = argv[5];; // "mu",
  string folder          = argv[6];; //"test",
  //
  string thisdir      = "/afs/cern.ch/user/a/atpathak/afswork/public/LFVStat_16Aug2017";
  //
  string xmlName         = "config/breakdown.xml";
  string technique       = "add";
  string catecory2eval   = "total";
  double precision       = 0.01;
  double corrCutoff      = 0.0;
  string loglevel        = "DEBUG";
  //
    TStopwatch timer_breakdown;
    timer_breakdown.Start();

    // store all settings for passing to other functions
    settings* config = new settings();
    config->inFileName = inFileName;
    config->wsName = wsName;
    config->modelConfigName = modelConfigName;
    config->dataName = dataName;
    config->poiName = poiName;
    config->xmlName = xmlName;
    config->technique = technique;
    config->catecory2eval = catecory2eval;
    config->precision = precision;
    config->corrCutoff = corrCutoff;
    config->folder = folder;
    config->loglevel = loglevel;

    // DEBUG OUTPUT
    // - ERROR
    // - WARNING
    // - INFO
    // - DEBUG
    LOG::ReportingLevel() = LOG::FromString(config->loglevel);

    // some settings
    ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");
    ROOT::Math::MinimizerOptions::SetDefaultStrategy(1);
    if (config->loglevel == "DEBUG") {
        ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(1);
    } else {
        ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(-1);
        RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
    }
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

    // loading the workspace etc.
    LOG(logINFO) << "Running over workspace: " << config->inFileName;

    vector<string> parsed = parseString(config->poiName, ",");

    TFile* file = new TFile(config->inFileName.c_str());
  
    RooWorkspace* ws = (RooWorkspace*)file->Get(config->wsName.c_str());
    if (!ws) {
        LOG(logERROR) << "Workspace: " << config->wsName << " doesn't exist!";
        exit(1);
    }
  
    ModelConfig* mc = (ModelConfig*)ws->obj(config->modelConfigName.c_str());
    if (!mc) {
        LOG(logERROR) << "ModelConfig: " << config->modelConfigName << " doesn't exist!";
        exit(1);
    }

    RooDataSet* data = (RooDataSet*)ws->data(config->dataName.c_str());
    if (!data) {
        LOG(logERROR) << "Dataset: " << config->dataName << " doesn't exist!";
        exit(1);
    }

    vector<RooRealVar*> pois;
    for (int i = 0; i < parsed.size(); i++) {
        RooRealVar* poi = (RooRealVar*)ws->var(parsed[i].c_str());
        if (!poi) {
            LOG(logERROR) << "POI: " << parsed[i] << " doesn't exist!";
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
    TIterator* nitr = nuis->createIterator();

    TString pname=config->folder;
    /*
    if (pname.EndsWith("_3")) {
      RooRealVar* narg;
      while ((narg=(RooRealVar*)nitr->Next())) {
	if (!narg) continue;
	TString nname=narg->GetName();
	if (nname.Contains("gamma_")) narg->setConstant(1);
      }
      nuis->Print("v");
    }
    */
    RooRealVar* var;
    
    RooArgSet* globs = (RooArgSet*)mc->GetGlobalObservables();
    if (!globs) {
        LOG(logERROR) << "GetGlobal observables don't exist!";
        exit(1);
    }

    ws->loadSnapshot("nominalNuis");
    /*
    if (TString(folder) == "embsmooth1tescrand_ajet_125_0") {
      ws->var("alpha_ATLAS_ANA_LH12_0jet_QCD")->setVal(0.3);    cout << "Variable :: alpha_ATLAS_ANA_LH12_0jet_QCD   initialized to value =  0.3" << endl;
      ws->var("alpha_ATLAS_ANA_LH12_1jet_QCD")->setVal(0.3);    cout << "Variable :: alpha_ATLAS_ANA_LH12_1jet_QCD   initialized to value =  0.3" << endl;
    }

    if (TString(folder) == "embsmooth7tescrand_0jet_125_0") {
      ws->var("alpha_ATLAS_TES_EOP_2012")->setVal(1.0);         cout << "Variable :: alpha_ATLAS_TES_EOP_2012        initialized to value =  1.0" << endl;
      ws->var("alpha_ATLAS_JVF_2012")->setVal(-0.5);            cout << "Variable :: alpha_ATLAS_JVF_2012            initialized to value = -0.5" << endl;
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
    for (int i = 0; i < pois.size(); i++) {
        pois[i]->setRange(-50., 50.);
        pois[i]->setConstant(0);
	pois[i]->setVal(0);
    }

    int numCPU = sysconf( _SC_NPROCESSORS_ONLN );
    numCPU = 1; // LXBATCH  like our NLL with multiple cores
    RooNLLVar* nll = (RooNLLVar*)mc->GetPdf()->createNLL(*data, Constrain(*nuis), GlobalObservables(*globs), Offset(1), NumCPU(numCPU, RooFit::Hybrid));

    ROOT::Math::MinimizerOptions::SetDefaultStrategy(1);
    RooFitResult* fitresult = minimize(nll);
    system(Form("mkdir -vp %s/root-files/%s_fit",thisdir.c_str(),config->folder.c_str()));
    fitresult->SaveAs(Form("%s/root-files/%s_fit/result.root",thisdir.c_str(),config->folder.c_str()));
    ROOT::Math::MinimizerOptions::SetDefaultStrategy(1);

    bool plotcorr=true;
    if (plotcorr) { 
      const RooArgList fitpars = fitresult->floatParsFinal();
      
      int npar = fitpars.getSize() ;
      vector<TString> parnames;
      for (int ipar=0;ipar<fitpars.getSize();++ipar){
	RooRealVar*v = (RooRealVar*)fitpars.at(ipar);
	TString vname=v->GetName();
	parnames.push_back(vname);
      }
      
      vector<double> corr_ij;
      for (int i=0;i<npar;++i) {
	RooRealVar* ivar=ws->var(parnames[i]);
	for (int j=0;j<npar;++j) {
	  RooRealVar* jvar=ws->var(parnames[j]);
	  corr_ij.push_back(fitresult->correlation(*ivar,*jvar));
	  // cout << i << " " << j << " " << parnames[i] << " " << parnames[j] << " " << corr_ij[i*npar+j] << " " << corr_ij.back() << endl;
	}
      }
      
      gROOT->SetStyle("Plain"); 
      gROOT->ForceStyle();
      gStyle->SetOptStat(0);
      //      gStyle->SetOptTitle(0);
      gStyle->SetLabelFont(62,"X");
      gStyle->SetLabelFont(62,"Y");
      gStyle->SetLabelSize(0.028,"X");
      gStyle->SetLabelSize(0.028,"Y");
      gStyle->SetTextFont(62);
      gStyle->SetTextSize(0.04);
      gStyle->SetPalette(1);
      gStyle->SetPaintTextFormat(".0f");
      //  // put tick marks on top and RHS of plots
      //  gStyle->SetPadTickX(0);
      //  gStyle->SetPadTickY(0);
      //  gStyle->SetPadTickX(0);//not for x-axis
      gStyle->SetNdivisions(5,"X");
      
      //
      TH2D* hcorr=new TH2D("hcorr","Full Correlation Matrix",npar,0.,npar*1.0,npar,0.,npar*1.0);
      for (int j=0;j<npar;++j) {
	TString jname=parnames[j];
	jname.ReplaceAll("alpha_","");
	jname.ReplaceAll("gamma_","");
	jname.ReplaceAll("ATLAS_","");
	for (int k=0;k<npar;++k) {
	  TString kname=parnames[k];
	  kname.ReplaceAll("alpha_","");
	  kname.ReplaceAll("gamma_","");
	  kname.ReplaceAll("ATLAS_","");
	  cout  << " j , k , corr = " << j << " " << k << " " << " " << jname << " " << " " << kname << " " << corr_ij[j*npar+k] << endl;
	  hcorr->SetBinContent(j+1,npar-k,100*corr_ij[j*npar+k]);
	  hcorr->GetXaxis()->SetBinLabel(j+1,jname);
	  hcorr->GetYaxis()->SetBinLabel(npar-k,kname);
	}
      }
      TCanvas*c2=new TCanvas("c2","c2",800,800);
      c2->cd();
      //
      c2->SetTopMargin(.125);
      c2->SetRightMargin(.125);
      c2->SetLeftMargin(.325);
      c2->SetBottomMargin(.325);
      //
      hcorr->SetMinimum(-100);
      hcorr->SetMaximum(+100);
      hcorr->LabelsOption("v");
      hcorr->Draw("colz");
      //	gPad->SetGrid(1,1);
      //	hcorr->GetXaxis()->SetAxisColor(17);
      //	hcorr->GetYaxis()->SetAxisColor(17);
      //	c2->RedrawAxis();
      system(Form("mkdir -vp %s/scans/%s",thisdir.c_str(),folder.c_str()));
      c2->SaveAs(Form("%s/scans/%s/corr_muhat_%s.eps",thisdir.c_str(),folder.c_str(),dataName.c_str()));  
      c2->SaveAs(Form("%s/scans/%s/corr_muhat_%s.pdf",thisdir.c_str(),folder.c_str(),dataName.c_str()));  
      c2->SaveAs(Form("%s/scans/%s/corr_muhat_%s.png",thisdir.c_str(),folder.c_str(),dataName.c_str()));  
      delete c2;    c2=0;
      delete hcorr; hcorr = 0;
      //
      // Reduced Correlation Matrix
      //
      vector<sort_np_by_corr> vec_sort_np_by_corr;
      for (int j=0;j<npar;++j) {
	if (parnames[j] != "SigXsecOverSM") continue;
	for (int k=0;k<npar;++k) {
	  sort_np_by_corr np;
	  np.name = parnames[k];
	  np.corr = corr_ij[j*npar+k];
	  vec_sort_np_by_corr.push_back(np);
	}
      }
      //
      sort(vec_sort_np_by_corr.begin(),vec_sort_np_by_corr.end());
      //
      int nfew=25;
      vector<TString> Top_Few_NP;
      for (int ifew=0;ifew<nfew;++ifew) {
	if (ifew < vec_sort_np_by_corr.size()) {
	  Top_Few_NP.push_back(vec_sort_np_by_corr[ifew].name);
	}
      }
      //
      vector<double> vecj_reduced;
      for (int j=0;j<npar;++j) {
//	//	if (parnames[j].Contains("gamma_")) continue;
//	int keep  = 0;
//	for (int k=0;k<npar;++k) {
//	  //	  if (parnames[k].Contains("gamma_")) continue;
//	  //	  if (fabs(corr_ij[j*npar+k]) < 1 && fabs(corr_ij[j*npar+k]) >= 0.33) ++keep;
//	}
//	if (keep>0) vecj_reduced.push_back(j);
//	  
	if (find(Top_Few_NP.begin(), Top_Few_NP.end(), parnames[j]) != Top_Few_NP.end()) vecj_reduced.push_back(j);
      }
      int nj=vecj_reduced.size();
      //
      vector<double> veck_reduced;
      for (int k=0;k<npar;++k) {
//	//	if (parnames[k].Contains("gamma_")) continue;
//	int keep  = 0;
//	for (int j=0;j<npar;++j) {
//	  //	  if (parnames[j].Contains("gamma_")) continue;
//	  //	  if (fabs(corr_ij[j*npar+k]) < 1 && fabs(corr_ij[j*npar+k]) >= 0.33) ++keep;
//	  if (find(Top_Few_NP.begin(), Top_Few_NP.end(), parnames[j]) != Top_Few_NP.end()) ++keep;
//	}
//	if (keep>0) veck_reduced.push_back(k);
//
	if (find(Top_Few_NP.begin(), Top_Few_NP.end(), parnames[k]) != Top_Few_NP.end()) veck_reduced.push_back(k);	
      }
      int nk=veck_reduced.size();
      //
      cout << "npar, nj, nk = " << npar << " " << nj << " " << nk << endl;
      TH2D* hcorr_re=new TH2D("hcorr_re",Form("%s: Top 25 NP's correlated with #mu",folder.c_str()),nj,0.,nj*1.0,nk,0.,nk*1.0);
      for (int j=0;j<nj;++j) {
	TString jname=parnames[vecj_reduced[j]];
	jname.ReplaceAll("alpha_","");
	jname.ReplaceAll("gamma_","");
	jname.ReplaceAll("ATLAS_","");
	for (int k=0;k<nk;++k) {
	  TString kname=parnames[veck_reduced[k]];
	  kname.ReplaceAll("alpha_","");
	  kname.ReplaceAll("gamma_","");
	  kname.ReplaceAll("ATLAS_","");
	  cout  << " j , k , corr_re = " << vecj_reduced[j] << " " << veck_reduced[k] << " " << " " << jname << " " << " " << kname << " " << corr_ij[vecj_reduced[j]*npar+veck_reduced[k]] << endl;
	  hcorr_re->SetBinContent(j+1,nk-k,100*corr_ij[vecj_reduced[j]*npar+veck_reduced[k]]);
	  hcorr_re->GetXaxis()->SetBinLabel(j+1,jname);
	  hcorr_re->GetYaxis()->SetBinLabel(nk-k,kname);
	}
      }
      TCanvas*c2_re=new TCanvas("c2_re","c2_re",800,800);
      c2_re->cd();
      //
      c2_re->SetTopMargin(.125);
      c2_re->SetRightMargin(.125);
      c2_re->SetLeftMargin(.325);
      c2_re->SetBottomMargin(.325);
      //
      hcorr_re->SetMinimum(-100);
      hcorr_re->SetMaximum(+100);
      hcorr_re->LabelsOption("v");
      if (nk<=16) {
	hcorr_re->SetMarkerSize(.7);
	hcorr_re->Draw("colz text"); 
      } else if (nk<=25) {
	hcorr_re->SetMarkerSize(.6);
	hcorr_re->Draw("colz text"); 
      } else if (nk<=36) {
	hcorr_re->SetMarkerSize(.5);
	hcorr_re->Draw("colz text"); 
      } else {
	hcorr_re->Draw("colz");
      }
      gPad->SetGrid(1,1);
      hcorr_re->GetXaxis()->SetAxisColor(17);
      hcorr_re->GetYaxis()->SetAxisColor(17);
      c2_re->RedrawAxis();
      system(Form("mkdir -vp %s/scans/%s",thisdir.c_str(),folder.c_str()));
      c2_re->SaveAs(Form("%s/scans/%s/corr_re_muhat_%s.eps",thisdir.c_str(),folder.c_str(),dataName.c_str()));  
      c2_re->SaveAs(Form("%s/scans/%s/corr_re_muhat_%s.pdf",thisdir.c_str(),folder.c_str(),dataName.c_str()));  
      c2_re->SaveAs(Form("%s/scans/%s/corr_re_muhat_%s.png",thisdir.c_str(),folder.c_str(),dataName.c_str()));  
      delete c2_re; c2_re = 0;
      delete hcorr_re; hcorr_re = 0;
      nj = 0; vecj_reduced.clear(); nk = 0; veck_reduced.clear(); nfew = 0; Top_Few_NP.clear(); vec_sort_np_by_corr.clear();
      //
      // Reduced Correlation Matrix [without Theory NP]
      //
      vector<sort_np_by_corr> vec_sort_np_NOTHEORY_by_corr;
      for (int j=0;j<npar;++j) {
	if (parnames[j] != "SigXsecOverSM") continue;
	for (int k=0;k<npar;++k) {
	  if (parnames[k].Contains("GenAcc")) continue;
	  if (parnames[k].Contains("pdf_")) continue;
	  if (parnames[k].Contains("QCDscale_")) continue;
	  if (parnames[k].Contains("BR_")) continue;
	  if (parnames[k].Contains("UE_")) continue;
	  sort_np_by_corr np;
	  np.name = parnames[k];
	  np.corr = corr_ij[j*npar+k];
	  vec_sort_np_NOTHEORY_by_corr.push_back(np);
	}
      }
      //
      sort(vec_sort_np_NOTHEORY_by_corr.begin(),vec_sort_np_NOTHEORY_by_corr.end());
      //
      int nfew_NOTHEORY=25;
      vector<TString> Top_Few_NP_NOTHEORY;
      for (int ifew=0;ifew<nfew_NOTHEORY;++ifew) {
	if (ifew < vec_sort_np_NOTHEORY_by_corr.size()) {
	  Top_Few_NP_NOTHEORY.push_back(vec_sort_np_NOTHEORY_by_corr[ifew].name);
	}
      }
      //
      vector<double> vecj_reduced_NOTHEORY;
      for (int j=0;j<npar;++j) {
//	//	if (parnames[j].Contains("gamma_")) continue;
//	int keep  = 0;
//	for (int k=0;k<npar;++k) {
//	  //	  if (parnames[k].Contains("gamma_")) continue;
//	  //	  if (fabs(corr_ij[j*npar+k]) < 1 && fabs(corr_ij[j*npar+k]) >= 0.33) ++keep;
//	}
//	if (keep>0) vecj_reduced.push_back(j);
//	  
	if (find(Top_Few_NP_NOTHEORY.begin(), Top_Few_NP_NOTHEORY.end(), parnames[j]) != Top_Few_NP_NOTHEORY.end()) vecj_reduced_NOTHEORY.push_back(j);
      }
      int nj_NOTHEORY=vecj_reduced_NOTHEORY.size();
      //
      vector<double> veck_reduced_NOTHEORY;
      for (int k=0;k<npar;++k) {
//	//	if (parnames[k].Contains("gamma_")) continue;
//	int keep  = 0;
//	for (int j=0;j<npar;++j) {
//	  //	  if (parnames[j].Contains("gamma_")) continue;
//	  //	  if (fabs(corr_ij[j*npar+k]) < 1 && fabs(corr_ij[j*npar+k]) >= 0.33) ++keep;
//	  if (find(Top_Few_NP.begin(), Top_Few_NP.end(), parnames[j]) != Top_Few_NP.end()) ++keep;
//	}
//	if (keep>0) veck_reduced.push_back(k);
//
	if (find(Top_Few_NP_NOTHEORY.begin(), Top_Few_NP_NOTHEORY.end(), parnames[k]) != Top_Few_NP_NOTHEORY.end()) veck_reduced_NOTHEORY.push_back(k);	
      }
      int nk_NOTHEORY=veck_reduced_NOTHEORY.size();
      //
      cout << "npar, nj_NOTHEORY, nk_NOTHEORY = " << npar << " " << nj_NOTHEORY << " " << nk_NOTHEORY << endl;
      TH2D* hcorr_re_NOTHEORY=new TH2D("hcorr_re_NOTHEORY",Form("%s: Top 25 NP's (NO THEORY) correlated with #mu ",folder.c_str()),nj_NOTHEORY,0.,nj_NOTHEORY*1.0,nk_NOTHEORY,0.,nk_NOTHEORY*1.0);
      for (int j=0;j<nj_NOTHEORY;++j) {
	TString jname=parnames[vecj_reduced_NOTHEORY[j]];
	jname.ReplaceAll("alpha_","");
	jname.ReplaceAll("gamma_","");
	jname.ReplaceAll("ATLAS_","");
	for (int k=0;k<nk_NOTHEORY;++k) {
	  TString kname=parnames[veck_reduced_NOTHEORY[k]];
	  kname.ReplaceAll("alpha_","");
	  kname.ReplaceAll("gamma_","");
	  kname.ReplaceAll("ATLAS_","");
	  cout  << " j , k , corr_re_NOTHEORY = " << vecj_reduced_NOTHEORY[j] << " " << veck_reduced_NOTHEORY[k] << " " << " " << jname << " " << " " << kname << " " << corr_ij[vecj_reduced_NOTHEORY[j]*npar+veck_reduced_NOTHEORY[k]] << endl;
	  hcorr_re_NOTHEORY->SetBinContent(j+1,nk_NOTHEORY-k,100*corr_ij[vecj_reduced_NOTHEORY[j]*npar+veck_reduced_NOTHEORY[k]]);
	  hcorr_re_NOTHEORY->GetXaxis()->SetBinLabel(j+1,jname);
	  hcorr_re_NOTHEORY->GetYaxis()->SetBinLabel(nk_NOTHEORY-k,kname);
	}
      }
      TCanvas*c2_re_NOTHEORY=new TCanvas("c2_re_NOTHEORY","c2_re_NOTHEORY",800,800);
      c2_re_NOTHEORY->cd();
      //
      c2_re_NOTHEORY->SetTopMargin(.125);
      c2_re_NOTHEORY->SetRightMargin(.125);
      c2_re_NOTHEORY->SetLeftMargin(.325);
      c2_re_NOTHEORY->SetBottomMargin(.325);
      //
      hcorr_re_NOTHEORY->SetMinimum(-100);
      hcorr_re_NOTHEORY->SetMaximum(+100);
      hcorr_re_NOTHEORY->LabelsOption("v");
      if (nk_NOTHEORY<=16) {
	hcorr_re_NOTHEORY->SetMarkerSize(.7);
	hcorr_re_NOTHEORY->Draw("colz text"); 
      } else if (nk_NOTHEORY<=25) {
	hcorr_re_NOTHEORY->SetMarkerSize(.6);
	hcorr_re_NOTHEORY->Draw("colz text"); 
      } else if (nk_NOTHEORY<=36) {
	hcorr_re_NOTHEORY->SetMarkerSize(.5);
	hcorr_re_NOTHEORY->Draw("colz text"); 
      } else {
	hcorr_re_NOTHEORY->Draw("colz");
      }
      gPad->SetGrid(1,1);
      hcorr_re_NOTHEORY->GetXaxis()->SetAxisColor(17);
      hcorr_re_NOTHEORY->GetYaxis()->SetAxisColor(17);
      c2_re_NOTHEORY->RedrawAxis();
      system(Form("mkdir -vp %s/scans/%s",thisdir.c_str(),folder.c_str()));
      c2_re_NOTHEORY->SaveAs(Form("%s/scans/%s/corr_re_NOTHEORY_muhat_%s.eps",thisdir.c_str(),folder.c_str(),dataName.c_str()));  
      c2_re_NOTHEORY->SaveAs(Form("%s/scans/%s/corr_re_NOTHEORY_muhat_%s.pdf",thisdir.c_str(),folder.c_str(),dataName.c_str()));  
      c2_re_NOTHEORY->SaveAs(Form("%s/scans/%s/corr_re_NOTHEORY_muhat_%s.png",thisdir.c_str(),folder.c_str(),dataName.c_str()));  
      delete c2_re_NOTHEORY; c2_re_NOTHEORY = 0 ;
      delete hcorr_re_NOTHEORY; hcorr_re_NOTHEORY = 0;
      nj_NOTHEORY = 0; vecj_reduced_NOTHEORY.clear(); nk_NOTHEORY = 0; veck_reduced_NOTHEORY.clear(); nfew_NOTHEORY = 0; Top_Few_NP_NOTHEORY.clear(); vec_sort_np_NOTHEORY_by_corr.clear();
    }

    RooArgSet nuisAndPOI(*mc->GetNuisanceParameters(), *mc->GetParametersOfInterest());
    ws->saveSnapshot("tmp_shot", nuisAndPOI);

    double nll_val_true = nll->getVal();
    vector<double> pois_hat;
    for (int i = 0; i < pois.size(); i++) {
        pois_hat.push_back(pois[i]->getVal());
    }

    vector<double> pois_up;
    vector<double> pois_down;

    if (config->catecory2eval == "total") {
        for (int i = 0; i < pois.size(); i++) {
            ws->loadSnapshot("tmp_shot");
            pois_up.push_back(findSigma(nll, nll_val_true, pois[i], pois_hat[i]+pois[i]->getErrorHi(), pois_hat[i], +1, config->precision));
            ws->loadSnapshot("tmp_shot");
            pois_down.push_back(findSigma(nll, nll_val_true, pois[i], pois_hat[i]-fabs(pois[i]->getErrorLo()), pois_hat[i], -1, config->precision));
        }
    } else if (config->catecory2eval == "srstat") {
        // setting everything constant, so that just data statistics is left
        ws->loadSnapshot("tmp_shot");
        nitr->Reset();
        while ((var = (RooRealVar*)nitr->Next())) {
            var->setConstant(1);
        }
        ws->saveSnapshot("tmp_shot_srstat",nuisAndPOI);

        for (int i = 0; i < pois.size(); i++) {
            ws->loadSnapshot("tmp_shot_srstat");
            pois_up.push_back(findSigma(nll, nll_val_true, pois[i], pois_hat[i]+pois[i]->getErrorHi(), pois_hat[i], +1, config->precision));
            ws->loadSnapshot("tmp_shot_srstat");
            pois_down.push_back(findSigma(nll, nll_val_true, pois[i], pois_hat[i]-fabs(pois[i]->getErrorLo()), pois_hat[i], -1, config->precision));
        }
    } else if (config->catecory2eval == "mcstat") {
        for (int i = 0; i < pois.size(); i++) {
            ws->loadSnapshot("tmp_shot");
            list<string> stat_list = addParams(config, "statistical");
            setParams(nuis, stat_list, config->technique, fitresult, pois[i], config->corrCutoff);
            nitr->Reset();
            while ((var = (RooRealVar*)nitr->Next())) {
                if (string(var->GetName()).find("gamma_stat") != string::npos) {
                    var->setConstant(0);
                }
            }
            ws->saveSnapshot("tmp_shot_mcstat",nuisAndPOI);

            ws->loadSnapshot("tmp_shot_mcstat");
            pois_up.push_back(findSigma(nll, nll_val_true, pois[i], pois_hat[i]+pois[i]->getErrorHi(), pois_hat[i], +1, config->precision));
            ws->loadSnapshot("tmp_shot_mcstat");
            pois_down.push_back(findSigma(nll, nll_val_true, pois[i], pois_hat[i]-fabs(pois[i]->getErrorLo()), pois_hat[i], -1, config->precision));
        }
    } else {
        list<string> nuis_list = addParams(config, config->catecory2eval);

        for (int i = 0; i < pois.size(); i++) {
            ws->loadSnapshot("tmp_shot");
            setParams(nuis, nuis_list, config->technique, fitresult, pois[i], config->corrCutoff);
            pois_up.push_back(findSigma(nll, nll_val_true, pois[i], pois_hat[i]+pois[i]->getErrorHi(), pois_hat[i], +1, config->precision));
            ws->loadSnapshot("tmp_shot");
            setParams(nuis, nuis_list, config->technique, fitresult, pois[i], config->corrCutoff);
            pois_down.push_back(findSigma(nll, nll_val_true, pois[i], pois_hat[i]-fabs(pois[i]->getErrorLo()), pois_hat[i], -1, config->precision));
        }
    }

    for (int i = 0; i < pois.size(); i++) {
        LOG(logINFO) << config->catecory2eval << " gives " << pois[i]->GetName() << " = " << pois_hat[i] << " +" << pois_up[i] << " / -" << pois_down[i];
    }

    system(("mkdir -vp " + thisdir + "/" + "root-files/" + config->folder + "_breakdown_" + technique).c_str());
    stringstream fileName;
    fileName << thisdir << "/" << "root-files/" << config->folder << "_breakdown_" << technique << "/" << config->catecory2eval << ".root";
    TFile fout(fileName.str().c_str(), "recreate");

    TH1D* h_out = new TH1D(config->catecory2eval.c_str(), config->catecory2eval.c_str(), 3 * pois.size(), 0, 3 * pois.size());

    int bin = 1;
    for (int i = 0; i < pois.size(); i++) {
        h_out->SetBinContent(bin, pois_hat[i]);
        h_out->SetBinContent(bin+1, fabs(pois_up[i]));
        h_out->SetBinContent(bin+2, fabs(pois_down[i]));

        h_out->GetXaxis()->SetBinLabel(bin, pois[i]->GetName());
        h_out->GetXaxis()->SetBinLabel(bin+1, "poi_up");
        h_out->GetXaxis()->SetBinLabel(bin+2, "poi_down");
        bin += 3;
    }

    fout.Write();
    fout.Close();

    timer_breakdown.Stop();
    timer_breakdown.Print();
}

// ____________________________________________________________________________|__________
// Set parameters constant or floating, depending on technique
void setParams(RooArgSet* nuis, list<string> tmp_list, string technique, RooFitResult* fitresult, RooRealVar* poi, double corrCutoff) {
    RooRealVar* var;
    TIterator* nitr = nuis->createIterator();

    while ((var = (RooRealVar*)nitr->Next())) {
        string varName = string(var->GetName());

        if (technique.find("sub") != string::npos) {
            var->setConstant(0);
        } else {
            var->setConstant(1);
        }

        bool found = 0;
        if (find(tmp_list.begin(), tmp_list.end(), varName) != tmp_list.end()) {
            LOG(logDEBUG) << "Found " << varName;
            found = 1;
        }

        if (found) {
            if (technique.find("sub") != string::npos) {
                var->setConstant(1);
            } else {
                var->setConstant(0);
            }
        }

        LOG(logDEBUG) << varName << " is constant -> " << var->isConstant();

        double correlation = fitresult->correlation(varName.c_str(), poi->GetName());

        LOG(logDEBUG) << "Correlation between poi and " << varName << " is " << correlation;

        if (abs(correlation) < corrCutoff) {
            LOG(logWARNING) << "Setting " << varName << " constant because it's not correlated to the POI.";
            var->setConstant(1);
        }
    }
}

// ____________________________________________________________________________|__________
// Add parameters to a list of nuisance parameters
list<string> addParams(settings* config, string catecory2eval) {
    list<string> tmp_list;

    TXMLEngine* xml = new TXMLEngine;
    XMLDocPointer_t xmldoc = xml->ParseFile(config->xmlName.c_str());
    if (!xmldoc) {
        LOG(logERROR) << "XML " << config->xmlName << " doesn't exist!";
        exit(1);
    }

    XMLNodePointer_t mainnode = xml->DocGetRootElement(xmldoc);
    XMLNodePointer_t category = xml->GetChild(mainnode);

    while (category != 0) {
        string categoryName = xml->GetNodeName(category);

        if (categoryName.find(catecory2eval) == string::npos) {
            LOG(logINFO) << "skipping " << categoryName;
            category = xml->GetNext(category);
        } else {
            bool breakdown = (string(xml->GetAttr(category, "breakdown")).find("yes") != string::npos) ? 1 : 0;

            XMLNodePointer_t systematic = xml->GetChild(category);
        
            if (config->technique.find("add") != string::npos && catecory2eval.find("statistical") == string::npos) {
                LOG(logINFO) << "Adding statistical parameters";
                tmp_list = addParams(config, "statistical");
            }

            while (systematic != 0) {
                XMLAttrPointer_t attr_syst = xml->GetFirstAttr(systematic);    
                while (attr_syst != 0) {
                    string systName = xml->GetAttrValue(attr_syst);

                    tmp_list.push_back(systName);
                    
                    if (breakdown) {
                        LOG(logINFO) << "Doing breakdown: " << systName;
                        writeTmpXML(systName, config);
			//                        runBreakdown(config->inFileName, config->wsName, config->modelConfigName, config->dataName, config->poiName, "config/tmp_"+systName+".xml", config->technique, systName, config->precision, config->corrCutoff, config->folder, config->loglevel);
                    }
                    attr_syst = xml->GetNextAttr(attr_syst);  
                }
                systematic = xml->GetNext(systematic);
            }
            category = xml->GetNext(category);
        }
    }
    return tmp_list;
}

// ___________________________________________________________________________|__________
// Write temporary XML for a single parameter
void writeTmpXML (string systName, settings* config) {
    // add the interesting category
    TXMLEngine* xml = new TXMLEngine;

    XMLNodePointer_t mainnode = xml->NewChild(0, 0, "breakdown");
    XMLAttrPointer_t description_main = xml->NewAttr(mainnode, 0, "description", "map of tmp uncertainties");

    XMLNodePointer_t child = xml->NewChild(mainnode, 0, systName.c_str());
    XMLAttrPointer_t description_child = xml->NewAttr(child, 0, "description", systName.c_str());
    XMLAttrPointer_t breakdown_child = xml->NewAttr(child, 0, "breakdown", "no");
    XMLAttrPointer_t scan_child = xml->NewAttr(child, 0, "scan", "no");

    XMLNodePointer_t subchild = xml->NewChild(child, 0, "systematic");
    XMLAttrPointer_t name_subchild = xml->NewAttr(subchild, 0, "name", systName.c_str());
 
    // add the statistical parameters as defined in top level xml
    XMLNodePointer_t child_stat = xml->NewChild(mainnode, 0, "statistical");
    XMLAttrPointer_t description_child_stat = xml->NewAttr(child_stat, 0, "description", "statistical uncertainties");
    XMLAttrPointer_t breakdown_child_stat = xml->NewAttr(child_stat, 0, "breakdown", "no");
    XMLAttrPointer_t scan_child_stat = xml->NewAttr(child_stat, 0, "scan", "no");

    TXMLEngine* xml_top = new TXMLEngine;
    XMLDocPointer_t xmldoc_top = xml_top->ParseFile(config->xmlName.c_str());

    XMLNodePointer_t mainnode_top = xml_top->DocGetRootElement(xmldoc_top);
    XMLNodePointer_t category_top = xml_top->GetChild(mainnode_top);

    while (category_top != 0) {
        string categoryName = xml_top->GetNodeName(category_top);

        if (categoryName.find("statistical") != string::npos) {
            xml->AddChild(child_stat, xml->GetChild(category_top));
        }
        category_top = xml_top->GetNext(category_top);
    }

    // save the new tmp xml
    XMLDocPointer_t xmldoc = xml->NewDoc();
    xml->DocSetRootElement(xmldoc, mainnode);
   
    xml->SaveDoc(xmldoc, ("config/tmp_"+systName+".xml").c_str());
    
    xml->FreeDoc(xmldoc);
    delete xml;
}
