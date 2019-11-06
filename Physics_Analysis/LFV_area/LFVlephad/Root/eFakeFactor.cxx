#include "LFVlephad/eFakeFactor.h"
#include <limits>
#include "TVector2.h"

// uncomment the following line to enable debug printouts
//#define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);

// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"

ClassImp(eFakeFactor)

//______________________________________________________________________________________________

eFakeFactor::eFakeFactor(){
  // default constructor
 
  this->setExpression(this->GetName() );
  
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

eFakeFactor::~eFakeFactor(){
  // default destructor
  DEBUGclass("destructor called");
} 


//______________________________________________________________________________________________

TObjArray* eFakeFactor::getBranchNames() const {
  // retrieve the list of branch names 
  // ownership of the list belongs to the caller of the function
  DEBUGclass("retrieving branch names");
  TObjArray* bnames = LepHadObservable::getBranchNames();

  //bnames->SetOwner(true);

  // add the branch names needed by your observable here, e.g.
  // bnames->Add(new TObjString("someBranch"));
  
  bnames->Add(new TObjString("tau_0_n_tracks"));
  bnames->Add(new TObjString("tau_0_phi"));
  
  return bnames;
}

//______________________________________________________________________________________________

double eFakeFactor::getValue() const {
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

  double FF_W = 0.;

  double factor = 1.;

  double FF_W_Err = 0.;

  double factor_Err = 1.;

  Int_t binFF = 0;

  TAxis* xAxisFF = FF_W_1Prong_SLT_2016->GetXaxis();
  double maxBinFF = xAxisFF->GetBinUpEdge(xAxisFF->GetLast());

  TH1D* FF_W_Hist = NULL;

  //if (tau_0_n_tracks != 1 || isMuon()) 
  //return 0.;

  FF_W_Hist = FF_W_1Prong_SLT_2016;

  if (tau_0_pt > maxBinFF) {
    binFF = FF_W_Hist->FindBin(maxBinFF - 0.0001);
  } else {
    binFF = FF_W_Hist->FindBin(tau_0_pt);
  }
  
  binFF = FF_W_Hist->FindBin(tau_0_pt);

  FF_W = FF_W_Hist->GetBinContent(binFF);
  factor = FF_W;
  factor_Err = FF_W_Err;

  if (var == "FFup") factor += factor_Err/2.;//factor *= 1.10;
  else if (var == "FFdown") factor -= factor_Err/2.; //factor /= 1.10;
  //else if (var == "SR2up") factor *= 1.10;
  //else if (var == "SR2down") factor /= 1.10;
  //else if (var == "SR3up") factor *= 1.2;
  //else if (var == "SR3down") factor /= 1.2;
  else if (var == "IDup") factor *= 1.10;
  else if (var == "IDdown") factor /= 1.10;
  //else if (var == "SR2Rup") factor /= 1.14;
  //else if (var == "SR2Rdown") factor /= 1.14;
  //else if (var == "SR3Rup") factor /= 1.15;
  //else if (var == "SR3Rdown") factor /= 1.15;
  else if (var == "MCup") factor *= 1.05;
  else if (var == "MCdown") factor /= 1.05;
  //else if (var == "SR2MCup") factor /= 1.05;
  //else if (var == "SR2MCdown") factor /= 1.05;
  //else if (var == "SR3MCup") factor /= 1.10;
  //else if (var == "SR3MCdown") factor /= 1.10;

  //if (factor < 0.) factor = 0.;
  
  DEBUGclass("tauPt: %f", tau_0_pt);
  DEBUGclass("FF_W: %f", FF_W);
  DEBUGclass("returning");

  return factor;
}
//______________________________________________________________________________________________

eFakeFactor::eFakeFactor(TString expression, TString ffFilePath):
    LepHadObservable(expression) {
  // constructor with expression argument
  DEBUGclass("constructor called with '%s'",expression.Data());
  // the predefined string member "expression" allows your observable to store an expression of your choice
  // this string will be the verbatim argument you passed to the constructor of your observable
  // you can use it to choose between different modes or pass configuration options to your observable
  this->SetName(TQObservable::makeObservableName(expression));
  this->setExpression(expression);

  if(expression.Contains("efake_FF_high")){
    var = "FFup";
  } else if(expression.Contains("efake_FF_low")){
    var = "FFdown";
    /*  } else if(expression.Contains("efake_SR2_high")){
    var = "SR2up";
  } else if(expression.Contains("efake_SR2_low")){
    var = "SR2down";
  } else if(expression.Contains("efake_SR3_high")){
    var = "SR3up";
  } else if(expression.Contains("efake_SR3_low")){
    var = "SR3down";
    */
  } else if(expression.Contains("efake_ID_high")){
    var = "IDup";
  } else if(expression.Contains("efake_ID_low")){
    var = "IDdown";
    /*  } else if(expression.Contains("efake_SR2_ID_high")){
    var = "SR2Rup";
  } else if(expression.Contains("efake_SR2_ID_low")){
    var = "SR2Rdown";
  } else if(expression.Contains("efake_SR3_ID_high")){
    var = "SR3Rup";
  } else if(expression.Contains("efake_SR3_ID_low")){
    var = "SR3Rdown";
    */
  } else if(expression.Contains("efake_MC_high")){
    var = "MCup";
  } else if(expression.Contains("efake_MC_low")){
    var = "MCdown";
    /*  } else if(expression.Contains("efake_SR2_MC_high")){
    var = "SR2MCup";
  } else if(expression.Contains("efake_SR2_MC_low")){
    var = "SR2MCdown";
  } else if(expression.Contains("efake_SR3_MC_high")){
    var = "SR3MCup";
  } else if(expression.Contains("efake_SR3_MC_low")){
    var = "SR3MCdown";
    */
  } else var = "";
  
  ffFile = TFile::Open(ffFilePath, "READ");
  if (!ffFile) {
    ERROR("Can't open FF file");
  }

  std::cout<<"FF START GETTING\n";

  FF_W_1Prong_SLT_2016 = (TH1D*)ffFile->Get("tauPt");

  FF_W_1Prong_SLT_2016->SetDirectory(0);

  ffFile->Close();
    
}

//______________________________________________________________________________________________

bool eFakeFactor::initializeSelf(){
  if (!LepHadObservable::initializeSelf()) {
      return false;
  }

  // this->bla = new TTreeFormula( "name", "formula", this->fTree);
  this->tau_0_n_tracks = new TTreeFormula( "tau_0_n_tracks", "tau_0_n_tracks", this->fTree);
  this->tau_0_pt = new TTreeFormula( "tau_0_pt", "tau_0_pt", this->fTree);

  return true;
}
 
//______________________________________________________________________________________________

bool eFakeFactor::finalizeSelf(){
  // finalize self - delete accessor
  this->clearParsedExpression();

  delete this->tau_0_n_tracks;
  delete this->tau_0_pt;

  return true;
}
