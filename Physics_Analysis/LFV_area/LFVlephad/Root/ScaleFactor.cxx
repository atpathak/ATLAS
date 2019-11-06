#include "LFVlephad/ScaleFactor.h"
#include <limits>
#include <iostream>

#include "TAxis.h"
#include "TFile.h"
#include "QFramework/TQUtils.h"

// #define _DEBUG_

// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"

ClassImp(ScaleFactor)


ScaleFactor::ScaleFactor(const TString& expression): LepHadObservable(expression) {
  // constructor with expression argument
  DEBUGclass("constructor called with '%s'",expression.Data());
  nominal = registerVariation("nominal");
  // the predefined string member "expression" allows your observable to store an expression of your choice
  // this string will be the verbatim argument you passed to the constructor of your observable
  // you can use it to choose between different modes or pass configuration options to your observable
  this->SetName(TQObservable::makeObservableName(expression));
  this->setExpression(expression);

  ////////////////////////////////////////////////////////////////////////////////

  // D0
  el_eff_id_nonprompt_d0_high = registerVariation("el_eff_id_nonprompt_d0_high");

  ////////////////////////////////////////////////////////////////////////////////
  //
  Condition z_theory_fac_low = registerVariation("theory_ztt_lhe3weight_mur1_muf05_pdf261000_high");
  Condition z_theory_fac_high = registerVariation("theory_ztt_lhe3weight_mur1_muf2_pdf261000_high");
  Condition z_theory_ren_low = registerVariation("theory_ztt_lhe3weight_mur05_muf1_pdf261000_high");
  Condition z_theory_ren_high = registerVariation("theory_ztt_lhe3weight_mur2_muf1_pdf261000_high");
  Condition z_theory_facren_low = registerVariation("theory_ztt_lhe3weight_mur05_muf05_pdf261000_high");
  Condition z_theory_facren_high = registerVariation("theory_ztt_lhe3weight_mur2_muf2_pdf261000_high");
  
  // Z theory, factoristion scale
  addScaleFactor(applyZttSyst | z_theory_fac_low,
    "theory_weights_LHE3Weight_MUR1_MUF05_PDF261000 / theory_weights_LHE3Weight_MUR1_MUF1_PDF261000");

  addScaleFactor(applyZttSyst | z_theory_fac_high,
    "theory_weights_LHE3Weight_MUR1_MUF2_PDF261000 / theory_weights_LHE3Weight_MUR1_MUF1_PDF261000");

  // Z theory, renormalization scale
  addScaleFactor(applyZttSyst | z_theory_ren_low,
    "theory_weights_LHE3Weight_MUR05_MUF1_PDF261000 / theory_weights_LHE3Weight_MUR1_MUF1_PDF261000");

  addScaleFactor(applyZttSyst | z_theory_ren_high,
    "theory_weights_LHE3Weight_MUR2_MUF1_PDF261000 / theory_weights_LHE3Weight_MUR1_MUF1_PDF261000");

  // Z theory, factoristion and renormalization scale
  addScaleFactor(applyZttSyst | z_theory_facren_low,
    "theory_weights_LHE3Weight_MUR05_MUF05_PDF261000 / theory_weights_LHE3Weight_MUR1_MUF1_PDF261000");

  addScaleFactor(applyZttSyst | z_theory_facren_high,
    "theory_weights_LHE3Weight_MUR2_MUF2_PDF261000 / theory_weights_LHE3Weight_MUR1_MUF1_PDF261000");

  // Z theory, PDF variation
  Condition z_theory_pdf_high = registerVariation("theory_ztt_PDF_high");
  Condition z_theory_pdf_low = registerVariation("theory_ztt_PDF_low");

  addScaleFactor(applyZttSyst | z_theory_pdf_high,
    "(theory_weights_PDF_central_value + theory_weights_PDF_error_up) / theory_weights_PDF_central_value");

  addScaleFactor(applyZttSyst | z_theory_pdf_low,
    "(theory_weights_PDF_central_value - theory_weights_PDF_error_down) / theory_weights_PDF_central_value");
  
  // variations using external histogram
  theory_ztt_qsf_low = registerVariation("theory_ztt_qsf_low");
  theory_ztt_qsf_high = registerVariation("theory_ztt_qsf_high");
  theory_ztt_ckk_low = registerVariation("theory_ztt_ckk_low");
  theory_ztt_ckk_high = registerVariation("theory_ztt_ckk_high");

  // ////////////////////////////////////////////////////////////////////////////////
  // // eVeto SF
  tau_eff_eleolr_trueele_low = registerVariation("tau_eff_eleolr_trueelectron_low");
  tau_eff_eleolr_trueele_high = registerVariation("tau_eff_eleolr_trueelectron_high");

  ////////////////////////////////////////////////////////////////////////////////
  // muon slt efficiency
  Condition mu_eff_trigger_stat_low = registerVariation("mu_eff_trigger_stat_low");
  Condition mu_eff_trigger_stat_high = registerVariation("mu_eff_trigger_stat_high");
  Condition mu_eff_trigger_sys_low = registerVariation("mu_eff_trigger_sys_low");
  Condition mu_eff_trigger_sys_high = registerVariation("mu_eff_trigger_sys_high");
    
  // muon slt efficiency 2015
  addScaleFactor(muon | slt | y2015,
    mu_eff_trigger_stat_low | mu_eff_trigger_stat_high | mu_eff_trigger_sys_low | mu_eff_trigger_sys_high,
    "lep_0_NOMINAL_MuEffSF_HLT_mu20_iloose_L1MU15_OR_HLT_mu40_QualMedium_IsoNone");

  addScaleFactor(muon | slt | y2015 | mu_eff_trigger_stat_low,
    "lep_0_MUON_EFF_TrigStatUncertainty_1down_MuEffSF_HLT_mu20_iloose_L1MU15_OR_HLT_mu40_QualMedium_IsoNone");

  addScaleFactor(muon | slt | y2015 | mu_eff_trigger_stat_high,
    "lep_0_MUON_EFF_TrigStatUncertainty_1up_MuEffSF_HLT_mu20_iloose_L1MU15_OR_HLT_mu40_QualMedium_IsoNone");

  addScaleFactor(muon | slt | y2015 | mu_eff_trigger_sys_low,
    "lep_0_MUON_EFF_TrigSystUncertainty_1down_MuEffSF_HLT_mu20_iloose_L1MU15_OR_HLT_mu40_QualMedium_IsoNone");

  addScaleFactor(muon | slt | y2015 | mu_eff_trigger_sys_high,
    "lep_0_MUON_EFF_TrigSystUncertainty_1up_MuEffSF_HLT_mu20_iloose_L1MU15_OR_HLT_mu40_QualMedium_IsoNone");

  // muon slt efficient 2016
  addScaleFactor(muon | slt | y2016,
    mu_eff_trigger_stat_low | mu_eff_trigger_stat_high | mu_eff_trigger_sys_low | mu_eff_trigger_sys_high,
    "lep_0_NOMINAL_MuEffSF_HLT_mu26_ivarmedium_OR_HLT_mu50_QualMedium_IsoNone");

  addScaleFactor(muon | slt | y2016 | mu_eff_trigger_stat_low,
    "lep_0_MUON_EFF_TrigStatUncertainty_1down_MuEffSF_HLT_mu26_ivarmedium_OR_HLT_mu50_QualMedium_IsoNone");

  addScaleFactor(muon | slt | y2016 | mu_eff_trigger_stat_high,
    "lep_0_MUON_EFF_TrigStatUncertainty_1up_MuEffSF_HLT_mu26_ivarmedium_OR_HLT_mu50_QualMedium_IsoNone");

  addScaleFactor(muon | slt | y2016 | mu_eff_trigger_sys_low,
    "lep_0_MUON_EFF_TrigSystUncertainty_1down_MuEffSF_HLT_mu26_ivarmedium_OR_HLT_mu50_QualMedium_IsoNone");

  addScaleFactor(muon | slt | y2016 | mu_eff_trigger_sys_high,
    "lep_0_MUON_EFF_TrigSystUncertainty_1up_MuEffSF_HLT_mu26_ivarmedium_OR_HLT_mu50_QualMedium_IsoNone");

  // ////////////////////////////////////////////////////////////////////////////////
  // // tau tlt efficiency
  // Condition tau_trigger_statdata_low = registerVariation("tau_trigger_statdata_low");
  // Condition tau_trigger_statdata_high = registerVariation("tau_trigger_statdata_high");
  // Condition tau_trigger_statmc_low = registerVariation("tau_trigger_statmc_low");
  // Condition tau_trigger_statmc_high = registerVariation("tau_trigger_statmc_high");
  // Condition tau_trigger_syst_low = registerVariation("tau_trigger_syst_low");
  // Condition tau_trigger_syst_high = registerVariation("tau_trigger_syst_high");
  // Condition tau_trigger_total2016_low = registerVariation("tau_trigger_total2016_low");
  // Condition tau_trigger_total2016_high = registerVariation("tau_trigger_total2016_high");

  // // tau tlt efficiency 2015
  // addScaleFactor(tlt | y2015,
  //   tau_trigger_statdata_low | tau_trigger_statdata_high | tau_trigger_statmc_low | tau_trigger_statmc_high | tau_trigger_syst_low | tau_trigger_syst_high,
  //   "tau_0_NOMINAL_TauEffSF_HLT_tau25_medium1_tracktwo_JETIDBDTMEDIUM");

  // addScaleFactor(tlt | y2015 | tau_trigger_statdata_low,
  //   "tau_0_TAUS_TRUEHADTAU_EFF_TRIGGER_STATDATA2015_1down_TauEffSF_HLT_tau25_medium1_tracktwo_JETIDBDTMEDIUM");
  
  // addScaleFactor(tlt | y2015 | tau_trigger_statdata_high,
  //   "tau_0_TAUS_TRUEHADTAU_EFF_TRIGGER_STATDATA2015_1up_TauEffSF_HLT_tau25_medium1_tracktwo_JETIDBDTMEDIUM");
  
  // addScaleFactor(tlt | y2015 | tau_trigger_statmc_low,
  //   "tau_0_TAUS_TRUEHADTAU_EFF_TRIGGER_STATMC2015_1down_TauEffSF_HLT_tau25_medium1_tracktwo_JETIDBDTMEDIUM");
  
  // addScaleFactor(tlt | y2015 | tau_trigger_statmc_high,
  //   "tau_0_TAUS_TRUEHADTAU_EFF_TRIGGER_STATMC2015_1up_TauEffSF_HLT_tau25_medium1_tracktwo_JETIDBDTMEDIUM");
  
  // addScaleFactor(tlt | y2015 | tau_trigger_syst_low,
  //   "tau_0_TAUS_TRUEHADTAU_EFF_TRIGGER_SYST2015_1down_TauEffSF_HLT_tau25_medium1_tracktwo_JETIDBDTMEDIUM");
  
  // addScaleFactor(tlt | y2015 | tau_trigger_syst_high,
  //   "tau_0_TAUS_TRUEHADTAU_EFF_TRIGGER_SYST2015_1up_TauEffSF_HLT_tau25_medium1_tracktwo_JETIDBDTMEDIUM");
  
  // // tau tlt efficiency 2016
  // addScaleFactor(tlt | y2016,
  //   tau_trigger_total2016_low | tau_trigger_total2016_high,
  //   "tau_0_NOMINAL_TauEffSF_HLT_tau25_medium1_tracktwo_JETIDBDTMEDIUM");
  
  // addScaleFactor(tlt | y2016 | tau_trigger_total2016_low,
  //   "tau_0_TAUS_TRUEHADTAU_EFF_TRIGGER_TOTAL2016_1down_TauEffSF_HLT_tau25_medium1_tracktwo_JETIDBDTMEDIUM");
  
  // addScaleFactor(tlt | y2016 | tau_trigger_total2016_high,
  //   "tau_0_TAUS_TRUEHADTAU_EFF_TRIGGER_TOTAL2016_1up_TauEffSF_HLT_tau25_medium1_tracktwo_JETIDBDTMEDIUM");

    
  ////////////////////////////////////////////////////////////////////////////////
  // electron slt efficiency
  Condition el_eff_trigger_low = registerVariation("el_eff_trigger_low");
  Condition el_eff_trigger_high = registerVariation("el_eff_trigger_high");

  addScaleFactor(electron | slt | y2015,
    el_eff_trigger_low | el_eff_trigger_high,
    "lep_0_NOMINAL_EleEffSF_SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0_MediumLLH_d0z0_v11_isolGradient");
  addScaleFactor(electron | slt | y2016,
    el_eff_trigger_low | el_eff_trigger_high,
    "lep_0_NOMINAL_EleEffSF_SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0_MediumLLH_d0z0_v11_isolGradient");

  addScaleFactor(electron | slt | el_eff_trigger_low,
    "lep_0_EL_EFF_Trigger_TOTAL_1NPCOR_PLUS_UNCOR_1down_EleEffSF_SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0_MediumLLH_d0z0_v11_isolGradient");

  addScaleFactor(electron | slt | el_eff_trigger_high,
    "lep_0_EL_EFF_Trigger_TOTAL_1NPCOR_PLUS_UNCOR_1up_EleEffSF_SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0_MediumLLH_d0z0_v11_isolGradient");

  // TODO muon and electron tlt missing!
  

  ////////////////////////////////////////////////////////////////////////////////
  // electron id efficiency
  Condition el_eff_id_low = registerVariation("el_eff_id_low");
  Condition el_eff_id_high = registerVariation("el_eff_id_high");

  addScaleFactor(electron,
    el_eff_id_low | el_eff_id_high,
    "lep_0_NOMINAL_EleEffSF_offline_MediumLLH_d0z0_v11");

  addScaleFactor(electron | el_eff_id_low,
    "lep_0_EL_EFF_ID_TOTAL_1NPCOR_PLUS_UNCOR_1down_EleEffSF_offline_MediumLLH_d0z0_v11");

  addScaleFactor(electron | el_eff_id_high,
    "lep_0_EL_EFF_ID_TOTAL_1NPCOR_PLUS_UNCOR_1up_EleEffSF_offline_MediumLLH_d0z0_v11");

  ////////////////////////////////////////////////////////////////////////////////
  // electron reco efficiency
  Condition el_eff_reco_low = registerVariation("el_eff_reco_low");
  Condition el_eff_reco_high = registerVariation("el_eff_reco_high");

  addScaleFactor(electron,
    el_eff_reco_low | el_eff_reco_high,
    "lep_0_NOMINAL_EleEffSF_offline_RecoTrk");

  addScaleFactor(electron | el_eff_reco_low,
    "lep_0_EL_EFF_Reco_TOTAL_1NPCOR_PLUS_UNCOR_1down_EleEffSF_offline_RecoTrk");

  addScaleFactor(electron | el_eff_reco_high,
    "lep_0_EL_EFF_Reco_TOTAL_1NPCOR_PLUS_UNCOR_1up_EleEffSF_offline_RecoTrk");

  ////////////////////////////////////////////////////////////////////////////////
  // electron isolation
  Condition el_eff_iso_low = registerVariation("el_eff_iso_low");
  Condition el_eff_iso_high = registerVariation("el_eff_iso_high");

  addScaleFactor(electron,
    el_eff_iso_low | el_eff_iso_high,
    "lep_0_NOMINAL_EleEffSF_Isolation_MediumLLH_d0z0_v11_isolGradient");

  addScaleFactor(electron | el_eff_iso_low,
    "lep_0_EL_EFF_Iso_TOTAL_1NPCOR_PLUS_UNCOR_1down_EleEffSF_Isolation_MediumLLH_d0z0_v11_isolGradient");

  addScaleFactor(electron | el_eff_iso_high,
    "lep_0_EL_EFF_Iso_TOTAL_1NPCOR_PLUS_UNCOR_1up_EleEffSF_Isolation_MediumLLH_d0z0_v11_isolGradient");
  
  ////////////////////////////////////////////////////////////////////////////////
  // muon reco efficiency
  Condition mu_eff_stat_low = registerVariation("mu_eff_stat_low");
  Condition mu_eff_stat_high = registerVariation("mu_eff_stat_high");
  Condition mu_eff_statlowpt_low = registerVariation("mu_eff_statlowpt_low");
  Condition mu_eff_statlowpt_high = registerVariation("mu_eff_statlowpt_high");
  Condition mu_eff_sys_low = registerVariation("mu_eff_sys_low");
  Condition mu_eff_sys_high = registerVariation("mu_eff_sys_high");
  Condition mu_eff_syslowpt_low = registerVariation("mu_eff_syslowpt_low");
  Condition mu_eff_syslowpt_high = registerVariation("mu_eff_syslowpt_high");

  addScaleFactor(muon,
    mu_eff_stat_low | mu_eff_stat_high | mu_eff_statlowpt_low | mu_eff_statlowpt_high | mu_eff_sys_low | mu_eff_sys_high | mu_eff_syslowpt_low | mu_eff_syslowpt_high,
    "lep_0_NOMINAL_MuEffSF_Reco_QualMedium");

  addScaleFactor(muon | mu_eff_stat_low,
    "lep_0_MUON_EFF_STAT_1down_MuEffSF_Reco_QualMedium");

  addScaleFactor(muon | mu_eff_stat_high,
    "lep_0_MUON_EFF_STAT_1up_MuEffSF_Reco_QualMedium");

  addScaleFactor(muon | mu_eff_statlowpt_low,
    "lep_0_MUON_EFF_STAT_LOWPT_1down_MuEffSF_Reco_QualMedium");

  addScaleFactor(muon | mu_eff_statlowpt_high,
    "lep_0_MUON_EFF_STAT_LOWPT_1up_MuEffSF_Reco_QualMedium");

  addScaleFactor(muon | mu_eff_sys_low,
    "lep_0_MUON_EFF_SYS_1down_MuEffSF_Reco_QualMedium");

  addScaleFactor(muon | mu_eff_sys_high,
    "lep_0_MUON_EFF_SYS_1up_MuEffSF_Reco_QualMedium");

  addScaleFactor(muon | mu_eff_syslowpt_low,
    "lep_0_MUON_EFF_SYS_LOWPT_1down_MuEffSF_Reco_QualMedium");

  addScaleFactor(muon | mu_eff_syslowpt_high,
    "lep_0_MUON_EFF_SYS_LOWPT_1up_MuEffSF_Reco_QualMedium");

  ////////////////////////////////////////////////////////////////////////////////
  // muon isolation efficient
  Condition mu_eff_isostat_low = registerVariation("mu_eff_isostat_low");
  Condition mu_eff_isostat_high = registerVariation("mu_eff_isostat_high");
  Condition mu_eff_isosys_low = registerVariation("mu_eff_isosys_low");
  Condition mu_eff_isosys_high = registerVariation("mu_eff_isosys_high");

  addScaleFactor(muon,
   mu_eff_isostat_low | mu_eff_isostat_high | mu_eff_isosys_low | mu_eff_isosys_high,
   "lep_0_NOMINAL_MuEffSF_IsoGradient");

  addScaleFactor(muon | mu_eff_isostat_low,
   "lep_0_MUON_ISO_STAT_1down_MuEffSF_IsoGradient");

  addScaleFactor(muon | mu_eff_isostat_high,
   "lep_0_MUON_ISO_STAT_1up_MuEffSF_IsoGradient");

  addScaleFactor(muon | mu_eff_isosys_low,
   "lep_0_MUON_ISO_SYS_1down_MuEffSF_IsoGradient");

  addScaleFactor(muon | mu_eff_isosys_high,
   "lep_0_MUON_ISO_SYS_1up_MuEffSF_IsoGradient");

  ////////////////////////////////////////////////////////////////////////////////
  // muon ttva
  // Condition mu_eff_ttvastat_low = registerVariation("mu_eff_ttvastat_low");
  // Condition mu_eff_ttvastat_high = registerVariation("mu_eff_ttvastat_high");
  // Condition mu_eff_ttvasys_low = registerVariation("mu_eff_ttvasys_low");
  // Condition mu_eff_ttvasys_high = registerVariation("mu_eff_ttvasys_high");

  // addScaleFactor(muon,
  //   mu_eff_ttvastat_low | mu_eff_ttvastat_high | mu_eff_ttvasys_low | mu_eff_ttvasys_high,
  //   "lep_0_NOMINAL_MuEffSF_TTVA");

  // addScaleFactor(muon | mu_eff_ttvastat_low,
  //   "lep_0_MUON_TTVA_STAT_1down_MuEffSF_TTVA");

  // addScaleFactor(muon | mu_eff_ttvastat_high,
  //   "lep_0_MUON_TTVA_STAT_1up_MuEffSF_TTVA");

  // addScaleFactor(muon | mu_eff_ttvasys_low,
  //   "lep_0_MUON_TTVA_SYS_1down_MuEffSF_TTVA");

  // addScaleFactor(muon | mu_eff_ttvasys_high,
  //   "lep_0_MUON_TTVA_SYS_1up_MuEffSF_TTVA");

  ////////////////////////////////////////////////////////////////////////////////
  // tau reco efficiency
  Condition tau_eff_reco_total_low = registerVariation("tau_eff_reco_total_low");
  Condition tau_eff_reco_total_high = registerVariation("tau_eff_reco_total_high");
  Condition tau_eff_reco_highpt_low = registerVariation("tau_eff_reco_highpt_low");
  Condition tau_eff_reco_highpt_high = registerVariation("tau_eff_reco_highpt_high");

  addScaleFactor(none,
    tau_eff_reco_total_low | tau_eff_reco_total_high | tau_eff_reco_highpt_low | tau_eff_reco_highpt_high,
    "tau_0_NOMINAL_TauEffSF_reco");

  addScaleFactor(tau_eff_reco_total_low,
    "tau_0_TAUS_TRUEHADTAU_EFF_RECO_TOTAL_1down_TauEffSF_reco");

  addScaleFactor(tau_eff_reco_total_high,
    "tau_0_TAUS_TRUEHADTAU_EFF_RECO_TOTAL_1up_TauEffSF_reco");

  addScaleFactor(tau_eff_reco_highpt_low,
    "tau_0_TAUS_TRUEHADTAU_EFF_RECO_HIGHPT_1down_TauEffSF_reco");

  addScaleFactor(tau_eff_reco_highpt_high,
    "tau_0_TAUS_TRUEHADTAU_EFF_RECO_HIGHPT_1up_TauEffSF_reco");


  ////////////////////////////////////////////////////////////////////////////////
  // tau electron overlap removal
  // TODO: don't use very loose

  // 1 prong
  
  addScaleFactor(is1pr,
    tau_eff_eleolr_trueele_low | tau_eff_eleolr_trueele_high,
    "tau_0_NOMINAL_TauEffSF_MediumEleBDTPlusVeto_electron");

  addScaleFactor(is1pr | tau_eff_eleolr_trueele_low,
		 "tau_0_TAUS_TRUEELECTRON_EFF_ELEOLR_TOTAL_1down_TauEffSF_MediumEleBDTPlusVeto_electron");

  addScaleFactor(is1pr | tau_eff_eleolr_trueele_high,
		 "tau_0_TAUS_TRUEELECTRON_EFF_ELEOLR_TOTAL_1up_TauEffSF_MediumEleBDTPlusVeto_electron");

  // 3 prong
  addScaleFactor(is3pr,
    tau_eff_eleolr_trueele_low | tau_eff_eleolr_trueele_high,
    "tau_0_NOMINAL_TauEffSF_VeryLooseLlhEleOLR_electron");

  addScaleFactor(is3pr | tau_eff_eleolr_trueele_low,
    "tau_0_TAUS_TRUEELECTRON_EFF_ELEOLR_TOTAL_1down_TauEffSF_VeryLooseLlhEleOLR_electron");

  addScaleFactor(is3pr | tau_eff_eleolr_trueele_high,
    "tau_0_TAUS_TRUEELECTRON_EFF_ELEOLR_TOTAL_1up_TauEffSF_VeryLooseLlhEleOLR_electron");

  /*
  // 1 prong muon
  addScaleFactor(muon1pr,
    tau_eff_eleolr_trueele_low | tau_eff_eleolr_trueele_high,
    "tau_0_NOMINAL_TauEffSF_VeryLooseLlhEleOLR_electron");

  addScaleFactor(muon1pr | tau_eff_eleolr_trueele_low,
    "tau_0_TAUS_TRUEELECTRON_EFF_ELEOLR_TOTAL_1down_TauEffSF_VeryLooseLlhEleOLR_electron");

  addScaleFactor(muon1pr | tau_eff_eleolr_trueele_high,
    "tau_0_TAUS_TRUEELECTRON_EFF_ELEOLR_TOTAL_1up_TauEffSF_VeryLooseLlhEleOLR_electron");
  */
  
  Condition tau_eff_eleolr_truehadtau_low = registerVariation("tau_eff_eleolr_truehadtau_low");
  Condition tau_eff_eleolr_truehadtau_high = registerVariation("tau_eff_eleolr_truehadtau_high");

  /*
  // 3 prong
  addScaleFactor(is3pr,
    tau_eff_eleolr_truehadtau_low | tau_eff_eleolr_truehadtau_high,
    "tau_0_NOMINAL_TauEffSF_HadTauEleOLR_tauhad");

  addScaleFactor(is3pr | tau_eff_eleolr_truehadtau_low,
    "tau_0_TAUS_TRUEHADTAU_EFF_ELEOLR_TOTAL_1down_TauEffSF_HadTauEleOLR_tauhad");

  addScaleFactor(is3pr | tau_eff_eleolr_truehadtau_high,
    "tau_0_TAUS_TRUEHADTAU_EFF_ELEOLR_TOTAL_1up_TauEffSF_HadTauEleOLR_tauhad");
  */
    
  // 1 prong muon
  //addScaleFactor(muon1pr,
  addScaleFactor(muon,
    tau_eff_eleolr_truehadtau_low | tau_eff_eleolr_truehadtau_high,
    "tau_0_NOMINAL_TauEffSF_HadTauEleOLR_tauhad");

  addScaleFactor(muon1pr | tau_eff_eleolr_truehadtau_low,
    "tau_0_TAUS_TRUEHADTAU_EFF_ELEOLR_TOTAL_1down_TauEffSF_HadTauEleOLR_tauhad");

  addScaleFactor(muon1pr | tau_eff_eleolr_truehadtau_high,
    "tau_0_TAUS_TRUEHADTAU_EFF_ELEOLR_TOTAL_1up_TauEffSF_HadTauEleOLR_tauhad");

  ////////////////////////////////////////////////////////////////////////////////
  // tau jetid efficiency
  Condition tau_eff_jetid_total_low = registerVariation("tau_eff_jetid_total_low");
  Condition tau_eff_jetid_total_high = registerVariation("tau_eff_jetid_total_high");
  Condition tau_eff_jetid_highpt_low = registerVariation("tau_eff_jetid_highpt_low");
  Condition tau_eff_jetid_highpt_high = registerVariation("tau_eff_jetid_highpt_high");

  addScaleFactor(none,
    tau_eff_jetid_total_low | tau_eff_jetid_total_high | tau_eff_jetid_highpt_low | tau_eff_jetid_highpt_high,
    "tau_0_NOMINAL_TauEffSF_JetBDTtight");

  addScaleFactor(tau_eff_jetid_total_low,
    "tau_0_TAUS_TRUEHADTAU_EFF_JETID_TOTAL_1down_TauEffSF_JetBDTtight");

  addScaleFactor(tau_eff_jetid_total_high,
    "tau_0_TAUS_TRUEHADTAU_EFF_JETID_TOTAL_1up_TauEffSF_JetBDTtight");

  addScaleFactor(tau_eff_jetid_highpt_low,
    "tau_0_TAUS_TRUEHADTAU_EFF_JETID_HIGHPT_1down_TauEffSF_JetBDTtight");

  addScaleFactor(tau_eff_jetid_highpt_high,
    "tau_0_TAUS_TRUEHADTAU_EFF_JETID_HIGHPT_1up_TauEffSF_JetBDTtight");

  ////////////////////////////////////////////////////////////////////////////////
  // btag 
  Condition btag_all;

  // b
  const int n_btag_b = 3;
  Condition btag_b_low[n_btag_b];
  Condition btag_b_high[n_btag_b];
  
  for (int i = 0; i < n_btag_b; i++) {
    btag_b_low[i] = registerVariation(TString::Format("btag_b_%d_low", i));
    btag_b_high[i] = registerVariation(TString::Format("btag_b_%d_high", i));
    btag_all |= btag_b_low[i];
    btag_all |= btag_b_high[i];

    addScaleFactor(btag_b_low[i], TString::Format("jet_FT_EFF_Eigen_B_%d_1down_global_effSF_MVX", i));
    addScaleFactor(btag_b_low[i], TString::Format("jet_FT_EFF_Eigen_B_%d_1down_global_ineffSF_MVX", i));
    addScaleFactor(btag_b_high[i], TString::Format("jet_FT_EFF_Eigen_B_%d_1up_global_effSF_MVX", i));
    addScaleFactor(btag_b_high[i], TString::Format("jet_FT_EFF_Eigen_B_%d_1up_global_ineffSF_MVX", i));
  }

  // c
  const int n_btag_c = 4;
  Condition btag_c_low[n_btag_c];
  Condition btag_c_high[n_btag_c];

  for (int i = 0; i < n_btag_c; i++) {
    btag_c_low[i] = registerVariation(TString::Format("btag_c_%d_low", i));
    btag_c_high[i] = registerVariation(TString::Format("btag_c_%d_high", i));
    btag_all |= btag_c_low[i];
    btag_all |= btag_c_high[i];

    addScaleFactor(btag_c_low[i], TString::Format("jet_FT_EFF_Eigen_C_%d_1down_global_effSF_MVX", i));
    addScaleFactor(btag_c_low[i], TString::Format("jet_FT_EFF_Eigen_C_%d_1down_global_ineffSF_MVX", i));
    addScaleFactor(btag_c_high[i], TString::Format("jet_FT_EFF_Eigen_C_%d_1up_global_effSF_MVX", i));
    addScaleFactor(btag_c_high[i], TString::Format("jet_FT_EFF_Eigen_C_%d_1up_global_ineffSF_MVX", i));
  }

  // light
  const int n_btag_light = 5;
  Condition btag_light_low[n_btag_light];
  Condition btag_light_high[n_btag_light];

  for (int i = 0; i < n_btag_light; i++) {
    btag_light_low[i] = registerVariation(TString::Format("btag_light_%d_low", i));
    btag_light_high[i] = registerVariation(TString::Format("btag_light_%d_high", i));
    btag_all |= btag_light_low[i];
    btag_all |= btag_light_high[i];

    addScaleFactor(btag_light_low[i], TString::Format("jet_FT_EFF_Eigen_Light_%d_1down_global_effSF_MVX", i));
    addScaleFactor(btag_light_low[i], TString::Format("jet_FT_EFF_Eigen_Light_%d_1down_global_ineffSF_MVX", i));
    addScaleFactor(btag_light_high[i], TString::Format("jet_FT_EFF_Eigen_Light_%d_1up_global_effSF_MVX", i));
    addScaleFactor(btag_light_high[i], TString::Format("jet_FT_EFF_Eigen_Light_%d_1up_global_ineffSF_MVX", i));
  }

  // extrapolation
  Condition btag_extrapolation_low = registerVariation("btag_extrapolation_low");
  Condition btag_extrapolation_high = registerVariation("btag_extrapolation_high");
  btag_all |= btag_extrapolation_low;
  btag_all |= btag_extrapolation_high;

  addScaleFactor(btag_extrapolation_low, "jet_FT_EFF_extrapolation_1down_global_effSF_MVX");
  addScaleFactor(btag_extrapolation_low, "jet_FT_EFF_extrapolation_1down_global_ineffSF_MVX");

  addScaleFactor(btag_extrapolation_high, "jet_FT_EFF_extrapolation_1up_global_effSF_MVX");
  addScaleFactor(btag_extrapolation_high, "jet_FT_EFF_extrapolation_1up_global_ineffSF_MVX");

  // extrapolation from charm
  Condition btag_extrapolation_from_charm_low = registerVariation("btag_extrapolation_from_charm_low");
  Condition btag_extrapolation_from_charm_high = registerVariation("btag_extrapolation_from_charm_high");
  btag_all |= btag_extrapolation_from_charm_low;
  btag_all |= btag_extrapolation_from_charm_high;

  addScaleFactor(btag_extrapolation_from_charm_low, "jet_FT_EFF_extrapolation_from_charm_1down_global_effSF_MVX");
  addScaleFactor(btag_extrapolation_from_charm_low, "jet_FT_EFF_extrapolation_from_charm_1down_global_ineffSF_MVX");

  addScaleFactor(btag_extrapolation_from_charm_high, "jet_FT_EFF_extrapolation_from_charm_1up_global_effSF_MVX");
  addScaleFactor(btag_extrapolation_from_charm_high, "jet_FT_EFF_extrapolation_from_charm_1up_global_ineffSF_MVX");
  
    
  // btag all
  addScaleFactor(none, btag_all, "jet_NOMINAL_global_effSF_MVX");
  addScaleFactor(none, btag_all, "jet_NOMINAL_global_ineffSF_MVX");

  ////////////////////////////////////////////////////////////////////////////////
  // jet jvt efficiency
  Condition jet_jvteff_low = registerVariation("jet_jvteff_low");
  Condition jet_jvteff_high = registerVariation("jet_jvteff_high");

  addScaleFactor(none, jet_jvteff_low | jet_jvteff_high, "jet_NOMINAL_central_jets_global_effSF_JVT");
  addScaleFactor(none, jet_jvteff_low | jet_jvteff_high, "jet_NOMINAL_central_jets_global_ineffSF_JVT");
  addScaleFactor(jet_jvteff_low, "jet_JET_JvtEfficiency_1down_central_jets_global_effSF_JVT");
  addScaleFactor(jet_jvteff_low, "jet_JET_JvtEfficiency_1down_central_jets_global_ineffSF_JVT");
  addScaleFactor(jet_jvteff_high, "jet_JET_JvtEfficiency_1up_central_jets_global_effSF_JVT");
  addScaleFactor(jet_jvteff_high, "jet_JET_JvtEfficiency_1up_central_jets_global_ineffSF_JVT");

  ////////////////////////////////////////////////////////////////////////////////
  // forward jet jvt efficiency
  Condition jet_fjvteff_low = registerVariation("jet_fjvteff_low");
  Condition jet_fjvteff_high = registerVariation("jet_fjvteff_high");

  addScaleFactor(none, jet_fjvteff_low | jet_fjvteff_high, "jet_NOMINAL_forward_jets_global_effSF_JVT");
  addScaleFactor(none, jet_fjvteff_low | jet_fjvteff_high, "jet_NOMINAL_forward_jets_global_ineffSF_JVT");
  addScaleFactor(jet_fjvteff_low, "jet_JET_JvtEfficiency_1down_forward_jets_global_effSF_JVT");
  addScaleFactor(jet_fjvteff_low, "jet_JET_JvtEfficiency_1down_forward_jets_global_ineffSF_JVT");
  addScaleFactor(jet_fjvteff_high, "jet_JET_JvtEfficiency_1up_forward_jets_global_effSF_JVT");
  addScaleFactor(jet_fjvteff_high, "jet_JET_JvtEfficiency_1up_forward_jets_global_ineffSF_JVT");
    
  
  ////////////////////////////////////////////////////////////////////////////////
  // pileup
  Condition pu_prw_low = registerVariation("pu_prw_low");
  Condition pu_prw_high = registerVariation("pu_prw_high");

  addScaleFactor(none, pu_prw_low | pu_prw_high, "NOMINAL_pileup_combined_weight");

  addScaleFactor(pu_prw_low, "PRW_DATASF_1down_pileup_combined_weight");
  addScaleFactor(pu_prw_high, "PRW_DATASF_1up_pileup_combined_weight");
  
  ////////////////////////////////////////////////////////////////////////////////
  // luminosity scaling
  addScaleFactor(y2015, &scale2015);
  addScaleFactor(y2016, &scale2016);

  ////////////////////////////////////////////////////////////////////////////////
  // mc weight
  addScaleFactor(isFiltSig, "theory_weights_nominal");
  addScaleFactor(none,isFiltSig, "weight_mc");
  //addScaleFactor(notFiltSig, "weight_mc");


  ////////////////////////////////////////////////////////////////////////////////
  // Signal theory, factoristion scale
  Condition theory_sig_alphaS_low = registerVariation("theory_sig_alphaS_low");
  Condition theory_sig_alphaS_high = registerVariation("theory_sig_alphaS_high");
  Condition theory_sig_pdf_0_high = registerVariation("theory_sig_pdf_0_high");
  Condition theory_sig_pdf_1_high = registerVariation("theory_sig_pdf_1_high");
  Condition theory_sig_pdf_2_high = registerVariation("theory_sig_pdf_2_high");
  Condition theory_sig_pdf_3_high = registerVariation("theory_sig_pdf_3_high");
  Condition theory_sig_pdf_4_high = registerVariation("theory_sig_pdf_4_high");
  Condition theory_sig_pdf_5_high = registerVariation("theory_sig_pdf_5_high");
  Condition theory_sig_pdf_6_high = registerVariation("theory_sig_pdf_6_high");
  Condition theory_sig_pdf_7_high = registerVariation("theory_sig_pdf_7_high");
  Condition theory_sig_pdf_8_high = registerVariation("theory_sig_pdf_8_high");
  Condition theory_sig_pdf_9_high = registerVariation("theory_sig_pdf_9_high");
  Condition theory_sig_pdf_10_high = registerVariation("theory_sig_pdf_10_high");
  Condition theory_sig_pdf_11_high = registerVariation("theory_sig_pdf_11_high");
  Condition theory_sig_pdf_12_high = registerVariation("theory_sig_pdf_12_high");
  Condition theory_sig_pdf_13_high = registerVariation("theory_sig_pdf_13_high");
  Condition theory_sig_pdf_14_high = registerVariation("theory_sig_pdf_14_high");
  Condition theory_sig_pdf_15_high = registerVariation("theory_sig_pdf_15_high");
  Condition theory_sig_pdf_16_high = registerVariation("theory_sig_pdf_16_high");
  Condition theory_sig_pdf_17_high = registerVariation("theory_sig_pdf_17_high");
  Condition theory_sig_pdf_18_high = registerVariation("theory_sig_pdf_18_high");
  Condition theory_sig_pdf_19_high = registerVariation("theory_sig_pdf_19_high");
  Condition theory_sig_pdf_20_high = registerVariation("theory_sig_pdf_20_high");
  Condition theory_sig_pdf_21_high = registerVariation("theory_sig_pdf_21_high");
  Condition theory_sig_pdf_22_high = registerVariation("theory_sig_pdf_22_high");
  Condition theory_sig_pdf_23_high = registerVariation("theory_sig_pdf_23_high");
  Condition theory_sig_pdf_24_high = registerVariation("theory_sig_pdf_24_high");
  Condition theory_sig_pdf_25_high = registerVariation("theory_sig_pdf_25_high");
  Condition theory_sig_pdf_26_high = registerVariation("theory_sig_pdf_26_high");
  Condition theory_sig_pdf_27_high = registerVariation("theory_sig_pdf_27_high");
  Condition theory_sig_pdf_28_high = registerVariation("theory_sig_pdf_28_high");
  Condition theory_sig_pdf_29_high = registerVariation("theory_sig_pdf_29_high");
  Condition theory_sig_qcd_0_high = registerVariation("theory_sig_qcd_0_high");
  Condition theory_sig_qcd_1_high = registerVariation("theory_sig_qcd_1_high");
  Condition theory_sig_qcd_2_high = registerVariation("theory_sig_qcd_2_high");
  Condition theory_sig_qcd_3_high = registerVariation("theory_sig_qcd_3_high");
  Condition theory_sig_qcd_4_high = registerVariation("theory_sig_qcd_4_high");
  Condition theory_sig_qcd_5_high = registerVariation("theory_sig_qcd_5_high");
  Condition theory_sig_qcd_6_high = registerVariation("theory_sig_qcd_6_high");
  Condition theory_sig_qcd_7_high = registerVariation("theory_sig_qcd_7_high");
  Condition theory_sig_qcd_8_high = registerVariation("theory_sig_qcd_8_high");

  addScaleFactor(applySigSyst | theory_sig_alphaS_low,
                 "theory_weights_alphaS_down / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_alphaS_high,
                 "theory_weights_alphaS_up / theory_weights_nominal");

  addScaleFactor(applySigSyst | theory_sig_pdf_0_high,
                 "theory_weights_pdf_signal_weight_0 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_1_high,
                 "theory_weights_pdf_signal_weight_1 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_2_high,
                 "theory_weights_pdf_signal_weight_2 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_3_high,
                 "theory_weights_pdf_signal_weight_3 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_4_high,
                 "theory_weights_pdf_signal_weight_4 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_5_high,
                 "theory_weights_pdf_signal_weight_5 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_6_high,
                 "theory_weights_pdf_signal_weight_6 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_7_high,
                 "theory_weights_pdf_signal_weight_7 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_8_high,
                 "theory_weights_pdf_signal_weight_8 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_9_high,
                 "theory_weights_pdf_signal_weight_9 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_10_high,
                 "theory_weights_pdf_signal_weight_10 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_11_high,
                 "theory_weights_pdf_signal_weight_11 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_12_high,
                 "theory_weights_pdf_signal_weight_12 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_13_high,
                 "theory_weights_pdf_signal_weight_13 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_14_high,
                 "theory_weights_pdf_signal_weight_14 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_15_high,
                 "theory_weights_pdf_signal_weight_15 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_16_high,
                 "theory_weights_pdf_signal_weight_16 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_17_high,
                 "theory_weights_pdf_signal_weight_17 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_18_high,
                 "theory_weights_pdf_signal_weight_18 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_19_high,
                 "theory_weights_pdf_signal_weight_19 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_20_high,
                 "theory_weights_pdf_signal_weight_20 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_21_high,
                 "theory_weights_pdf_signal_weight_21 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_22_high,
                 "theory_weights_pdf_signal_weight_22 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_23_high,
                 "theory_weights_pdf_signal_weight_23 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_24_high,
                 "theory_weights_pdf_signal_weight_24 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_25_high,
                 "theory_weights_pdf_signal_weight_25 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_26_high,
                 "theory_weights_pdf_signal_weight_26 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_27_high,
                 "theory_weights_pdf_signal_weight_27 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_28_high,
                 "theory_weights_pdf_signal_weight_28 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_pdf_29_high,
                 "theory_weights_pdf_signal_weight_29 / theory_weights_nominal");

  addScaleFactor(applySigSyst | theory_sig_qcd_0_high,
                 "theory_weights_qcd_weight_0 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_qcd_1_high,
                 "theory_weights_qcd_weight_1 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_qcd_2_high,
                 "theory_weights_qcd_weight_2 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_qcd_3_high,
                 "theory_weights_qcd_weight_3 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_qcd_4_high,
                 "theory_weights_qcd_weight_4 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_qcd_5_high,
                 "theory_weights_qcd_weight_5 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_qcd_6_high,
                 "theory_weights_qcd_weight_6 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_qcd_7_high,
                 "theory_weights_qcd_weight_7 / theory_weights_nominal");
  addScaleFactor(applySigSyst | theory_sig_qcd_8_high,
                 "theory_weights_qcd_weight_8 / theory_weights_nominal");




  /*
  ////////////////////////////////////////////////////////////////////////////////
  // fail safe
  addScaleFactor(none, muon | electron, &zero);
  addScaleFactor(none, y2015 | y2016, &zero);
  addScaleFactor(none, slt | tlt, &zero);
  */
  ////////////////////////////////////////////////////////////////////////////////
  // pileup
  TFile* pileup_weights_file = TFile::Open("weights/pileup_weights.root");
  qsf = (TH2*) pileup_weights_file->Get("qsf");
  ckk = (TH2*) pileup_weights_file->Get("ckk");

  ////////////////////////////////////////////////////////////////////////////////
  // eVeto SF
  TFile* eVeto_sf_file = TFile::Open("input/EleOLR_TrueElectron_2017-moriond.root");
  eVetoSF    = (TH2*) eVeto_sf_file->Get("sf_eleBDTMediumPlusVeto_1p");
  eVetoSFErr = (TH2*) eVeto_sf_file->Get("TOTAL_eleBDTMediumPlusVeto_1p");
  
}

