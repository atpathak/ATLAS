#include "QFramework/TQTreeFormulaObservable.h"
#include "QFramework/TQUtils.h"

// #define _DEBUG_
#include "QFramework/TQLibrary.h"

ClassImp(TQTreeFormulaObservable)

//______________________________________________________________________________________________

TQTreeFormulaObservable::TQTreeFormulaObservable() :
TQTreeObservable()
{
  // default constructor
}

//______________________________________________________________________________________________

TQTreeFormulaObservable::TQTreeFormulaObservable(const TString& expression):
  TQTreeObservable(expression)
{
  // default constructor
  DEBUGclass("constructor called with expression '%s'",expression.Data());
  this->setExpression(expression);
}

//______________________________________________________________________________________________

bool TQTreeFormulaObservable::initializeSelf(){
  // initialize the formula of this observable
  DEBUGclass("initializing...");
  if(fExpression.IsNull()){
    ERRORclass("expression is NULL, terminating");
    return false;
  }

  if(!this->fTree){
    DEBUGclass("no tree, terminating");
    return false;
  }
 
  TString expr = this->getCompiledExpression(fSample);

  // under some circumstances, it is necessary to call TTreeFormula::GetNdata() on each event
  // here, we check whether any of these conditions are given to not waste any performance
  this->fcallGetNdata = false;
  // https://root.cern.ch/phpBB3/viewtopic.php?f=14&t=16775&p=72308
  if(expr.Contains("MinIf")) this->fcallGetNdata = true;
  // https://sft.its.cern.ch/jira/browse/ROOT-7465
  if(expr.Contains("[")) this->fcallGetNdata = true;

  // if the formula is extremly long put a safe guard in terms of number of operators
  int totalNop = expr.CountChar('*') + expr.CountChar('/') + expr.CountChar('+') + expr.CountChar('-') + expr.CountChar('(');
  if (totalNop> 400){
    TTreeFormula::SetMaxima(10000,1000,1000);
  }

  DEBUGclass("for observable '%s', creating formula with expression '%s'",this->GetName(),expr.Data());
  //  if(!gAllowErrorMessages) TQLibrary::redirect_stdout("/dev/null");
  this->fFormula = new TTreeFormula(this->GetName(),expr.Data(),this->fTree);
  //  if(!gAllowErrorMessages) TQLibrary::restore_stdout();
  
  if(!this->fFormula || !this->fFormula->GetTree()){
    ERRORclass("An error occured while creating the formula for TTreeFormulaObservable with expression '%s' created from raw expression '%s'.",expr.Data(),this->getExpression().Data());
    ERRORclass("This could have multiple reasons: Either you used a non-existing branch name or misspelled it. If parts of the (raw) expression are not supposed to be read as branch name(s) but, e.g., a dedicated (custom) observable then the creation of this instance of TQTreeFormulaObservable was performed erroneously. When an observable is requested the following steps listed below are attempted. The default fallback is the creation of a TTreeFormulaObservable.");
    TString expression(this->getExpression());
    expression.ReplaceAll("\\","");
    TString incarnation = TQObservable::unreplaceBools(TQObservable::compileExpression(this->getExpression(),this->fSample,true));
    TString obsname(TQObservable::makeObservableName(expression));
    TString incname(TQObservable::makeObservableName(incarnation));
    ERRORclass("The following steps have been attempted in the observable retrieval:");
    ERRORclass("1) Trying to find a) existing observable with name matching '%s' or",incname.Data());
    ERRORclass("                  b) already existing and initialized observable with activeExpression matching '%s' or",incarnation.Data()); 
    ERRORclass("                  c) already existing and UNinitialized observable expression matching '%s' or",expression.Data()); 
    ERRORclass("2) Trying to find existing observable instances which would yield a compiledExpression matching '%s' if initialized with the provided tags (tags see above). either such an observable was not found or the best match has a factory (in which case we do not clone an existing instance but ask the factory to create a new instance.",incarnation.Data());
    ERRORclass("3) No factory but the TQTreeFormulaObservable one managed to produce a working observable instance."); 
    ERRORclass("A few points to try:");
    ERRORclass("- Are there some square brackets missing? Subexpressions inside [...] are considered as a subobservable. If multiple levels of [] are present this is evaluated recursively. Instead of 'myCustomObservable + myBranchName/1000.' you should write '[myCustomObservable] + [myBranchName]/1000.' .\n The same applies if none of the involved sub expressions corresponds to a branch name:" );
    ERRORclass("- Did you forget to inject an instance of your custom observable at runtime via TQObservable::addObservable? Some analysis packages do this via small python snippets.");
    return false;
  } else {
    DEBUGclass("successfully initialized observable");
  }

  if (totalNop> 400) TTreeFormula::SetMaxima(1000,1000,1000);

  return true;
}

//______________________________________________________________________________________________

bool TQTreeFormulaObservable::finalizeSelf(){
  // finalize self, delete TTreeFormula member
  if (fFormula) {
    delete fFormula;
    fFormula = 0;
  }
  return true;
}

//______________________________________________________________________________________________

TString TQTreeFormulaObservable::getActiveExpression() const {
  // retrieve the expression associated with this observable
  if(this->fFormula){
    return this->fFormula->GetTitle();
  }
  return TQStringUtils::emptyString;
}

//______________________________________________________________________________________________

TQTreeFormulaObservable::~TQTreeFormulaObservable(){
  if(this->fFormula) delete this->fFormula;
}

//______________________________________________________________________________________________

double TQTreeFormulaObservable::getValue() const {
  // retrieve the value of this tree observable
  // return NaN in case of failure
  if(!this->fFormula){
    WARNclass("in observable '%s', returning NaN!",this->GetName());
    return std::numeric_limits<double>::quiet_NaN();
  }
  
  if(this->fcallGetNdata){
    // if we really need to, we call this function
    if(fFormula->GetNdata() == 0){
      return std::numeric_limits<double>::quiet_NaN();
    }
  }
  
#ifdef _DEBUG_
  try {
#endif
    // actually calculate the value
    const double retval = fFormula->EvalInstance();
    // return the result
    DEBUGclass("retrieved value '%f' from expression '%s'",retval,this->getActiveExpression().Data());
    return retval;
#ifdef _DEBUG_
  } catch (std::exception& e){
    ERRORclass("in observable '%s', encountered error: %s",this->getActiveExpression().Data(),e.what());
  }
  return std::numeric_limits<double>::quiet_NaN();
#endif
}

//______________________________________________________________________________________________

TObjArray* TQTreeFormulaObservable::getBranchNames() const {
  // retrieve the list of branch names 
  // ownership of the list belongs to the caller of the function
  return TQUtils::getBranchNames(this->getCompiledExpression(this->fSample));
}

//______________________________________________________________________________________________

const TString& TQTreeFormulaObservable::getExpression() const {
  // retrieve the expression associated with this observable
  return this->fExpression;
}

//______________________________________________________________________________________________

bool TQTreeFormulaObservable::hasExpression() const {
  // check if this observable type knows expressions (default false)
  return true;
}

//______________________________________________________________________________________________

void TQTreeFormulaObservable::setExpression(const TString& expr){
  // set the expression to a given string
  this->fExpression = TQStringUtils::compactify(expr);
}

//______________________________________________________________________________________________

DEFINE_OBSERVABLE_FACTORY(TQTreeFormulaObservable,const TString& expr){
  // try to create an instance of this observable from the given expression
  return new TQTreeFormulaObservable(expr);
}
