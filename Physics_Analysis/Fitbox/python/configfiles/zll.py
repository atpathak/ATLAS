# configuration file for the in-situ TES analysis
import ROOT
from AnalysisSetup import *

setup = AnalysisSetup( 
    standards = Standards(
        lumi = 36.5,
        analysis = 'Zll',
        signal = 'Zmumu',
        cosmetics = { #component, fill color, line color, line width, marker type
            "Data" :[0, ROOT.kBlack, 2, 20],
            "Zmumu"  :[ROOT.kAzure-4, ROOT.kAzure, 2, 1],
            "top"  :[ROOT.kMagenta-9, ROOT.kMagenta, 2, 1], 
            "ZmumuSS" :[ROOT.kGreen-7, ROOT.kGreen+1, 2, 1], 
            "Wjets"    :[ROOT.kOrange-4, ROOT.kOrange-3, 2, 1], 
            "Zjets"  :[ROOT.kGray, ROOT.kGray+1, 2, 1],
            "VV"  :[ROOT.kRed, ROOT.kRed-1, 2, 1],
            "ttV"  :[ROOT.kYellow-6, ROOT.kYellow+2, 2, 1],
            }
        ),
    observables = [ #variable, [filters], x-axis title, log y-axis
        Observable("mass", ["Channel_Zmumu"], "m_{#mu#mu} [GeV]", True)
        ]
    )