//______________________________________________________________________________________________
bool ScaleFactor::parseExpression(const TString& expr) {
    return true;
}

//______________________________________________________________________________________________
bool ScaleFactor::initializeSelf() {
  if (!LepHadObservable::initializeSelf()) {
      ERROR("Initialization of LepHadObservable failed.");
      return false;
  }

  this->fSample->getTag("~scale2016", scale2016);
  this->fSample->getTag("~scale2015", scale2015);

  this->fSample->getTag("~applySigSyst", _applySigSyst);
  this->fSample->getTag("~applyZttSyst", _applyZttSyst);
  this->fSample->getTag("~applyd0Syst", _applyd0Syst);
  this->fSample->getTag("~isZll", _isZll);
  this->fSample->getTag("~isFiltSig", _isFiltSig);

  TString variation_name;
  if (!this->fSample->getTag("~weightvariation", variation_name)) {
    ERROR("Tag 'weightvariation' not found!");
    return false;
  }
  DEBUGclass("weightvariation tag: %s", variation_name.Data());

  variation = none;
  for (unsigned int i = 0; i < variations.size(); i++) {
    DEBUGclass("test: '%s' == '%s'", variations[i].first.Data(), variation_name.Data());
    if (variations[i].first == variation_name) {
      variation = variations[i].second;
      DEBUG("ScaleFactor observable returns '%s' scale factors.", variation_name.Data());
      break;
    }
  }
  if (variation == none) {
    ERROR("ScaleFactor variation '%s' not found.", variation_name.Data());
    return false;
  }
  // These flags do not change on an event-to-event basis, they are constant
  // until initializeSelf is called again.
  staticConditionsMask |= applySigSyst;
  staticConditionsMask |= applyZttSyst;
  staticConditionsMask |= isFiltSig;
  staticConditionsMask |= notFiltSig;

  Condition staticConditions = variation;
  if (_applySigSyst) {
    staticConditions |= applySigSyst;
  }
  if (_applyZttSyst) {
    staticConditions |= applyZttSyst;
  }

  if (_isFiltSig) {
    staticConditions |= isFiltSig;
  }
  if (!(_isFiltSig)) {
    staticConditions |= notFiltSig;
  }

  
  for (unsigned int i = 0; i < branches.size(); i++) {
    Condition requirement = std::get<0>(branches[i]);
    Condition veto = std::get<1>(branches[i]);

    std::get<3>(branches[i]) = NULL;
    TString name = std::get<2>(branches[i]);
      
    // remove those, which are already impossible due to variation
    if ((requirement & staticConditionsMask & ~staticConditions).any()) {
        DEBUGclass("skipping %s, condition never fullfilled on this sample", name.Data());
        DEBUGclass("    requirement: %s", requirement.to_string().c_str());
        DEBUGclass("  s. cond. Mask: %s", staticConditionsMask.to_string().c_str());
        DEBUGclass("       s. cond.: %s", staticConditions.to_string().c_str());
        continue;
    }
    if ((veto & staticConditions).any()) {
        DEBUGclass("skipping %s, veto always satisfied on this sample", name.Data());
        continue;
    }
    DEBUGclass("booking: %s", name.Data());
    
    std::get<3>(branches[i]) = new TTreeFormula(name, name, this->fTree);
  }
    
  event_number = new TTreeFormula("event_number", "event_number",  this->fTree);
  z_pt = new TTreeFormula("parent_pt", "parent_pt",  this->fTree);
  n_truth_jets_pt20_eta45 = new TTreeFormula("n_truth_jets_pt20_eta45", "n_truth_jets_pt20_eta45",  this->fTree);

  tau_pdgId = new TTreeFormula("tau_0_truth_pdgId", "tau_0_truth_pdgId",  this->fTree);
  tau_pt = new TTreeFormula("tau_0_pt", "tau_0_pt",  this->fTree);
  tau_eta = new TTreeFormula("tau_0_eta", "tau_0_eta",  this->fTree);
  tau_n_tracks = new TTreeFormula("tau_0_n_tracks", "tau_0_n_tracks",  this->fTree);
  tau_0_ele_BDTEleScoreTrans_run2 = new TTreeFormula("tau_0_ele_BDTEleScoreTrans_run2", "tau_0_ele_BDTEleScoreTrans_run2",  this->fTree);
  lep_pt= new TTreeFormula("lep_0_pt", "lep_0_pt", this->fTree);
  tau_0_jet_bdt_tight = new TTreeFormula("tau_0_jet_bdt_tight", "tau_0_jet_bdt_tight", this->fTree);

  return true;
}
 
