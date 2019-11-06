//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __BLINDINGOBSERVALBLE__
#define __BLINDINGOBSERVALBLE__
#include "QFramework/TQTreeObservable.h"

class BlindingObservable : public TQTreeObservable {
protected:
  // put here any data members your class might need
 
public:
  virtual double getValue() const override;
  virtual TObjArray* getBranchNames() const override;

  virtual double getValueAt(int index) const override;
  virtual int getNevaluations() const override;
  inline virtual TQObservable::ObservableType
    getObservableType() const override {
        return TQObservable::ObservableType::vector;
    }

protected:
  virtual bool initializeSelf() override;
  virtual bool finalizeSelf() override;
protected:
  TString fExpression = "";
  TString fActiveExpression = "";
  TString fCutString = "";
  TString fValueString = "";

  TTreeFormula* fCut = NULL;
  TTreeFormula* fValue = NULL;

public:
  virtual bool hasExpression() const override;
  virtual const TString& getExpression() const override;
  virtual void setExpression(const TString& expr) override;

  BlindingObservable();
  BlindingObservable(const TString& expression);
  virtual ~BlindingObservable();
public:
  bool parseExpression(const TString& expr);
  void clearParsedExpression();

  virtual TString getActiveExpression() const override;

  static int registerFactory();

public:
  DECLARE_OBSERVABLE_FACTORY(BlindingObservable,TString expr)

  ClassDefOverride(BlindingObservable, 0);

};
#endif
