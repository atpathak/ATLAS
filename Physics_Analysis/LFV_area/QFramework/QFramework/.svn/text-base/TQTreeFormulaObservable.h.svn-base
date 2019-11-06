//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __TQTREEFORMULAOBSERVABLE__
#define __TQTREEFORMULAOBSERVABLE__

#include "TTreeFormula.h"
#include "QFramework/TQTreeObservable.h"

class TQTreeFormulaObservable : public TQTreeObservable {

protected:
  TString fExpression = "";
  TTreeFormula * fFormula = NULL;
  bool fcallGetNdata = false;

  virtual bool initializeSelf() override;
  virtual bool finalizeSelf() override;

public:

  TQTreeFormulaObservable();
  TQTreeFormulaObservable(const TString& expression);

  virtual double getValue() const override;

  virtual bool hasExpression() const override;
  virtual const TString& getExpression() const override;
  virtual void setExpression(const TString& expr) override;
  virtual TString getActiveExpression() const override;

  virtual TObjArray* getBranchNames() const override;
 
  virtual ~TQTreeFormulaObservable();

  DECLARE_OBSERVABLE_FACTORY(TQTreeFormulaObservable,const TString& expr)
  
  ClassDefOverride(TQTreeFormulaObservable, 0);

};

#endif
