#include "QFramework/TQCounterGrid.h"
#include "TMath.h"
#include <iostream>
#include <map>

#include "QFramework/TQLibrary.h"

#include <stdint.h>
#include <sstream>
#include <limits>

ClassImp(TQCounterGridVariable)

////////////////////////////////////////////////////////////////////////////////////////////////
//
// TQCounterGridVariable
//
// This class represents one variable (or dimension) in the TQCounterGrid
// multi-dimensional histgram-like data structure
//
////////////////////////////////////////////////////////////////////////////////////////////////

TQCounterGridVariable::TQCounterGridVariable(): TNamed(),
  fObservable(NULL),
  name(""),
  title(""),
  expression(""),
  min(0),
  max(0),
  nSteps(0),
  step(0)
{
  // standard constructor
}

TQCounterGridVariable::TQCounterGridVariable(const TString& name): TNamed(name,title),
                                                                   fObservable(NULL),
                                                                   name(name),
                                                                   title(name),
                                                                   expression(name),
                                                                   min(0),
                                                                   max(0),
                                                                   nSteps(0),
                                                                   step(0)
{
  // standard constructor
}

TQCounterGridVariable::TQCounterGridVariable(const TQCounterGridVariable* other): TNamed(other ? other->GetName() : "", other?other->GetTitle() : ""),
                                                                                  fObservable(other ? other->fObservable : NULL),
                                                                                  name(other ? other->name : ""),
                                                                                  title(other ? other->title : ""),
                                                                                  expression(other ? other->expression : ""),
                                                                                  min(other ? other->min : 0),
                                                                                  max(other ? other->max : 0),
                                                                                  nSteps(other ? other->nSteps : 0),
                                                                                  step(other ? other->step : 0)
{
  // copy constructor, pointer variant
}

TQCounterGridVariable::TQCounterGridVariable(const TQCounterGridVariable& other) : TNamed(other.GetName(),other.GetTitle()),
                                                                                   fObservable(other.fObservable),
                                                                                   name(other.name),
                                                                                   title(other.title),
                                                                                   expression(other.expression),
                                                                                   min(other.min),
                                                                                   max(other.max),
                                                                                   nSteps(other.nSteps),
                                                                                   step(other.step)
{
  // copy constructor, reference variant
}

TQCounterGridVariable::TQCounterGridVariable(const TString& name_, const TString& title_, int nSteps_, double min_, double max_, const TString& expr_):
  TNamed(name,title),
  fObservable(NULL),
  name(name_),
  title(title_),
  expression(expr_.IsNull() ? name_ : expr_),
  min(min_),
  max(max_),
  nSteps(nSteps_),
  step((max_-min_)/nSteps_) 
{
  // standard constructor, with variable initzialization
}

double TQCounterGridVariable::evaluateFromTree() const{
  // evaluate the TTreeFormula and return the result
  if(!this->fObservable){
    ERRORclass("no observable set!");
    return std::numeric_limits<double>::quiet_NaN();
  }
  return this->fObservable->getValue();
}

TQObservable* TQCounterGridVariable::getObservable(){
  // return the observable associated to this variable
  return this->fObservable;
}

bool TQCounterGridVariable::initialize(TQSample* s){
  // finalize this variable (and the observable below it)
  this->fObservable = TQObservable::getObservable(this->expression,s);
  if (!this->fObservable) {
    ERRORclass("Failed to obtain observable with expression '%s' in TQCounterGridVariable '%s'",this->expression.Data(),this->GetName());
    return false;
  }
  if (!this->fObservable->initialize(s)) {
    ERRORclass("Failed to initialize observable created from expression '%s' in TQCounterGridVariable '%s'",this->expression.Data(),this->GetName());
    return false;
  }
  return true; 
}

bool TQCounterGridVariable::finalize(){
  // initialize this variable (and the observable below it)
  return this->fObservable->finalize();
}

TQCounterGridVariable::~TQCounterGridVariable(){
  // standard destructor
}

bool operator< (TQCounterGridVariable &v1, TQCounterGridVariable &v2){
  // the variables need to have the same name
  if(v1.name != v2.name)
    return false;
  if(v1.max < v2.min)
    return true;
  return false;
}

bool operator> (TQCounterGridVariable &v1, TQCounterGridVariable &v2){
  // the variables need to have the same name
  if(v1.name != v2.name)
    return false;
  if(v1.min > v2.max)
    return true;
  return false;
}

bool operator<= (TQCounterGridVariable &v1, TQCounterGridVariable &v2){
  // the variables need to have the same name
  if(v1.name != v2.name)
    return false;
  if(v1.min > v2.min)
    return false;
  if(TMath::AreEqualRel(v1.step,v2.step,std::numeric_limits<double>::epsilon()))
    if(TMath::AreEqualRel((v2.min - v1.min)/v1.step,TMath::Nint((v2.min - v1.min)/v1.step),std::numeric_limits<double>::epsilon()))
      return true;
  return false;
}

bool operator>= (TQCounterGridVariable &v1, TQCounterGridVariable &v2){
  // the variables need to have the same name
  if(v1.name != v2.name)
    return false;
  if(v1.min < v2.min)
    return false;
  if(TMath::AreEqualRel(v1.step,v2.step,std::numeric_limits<double>::epsilon()))
    if(TMath::AreEqualRel((v1.min - v2.min)/v1.step,TMath::Nint((v1.min - v2.min)/v1.step),std::numeric_limits<double>::epsilon()))
      return true;
  return false;
}
 
bool operator== (TQCounterGridVariable &v1, TQCounterGridVariable &v2){
  // the variables need to have the same name
  return (v1.name == v2.name);
}

bool operator!= (TQCounterGridVariable &v1, TQCounterGridVariable &v2){
  // the variables cant have the same name
  return (v1.name != v2.name);
}



