# configuration file for the SM H->tautau lephad analysis
import ROOT
from AnalysisSetup import *

setup = AnalysisSetup( 
    #standard fixed fields
    standards = Standards(
        lumi = 36.5,
        analysis = 'SMlh',
        signals = ['VBFH125', 'ggH125', 'WH125', 'ZH125', 'ttH125'],
        cosmetics = [ #histo key component, label, fill color, line color, line width, marker type
            Cosmetic("Data", "Data", 0, ROOT.kBlack, 2, 20),
            Cosmetic("VBFH125", "VBF H(125 GeV)", ROOT.kYellow-7, ROOT.kYellow, 2, 1),
            Cosmetic("ggH125", "ggH(125 GeV)", ROOT.kYellow-7, ROOT.kYellow, 2, 1),
            Cosmetic("WH125", "WH(125 GeV)", ROOT.kYellow-7, ROOT.kYellow, 2, 1),
            Cosmetic("ZH125", "ZH(125 GeV)", ROOT.kYellow-7, ROOT.kYellow, 2, 1),
            Cosmetic("ttH125", "ttH(125 GeV)", ROOT.kYellow-7, ROOT.kYellow, 2, 1),
            Cosmetic("Ztt", "Z#rightarrow#tau#tau", ROOT.kAzure-4, ROOT.kAzure, 2, 1),
            Cosmetic("Top", "t/t#bar{t}", ROOT.kMagenta-9, ROOT.kMagenta, 2, 1), 
            Cosmetic("Fake", "Fakes", ROOT.kGreen-7, ROOT.kGreen+1, 2, 1), 
            Cosmetic("Otherslh", "Other", ROOT.kOrange-4, ROOT.kOrange-3, 2, 1), 
            Cosmetic("Zll", "Z#rightarrow_{}ll", ROOT.kGray, ROOT.kGray+1, 2, 1)
            ],
        signal_label = "Higgs#rightarrow#tau#tau(125 GeV)",
        join_signals = True,
        stats = True,
        purity = False,
        ),
    #various observables
    observables = [ #variable, [filters], x-axis title, log y-axis
   #     Observable("MMC", ["Htt", "yearAll", "chanlh","boostlowpth", "regsig","CBA"], "BoostLowPtH", False),
   #     Observable("MMC", ["Htt", "yearAll", "chanlh","boosthighpth","regsig","CBA"], "BoostHighPtH", False),
   #	Observable("MMC", ["Htt", "yearAll", "chanlh","boost","regtop","CBA"], "BoostTop", False),
    	Observable("MMC", ["Htt", "yearAll", "chanlh","vbf","regsig","CBA"], "VBFLoose", False),
      	#Observable("MMC", ["Htt", "yearAll", "chanlh","vbftight","regsig","CBA"], "VBFTight", False),
        #Observable("MMC", ["Htt", "yearAll", "chanlh","vbf","regtop","CBA"],"VBFTop",False),
        ]
    )
