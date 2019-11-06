# configuration file for the SM H->tautau lephad analysis
import ROOT
from AnalysisSetup import *

setup = AnalysisSetup( 
    #standard fixed fields
    standards = Standards(
        lumi = 36.5,
        analysis = 'LFV',
        signals = ['VBFH', 'ggH'],
        cosmetics = [ #histo key component, label, fill color, line color, line width, marker type
            Cosmetic("Data", "Data", 0, ROOT.kBlack, 2, 20),
            Cosmetic("VBFH", "VBFH", ROOT.kYellow-7, ROOT.kYellow, 2, 1),
            Cosmetic("ggH", "ggH", ROOT.kYellow-7, ROOT.kYellow, 2, 1),
            Cosmetic("Ztt", "Z#rightarrow#tau#tau", ROOT.kAzure-4, ROOT.kAzure, 2, 1),
            Cosmetic("Top", "t/t#bar{t}", ROOT.kMagenta-9, ROOT.kMagenta, 2, 1), 
            Cosmetic("Fakes", "Fakes", ROOT.kGreen-7, ROOT.kGreen+1, 2, 1), 
            Cosmetic("VV", "VV", ROOT.kOrange-4, ROOT.kOrange-3, 2, 1),
            Cosmetic("VH", "VH", ROOT.kRed+3, ROOT.kRed+2, 2, 1),
            Cosmetic("Zll", "Z#rightarrow_{}ll", ROOT.kGray, ROOT.kGray+1, 2, 1),
            #Cosmetic("eFakes", "efake Z#rightarrow_{}ll", ROOT.kGray, ROOT.kGray+1, 2, 1),
            Cosmetic("Htt", "SM H", ROOT.kGray, ROOT.kGray+2, 2, 1)
            ],
        #signal_label = "H#rightarrowe#tau",
        signal_label = "H#rightarrow#mu#tau",
        join_signals = True,
        stats = True,
        purity = Purity(show = False, line_color = ROOT.kYellow+2, yaxis_x = 0.0, yaxis_y_low = 0.4e3, yaxis_y_up = 1e3, yaxis_y_auto = False)
        ),
    #various observables
    observables = [ #variable, [filters], x-axis title, log y-axis
   #     Observable("MMC", ["Htt", "yearAll", "chanlh","boostlowpth", "regsig","CBA"], "BoostLowPtH", False),
   #     Observable("MMC", ["Htt", "yearAll", "chanlh","boosthighpth","regsig","CBA"], "BoostHighPtH", False),
   #	Observable("MMC", ["Htt", "yearAll", "chanlh","boost","regtop","CBA"], "BoostTop", False),
   #     Observable("MColl", ["etau","sr1","vbf","signal"], "", False),
    #    Observable("MColl", ["etau","sr2","vbf","signal"], "", False),
    #	Observable("MColl", ["etau","sr3","vbf","signal"], "", False),
     #   Observable("MColl", ["etau","cba_vbf","signal"], "", False),
        Observable("MColl", ["chanmtau","regsr1vbf","selCBA","signal"], "", False),
        Observable("MColl", ["chanmtau","regsr2vbf","selCBA","signal"], "", False),
    	Observable("MColl", ["chanmtau","regsr3vbf","selCBA","signal"], "", False),
        Observable("MColl", ["chanmtau","regvbf","selCBA","signal"], "", False),
      	#Observable("MMC", ["Htt", "yearAll", "chanlh","vbftight","regsig","CBA"], "VBFTight", False),
        #Observable("MMC", ["Htt", "yearAll", "chanlh","vbf","regtop","CBA"],"VBFTop",False),
        ]
    )
#ggH__obs_x_LFV_mtau_cba_sr3_vbf_signal
#CHECK :	L_x_ggH_LFV_mtau_cba_vbf_signal_overallSyst_x_HistSyst	10.0655 +-2.71791
#CHECK :	L_x_VBFH_LFV_mtau_cba_vbf_signal_overallSyst_x_HistSyst	19.6655 +-4.19703
#CHECK :	L_x_Top_LFV_mtau_cba_vbf_signal_overallSyst_x_StatUncert	72.7068 +-8.46731
#CHECK :	L_x_VV_LFV_mtau_cba_vbf_signal_overallSyst_x_StatUncert	40.5036 +-2.80974
#CHECK :	L_x_Zll_LFV_mtau_cba_vbf_signal_overallSyst_x_StatUncert	47.4293 +-4.19989
#CHECK :	L_x_Ztt_LFV_mtau_cba_vbf_signal_overallSyst_x_StatUncert	318.253 +-19.6469
#CHECK :	L_x_Fakes_LFV_mtau_cba_vbf_signal_overallSyst_x_StatUncert	211.487 +-5.69418
#CHECK :	L_x_Htt_LFV_mtau_cba_vbf_signal_overallSyst_x_StatUncert	21.6283 +-1.6603


