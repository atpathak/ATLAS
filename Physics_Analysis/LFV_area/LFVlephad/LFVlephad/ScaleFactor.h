//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __SCALEFACTORAUTO__
#define __SCALEFACTORAUTO__
#include "LFVlephad/LepHadObservable.h"

#include <tuple>
#include <bitset>
#include <vector>
#include "TTreeFormula.h"
#include "TH2.h"


typedef std::bitset<256> Condition;

class ScaleFactor : public LepHadObservable {
protected:
  // put here any data members your class might need
public:
  virtual double getValue() const override;
  virtual TObjArray* getBranchNames() const override;
  TString dir = "";
protected:
  virtual bool initializeSelf() override;
  virtual bool finalizeSelf() override;
protected:
  TString fExpression = "";

  double scale2016 = 1;
  double scale2015 = 1;
  const double zero = 0;
  const double one = 1;


  std::vector< std::pair<TString, Condition> > variations;
  Condition variation;

  virtual Condition registerVariation(TString name);
  unsigned int nextBitPosition = 13;
  
  std::vector< std::tuple<Condition, Condition, TString, TTreeFormula*> > branches;
  std::vector< std::tuple<Condition, Condition, const double*> > factors;

  virtual void addScaleFactor(Condition requirement, TString branch);
  virtual void addScaleFactor(Condition requirement, Condition veto, TString branch);

  virtual void addScaleFactor(Condition requirement, const double* factor);
  virtual void addScaleFactor(Condition requirement, Condition veto, const double* factor);

  TTreeFormula* event_number;

  // applications
  const Condition none = 0;
  const Condition y2015 = 1<<0;
  const Condition y2016 = 1<<1;
  const Condition slt = 1<<2;
  const Condition tlt = 1<<3;
  const Condition muon = 1<<4;
  const Condition electron = 1<<5;
  const Condition applySigSyst = 1<<6;
  const Condition applyZttSyst = 1<<7;
  const Condition is3pr = 1<<8;
  const Condition muon1pr = 1<<9;
  const Condition is1pr = 1<<10;
  const Condition isFiltSig = 1<<11;
  const Condition notFiltSig = 1<<12;
  const Condition tightTau = 1 <<13;

  bool _applySigSyst = false;
  bool _applyZttSyst = false;
  bool _applyd0Syst = false;
  bool _isZll = false;
  bool _isFiltSig = false;

  Condition nominal;

  // ugly pileup systematics
  Condition theory_ztt_qsf_low;
  Condition theory_ztt_qsf_high;
  Condition theory_ztt_ckk_low;
  Condition theory_ztt_ckk_high;

  Condition el_eff_id_nonprompt_d0_high;

  TH2* qsf = NULL;
  TH2* ckk = NULL;

  TTreeFormula* z_pt;
  TTreeFormula* n_truth_jets_pt20_eta45;

  // ugly eVeto systematics
  Condition tau_eff_eleolr_trueele_low;
  Condition tau_eff_eleolr_trueele_high;

  TH2* eVetoSF = NULL;
  TH2* eVetoSFErr = NULL;

  TTreeFormula* tau_pdgId;
  TTreeFormula* tau_pt;
  TTreeFormula* tau_eta;
  TTreeFormula* tau_n_tracks;
  TTreeFormula* tau_0_ele_BDTEleScoreTrans_run2;
  TTreeFormula* lep_pt;
  TTreeFormula* tau_0_jet_bdt_tight;

  virtual void confine(double& value, TAxis* axis) const;

  // This variable contains all flags, which do not change until
  // initializeSelf is called again. It can therefore be used to determine
  // which branches need to be registered.
  Condition staticConditionsMask;

public:
  ScaleFactor();
  ScaleFactor(const TString& expression);
  virtual ~ScaleFactor();
public:
  bool parseExpression(const TString& expr);

  ClassDef(ScaleFactor, 1);
};
#endif
