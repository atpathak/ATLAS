//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __EFAKEFACTOR__
#define __EFAKEFACTOR__
#include "LFVlephad/LepHadObservable.h"

#include "TTreeFormula.h"
#include "TH1D.h"
#include "TFile.h"

class eFakeFactor : public LepHadObservable {
protected:
  // put here any data members your class might need
public:
  virtual double getValue() const override;
  virtual TObjArray* getBranchNames() const override;
protected:
  virtual bool initializeSelf() override;
  virtual bool finalizeSelf() override;
protected:
  TString fExpression = "";

  TTreeFormula* tau_0_n_tracks = NULL;
  TTreeFormula* tau_0_pt = NULL;

  TString ffFilePath = "";

  TFile* ffFile = NULL;

  TH1D* FF_W_1Prong_SLT_2016 = NULL;

  TString var = "";
  TString SSvar = "";
  TString cat = "";
  

  const double NOMINAL_THRESHOLD = 0.0001;
  const double VARIATION_THRESHOLD = 0.00005;

public:
  eFakeFactor();
  eFakeFactor(TString expression, TString ffFilePath);

  virtual ~eFakeFactor();
public:

  ClassDefOverride(eFakeFactor, 1);


};
#endif