//______________________________________________________________________________________________

bool ScaleFactor::finalizeSelf(){
  // finalize self - delete accessor
  this->clearParsedExpression();

  for (unsigned int i = 0; i < branches.size(); i++) {
    TTreeFormula* formula = std::get<3>(branches[i]);
    if (formula != NULL) {
      delete formula;
      formula = NULL;
    }
  }

  return true;
}

//______________________________________________________________________________________________
Condition ScaleFactor::registerVariation(TString name) {
  Condition variation;
  variation.set(nextBitPosition);
  staticConditionsMask.set(nextBitPosition);
  nextBitPosition++;
  
  
  std::pair<TString, Condition> pair(name, variation);
  variations.push_back(pair);

  return variation;
}

//______________________________________________________________________________________________
void ScaleFactor::addScaleFactor(Condition requirement, TString branch) {
  addScaleFactor(requirement, none, branch);
}

//______________________________________________________________________________________________
void ScaleFactor::addScaleFactor(Condition requirement, Condition veto, TString branch) {
  std::tuple<Condition, Condition, TString, TTreeFormula*> sf;

  std::get<0>(sf) = requirement;
  std::get<1>(sf) = veto;
  std::get<2>(sf) = branch;
  std::get<3>(sf) = NULL;

  branches.push_back(sf);
}

