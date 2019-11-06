try:
  # command line agrument parser
  import argparse
  parser = argparse.ArgumentParser()
  parser.add_argument('-ws', '--wsInputFile', help='Name of the combined WS input root file')
  parser.add_argument('-o', '--overallSysFileName', default = "WorkspaceBuilder/data/Htautau/overallsystematics13TevZtt.txt", help='Name of the overall sys file for global yield variations')
  parser.add_argument('--smooth' , default=False, action='store_true', help='Smooth the relative variations based on preselection histograms.')
  parser.add_argument('--dontRebinHH', default=False, action='store_true', help='By default last bins (140-200 GeV) of the hh preselection plot will be merged to reduce stat. fluctuations. Use this option to turn this behavior off.')
  
  # shape variations
  parser.add_argument('--cba_ll_weight', default = "hist", help='Weight function to be used to create the MG vs Sherpa shape variation in CBA LL channel. If \"hist\" option is chosen (default), MG vs Sherpa histograms at preselection will be used directly as weight functions.')  
  parser.add_argument('--cba_lh_weight', default = "hist", help='Weight function to be used to create the MG vs Sherpa shape variation in CBA LH channel. If \"hist\" option is chosen (default), MG vs Sherpa histograms at preselection will be used directly as weight functions.')  
  parser.add_argument('--cba_hh_weight', default = "hist", help='Weight function to be used to create the MG vs Sherpa shape variation in CBA HH channel. If \"hist\" option is chosen (default), MG vs Sherpa histograms at preselection will be used directly as weight functions.')  
  
  parser.add_argument('--mva_ll_weight', default = "", help='Weight function to be used to create the MG vs Sherpa shape variation in MVA LL channel.')  
  parser.add_argument('--mva_lh_weight', default = "", help='Weight function to be used to create the MG vs Sherpa shape variation in MVA LH channel.')  
  parser.add_argument('--mva_hh_weight', default = "", help='Weight function to be used to create the MG vs Sherpa shape variation in MVA HH channel.')  

  # CBA overall variations
  parser.add_argument('--ll_boost_loose_theory_ztt_MGvsSh_high', default = 1., help='Overall LL Boosted Loose variation')
  parser.add_argument('--ll_boost_tight_theory_ztt_MGvsSh_high', default = 1., help='Overall LL Boosted tight variation')
  parser.add_argument('--lh_boost_loose_theory_ztt_MGvsSh_high', default = 1., help='Overall LH Boosted Loose variation')
  parser.add_argument('--lh_boost_tight_theory_ztt_MGvsSh_high', default = 1., help='Overall LH Boosted tight variation')
  parser.add_argument('--hh_boost_loose_theory_ztt_MGvsSh_high', default = 1., help='Overall HH Boosted Loose variation')
  parser.add_argument('--hh_boost_tight_theory_ztt_MGvsSh_high', default = 1., help='Overall HH Boosted tight variation')
  parser.add_argument('--ll_vbf_loose_theory_ztt_MGvsSh_high',   default = 1., help='Overall LL VBF Loose variation')
  parser.add_argument('--ll_vbf_tight_theory_ztt_MGvsSh_high',   default = 1., help='Overall LL VBF tight variation')
  parser.add_argument('--lh_vbf_loose_theory_ztt_MGvsSh_high',   default = 1., help='Overall LH VBF Loose variation')
  parser.add_argument('--lh_vbf_tight_theory_ztt_MGvsSh_high',   default = 1., help='Overall LH VBF tight variation')
  parser.add_argument('--hh_vbf_loose_theory_ztt_MGvsSh_high',   default = 1., help='Overall HH VBF Loose variation')
  parser.add_argument('--hh_vbf_tight_theory_ztt_MGvsSh_high',   default = 1., help='Overall HH VBF tight variation')
  parser.add_argument('--hh_vbf_highPt_theory_ztt_MGvsSh_high',  default = 1., help='Overall HH VBF high-pT variation')
  
  # MVA overall variations
  parser.add_argument('--ll_boost_sf_theory_ztt_MGvsSh_high', default = 1., help='Overall LL Boosted SF variation (MVA region)')
  parser.add_argument('--ll_boost_df_theory_ztt_MGvsSh_high', default = 1., help='Overall LL Boosted DF variation (MVA region)')
  parser.add_argument('--lh_boost_theory_ztt_MGvsSh_high'   , default = 1., help='Overall LH Boosted variation (MVA region)')
  parser.add_argument('--hh_boost_theory_ztt_MGvsSh_high'   , default = 1., help='Overall HH Boosted variation (MVA region)')
  parser.add_argument('--ll_vbf_theory_ztt_MGvsSh_high'     , default = 1., help='Overall LL VBF variation (MVA region)')
  parser.add_argument('--lh_vbf_theory_ztt_MGvsSh_high'     , default = 1., help='Overall LH VBF variation (MVA region)')
  parser.add_argument('--hh_vbf_theory_ztt_MGvsSh_high'     , default = 1., help='Overall HH VBF variation (MVA region)')

  args = parser.parse_args()

  # have to import root after the parser
  from ROOT import TFile, TCanvas, TF1, Double, kBlack, kRed, kGreen, kBlue
  from wsPostProcess import *
  from math import sqrt
  import os, sys

  # open the WS inpt file
  f    = TFile.Open(args.wsInputFile)
  
  print "Loading WS input folder tree..."
  tree = readFolderTree(f)
  print "...done"
  
  # sample
  sample = 'Ztt'

  # names of the categories (cba and mva)
  categories = [ 'boost', 'vbf', 'boost_mva', 'vbf_mva' ]
  
  # define indices for easier access of the lists
  chNames = ['ll', 'lh', 'hh']
  ll = 0 
  lh = 1
  hh = 2
  
  # define CBA and MVA regions
  regionsDict = { 'boost' :  [('ll_boost_loose_theory_ztt_MGvsSh_high',ll , ['llAll_cba_boost_loose_signal']),
                              ('ll_boost_tight_theory_ztt_MGvsSh_high',ll , ['llAll_cba_boost_tight_signal']),
                              ('lh_boost_loose_theory_ztt_MGvsSh_high',lh , ['mhAll_cba_boost_loose_signal', 'ehAll_cba_boost_loose_signal']),
                              ('lh_boost_tight_theory_ztt_MGvsSh_high',lh , ['mhAll_cba_boost_tight_signal', 'ehAll_cba_boost_tight_signal']),
                              ('hh_boost_loose_theory_ztt_MGvsSh_high',hh , ['hhAll_cba_boost_loose_signal']),
                              ('hh_boost_tight_theory_ztt_MGvsSh_high',hh , ['hhAll_cba_boost_tight_signal']),
                              ],
  
                  'vbf' :    [('ll_vbf_loose_theory_ztt_MGvsSh_high',  ll , ['llAll_cba_vbf_loose_signal']),
                              ('ll_vbf_tight_theory_ztt_MGvsSh_high',  ll , ['llAll_cba_vbf_tight_signal']),
                              ('lh_vbf_loose_theory_ztt_MGvsSh_high',  lh , ['mhAll_cba_vbf_loose_signal', 'ehAll_cba_vbf_loose_signal']),
                              ('lh_vbf_tight_theory_ztt_MGvsSh_high',  lh , ['mhAll_cba_vbf_tight_signal', 'ehAll_cba_vbf_tight_signal']),
                              ('hh_vbf_loose_theory_ztt_MGvsSh_high',  hh , ['hhAll_cba_vbf_highdr_loose_signal']),
                              ('hh_vbf_tight_theory_ztt_MGvsSh_high',  hh , ['hhAll_cba_vbf_highdr_tight_signal']),
                              ('hh_vbf_highPt_theory_ztt_MGvsSh_high', hh , ['hhAll_cba_vbf_lowdr_signal']),
                              ],
                              
                  'boost_mva' :  [('ll_boost_sf_theory_ztt_MGvsSh_high' ,ll , ['llAll_mva_boost_sf_signal']),
                                  ('ll_boost_df_theory_ztt_MGvsSh_high' ,ll , ['llAll_mva_boost_df_signal']),
                                  ('lh_boost_theory_ztt_MGvsSh_high'    ,lh , ['mhAll_mva_boost_signal', 'ehAll_mva_boost_signal']),
                                  ('hh_boost_theory_ztt_MGvsSh_high'    ,hh , ['hhAll_mva_boost_signal']),
                                  ],                              
                                  
                  'vbf_mva' : [('ll_vbf_theory_ztt_MGvsSh_high' ,ll , ['llAll_mva_vbf_signal']),
                               ('lh_vbf_theory_ztt_MGvsSh_high' ,lh , ['mhAll_mva_vbf_signal', 'ehAll_mva_vbf_signal']),
                               ('hh_vbf_theory_ztt_MGvsSh_high' ,hh , ['hhAll_mva_vbf_signal']),
                              ],
                 }
  
  # preselection folders (named differently in each channel :-)
  preselDict = { 'cba_ll_weight' : ['llAll_cba_presel_signal'],
                 'cba_lh_weight' : ['ehAll_cba_presel_signal', 'mhAll_cba_presel_signal'],
                 'cba_hh_weight' : ['hhAll_preselection'],
                 
                 'mva_ll_weight' : ['llAll_mva_presel_signal'],
                 'mva_lh_weight' : ['ehAll_mva_presel_signal', 'mhAll_mva_presel_signal'],
                 'mva_hh_weight' : ['hhAll_mva_preselection'],                              # TODO: Is this a correct name?
                 }
  
  # loop over categories
  for category in categories:
    
    # define which regions are used in the fit
    regions = regionsDict[category]
    
    # loop over regions
    for name,ch,subRegions in regions:
      # create the weight function
      weightFunName = 'cba_' if 'cba_' in subRegions[0] else 'mva_'
      weightFunName += chNames[ch] + "_weight"
      
      # loop over sub-regions (only needed for lep-had inputs which use flavor-split inputs)
      for region,preselRegion in zip(subRegions, preselDict[weightFunName]):
        if region in tree:
          
          if hasattr(args, weightFunName) and getattr(args,weightFunName)!="":
            print "In region {}: weight function {} provided and will be aplied: {}".format(region, weightFunName, getattr(args,weightFunName))

            # retrieve nominal plot ...
            plotNominal = tree[region][sample]['nominal']
            
            # ... and make clone 
            plotVariation = plotNominal.Clone("theory_ztt_MGvsSh_shape_high")

            # MG vs Sherpa reweighting:
            if getattr(args,weightFunName) != 'hist':
              # create the weight function using TF1 class
              weightFun = TF1(region+"_weightFun", getattr(args,weightFunName), plotNominal.GetXaxis().GetXmin(), plotNominal.GetXaxis().GetXmax())
            
            else:
              # or use MMC distribution at preselection.
              if preselRegion in tree:
                if 'theory_ztt_MGvsSh_high' in tree[preselRegion][sample]:
                  nominalPresel = tree[preselRegion][sample]['nominal']
                  mgVsShPresel  = tree[preselRegion][sample]['theory_ztt_MGvsSh_high']

                  # normalize to the same integral:
                  if mgVsShPresel.Integral()!=0:
                    mgVsShPresel.Scale(nominalPresel.Integral() / mgVsShPresel.Integral() )
                  else:
                    print "ERROR: histogram 'theory_ztt_MGvsSh_high' in the {}/{} folder is empty".format(region, sample)

                  # apply HH rebinning (hack)
                  if not args.dontRebinHH:
                    nominalSum = 0
                    mgSum      = 0
                    nBins      = 0
                    for b in range(1,nominalPresel.GetNbinsX()+1):
                      if nominalPresel.GetXaxis().GetBinLowEdge(b) >= 110:
                        nominalSum += nominalPresel.GetBinContent(b)
                        mgSum      += mgVsShPresel.GetBinContent(b)
                        nBins += 1
                        
                    for b in range(1,nominalPresel.GetNbinsX()+1):
                      if nominalPresel.GetXaxis().GetBinLowEdge(b) >= 110:
                        nominalPresel.SetBinContent(b, nominalSum / float(nBins) )
                        mgVsShPresel.SetBinContent(b, mgSum / float(nBins) )

                  # extract the binning             
                  new_bins = []
                  for b in range(1,plotNominal.GetNbinsX()+2):
                    new_bins += [ plotNominal.GetXaxis().GetBinLowEdge(b) ] 
                    
                  #rebin the preselection plots
                  nominalPresel = rebin(nominalPresel, new_bins, nominalPresel.GetName()+'_rebin')
                  mgVsShPresel  = rebin(mgVsShPresel , new_bins, mgVsShPresel .GetName()+'_rebin')
                  
                  # create the ratio
                  mgVsShPresel.Divide(nominalPresel)
                  
                  # apply smoothing:
                  if args.smooth:
                    mgVsShPresel.Smooth()
                  
                else:
                  print "ERROR: histogram 'theory_ztt_MGvsSh_high' not stored in the {}/{} folder. Needed for the MG vs Sherpa shape uncertainty construction with the --{} == 'hist' option".format(region, sample, weightFunName)
              else:
                print "ERROR: folder {} not present in the WS inputs. It is needed for the MG vs Sherpa shape uncertainty construction with the --{} == 'hist' option".format(region, weightFunName)
            
            # reweight the variation plot
            for b in range(1,plotVariation.GetNbinsX()+1):
              if getattr(args,weightFunName) != 'hist':
                plotVariation.SetBinContent(b, plotVariation.GetBinContent(b) * weightFun.Eval(plotVariation.GetXaxis().GetBinCenter(b)) )
              else:
                if 'mgVsShPresel' in dir():
                  plotVariation.SetBinContent(b, plotVariation.GetBinContent(b) * mgVsShPresel.GetBinContent(b) )
            
            # renormalize to the same integral as nominal - this is a pure shape uncertainty:
            if plotVariation.Integral()!=0:
              plotVariation.Scale(plotNominal.Integral() / plotVariation.Integral())
              
            # and then apply the overall scale (which is 1. by default --> i.e. shape only systematic)
            if float(getattr(args,name))!=1.:
              print "  Applying overall scale {}".format(float(getattr(args,name)))
              plotVariation.Scale( float(getattr(args,name)) )
              
            # store in the WS input
            tree[region][sample][plotVariation.GetName()] = plotVariation
        
   
  # finally, save the modified WS input tree:
  if '.root' in args.wsInputFile:
   newFileName = args.wsInputFile.replace('.root','_MGvsShShape.root')
   #newFileName = 'WS_inputs/'+newFileName
  else:
   newFileName = 'WS_inputs/'+args.wsInputFile + '_MGvsShShape'
  
  print "Creating the processed WS input file {} ...".format(newFileName)
  fOut = TFile.Open(newFileName, 'recreate')
  save(tree, fOut)
  print "...done"
     
  
except SystemExit as e:
  pass
except Exception:
  raise
  
  
# the end
