import ROOT
from tools import build_workspace
from argparse import ArgumentParser



input_paths = [
    '/eos/user/q/qbuat/workspaces/htt_sept28/leplep_17-09-26.all.dilep_pt_rebin_fake_WithTheoryEnv_WithModJERVar.root',
    '/eos/user/q/qbuat/workspaces/htt_boost_rebinning_np_study/lephad_mh0059_MMC_rebinned_WithModJERVar_WithTheoryEnv_rebinned.root',
    '/eos/user/q/qbuat/workspaces/htt_boost_rebinning_np_study/hhAll_merged_WSinput_rebinned_WithTheoryEnv_WithModJERVar_rebinned.root',
]

xmls = [
    'data/Htautau/HTauTau13TeVCombinationCBA.xml',
    'data/Htautau/HTauTau13TeVPOISingleMu.xml'
]


ROOT.gROOT.Macro('$ROOTCOREDIR/scripts/load_packages.C')

stat_analysis = [
    # 'HTauTau13TeVCombinationCBA',
    # 'HTauTau13TeVCombinationBoostCBA',
    # 'HTauTau13TeVCombinationVBFCBA',
    # 'HTauTau13TeVHHBoostCBA',
    # 'HTauTau13TeVHHCBA',
    # 'HTauTau13TeVHHVBFCBA',
    # 'HTauTau13TeVLHBoostCBA',
    # 'HTauTau13TeVLHCBA',
    # 'HTauTau13TeVLHVBFCBA',
    # 'HTauTau13TeVLLBoostCBA',
    'HTauTau13TeVLLCBA',
    'HTauTau13TeVLLVBFCBA',
]    

for ana in stat_analysis:
    build_workspace(input_paths, 'boost_rebin', xmls, ana)
