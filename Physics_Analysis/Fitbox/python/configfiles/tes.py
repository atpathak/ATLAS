# configuration file for the in-situ TES analysis
import ROOT
from AnalysisSetup import *

eta="CentEta"

setup = AnalysisSetup( 
    #standard fixed fields
    standards = Standards(
        lumi = 36.1,
        analysis = 'TES',
        signals = ['Ztt'],
        cosmetics = [ #histo key component, label, legend entry, fill color, line color, line width, marker type
            Cosmetic("Data", "Data", 0, ROOT.kBlack, 2, 20),
            Cosmetic("Ztt", "Z#rightarrow#tau#tau", ROOT.kAzure-4, ROOT.kAzure, 2, 1),
            Cosmetic("Top", "Top", ROOT.kMagenta-9, ROOT.kMagenta, 2, 1), 
            Cosmetic("Fake", "Fake", ROOT.kGreen-7, ROOT.kGreen+1, 2, 1), 
            Cosmetic("W", "W", ROOT.kOrange-4, ROOT.kOrange-3, 2, 1), 
            Cosmetic("Zll", "Z#rightarrow_{}ll",ROOT.kGray, ROOT.kGray+1, 2, 1)
            ],
        signal_label = "Z#rightarrow#tau_{#mu}#tau_{had}",
        join_signals = False,
        stats = True,
        purity = Purity(show = True, line_color = ROOT.kYellow+2, yaxis_x = 0.0, yaxis_y_low = 1.5e3, yaxis_y_up = 2.5e3, yaxis_y_auto = True)
        ),
    #various observables
    observables = [ #variable, [filters], x-axis title, log y-axis
        #
        #Observable("Y", ["TES", "Calo", "ChannelMuHad", "1P", eta, "RegSignal"], "Y(1p)", False),
        #Observable("Y", ["TES", "Calo", "ChannelMuHad", "1P", eta, "RegFakes"], "Y(1p)", False),
        #
        Observable("Y", ["TES", "Calo", "ChannelMuHad", "1P", eta, "RegSignal", "pTslice1"], "Y(1p)", False), 
        #Observable("Y", ["TES", "Calo", "ChannelMuHad", "1P", eta, "RegSignal", "pTslice2"], "Y(1p)", False),
        #Observable("Y", ["TES", "Calo", "ChannelMuHad", "1P", eta, "RegSignal", "pTslice3"], "Y(1p)", False),
        #Observable("Y", ["TES", "Calo", "ChannelMuHad", "1P", eta, "RegSignal", "pTslice4"], "Y(1p)", False),
        #
        #Observable("Y", ["TES", "Calo", "ChannelMuHad", "3P", eta, "RegSignal"], "Y(3p)", False),
        #Observable("Y", ["TES", "Calo", "ChannelMuHad", "3P", eta, "RegFakes"], "Y(3p)", False)
        ]
    )

