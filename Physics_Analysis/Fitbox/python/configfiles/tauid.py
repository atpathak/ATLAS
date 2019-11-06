# configuration file for the in-situ TES analysis
import ROOT
from AnalysisSetup import *

setup = AnalysisSetup( 
    #standard fixed fields
    standards = Standards(
        lumi = 32.8,
        analysis = 'TauID',
        signals = ['Ztt'],
        cosmetics = [ #component, fill color, line color, line width, marker type
             Cosmetic("Data", "Data", 0, ROOT.kBlack, 2, 20),
             Cosmetic("Ztt", "Z#rightarrow#tau#tau", ROOT.kAzure-4, ROOT.kAzure, 2, 1),
             Cosmetic("Top", "Top", ROOT.kMagenta-9, ROOT.kMagenta, 2, 1),
             Cosmetic("Fake", "Fake", ROOT.kGreen-7, ROOT.kGreen+1, 2, 1),
             Cosmetic("W", "W", ROOT.kOrange-4, ROOT.kOrange-3, 2, 1),
             Cosmetic("Zll", "Z#rightarrow_{}ll",ROOT.kGray, ROOT.kGray+1, 2, 1)
            ],
        signal_label = "Z#rightarrow#tau#tau",
        join_signals = False,
        stats = True
        ),
    #various observables
    observables = [ #variable, [filters], x-axis title, log y-axis
        Observable("NTrk", ["TauID", "ChannelMuHad", "1P", "InclEta", "FailLooseID", "RegSignal"], "Fail Loose ID NTrk(1p)", False),
        Observable("NTrk", ["TauID", "ChannelMuHad", "1P", "InclEta", "PassLooseFailMediumID", "RegSignal"], "Pass Loose Fail Medium ID Ntrk(1p)", False),
        Observable("NTrk", ["TauID", "ChannelMuHad", "1P", "InclEta", "PassMediumFailTightID", "RegSignal"], "Pass Medium Fail Tight ID Ntrk(1p)", False),
        Observable("NTrk", ["TauID", "ChannelMuHad", "1P", "InclEta", "PassTightID", "RegSignal"], "Pass Tight Ntrk(1p)", False),

        Observable("NTrk", ["TauID", "ChannelMuHad", "3P", "InclEta", "FailLooseID", "RegSignal"], "Fail Loose ID NTrk(3p)", False),
        Observable("NTrk", ["TauID", "ChannelMuHad", "3P", "InclEta", "PassLooseFailMediumID", "RegSignal"], "Pass Loose Fail Medium ID Ntrk(3p)", False),
        Observable("NTrk", ["TauID", "ChannelMuHad", "3P", "InclEta", "PassMediumFailTightID", "RegSignal"], "Pass Medium Fail Tight ID Ntrk(3p)", False),
        Observable("NTrk", ["TauID", "ChannelMuHad", "3P", "InclEta", "PassTightID", "RegSignal"], "Pass Tight Ntrk(3p)", False),
        ]
    )

