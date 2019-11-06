#include "LFVlephad/LepHadObservable.h"

// uncomment the following line to enable debug printouts
//#define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);


// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"

// ClassImp(LepHadObservable)

//______________________________________________________________________________________________

LepHadObservable::LepHadObservable(){
  // default constructor
 
  this->setExpression(this->GetName() );
  
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

LepHadObservable::~LepHadObservable(){
  // default destructor
  DEBUGclass("destructor called");
} 


//______________________________________________________________________________________________

TObjArray* LepHadObservable::getBranchNames() const {
  // retrieve the list of branch names 
  // ownership of the list belongs to the caller of the function
  DEBUGclass("retrieving branch names");
  TObjArray* bnames = new TObjArray();

  //bnames->SetOwner(true);

  // add the branch names needed by your observable here, e.g.
  // bnames->Add(new TObjString("someBranch"));
  
  bnames->Add(new TObjString("tau_0_pt"));
  bnames->Add(new TObjString("jet_0_pt"));
  bnames->Add(new TObjString("jet_0_eta"));
  bnames->Add(new TObjString("lep_0_pt"));

  if (isData()) {
    bnames->Add(new TObjString("run_number"));
  } else {
    bnames->Add(new TObjString("PRW_DATASF_1up_pileup_random_run_number"));
    bnames->Add(new TObjString("PRW_DATASF_1down_pileup_random_run_number"));
    bnames->Add(new TObjString("NOMINAL_pileup_random_run_number"));
  }

  return bnames;
}

//______________________________________________________________________________________________

LepHadObservable::LepHadObservable(const TString& expression): TQTreeObservable(expression)
{
  // constructor with expression argument
  DEBUGclass("constructor called with '%s'",expression.Data());
  // the predefined string member "expression" allows your observable to store an expression of your choice
  // this string will be the verbatim argument you passed to the constructor of your observable
  // you can use it to choose between different modes or pass configuration options to your observable
  this->SetName(TQObservable::makeObservableName(expression));
  this->setExpression(expression);
}

//______________________________________________________________________________________________

const TString& LepHadObservable::getExpression() const {
  // retrieve the expression associated with this observable
  return this->fExpression;
}

//______________________________________________________________________________________________

bool LepHadObservable::hasExpression() const {
  // check if this observable type knows expressions
  return true;
}

//______________________________________________________________________________________________

void LepHadObservable::setExpression(const TString& expr){
  // set the expression to a given string
  this->fExpression = expr;
}
//______________________________________________________________________________________________

bool LepHadObservable::parseExpression(const TString& expr){
  // parse the expression
  return true;
}

//______________________________________________________________________________________________

void LepHadObservable::clearParsedExpression(){
  // clear the current expression
}

//______________________________________________________________________________________________

TString LepHadObservable::getActiveExpression() const {
  // retrieve the expression associated with this incarnation
  
  return this->getExpression();
}

//______________________________________________________________________________________________
bool LepHadObservable::isSherpa() const {
  return _isSherpa;
}

//______________________________________________________________________________________________
bool LepHadObservable::isData() const {
  return _isData;
}

//______________________________________________________________________________________________
bool LepHadObservable::isMC() const {
  return !_isData;
}
//______________________________________________________________________________________________
bool LepHadObservable::isFake() const {
  return _isFake;
}

//______________________________________________________________________________________________
bool LepHadObservable::is2015() const {
  if (!_use2015) { return false; }

  long xrn = x_run_number->EvalInstance();
  return xrn <= LIMIT_2016 && xrn > 0;
}

//______________________________________________________________________________________________
bool LepHadObservable::is2016() const {
  if (!_use2016) { return false; }

  long xrn = x_run_number->EvalInstance();
  return xrn > LIMIT_2016;
}

//______________________________________________________________________________________________
bool LepHadObservable::is2016_uptoD3() const {
  if (!_use2016) { return false; }

  long xrn = x_run_number->EvalInstance();
  return xrn <= LIMIT_D3 && xrn > LIMIT_2016;
}

//______________________________________________________________________________________________
bool LepHadObservable::is2016_postD3() const {
  if (!_use2016) { return false; }

  long xrn = x_run_number->EvalInstance();
  return xrn > LIMIT_D3;
}

//______________________________________________________________________________________________
bool LepHadObservable::isMuon() const {
  return n_muons->EvalInstance() > 0 && n_electrons->EvalInstance() == 0;
}

//______________________________________________________________________________________________
bool LepHadObservable::isElectron() const {
  return n_muons->EvalInstance() == 0 && n_electrons->EvalInstance() > 0;
}

//______________________________________________________________________________________________
bool LepHadObservable::isTLT() const {
  if (!_useTLT) { return false; }

  if (!(tau_0_pt->EvalInstance() > 30.
      && jet_0_pt->EvalInstance() > 70.
      && fabs(jet_0_eta->EvalInstance()) < 3.2)) {
    return false;
  }

  if (isMuon()) {
    if (is2015()) {
      return (lep_0_pt->EvalInstance() > 14.7 && lep_0_pt->EvalInstance() < 21.);
    } else if (is2016()) {
      return (lep_0_pt->EvalInstance() > 14.7 && lep_0_pt->EvalInstance() < 25.2);
    }
  } else if (isElectron()) {
    return (lep_0_pt->EvalInstance() > 18. && lep_0_pt->EvalInstance() < 25.);
  }

  return false;
}

//______________________________________________________________________________________________
bool LepHadObservable::isSLT() const {
  if (!_useSLT) { return false; }
  /*
  if (isMuon()) {
    if (is2015()) {
      return lep_0_pt->EvalInstance() > 21.;
    } else if (is2016_uptoD3()) {
      return lep_0_pt->EvalInstance() > 25.2;
    } else if (is2016_postD3()) {
      return lep_0_pt->EvalInstance() > 27.3;
    }
  } else if (isElectron()) {
    if (is2015() || is2016_uptoD3()) {
      return lep_0_pt->EvalInstance() > 25.;
    } else if (is2016_postD3()) {
      return lep_0_pt->EvalInstance() > 27.;
    }
  }

  return false;
  */
  return true;
}

//______________________________________________________________________________________________
bool LepHadObservable::initializeSelf(){
  // initialize self - compile container name, construct accessor
  if(!this->parseExpression(TQObservable::compileExpression(this->fExpression,this->fSample))){
    return false;
  }

  if (!this->fSample->getTag("~isFailID", _isFake)) {
    ERROR("tag isFailID missing");
    return false;
  }
  if (!this->fSample->getTag("~isData", _isData)) {
    ERROR("tag isData missing");
    return false;
  }

  if (!this->fSample->getTag("~isSherpa", _isSherpa)) {
    ERROR("tag isSherpa missing");
    return false;
  }

  if (!this->fSample->getTag("~use2016", _use2016)) {
    WARN("tag use2016 missing, will use 2016 by default.");
  }

  if (!this->fSample->getTag("~use2015", _use2015)) {
    WARN("tag use2015 missing, will use 2015 by default.");
  }

  if (!this->fSample->getTag("~useSLT", _useSLT)) {
    WARN("tag useSLT missing, will use SLT by default.");
  }

  if (!this->fSample->getTag("~useTLT", _useTLT)) {
    WARN("tag useTLT missing, will use TLT by default.");
  }

  if (!this->fSample->getTag("~weightvariation", _weightvariation)) {
    ERROR("tag weightvariation missing.");
    return false;
  }

  _pileup_low = _weightvariation == "pu_prw_low";
  _pileup_high = _weightvariation == "pu_prw_high";
  _pileup = _pileup_low || _pileup_high;

  // if (isData()) {
  //   INFO("this is data");
  // } else {
  //   INFO("this is MC");
  // }

  // if (isFake()) {
  //   INFO("this is fake");
  // } else {
  //   INFO("this is non-fake");
  // }


  this->tau_0_pt = new TTreeFormula( "tau_0_pt", "tau_0_pt", this->fTree);
  this->jet_0_pt = new TTreeFormula( "jet_0_pt", "jet_0_pt", this->fTree);
  this->jet_0_eta = new TTreeFormula( "jet_0_eta", "jet_0_eta", this->fTree);
  this->lep_0_pt = new TTreeFormula( "lep_0_pt", "lep_0_pt", this->fTree);

  this->n_muons = new TTreeFormula( "n_muons", "n_muons", this->fTree);
  this->n_electrons = new TTreeFormula( "n_electrons", "n_electrons", this->fTree);

  if (isData()) {
    this->x_run_number = new TTreeFormula("run_number", "run_number", this->fTree);
  } else {
    if (_pileup_high) {
      this->x_run_number = new TTreeFormula("PRW_DATASF_1up_pileup_random_run_number", "PRW_DATASF_1up_pileup_random_run_number", this->fTree);
    } else if (_pileup_low) {
      this->x_run_number = new TTreeFormula("PRW_DATASF_1down_pileup_random_run_number", "PRW_DATASF_1down_pileup_random_run_number", this->fTree);
    } else {
      this->x_run_number = new TTreeFormula("NOMINAL_pileup_random_run_number", "NOMINAL_pileup_random_run_number", this->fTree);
    }
  }

  return true;
}
 
//______________________________________________________________________________________________

bool LepHadObservable::finalizeSelf(){
  // finalize self - delete accessor
  this->clearParsedExpression();
  
  delete this->n_muons;
  delete this->n_electrons;

  delete this->tau_0_pt;
  delete this->jet_0_pt;
  delete this->jet_0_eta;
  delete this->lep_0_pt;
  delete this->x_run_number;

  return true;
}
