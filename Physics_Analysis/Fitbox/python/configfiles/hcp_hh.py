# configuration file for the HCP decay analysis
import ROOT
from AnalysisSetup import *

setup = AnalysisSetup( 
    #standard fixed fields
    standards = Standards(
        lumi = 36.5,
        analysis = 'hcp_hh',
        signals = ['HttTheta'],
        cosmetics = [ #histo key component, label, fill color, line color, line width, marker type
            Cosmetic("Data", "Data", 0, ROOT.kBlack, 2, 20),
            Cosmetic("HttThetaX", "Htt #theta = 90", ROOT.kRed, ROOT.kBlack, 1, 1),
            Cosmetic("HttSM", "HttSM", ROOT.kYellow-7, ROOT.kBlack, 1, 1),
            Cosmetic("Ztt", "Z#rightarrow#tau#tau", 432, ROOT.kBlack, 1, 1),
            Cosmetic("Top", "t/t#bar{t}", 810, ROOT.kBlack, 1, 1), 
            Cosmetic("Fake", "Fakes", ROOT.kYellow, ROOT.kBlack, 1, 1), 
            Cosmetic("Others", "Other", ROOT.kOrange+1, ROOT.kBlack, 1, 1), 
            Cosmetic("Zll", "Z#rightarrow_{}ll", 435, ROOT.kGray+1, 2, 1)
            ],
        signal_label = "Fit",
        join_signals = True,
        stats = False,
        purity = Purity(show=False),
        ),
    #various observables
    observables = [ #variable, [filters], x-axis title, log y-axis
        # Observable("phistar_ipip", ['vbf_lowdr', 'ipip', 'd0sig_high'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        # Observable("phistar_ipip", ['vbf_lowdr', 'ipip', 'd0sig_low'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        # Observable("phistar_iprho", ['vbf_lowdr', 'iprho', 'd0sigy_high'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        # Observable("phistar_iprho", ['vbf_lowdr', 'iprho', 'd0sigy_low'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        # Observable("phistar_rhorho", ['vbf_lowdr', 'rhorho', 'y0y1_high'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        # Observable("phistar_rhorho", ['vbf_lowdr', 'rhorho', 'y0y1_low'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        
        # Observable("phistar_ipip", ['vbf_highdr_tight', 'ipip', 'd0sig_high'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        # Observable("phistar_ipip", ['vbf_highdr_tight', 'ipip', 'd0sig_low'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        # Observable("phistar_iprho", ['vbf_highdr_tight', 'iprho', 'd0sigy_high'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        # Observable("phistar_iprho", ['vbf_highdr_tight', 'iprho', 'd0sigy_low'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        # Observable("phistar_rhorho", ['vbf_highdr_tight', 'rhorho', 'y0y1_high'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        # Observable("phistar_rhorho", ['vbf_highdr_tight', 'rhorho', 'y0y1_low'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        
        # Observable("phistar_ipip", ['vbf_highdr_loose', 'ipip', 'd0sig_high'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        # Observable("phistar_ipip", ['vbf_highdr_loose', 'ipip', 'd0sig_low'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        # Observable("phistar_iprho", ['vbf_highdr_loose', 'iprho', 'd0sigy_high'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        # Observable("phistar_iprho", ['vbf_highdr_loose', 'iprho', 'd0sigy_low'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        # Observable("phistar_rhorho", ['vbf_highdr_loose', 'rhorho', 'y0y1_high'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        # Observable("phistar_rhorho", ['vbf_highdr_loose', 'rhorho', 'y0y1_low'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        
        Observable("phistar_ipip", ['boost_tight', 'ipip', 'd0sig_high'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        Observable("phistar_ipip", ['boost_tight', 'ipip', 'd0sig_low'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        Observable("phistar_iprho", ['boost_tight', 'iprho', 'd0sigy_high'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        Observable("phistar_iprho", ['boost_tight', 'iprho', 'd0sigy_low'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        Observable("phistar_rhorho", ['boost_tight', 'rhorho', 'y0y1_high'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        Observable("phistar_rhorho", ['boost_tight', 'rhorho', 'y0y1_low'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        
        Observable("phistar_ipip", ['boost_loose', 'ipip', 'd0sig_high'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        Observable("phistar_ipip", ['boost_loose', 'ipip', 'd0sig_low'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        Observable("phistar_iprho", ['boost_loose', 'iprho', 'd0sigy_high'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        Observable("phistar_iprho", ['boost_loose', 'iprho', 'd0sigy_low'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        Observable("phistar_rhorho", ['boost_loose', 'rhorho', 'y0y1_high'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        Observable("phistar_rhorho", ['boost_loose', 'rhorho', 'y0y1_low'], "#it{#varphi}*_{#kern[-0.4]{CP}}", False),
        ]
    )
