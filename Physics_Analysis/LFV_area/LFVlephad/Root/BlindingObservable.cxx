#include "LFVlephad/BlindingObservable.h"
#include <limits>
#include <vector>
#include "TTreeFormula.h"
#include "TString.h"
#include "QFramework/TQStringUtils.h"
#include "QFramework/TQUtils.h"

// uncomment the following line to enable debug printouts
// #define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);

// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"

ClassImp(BlindingObservable)

//______________________________________________________________________________________________

BlindingObservable::BlindingObservable(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

BlindingObservable::~BlindingObservable(){
  // default destructor
  DEBUGclass("destructor called");
} 


//______________________________________________________________________________________________

TObjArray* BlindingObservable::getBranchNames() const {
  // retrieve the list of branch names 
  // ownership of the list belongs to the caller of the function
  DEBUGclass("retrieving branch names");
  TString exprArgs = fExpression(6, fExpression.Length() - 7); // remove opening and closing parenthesis
  return TQUtils::getBranchNames(exprArgs);
}

//______________________________________________________________________________________________

double BlindingObservable::getValue() const {
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

  throw std::runtime_error("Called vector BlindingObservable in scale context.");
  return -999.;
}

int BlindingObservable::getNevaluations() const {
  if (this->fCut->EvalInstance()) {
    return 0;
  } else {
    return 1;
  }
}

double BlindingObservable::getValueAt(int index) const {
  if (index >= BlindingObservable::getNevaluations()) {
    throw std::runtime_error("Caught attempt to evaluate BlindingObservable out of bounds!");
    return -999.;
  } else {
    return this->fValue->EvalInstance();
  }
}
//______________________________________________________________________________________________

BlindingObservable::BlindingObservable(const TString& expression):
TQTreeObservable(expression)
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

const TString& BlindingObservable::getExpression() const {
  // retrieve the expression associated with this observable
  return this->fExpression;
}

//______________________________________________________________________________________________

bool BlindingObservable::hasExpression() const {
  // check if this observable type knows expressions
  return true;
}

//______________________________________________________________________________________________
void BlindingObservable::setExpression(const TString& expr){
  // set the expression to a given string
  this->fExpression = expr;
}
//______________________________________________________________________________________________

bool BlindingObservable::parseExpression(const TString& expr){
  // parse the expression
  if (!expr.BeginsWith("Blind(") || !expr.EndsWith(")")) {
    return false;
  }

  TString exprArgs = expr(6, expr.Length() - 7); // remove opening and closing parenthesis
  std::vector<TString> tokens = TQStringUtils::tokenizeVector(exprArgs, ",", true, "()[]{}", "'\"");
  if (tokens.size() != 2) {
    return false;
  }
  this->fValueString = tokens[0];
  this->fCutString = tokens[1];


  this->fValue = new TTreeFormula(this->fValueString, this->fValueString, this->fTree);
  this->fCut = new TTreeFormula(this->fCutString, this->fCutString, this->fTree);

  fActiveExpression = expr;
  return true;
}

//______________________________________________________________________________________________

void BlindingObservable::clearParsedExpression(){
  // clear the current expression
  if (fValue != NULL) {
    delete fValue;
    fValue = NULL;
  }
  if (fCut != NULL) {
    delete fCut;
    fCut = NULL;
  }
}

//______________________________________________________________________________________________

TString BlindingObservable::getActiveExpression() const {
  // retrieve the expression associated with this incarnation
  return this->fActiveExpression;
}

//______________________________________________________________________________________________

bool BlindingObservable::initializeSelf(){
  // initialize self - compile container name, construct accessor
  TString compiledExpression = TQObservable::compileExpression(this->fExpression,this->fSample);
  if(!this->parseExpression(compiledExpression)) {
    return false;
  }
  return true;
}
 
//______________________________________________________________________________________________

bool BlindingObservable::finalizeSelf(){
  // finalize self - delete accessor
  this->clearParsedExpression();
  return true;
}
//______________________________________________________________________________________________
int BlindingObservable::registerFactory() {
  TQObservable::manager.registerFactory(BlindingObservable::getFactory(),true);
  return 0;
}


// the following preprocessor macro defines an "observable factory"
// that is supposed to create instances of your class based on input
// expressions.

// it should receive an expression as an input and decide whether this
// expression should be used to construct an instance of your class,
// in which case it should take care of this, or return NULL.

// in addition to defining your observable factory here, you need to
// register it with the TQObservable manager. This can either be done
// from C++ code, using the line
//   TQObservable::manager.registerFactory(BlindingObservable::getFactory(),true);
// or from python code, using the line
//   TQObservable.manager.registerFactory(BlindingObservable.getFactory(),True)
// Either of these lines need to be put in a location where they are
// executed before observables are retrieved. You might want to 'grep'
// for 'registerFactory' in the package you are adding your observable
// to in order to get some ideas on where to put these!

DEFINE_OBSERVABLE_FACTORY(BlindingObservable,TString expr){
  // try to create an instance of this observable from the given expression
  // return the newly created observable upon success
  // or NULL upon failure

  // first, check if the expression fits your observable type
  // for example, you can grab all expressions that begin wth "BlindingObservable:"
  // if this is the case, then we call the expression-constructor
  if (!expr.BeginsWith("Blind(") || !expr.EndsWith(")")) {
    return NULL;
  }

  TString exprArgs = expr(6, expr.Length() - 7); // remove opening and closing parenthesis
  std::vector<TString> tokens = TQStringUtils::tokenizeVector(exprArgs, ",", true, "()[]{}", "'\"");

  if (tokens.size() != 2) {
    return NULL;
  }
  return new BlindingObservable(expr);
  
  // else, that is, if the expression doesn't match the pattern we
  // expect, we return this is important, because only in doing so we
  // give other observable types the chance to check for themselves
  return NULL;
  // if you do not return NULL here, your observable will become the
  // default observable type for all expressions that don't match
  // anything else, which is probably not what you want...
}

int _dummy_BlindingObservableRegisterFactory = BlindingObservable::registerFactory();
