#include "LFVlephad/fakeFactor.h"
#include <limits>
#include "TVector2.h"
#include "TRandom.h"

// uncomment the following line to enable debug printouts
//#define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);

// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"

ClassImp(fakeFactor)

//______________________________________________________________________________________________

fakeFactor::fakeFactor(){
  // default constructor
 
  this->setExpression(this->GetName() );
  
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

fakeFactor::~fakeFactor(){
  // default destructor
  DEBUGclass("destructor called");
} 


//______________________________________________________________________________________________

TObjArray* fakeFactor::getBranchNames() const {
  // retrieve the list of branch names 
  // ownership of the list belongs to the caller of the function
  DEBUGclass("retrieving branch names");
  TObjArray* bnames = LepHadObservable::getBranchNames();

  //bnames->SetOwner(true);

  // add the branch names needed by your observable here, e.g.
  // bnames->Add(new TObjString("someBranch"));
  
  bnames->Add(new TObjString("tau_0_n_tracks"));
  bnames->Add(new TObjString("tau_0_phi"));
  bnames->Add(new TObjString("met_reco_phi"));
  
  return bnames;
}

//______________________________________________________________________________________________

double fakeFactor::getValue() const {
  // in the rest of this function, you should retrieve the data and calculate your return value
  // here is the place where most of your custom code should go
  // a couple of comments should guide you through the process
  // when writing your code, please keep in mind that this code can be executed several times on every event
  // make your code efficient. catch all possible problems. when in doubt, contact experts!
  // here, you should calculate your return value
  // of course, you can use other data members of your observable at any time
  /* example block for TTreeFormula method:
  const double retval = this->fFormula->Eval(0.);
  */
  /* exmple block for TTree::SetBranchAddress method:
  const double retval = this->fBranch1 + this->fBranch2;
  */

  //std::cout<<" In getValue "<<std::endl;

  
  double tau_0_n_tracks = this->tau_0_n_tracks->EvalInstance();
  double tau_0_pt = this->tau_0_pt->EvalInstance();
  double tau_0_phi = this->tau_0_phi->EvalInstance();
  double met_reco_phi = this->met_reco_phi->EvalInstance();
  double met_reco_et = this->met_reco_et->EvalInstance();
  double lephad_mt_lep0_met = this->lephad_mt_lep0_met->EvalInstance();
  double lep_0_pt = this->lep_0_pt->EvalInstance();

  double dPhiTauMET = -99.;

  double FF_W = 0.;
  double FF_Top = 0.;
  double FF_Z = 0;
  double FF_QCD = 0.;

  double R_W = 0.;
  double R_Top = 0.;
  double R_Z = 0;
  double R_QCD = 0.;

  double factor = 1.;

  double FF_W_Err = 0.;
  double FF_Top_Err = 0.;
  double FF_Z_Err = 0;
  double FF_QCD_Err = 0.;

  double R_W_Err = 0.;
  double R_Top_Err = 0.;
  double R_Z_Err = 0;
  double R_QCD_Err = 0.;

  double factor_Err = 1.;

  Int_t binFF = 0;
  Int_t binR = 0;
  Int_t binSF = 0;
  Int_t binSF1 = 0;

  TAxis* xAxisFF = FF_W_1Prong_SLT_2016->GetXaxis();
  double maxBinFF = xAxisFF->GetBinUpEdge(xAxisFF->GetLast());

  TAxis* xAxisR = R_W_1Prong_SLT_2016->GetXaxis();
  double maxBinR = xAxisR->GetBinUpEdge(xAxisR->GetLast());

  TAxis* xAxisSF = SF->GetXaxis();
  double maxBinSF = xAxisSF->GetBinUpEdge(xAxisSF->GetLast());
  //TAxis* xAxisSF1 = SF1->GetXaxis();
  //double maxBinSF1 = xAxisSF1->GetBinUpEdge(xAxisSF1->GetLast());

  TH1D* FF_W_Hist = NULL;
  TH1D* FF_Top_Hist = NULL;
  TH1D* FF_Z_Hist = NULL;
  TH1D* FF_QCD_Hist = NULL;

  TH1D* R_W_Hist = NULL;
  TH1D* R_Top_Hist = NULL;
  TH1D* R_Z_Hist = NULL;
  TH1D* R_QCD_Hist = NULL;
  
  dPhiTauMET = fabs(TVector2::Phi_mpi_pi(tau_0_phi - met_reco_phi));

  if (isSLT()) {
    if (tau_0_n_tracks == 1) {
      if (isMuon()) {
	FF_W_Hist = FF_W_1Prong_SLT_2015;
	//FF_Top_Hist = FF_Top_1Prong_SLT_2015;
	//FF_Z_Hist = FF_Z_1Prong_SLT_2015;
	FF_QCD_Hist = FF_QCD_1Prong_SLT_2015;
	
	R_W_Hist = R_W_1Prong_SLT_2015;
	//R_Top_Hist = R_Top_1Prong_SLT_2015;
	//R_Z_Hist = R_Z_1Prong_SLT_2015;
	R_QCD_Hist = R_QCD_1Prong_SLT_2015;
	} else {
	FF_W_Hist = FF_W_1Prong_SLT_2016;
	//FF_Top_Hist = FF_Top_1Prong_SLT_2016;
	//FF_Z_Hist = FF_Z_1Prong_SLT_2016;
	FF_QCD_Hist = FF_QCD_1Prong_SLT_2016;
	
	R_W_Hist = R_W_1Prong_SLT_2016;
	//R_Top_Hist = R_Top_1Prong_SLT_2016;
	//R_Z_Hist = R_Z_1Prong_SLT_2016;
	R_QCD_Hist = R_QCD_1Prong_SLT_2016;
      }
    } else {
      if (isMuon()) {
	FF_W_Hist = FF_W_3Prong_SLT_2015;
	//FF_Top_Hist = FF_Top_3Prong_SLT_2015;
	//FF_Z_Hist = FF_Z_3Prong_SLT_2015;
	FF_QCD_Hist = FF_QCD_3Prong_SLT_2015;
	
	R_W_Hist = R_W_3Prong_SLT_2015;
	//R_Top_Hist = R_Top_3Prong_SLT_2015;
	//R_Z_Hist = R_Z_3Prong_SLT_2015;
	R_QCD_Hist = R_QCD_3Prong_SLT_2015;
	} else { 
	FF_W_Hist = FF_W_3Prong_SLT_2016;
	//FF_Top_Hist = FF_Top_3Prong_SLT_2016;
	//FF_Z_Hist = FF_Z_3Prong_SLT_2016;
	FF_QCD_Hist = FF_QCD_3Prong_SLT_2016;
	
	R_W_Hist = R_W_3Prong_SLT_2016;
	//R_Top_Hist = R_Top_3Prong_SLT_2016;
	//R_Z_Hist = R_Z_3Prong_SLT_2016;
	R_QCD_Hist = R_QCD_3Prong_SLT_2016;
      }
    }
  }

  if (tau_0_pt > maxBinFF) {
    binFF = FF_W_Hist->FindBin(maxBinFF - 0.0001);
  } else {
    binFF = FF_W_Hist->FindBin(tau_0_pt);
  }

  if (lephad_mt_lep0_met > maxBinR) {
    binR = R_W_Hist->FindBin(maxBinR - 0.0001);
  } else {
    binR = R_W_Hist->FindBin(lephad_mt_lep0_met);
  }

  if (lephad_mt_lep0_met > maxBinSF) {
    binSF = SF->FindBin(maxBinSF - 0.0001);
  } else {
    binSF = SF->FindBin(lephad_mt_lep0_met);
  }

  /*if (tau_0_pt > maxBinSF1) {
    binSF1 = SF1->FindBin(maxBinSF1 - 0.0001);
  } else {
    binSF1 = SF1->FindBin(tau_0_pt);
    }*/

  FF_W = FF_W_Hist->GetBinContent(binFF);
  //FF_Top = FF_Top_Hist->GetBinContent(binFF);
  //FF_Z = FF_Z_Hist->GetBinContent(binFF);
  FF_QCD = FF_QCD_Hist->GetBinContent(binFF);

  FF_W_Err = FF_W_Hist->GetBinError(binFF);
  //FF_Top_Err = FF_Top_Hist->GetBinError(binFF);
  //FF_Z_Err = FF_Z_Hist->GetBinError(binFF);
  FF_QCD_Err = FF_QCD_Hist->GetBinError(binFF);

  R_W = R_W_Hist->GetBinContent(binR);
  //R_Top = R_Top_Hist->GetBinContent(binR);
  //R_Z = R_Z_Hist->GetBinContent(binR);
  R_QCD = R_QCD_Hist->GetBinContent(binR);

  R_W_Err = R_W_Hist->GetBinError(binR);
  //R_Top_Err = R_Top_Hist->GetBinError(binR);
  //R_Z_Err = R_Z_Hist->GetBinError(binR);
  R_QCD_Err = R_QCD_Hist->GetBinError(binR);

//if(Top.Contains("MC")){
//  factor = FF_Top;
//  factor_Err = FF_Top_Err;
//}
//  else 
//  if(W.Contains("MC")){
//    factor = FF_W;
//    factor_Err = FF_W_Err;
//  }
//else if(Z.Contains("MC")){
//  factor = FF_Z;
//  factor_Err = FF_Z_Err;
//}
//  else{
  //factor = FF_W*R_W + FF_Top*R_Top + FF_Z*R_Z + FF_QCD*R_QCD;
  //factor = FF_W*R_W + FF_QCD*(1-R_W); //R_QCD; // test

  //factor = FF_W*R_W + FF_QCD*R_QCD; // test2

  factor = FF_W*(1-R_QCD) + FF_QCD*R_QCD; // test 3
  
  factor_Err = pow(pow(R_QCD_Err*FF_W,2)+pow((1-R_QCD)*FF_W_Err,2)
                     +pow(R_QCD_Err*FF_QCD,2)+pow(R_QCD*FF_QCD_Err,2),0.5);

  // factor_Err =  pow(pow(FF_W - FF_QCD,2)  * pow(R_QCD_Err,2) + pow(R_QCD,2) * pow(FF_QCD_Err,2) + pow(1-R_QCD,2) * pow(FF_W_Err,2),0.5);
  
  //factor_Err =  FF_W_Err; //pow(pow(R_W_Err*FF_W,2)+pow(R_W*FF_W_Err,2)
                //   +pow(R_QCD_Err*FF_QCD,2)+pow(R_QCD*FF_QCD_Err,2),0.5);
    
    //factor*=SF->GetBinContent(binSF);

    //factor = FF_W; // new maybe Phuong does this //*R_W + FF_W * SF1->GetBinContent(binSF1) * (1-R_W);

    //factor_Err = pow(pow(R_W_Err*FF_W,2)+pow(R_W*FF_W_Err,2)
    //		     +pow(R_Top_Err*FF_Top,2)+pow(R_Top*FF_Top_Err,2)
    //		     +pow(R_Z_Err*FF_Z,2)+pow(R_Z*FF_Z_Err,2)
    //		     +pow(R_QCD_Err*FF_QCD,2)+pow(R_QCD*FF_QCD_Err,2),0.5);
    //  }
  
  if (var == "SR1up" || var == "SR2up" || var == "SR3up") {
    factor += factor_Err/2.;
    //factor *= 1.17;
  }
  else if (var == "SR1down" || var == "SR2down" || "SR3down") {
    factor -= factor_Err/2.;
    //factor *= 0.83;
  }  

    //    if (lephad_mt_lep0_met > 55.) 
    //  factor *= 1.1;

    /*
    if (var == "SR1up") factor *= 1.15;
    else if (var == "SR1down") factor /= 1.15;
    else if (var == "SR2up") factor *= 1.12;
    else if (var == "SR2down") factor /= 1.12;
    else if (var == "SR3up") factor *= 1.2;
    else if (var == "SR3down") factor /= 1.2;
    else if (var == "SR1Rup") factor *= 1.05;
    else if (var == "SR1Rdown") factor /= 1.05;
    else if (var == "SR2Rup") factor *= 1.04;
    else if (var == "SR2Rdown") factor /= 1.04;
    else if (var == "SR3Rup") factor *= 1.06;
    else if (var == "SR3Rdown") factor /= 1.06;
    else if (var == "SR1MCup") factor *= 1.05;
    else if (var == "SR1MCdown") factor /= 1.05;
    else if (var == "SR2MCup") factor *= 1.05;
    else if (var == "SR2MCdown") factor /= 1.05;
    else if (var == "SR3MCup") factor *= 1.07;
    else if (var == "SR3MCdown") factor /= 1.07;
    */


  //if (cat == "SR3" && lephad_mt_lep0_met < 100)
  //factor *= 0.9+0.003*(lephad_mt_lep0_met-10);
  //if (cat == "SR1" && binR > 2)
  //factor *= (binR-2)*0.06;

  //double ratioPt = lep_0_pt/tau_0_pt;
  //factor /= (0.8061 + 0.1685*ratioPt - 0.0158*ratioPt*ratioPt);

  
  // // "central values that go negative are put at 0.0001, while variations that
  // // go negative are set to 0.00005"
  // if (factor < 0) {
  //   if (var == "") {
  //     factor = NOMINAL_THRESHOLD;
  //   } else {
  //     factor = VARIATION_THRESHOLD;
  //   }
  // }
  
  DEBUGclass("tauPt: %f", tau_0_pt);
  DEBUGclass("dPhiTauMET: %f", dPhiTauMET);
  // DEBUGclass("leptonPt: %f", lep_0_pt->EvalInstance());
  // DEBUGclass("nProngs: %f", tau_0_n_tracks->EvalInstance());
  // DEBUGclass("bin: %i", binx->EvalInstance());
  DEBUGclass("FF_W: %f", FF_W);
//DEBUGclass("FF_Top: %f", FF_Top);
//DEBUGclass("FF_Z: %f", FF_Z);
  DEBUGclass("FF_QCD: %f", FF_QCD);
  DEBUGclass("R_W: %f", R_W);
//DEBUGclass("R_Top: %f", R_Top);
//DEBUGclass("R_Z: %f", R_Z);
  DEBUGclass("R_QCD: %f", R_QCD);
  DEBUGclass("FF: %f", factor);
  
  DEBUGclass("returning");

  return factor;
}
//______________________________________________________________________________________________

fakeFactor::fakeFactor(TString expression, TString ffFilePath, TString rFilePath, TString category, TString region, TString mc):
    LepHadObservable(expression) {
  // constructor with expression argument
  DEBUGclass("constructor called with '%s'",expression.Data());
  // the predefined string member "expression" allows your observable to store an expression of your choice
  // this string will be the verbatim argument you passed to the constructor of your observable
  // you can use it to choose between different modes or pass configuration options to your observable
  this->SetName(TQObservable::makeObservableName(expression));
  this->setExpression(expression);
  
  if(expression.Contains("fake_SR1_high") && category == "SR1"){
    var = "SR1up";
  } else if(expression.Contains("fake_SR1_low") && category == "SR1"){
    var = "SR1down";
  } else if(expression.Contains("fake_SR2_high") && category == "SR2"){
    var = "SR2up";
  } else if(expression.Contains("fake_SR2_low") && category == "SR2"){
    var = "SR2down";
  } else if(expression.Contains("fake_SR3_high") && category == "SR3"){
    var = "SR3up";
  } else if(expression.Contains("fake_SR3_low") && category == "SR3"){
    var = "SR3down";
  } else if(expression.Contains("fake_SR1_R_high") && category == "SR1"){
    var = "SR1Rup";
  } else if(expression.Contains("fake_SR1_R_low") && category == "SR1"){
    var = "SR1Rdown";
  } else if(expression.Contains("fake_SR2_R_high") && category == "SR2"){
    var = "SR2Rup";
  } else if(expression.Contains("fake_SR2_R_low") && category == "SR2"){
    var = "SR2Rdown";
  } else if(expression.Contains("fake_SR3_R_high") && category == "SR3"){
    var = "SR3Rup";
  } else if(expression.Contains("fake_SR3_R_low") && category == "SR3"){
    var = "SR3Rdown";
  } else if(expression.Contains("fake_SR1_MC_high") && category == "SR1"){
    var = "SR1MCup";
  } else if(expression.Contains("fake_SR1_MC_low") && category == "SR1"){
    var = "SR1MCdown";
  } else if(expression.Contains("fake_SR2_MC_high") && category == "SR2"){
    var = "SR2MCup";
  } else if(expression.Contains("fake_SR2_MC_low") && category == "SR2"){
    var = "SR2MCdown";
  } else if(expression.Contains("fake_SR3_MC_high") && category == "SR3"){
    var = "SR3MCup";
  } else if(expression.Contains("fake_SR3_MC_low") && category == "SR3"){
    var = "SR3MCdown";
  } else var = "";

  //if(mc.Contains("MC")){
    //    if(mc.Contains("TopMC")){
    //  Top = "TopMC";
    //}
    //    else 
  if(mc.Contains("WMC")){
      W = "WMC";
    }
    //else if(mc.Contains("ZMC")){
    //  Z = "ZMC";
    //}
  //  }
    
  cat = category;

  ffFile = TFile::Open(ffFilePath, "READ");
  if (!ffFile) {
    ERROR("Can't open FF file");
  }
  //std::cout << " ff root file : " << ffFilePath << std::endl;
  std::cout<<"FF START GETTING\n";
  
  TString category1 = category;
  if(category1 == "SR" || category1 == "SR0")
    category1 = "Preselection";
  FF_W_1Prong_SLT_2015 = (TH1D*)ffFile->Get("mtau_"+W+"_"+category+"_1Prong_SLT_2016");
  FF_W_3Prong_SLT_2015 = (TH1D*)ffFile->Get("mtau_"+W+"_"+category+"_3Prong_SLT_2016");
  FF_W_1Prong_SLT_2016 = (TH1D*)ffFile->Get("etau_"+W+"_"+category+"_1Prong_SLT_2016");
  FF_W_3Prong_SLT_2016 = (TH1D*)ffFile->Get("etau_"+W+"_"+category+"_3Prong_SLT_2016");
  
  /*FF_W_1Prong_SLT_2015 = (TH1D*)ffFile->Get("mtau_W_"+category1+"_1Prong");
  FF_W_3Prong_SLT_2015 = (TH1D*)ffFile->Get("mtau_W_"+category1+"_3Prong");
  FF_W_1Prong_SLT_2016 = (TH1D*)ffFile->Get("etau_W_"+category1+"_1Prong");
  FF_W_3Prong_SLT_2016 = (TH1D*)ffFile->Get("etau_W_"+category1+"_3Prong");
  */
  //FF_Top_1Prong_SLT_2015 = (TH1D*)ffFile->Get(Top+"_"+category+"_1Prong_SLT_2015");
  //FF_Top_3Prong_SLT_2015 = (TH1D*)ffFile->Get(Top+"_"+category+"_3Prong_SLT_2015");
  //FF_Top_1Prong_SLT_2016 = (TH1D*)ffFile->Get(Top+"_"+category+"_1Prong_SLT_2016");
  //FF_Top_3Prong_SLT_2016 = (TH1D*)ffFile->Get(Top+"_"+category+"_3Prong_SLT_2016");

  //FF_Z_1Prong_SLT_2015 = (TH1D*)ffFile->Get(Z+"_"+category+"_1Prong_SLT_2015");
  //FF_Z_3Prong_SLT_2015 = (TH1D*)ffFile->Get(Z+"_"+category+"_3Prong_SLT_2015");
  //FF_Z_1Prong_SLT_2016 = (TH1D*)ffFile->Get(Z+"_"+category+"_1Prong_SLT_2016");
  //FF_Z_3Prong_SLT_2016 = (TH1D*)ffFile->Get(Z+"_"+category+"_3Prong_SLT_2016");

  FF_QCD_1Prong_SLT_2015 = (TH1D*)ffFile->Get("mtau_QCD_"+category+"_1Prong_SLT_2016");
  FF_QCD_3Prong_SLT_2015 = (TH1D*)ffFile->Get("mtau_QCD_"+category+"_3Prong_SLT_2016");
  FF_QCD_1Prong_SLT_2016 = (TH1D*)ffFile->Get("etau_QCD_"+category+"_1Prong_SLT_2016");
  FF_QCD_3Prong_SLT_2016 = (TH1D*)ffFile->Get("etau_QCD_"+category+"_3Prong_SLT_2016");
  
  /*FF_QCD_1Prong_SLT_2015 = (TH1D*)ffFile->Get("mtau_QCD_"+category1+"_1Prong");
  FF_QCD_3Prong_SLT_2015 = (TH1D*)ffFile->Get("mtau_QCD_"+category1+"_3Prong");
  FF_QCD_1Prong_SLT_2016 = (TH1D*)ffFile->Get("etau_QCD_"+category1+"_1Prong");
  FF_QCD_3Prong_SLT_2016 = (TH1D*)ffFile->Get("etau_QCD_"+category1+"_3Prong");
  */
  /*  
  TRandom *random = new TRandom();
  for (int i = 1; i < FF_W_1Prong_SLT_2015->GetNbinsX(); ++i) {
  FF_W_1Prong_SLT_2015->SetBinContent(i, FF_W_1Prong_SLT_2015->GetBinContent(i)*(1+random->Gaus(0,1)*0.03));
  FF_W_3Prong_SLT_2015->SetBinContent(i, FF_W_3Prong_SLT_2015->GetBinContent(i)*(1+random->Gaus(0,1)*0.03));
  FF_W_1Prong_SLT_2016->SetBinContent(i, FF_W_1Prong_SLT_2016->GetBinContent(i)*(1+random->Gaus(0,1)*0.03));
  FF_W_3Prong_SLT_2016->SetBinContent(i, FF_W_3Prong_SLT_2016->GetBinContent(i)*(1+random->Gaus(0,1)*0.03));
  }
  */

  //std::cout<<"FF END GETTING\n";

  rFile = TFile::Open(rFilePath, "READ");
  if (!rFile) {
    ERROR("Can't open R file");
  }
  //std::cout << " R root file : " << rFilePath << std::endl;
  
  TString Rvar = "";
  
  if(expression.Contains("Rvar_high")) {
    Rvar = "_up";
  } else if(expression.Contains("Rvar_low")) {
    Rvar = "_dn";
  }
  
  if(expression.Contains("SS_syst")) {
    if(expression.Contains("VBF")) {
      SSvar = "doSSsystVBF";
    } else if(expression.Contains("BOOST")) {
      SSvar = "doSSsystBOOST";
    }
  } 

  // std::cout<<"PRE REGION "<<category<<"\n";
  // if(category.Contains("Pre")) {
  //   category = "VBFCB";
  // }
  // std::cout<<"AFTER REGION "<<category<<"\n";
  //std::cout<<"R START GETTING\n";
  
  R_W_1Prong_SLT_2015 = (TH1D*)rFile->Get("mtau_W_"+category+"_1Prong_SLT_"+region+"_2016"+Rvar);
  R_W_3Prong_SLT_2015 = (TH1D*)rFile->Get("mtau_W_"+category+"_3Prong_SLT_"+region+"_2016"+Rvar);
  R_W_1Prong_SLT_2016 = (TH1D*)rFile->Get("etau_W_"+category+"_1Prong_SLT_"+region+"_2016"+Rvar);
  R_W_3Prong_SLT_2016 = (TH1D*)rFile->Get("etau_W_"+category+"_3Prong_SLT_"+region+"_2016"+Rvar);
  
  //R_Top_1Prong_SLT_2015 = (TH1D*)rFile->Get("Top_"+category+"_1Prong_SLT_"+region+"_2015"+Rvar);
  //R_Top_3Prong_SLT_2015 = (TH1D*)rFile->Get("Top_"+category+"_3Prong_SLT_"+region+"_2015"+Rvar);
  //R_Top_1Prong_SLT_2016 = (TH1D*)rFile->Get("Top_"+category+"_1Prong_SLT_"+region+"_2016"+Rvar);
  //R_Top_3Prong_SLT_2016 = (TH1D*)rFile->Get("Top_"+category+"_3Prong_SLT_"+region+"_2016"+Rvar);
  
  //R_Z_1Prong_SLT_2015 = (TH1D*)rFile->Get("Z_"+category+"_1Prong_SLT_"+region+"_2015"+Rvar);
  //R_Z_3Prong_SLT_2015 = (TH1D*)rFile->Get("Z_"+category+"_3Prong_SLT_"+region+"_2015"+Rvar);
  //R_Z_1Prong_SLT_2016 = (TH1D*)rFile->Get("Z_"+category+"_1Prong_SLT_"+region+"_2016"+Rvar);
  //R_Z_3Prong_SLT_2016 = (TH1D*)rFile->Get("Z_"+category+"_3Prong_SLT_"+region+"_2016"+Rvar);
  
  R_QCD_1Prong_SLT_2015 = (TH1D*)rFile->Get("mtau_QCD_"+category+"_1Prong_SLT_"+region+"_2016"+Rvar);
  R_QCD_3Prong_SLT_2015 = (TH1D*)rFile->Get("mtau_QCD_"+category+"_3Prong_SLT_"+region+"_2016"+Rvar);
  R_QCD_1Prong_SLT_2016 = (TH1D*)rFile->Get("etau_QCD_"+category+"_1Prong_SLT_"+region+"_2016"+Rvar);
  R_QCD_3Prong_SLT_2016 = (TH1D*)rFile->Get("etau_QCD_"+category+"_3Prong_SLT_"+region+"_2016"+Rvar);
   
  //std::cout<<"R END GETTING\n";


  FF_W_1Prong_SLT_2015->SetDirectory(0);
  FF_W_3Prong_SLT_2015->SetDirectory(0);
  FF_W_1Prong_SLT_2016->SetDirectory(0);
  FF_W_3Prong_SLT_2016->SetDirectory(0);

  //FF_Top_1Prong_SLT_2015->SetDirectory(0);
  //FF_Top_3Prong_SLT_2015->SetDirectory(0);
  //FF_Top_1Prong_SLT_2016->SetDirectory(0);
  //FF_Top_3Prong_SLT_2016->SetDirectory(0);

  //FF_Z_1Prong_SLT_2015->SetDirectory(0);
  //FF_Z_3Prong_SLT_2015->SetDirectory(0);
  //FF_Z_1Prong_SLT_2016->SetDirectory(0);
  //FF_Z_3Prong_SLT_2016->SetDirectory(0);

  FF_QCD_1Prong_SLT_2015->SetDirectory(0);
  FF_QCD_3Prong_SLT_2015->SetDirectory(0);
  FF_QCD_1Prong_SLT_2016->SetDirectory(0);
  FF_QCD_3Prong_SLT_2016->SetDirectory(0);
  
  R_W_1Prong_SLT_2015->SetDirectory(0);
  R_W_3Prong_SLT_2015->SetDirectory(0);
  R_W_1Prong_SLT_2016->SetDirectory(0);
  R_W_3Prong_SLT_2016->SetDirectory(0);

  //R_Top_1Prong_SLT_2015->SetDirectory(0);
  //R_Top_3Prong_SLT_2015->SetDirectory(0);
  //R_Top_1Prong_SLT_2016->SetDirectory(0);
  //R_Top_3Prong_SLT_2016->SetDirectory(0);

  //R_Z_1Prong_SLT_2015->SetDirectory(0);
  //R_Z_3Prong_SLT_2015->SetDirectory(0);
  //R_Z_1Prong_SLT_2016->SetDirectory(0);
  //R_Z_3Prong_SLT_2016->SetDirectory(0);

  R_QCD_1Prong_SLT_2015->SetDirectory(0);
  R_QCD_3Prong_SLT_2015->SetDirectory(0);
  R_QCD_1Prong_SLT_2016->SetDirectory(0);
  R_QCD_3Prong_SLT_2016->SetDirectory(0);

  sFile = TFile::Open("ratio_mediumTau.root", "READ");
  SF = (TH1D*)sFile->Get("transverseMassLepMET");
  SF->SetDirectory(0);

  //sFile1 = TFile::Open("HSG3_histo.root", "READ");
  //SF1 = (TH1D*)sFile1->Get("tauPt");
  //SF1->SetDirectory(0);

  ffFile->Close();
  rFile->Close();
  sFile->Close();
  //sFile1->Close();
    
}

//______________________________________________________________________________________________

bool fakeFactor::initializeSelf(){
  if (!LepHadObservable::initializeSelf()) {
      return false;
  }

  // this->bla = new TTreeFormula( "name", "formula", this->fTree);
  this->tau_0_n_tracks = new TTreeFormula( "tau_0_n_tracks", "tau_0_n_tracks", this->fTree);
  this->tau_0_phi = new TTreeFormula( "tau_0_phi", "tau_0_phi", this->fTree);
  this->met_reco_phi = new TTreeFormula( "met_reco_phi", "met_reco_phi", this->fTree);
  this->met_reco_et = new TTreeFormula( "met_reco_et", "met_reco_et", this->fTree);
  this->lephad_mt_lep0_met = new TTreeFormula( "lephad_mt_lep0_met", "lephad_mt_lep0_met", this->fTree);
  this->tau_0_pt = new TTreeFormula( "tau_0_pt", "tau_0_pt", this->fTree);
  this->lep_0_pt = new TTreeFormula( "lep_0_pt", "lep_0_pt", this->fTree);

  return true;
}
 
//______________________________________________________________________________________________

bool fakeFactor::finalizeSelf(){
  // finalize self - delete accessor
  this->clearParsedExpression();

  delete this->tau_0_n_tracks;
  delete this->tau_0_phi;
  delete this->met_reco_phi;
  delete this->met_reco_et;
  delete this->tau_0_pt;
  delete this->lep_0_pt;

  return true;
}