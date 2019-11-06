//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __TRIGGER__
#define __TRIGGER__
#include "LFVlephad/LepHadObservable.h"

#include "TTreeFormula.h"

class Trigger : public LepHadObservable {
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
  TTreeFormula* HLT_e24_lhmedium_L1EM20VH = NULL;
  TTreeFormula* eleTrigMatch_0_HLT_e24_lhmedium_L1EM20VH = NULL;
  TTreeFormula* HLT_e60_lhmedium = NULL;
  TTreeFormula* eleTrigMatch_0_HLT_e60_lhmedium = NULL;
  TTreeFormula* HLT_e120_lhloose = NULL;
  TTreeFormula* eleTrigMatch_0_HLT_e120_lhloose = NULL;
  TTreeFormula* HLT_mu20_iloose_L1MU15 = NULL;
  TTreeFormula* muTrigMatch_0_HLT_mu20_iloose_L1MU15 = NULL;
  TTreeFormula* HLT_mu40 = NULL;
  TTreeFormula* muTrigMatch_0_HLT_mu40 = NULL;
  TTreeFormula* HLT_e17_lhmedium_tau25_medium1_tracktwo = NULL;
  TTreeFormula* eleTrigMatch_0_HLT_e17_lhmedium_tau25_medium1_tracktwo = NULL;
  TTreeFormula* tau_0_trig_HLT_e17_lhmedium_tau25_medium1_tracktwo = NULL;
  TTreeFormula* HLT_mu14_tau25_medium1_tracktwo = NULL;
  TTreeFormula* muTrigMatch_0_HLT_mu14_tau25_medium1_tracktwo = NULL;
  TTreeFormula* tau_0_trig_HLT_mu14_tau25_medium1_tracktwo = NULL;
  TTreeFormula* HLT_e24_lhtight_nod0_ivarloose = NULL;
  TTreeFormula* eleTrigMatch_0_HLT_e24_lhtight_nod0_ivarloose = NULL;
  TTreeFormula* HLT_e60_lhmedium_nod0 = NULL;
  TTreeFormula* eleTrigMatch_0_HLT_e60_lhmedium_nod0 = NULL;
  TTreeFormula* HLT_e140_lhloose_nod0 = NULL;
  TTreeFormula* eleTrigMatch_0_HLT_e140_lhloose_nod0 = NULL;
  TTreeFormula* HLT_mu24_ivarmedium = NULL;
  TTreeFormula* muTrigMatch_0_HLT_mu24_ivarmedium = NULL;
  TTreeFormula* HLT_mu50 = NULL;
  TTreeFormula* muTrigMatch_0_HLT_mu50 = NULL;
  TTreeFormula* HLT_e26_lhtight_nod0_ivarloose = NULL;
  TTreeFormula* eleTrigMatch_0_HLT_e26_lhtight_nod0_ivarloose = NULL;
  TTreeFormula* HLT_mu26_ivarmedium = NULL;
  TTreeFormula* muTrigMatch_0_HLT_mu26_ivarmedium = NULL;
  TTreeFormula* HLT_e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo = NULL;
  TTreeFormula* eleTrigMatch_0_HLT_e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo = NULL;
  TTreeFormula* tau_0_trig_HLT_e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo = NULL;
  TTreeFormula* HLT_mu14_ivarloose_tau25_medium1_tracktwo = NULL;
  TTreeFormula* muTrigMatch_0_HLT_mu14_ivarloose_tau25_medium1_tracktwo = NULL;
  TTreeFormula* tau_0_trig_HLT_mu14_ivarloose_tau25_medium1_tracktwo = NULL;
  TTreeFormula* muon_iso = NULL;
  
public:
  Trigger();
  Trigger(const TString& expression);
  virtual ~Trigger();
public:
  bool parseExpression(const TString& expr);

  ClassDefOverride(Trigger, 1);


};
#endif
