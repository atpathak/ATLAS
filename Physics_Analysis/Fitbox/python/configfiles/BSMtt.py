# configuration file for the in-situ TES analysis
import ROOT
from AnalysisSetup import *

setup = AnalysisSetup( 
    #standard fixed fields
    standards = Standards(
        lumi = 36.1,
        analysis = 'hadhad-b-veto',
        signals = ["ggH500","bbH500"],
        cosmetics = { #histo key component, label, legend entry, fill color, line color, line width, marker type
            Cosmetic("Data", "Data", 0, ROOT.kBlack, 2, 20),
            Cosmetic("ggH500", "ggH#rightarrow#tau#tau", ROOT.kYellow-4, ROOT.kAzure, 2, 1),
            Cosmetic("bbH500", "bbH#rightarrow#tau#tau", ROOT.kYellow-4, ROOT.kAzure, 2, 1),
            Cosmetic("Ztautau", "Z#rightarrow#tau#tau", ROOT.kAzure-4, ROOT.kAzure, 2, 1),
            Cosmetic("Top", "Top", ROOT.kMagenta-9, ROOT.kMagenta, 2, 1), 
            Cosmetic("Wtaunu", "W#righarrow#tau#nu", ROOT.kOrange-4, ROOT.kOrange-3, 2, 1), 
            Cosmetic("Multijet", "Multijet",ROOT.kGray, ROOT.kGray+1, 2, 1),
            Cosmetic("Others", "Others",ROOT.kViolet+3, ROOT.kViolet+1, 2, 1),
            Cosmetic("ZplusJets", "Z+jets", ROOT.kAzure-4, ROOT.kAzure, 2, 1),
            Cosmetic("DYZ", "DYZ", ROOT.kAzure-6, ROOT.kAzure-2, 2, 1),
            Cosmetic("Fakes", "Fakes",ROOT.kGray, ROOT.kGray+1, 2, 1),
            Cosmetic("Diboson", "Diboson", ROOT.kOrange-4, ROOT.kOrange-3, 2, 1), 
            
        },
        signal_label = "m_A = 500GeV, tan#beta = 10",
        join_signals = True,
        stats = True,
        purity = False
    ),
    #various observables
    observables = [ #variable, [filters], x-axis title, log y-axis
        Observable("obs_x_Region_BMin0_J0_T0_isMVA0_L0_Y2015_distMTtot_DHadHad",["BMin0","J0","T0","isMVA0","L0","Y2015","distMTtot","DHadHad"],"MT_{TOT}", True),
        Observable("obs_x_Region_BMin0_J0_T1_isMVA0_L0_Y2015_distMTtot_DHadHad",["BMin0","J0","T1","isMVA0","L0","Y2015","distMTtot","DHadHad"],"MT_{TOT}", False),
        Observable("obs_x_Region_BMin0_J0_T1_isMVA0_L1_Y2015_distMTtot_DMuHadTcr",["DMuHadTcr"],"MT_{TOT}", False),
        Observable("obs_x_Region_BMin0_J0_T1_isMVA0_L1_Y2015_distMTtot_DElHadTcr",["DElHadTcr"],"MT_{TOT}", False),
        Observable("obs_x_Region_BMin0_J0_T1_isMVA0_L1_Y2015_distMTtot_DMuHadSig",["T1","DMuHadSig"],"MT_{TOT}", False),
        Observable("obs_x_Region_BMin0_J0_T1_isMVA0_L1_Y2015_distMTtot_DElHadSig",["T1","DElHadSig"],"MT_{TOT}", False),
        Observable("obs_x_Region_BMin0_J0_T0_isMVA0_L1_Y2015_distMTtot_DMuHad",["T0","DMuHad"],"MT_{TOT}", True),
        Observable("obs_x_Region_BMin0_J0_T0_isMVA0_L1_Y2015_distMTtot_DElHad",["T0","DElHad"],"MT_{TOT}", True),
        ]
    )

