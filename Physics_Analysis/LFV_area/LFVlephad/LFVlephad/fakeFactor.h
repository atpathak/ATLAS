//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __FAKEFACTOR__
#define __FAKEFACTOR__
#include "LFVlephad/LepHadObservable.h"

#include "TTreeFormula.h"
#include "TH1D.h"
#include "TFile.h"

class fakeFactor : public LepHadObservable {
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
  TTreeFormula* tau_0_phi = NULL;
  TTreeFormula* met_reco_phi = NULL;
  TTreeFormula* met_reco_et = NULL;
  TTreeFormula* lephad_mt_lep0_met = NULL;
  TTreeFormula* tau_0_pt = NULL;
  TTreeFormula* lep_0_pt = NULL;

  TString ffFilePath = "";
  TString rFilePath = "";

  TFile* ffFile = NULL;
  TFile* rFile = NULL;
  TFile* sFile = NULL;
  TFile* sFile1 = NULL;

  TH1D* SF = NULL;
  TH1D* SF1 = NULL;

  TH1D* FF_W_1Prong_SLT_2015 = NULL;
  TH1D* FF_W_3Prong_SLT_2015 = NULL;
  TH1D* FF_W_1Prong_SLT_2016 = NULL;
  TH1D* FF_W_3Prong_SLT_2016 = NULL;

  TH1D* FF_Top_1Prong_SLT_2015 = NULL;
  TH1D* FF_Top_3Prong_SLT_2015 = NULL;
  TH1D* FF_Top_1Prong_SLT_2016 = NULL;
  TH1D* FF_Top_3Prong_SLT_2016 = NULL;

  TH1D* FF_Z_1Prong_SLT_2015 = NULL;
  TH1D* FF_Z_3Prong_SLT_2015 = NULL;
  TH1D* FF_Z_1Prong_SLT_2016 = NULL;
  TH1D* FF_Z_3Prong_SLT_2016 = NULL;

  TH1D* FF_QCD_1Prong_SLT_2015 = NULL;
  TH1D* FF_QCD_3Prong_SLT_2015 = NULL;
  TH1D* FF_QCD_1Prong_SLT_2016 = NULL;
  TH1D* FF_QCD_3Prong_SLT_2016 = NULL;

  TH1D* R_W_1Prong_SLT_2015 = NULL;
  TH1D* R_W_3Prong_SLT_2015 = NULL;
  TH1D* R_W_1Prong_SLT_2016 = NULL;
  TH1D* R_W_3Prong_SLT_2016 = NULL;

  TH1D* R_Top_1Prong_SLT_2015 = NULL;
  TH1D* R_Top_3Prong_SLT_2015 = NULL;
  TH1D* R_Top_1Prong_SLT_2016 = NULL;
  TH1D* R_Top_3Prong_SLT_2016 = NULL;

  TH1D* R_Z_1Prong_SLT_2015 = NULL;
  TH1D* R_Z_3Prong_SLT_2015 = NULL;
  TH1D* R_Z_1Prong_SLT_2016 = NULL;
  TH1D* R_Z_3Prong_SLT_2016 = NULL;

  TH1D* R_QCD_1Prong_SLT_2015 = NULL;
  TH1D* R_QCD_3Prong_SLT_2015 = NULL;
  TH1D* R_QCD_1Prong_SLT_2016 = NULL;
  TH1D* R_QCD_3Prong_SLT_2016 = NULL;

  TString var = "";
  TString SSvar = "";
  TString cat = "";
  
  TString Top = "Top";
  TString W = "W";
  TString Z = "Z";

  const double NOMINAL_THRESHOLD = 0.0001;
  const double VARIATION_THRESHOLD = 0.00005;

public:
  fakeFactor();
  fakeFactor(TString expression, TString ffFilePath, TString rFilePath, TString category, TString region, TString mc);

  virtual ~fakeFactor();
public:

  ClassDefOverride(fakeFactor, 1);


};
#endif
