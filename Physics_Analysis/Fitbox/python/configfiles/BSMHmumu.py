# configuration file for the SM H->tautau lephad analysis
import ROOT
from AnalysisSetup import *

'''
Diboson & 167.978 $\pm$ 1.06527 \\
	      SingleTop & 1543.06 $\pm$ 10.5623 \\
		      ttbar & 17305.1 $\pm$ 100.035 \\
			      ZjetsbTag & 2824.98 $\pm$ 12.7956 \\
				      bbH4004 & 32.9717 $\pm$ 23.6221 \\
					'''


setup = AnalysisSetup( 
    #standard fixed fields
    standards = Standards(
        lumi = 36.5,
        analysis = 'BSMHmumu',
        signals = ['bbH4004'],
        cosmetics = [ #histo key component, label, fill color, line color, line width, marker type
            Cosmetic("Data", "Data", 0, ROOT.kBlack, 2, 20),
            Cosmetic("bbH4004", "bbH4004", ROOT.kYellow-7, ROOT.kYellow, 2, 1),
            Cosmetic("ZjetsbTag", "ZjetsbTag", ROOT.kAzure-4, ROOT.kAzure, 2, 1),
            Cosmetic("ZjetsbVeto", "ZjetsbVeto", ROOT.kAzure-4, ROOT.kAzure, 2, 1),
            Cosmetic("ttbar", "t#bar{t}", ROOT.kMagenta-9, ROOT.kMagenta, 2, 1), 
            Cosmetic("SingleTop", "SingleTop", ROOT.kGreen-7, ROOT.kGreen+1, 2, 1), 
            Cosmetic("Diboson", "Diboson", ROOT.kOrange-4, ROOT.kOrange-3, 2, 1) 
            #Cosmetic("Zll", "Z#rightarrow_{}ll", ROOT.kGray, ROOT.kGray+1, 2, 1)
            ],
        signal_label = "",
        join_signals = False,
        stats = True,
        purity = False,
        ),
    #various observables
    observables = [ #variable, [filters], x-axis title, log y-axis
	 Observable("mass", ["SRbTag"], "M(#mu#mu) [GeV]", False),
	 Observable("mass", ["SRbVeto"], "M(#mu#mu) [GeV]", False),
	 Observable("mass", ["CRbTag"], "M(#mu#mu) [GeV]", False),
	 Observable("mass", ["CRbVeto"], "M(#mu#mu) [GeV]", False),
	 Observable("mass", ["CRttbar"], "M(#mu#mu) [GeV]", False),
	 ]
    )