//______________________________________________________________________________________________
void ScaleFactor::addScaleFactor(Condition requirement, const double* factor) {
  addScaleFactor(requirement, none, factor);
}

//______________________________________________________________________________________________
void ScaleFactor::addScaleFactor(Condition requirement, Condition veto, const double* factor) {
  std::tuple<Condition, Condition, const double*> sf;

  std::get<0>(sf) = requirement;
  std::get<1>(sf) = veto;
  std::get<2>(sf) = factor;

  factors.push_back(sf);
}

//______________________________________________________________________________________________
ScaleFactor::ScaleFactor(){
  this->setExpression(this->GetName() );
  
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________
ScaleFactor::~ScaleFactor(){
  // default destructor
  DEBUGclass("destructor called");
} 

//______________________________________________________________________________________________
TObjArray* ScaleFactor::getBranchNames() const {
  // retrieve the list of branch names 
  // ownership of the list belongs to the caller of the function
  DEBUGclass("retrieving branch names");
  TObjArray* bnames = LepHadObservable::getBranchNames();

  for (unsigned int i = 0; i < branches.size(); i++) {
    TString name = std::get<2>(branches[i]);
    TObjArray* tmp = TQUtils::getBranchNames(name);
    for (int i = 0; i < tmp->GetEntries(); i++) {
      bnames->Add(tmp->At(i));
    }
  }

  bnames->Add(new TObjString("event_number"));
  bnames->Add(new TObjString("parent_pt"));
  bnames->Add(new TObjString("n_truth_jets_pt20_eta45"));
  bnames->Add(new TObjString("tau_0_n_tracks"));
  bnames->Add(new TObjString("tau_0_eta"));
  bnames->Add(new TObjString("tau_0_truth_pdgId"));
  bnames->Add(new TObjString("lep_0_pt"));
  bnames->Add(new TObjString("tau_0_jet_bdt_tight"));

  return bnames;
}

//______________________________________________________________________________________________

double ScaleFactor::getValue() const {
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

  double scaleFac = 1.;
  double tau_n_tracks_v = tau_n_tracks->EvalInstance();
  double tau_0_ele_BDT = tau_0_ele_BDTEleScoreTrans_run2->EvalInstance(); 
  double tau_jet_bdt_tight = tau_0_jet_bdt_tight->EvalInstance();
  Condition status(variation);


  //double event_n = event_number->EvalInstance();
  //std::cout << std::fixed;
  //std::cout << "Event number: " << event_n << std::endl;


  if (is2016())                        { status |= y2016; }
  if (is2015())                        { status |= y2015; }
  if (isSLT())                         { status |= slt; }
  if (isTLT())                         { status |= tlt; }
  if (isMuon())                        { status |= muon; }
  if (isElectron())                    { status |= electron; }
  if (tau_n_tracks_v == 3 && isElectron()) { status |= is3pr; }
  if (tau_n_tracks_v == 1 && isMuon()) { status |= muon1pr; }
  if (tau_n_tracks_v == 1 && tau_0_ele_BDT > 0.15 && isElectron()) { status |= is1pr; }
  if (_applySigSyst)                   { status |= applySigSyst; }
  if (_applyZttSyst)                   { status |= applyZttSyst; }
  if (_isFiltSig)                      { status |= isFiltSig; }
  if (!(_isFiltSig))                   { status |= notFiltSig; }
  if (tau_jet_bdt_tight == 1)          { status |= tightTau; }

  DEBUGclass("================================================================================");
  DEBUGclass("y2016 = %d", (status & y2016).any());
  DEBUGclass("y2015 = %d", (status & y2015).any());
  DEBUGclass("slt = %d", (status & slt).any());
  DEBUGclass("tlt = %d", (status & tlt).any());
  DEBUGclass("muon = %d", (status & muon).any());
  DEBUGclass("electron = %d", (status & electron).any());
  DEBUGclass("is3pr = %d", (status & is3pr).any());
  DEBUGclass("muon1pr = %d", (status & muon1pr).any());
  DEBUGclass("applySigSyst = %d", (status & applySigSyst).any());
  DEBUGclass("applyZttSyst = %d", (status & applyZttSyst).any());
  DEBUGclass("--------");

  // apply branches
  for (unsigned int i = 0; i < branches.size(); i++) {
    Condition requirement = std::get<0>(branches[i]);
    Condition veto = std::get<1>(branches[i]);

    // some requirements are not met
    if ((requirement & ~status).any()) { continue; }
    // some vetos are triggered
    if ((veto & status ).any()) { continue; }

    TTreeFormula* formula = std::get<3>(branches[i]);
    DEBUGclass("appying formula: %s = %f", formula->GetName(), formula->EvalInstance());
    scaleFac *= formula->EvalInstance();
    //std::cout << "appying formula: " << formula->GetName() << " = " << formula->EvalInstance() << "  -  scalefac = " << scaleFac << std::endl; 
  }


  // apply constant factors
  for (unsigned int i = 0; i < factors.size(); i++) {
    Condition requirement = std::get<0>(factors[i]);
    Condition veto = std::get<1>(factors[i]);

    // some requirements are not met
    if ((requirement & ~status).any()) { continue; }
    // some vetos are triggered
    if ((veto & status ).any()) { continue; }

    scaleFac *= (*std::get<2>(factors[i]));
    DEBUGclass("appying const #%i: %f", i, (*std::get<2>(factors[i])));
  }

  //Ztt theory systematics
  TH2* histogram = NULL;
    if (((theory_ztt_qsf_low | theory_ztt_qsf_high) & status).any() && _applyZttSyst) {
    histogram = qsf;
    DEBUGclass("z_theory_weight will be qsf");
  } else if (((theory_ztt_ckk_low | theory_ztt_ckk_high) & status).any() && _applyZttSyst) {
    histogram = ckk;
    DEBUGclass("z_theory_weight will be ckk");
  }
  
  double pileup_weight = 1;
  if (histogram != NULL) {
    TAxis* xAxis = histogram->GetXaxis();
    TAxis* yAxis = histogram->GetYaxis();
    double z_pt_v = z_pt->EvalInstance();
    double n_truth_jets_pt20_eta45_v = n_truth_jets_pt20_eta45->EvalInstance();
    confine(z_pt_v, xAxis);
    confine(n_truth_jets_pt20_eta45_v, yAxis);
    int binx = xAxis->FindBin(z_pt_v);
    int biny = yAxis->FindBin(n_truth_jets_pt20_eta45_v);
    pileup_weight = histogram->GetBinContent(binx, biny, 0);
  }

  if (((theory_ztt_qsf_high | theory_ztt_ckk_high) & status).any()) {
    scaleFac *= pileup_weight;
    DEBUGclass("appying: z_theory_weight(1) = %f", pileup_weight);
  } else if (((theory_ztt_qsf_low | theory_ztt_ckk_low) & status).any()) {
    scaleFac *= 2 - pileup_weight;
    DEBUGclass("appying: z_theory_weight(2) = %f", 2-pileup_weight);
  }
  
  /*  
  //eVeto SF systematics
  double eVeto_weight = 1;
  //  if (_isZll) {//To be sure that we are applying SF for the real isolated electron
  int tau_pdgId_v = tau_pdgId->EvalInstance();
          
  if(abs(tau_pdgId_v)==11 && tau_n_tracks_v == 1 && isElectron() && tau_0_ele_BDT > 0.15){ //applying only for e->tau fake, 1prong and ehad
    TAxis* xAxis = eVetoSF->GetXaxis();
    TAxis* yAxis = eVetoSF->GetYaxis();
    double tau_pt_v = tau_pt->EvalInstance();
    double tau_eta_v = fabs(tau_eta->EvalInstance());
    //double tau_eta_v = tau_eta->EvalInstance();
    confine(tau_pt_v, xAxis);
    confine(tau_eta_v, yAxis);
    int binx = xAxis->FindBin(tau_pt_v);
    int biny = yAxis->FindBin(tau_eta_v);

    //std::cout << "tau pt = " << tau_pt_v << " : " << binx << std::endl;
    //std::cout << "tau eta = " << tau_eta_v << " : " << tau_eta->EvalInstance() << std::endl;
        
    eVeto_weight = eVetoSF->GetBinContent(binx, biny, 0);
    if ((tau_eff_eleolr_trueele_low & status).any()) {
      double eVeto_weight_err = eVetoSFErr->GetBinContent(binx, biny, 0);
      scaleFac *= (eVeto_weight-eVeto_weight_err);
      DEBUGclass("appying: eVeto_weight(1) = %f", eVeto_weight);
    } else if ((tau_eff_eleolr_trueele_high & status).any()) {
      double eVeto_weight_err = eVetoSFErr->GetBinContent(binx, biny, 0);
      scaleFac *= (eVeto_weight+eVeto_weight_err);
      DEBUGclass("appying: eVeto_weight(2) = %f", eVeto_weight);
    } else {
      scaleFac *= eVeto_weight;
      DEBUGclass("appying: eVeto_weight(3) = %f", eVeto_weight);
      //std::cout << "appying: eVeto_weight = " << eVeto_weight << std::endl;
    }
  
  }
  //  }
  //
  */

  // D0
  if (isElectron() && !isMuon() && _applyd0Syst) {
    double lepton_pt = lep_pt->EvalInstance();
    double d0_sf = (0.051779 / (exp(-0.000141434*( (lepton_pt*1000) - 13400.0))+1) + 0.923748 + 1.10131e-07*(lepton_pt*1000) -1) +1;

    //scaleFac *= d0_sf;

    if ((el_eff_id_nonprompt_d0_high & status).any()) {
      // apply second time
      scaleFac *= d0_sf;
    }
  }



  DEBUGclass("full scale factor = %f", scaleFac);
  //std::cout << "full scale factor = " << scaleFac << std::endl;
  DEBUGclass("returning");


  return scaleFac;
}

void ScaleFactor::confine(double& v, TAxis* axis) const {
  double upper = axis->GetBinUpEdge(axis->GetLast());
  double lower = axis->GetBinLowEdge(1);
  if (v <= lower) {
    v = (lower + axis->GetBinUpEdge(1)) / 2;
  } else if (v >= upper) {
    v = (upper + axis->GetBinLowEdge(axis->GetLast())) / 2;
  }
}

//______________________________________________________________________________________________
