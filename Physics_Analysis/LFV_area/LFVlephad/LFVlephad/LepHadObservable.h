//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __LEPHADOBSERVABLE__
#define __LEPHADOBSERVABLE__
#include "QFramework/TQTreeObservable.h"

#include "TTreeFormula.h"

class LepHadObservable : public TQTreeObservable {
protected:
  // put here any data members your class might need
public:
  virtual TObjArray* getBranchNames() const override;

protected:
  virtual bool initializeSelf() override;
  virtual bool finalizeSelf() override;
protected:
  TString fExpression = "";

  const long LIMIT_2016 = 284484; // last run in 2015
  const long LIMIT_D3 = 302872;  // last run in D3

  bool _isData = false;
  bool _isMC = false;
  bool _isFake = false;
  bool _isSherpa = false;

  bool _use2015 = true;
  bool _use2016 = true;
  bool _useSLT = true;
  bool _useTLT = true;

  TString _weightvariation;
  bool _pileup_low = false;
  bool _pileup_high = false;
  bool _pileup = false;

  TTreeFormula* x_run_number = NULL;

  TTreeFormula* tau_0_pt = NULL;
  TTreeFormula* jet_0_pt = NULL;
  TTreeFormula* jet_0_eta = NULL;
  TTreeFormula* lep_0_pt = NULL;
  TTreeFormula* n_electrons = NULL;
  TTreeFormula* n_muons = NULL;

  TString var = "";

public:
  virtual bool hasExpression() const override;
  virtual const TString& getExpression() const override;
  virtual void setExpression(const TString& expr) override;

  LepHadObservable();
  LepHadObservable(const TString& expression);

  bool isData() const;
  bool isMC() const;
  bool isFake() const;
  bool isSherpa() const;
  
  bool is2016_uptoD3() const;
  bool is2016_postD3() const;
  bool is2016() const;
  bool is2015() const;

  bool isTLT() const;
  bool isSLT() const;

  bool isMuon() const;
  bool isElectron() const;

  virtual ~LepHadObservable();
public:
  virtual bool parseExpression(const TString& expr);
  virtual void clearParsedExpression();

  virtual TString getActiveExpression() const override;

  // ClassDef(LepHadObservable, 0);
};
#endif
