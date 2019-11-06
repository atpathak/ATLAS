//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __MXLG__
#define __MXLG__
#include "LFVlephad/LepHadObservable.h"

#include <TF1.h>
#include <TFitResultPtr.h>
#include <TFitResult.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "TLorentzVector.h"
#include "TFile.h"
#include "TLegend.h"
#include "TString.h"
#include <fstream>
#include "TRandom.h"
#include "TMath.h"
#include "TArrow.h"
#include <iostream>

class MXLG : public LepHadObservable {
protected:
  // put here any data members your class might need
  
public:
  virtual double getValue() const override;
  virtual TObjArray* getBranchNames() const override;
  virtual Long64_t getCurrentEntry() const override;

protected:
  virtual bool initializeSelf() override;
  virtual bool finalizeSelf() override;
  //void load_pdf_xlg() const;
  //double fitfunc(double* x, double* par);
  void cal_mtt_xlg(double& mtt_maxb, double& mtt_maxw, bool lfv, TLorentzVector P_vis[2], int vis_type[2], double MET_et[2], double sigma_MET, double mmax=1000, double binw=2, int nsampling = 1000) const;
  //TH2F* h_Minvvis_pperp[3];
  TString m_filepath;
  TTreeFormula* tau_0_n_tracks = NULL;
  TTreeFormula* tau_0_pt = NULL;
  TTreeFormula* tau_0_eta = NULL;
  TTreeFormula* tau_0_phi = NULL;
  TTreeFormula* tau_0_m = NULL;
  TTreeFormula* lep_0_pt = NULL;
  TTreeFormula* lep_0_eta = NULL;
  TTreeFormula* lep_0_phi = NULL;
  TTreeFormula* lep_0_m = NULL;
  TTreeFormula* met_reco_sumet = NULL;
  TTreeFormula* lephad_dphi = NULL;
  TTreeFormula* met_reco_etx = NULL;
  TTreeFormula* met_reco_ety = NULL;

public:
  MXLG();
  MXLG(const TString& name, TString filepath);
  virtual ~MXLG();
  ClassDefOverride(MXLG, 1);


};
#endif
