try:
  # command line agrument parser
  import argparse
  parser = argparse.ArgumentParser()
  parser.add_argument('-ws', '--wsInputFile', help='Name of the combined WS input root file')
  parser.add_argument('-sws', '--secondaryWsInputFile', default = None, help='Name of the combined WS input root file.')
  parser.add_argument('-o', '--overallSysFileName', default = "overallsystematics13TevZtt.txt", help='Name of the overall sys file for global yield variations')
  args = parser.parse_args()

  # have to import root after the parser
  from ROOT import TFile, TH1D, TCanvas, Double, kBlack, kRed, kGreen, kBlue
  from wsPostProcess import *
  from math import sqrt


  # open the WS inpt file
  f    = TFile.Open(args.wsInputFile)
  
  print "Loading WS input folder tree..."
  tree = readFolderTree(f)
  print "...done"
  
  if args.secondaryWsInputFile:
    print "Loading WS secondary input folder tree..."
    f2    = TFile.Open(args.secondaryWsInputFile)
    tree2 = readFolderTree(f2)
    tree = dict(tree.items() + tree2.items())
    
    print "...done"
    
  
  # names of the systematic variation histograms to be processed (without _high/low suffix) and the corresponding samples
  variations  = ['theory_ztt_ckk', 'theory_ztt_qsf', 'theory_ztt_PDF', 'theory_ztt_mur_muf_envelope', 'theory_ztt_MGvsSh']
  sample = 'Ztt'

  # names of the categories (cba and mva)
  categories = [ 'boost', 'vbf', 'boost_mva', 'vbf_mva' ]
  
  # define indices for easier access of the lists
  chNames = ['ll', 'lh', 'hh']
  ll = 0 
  lh = 1
  hh = 2
  
  # define CBA and MVA regions
  regionsDict = { 'boost' :  [('LL SR BOOST(Loose)',ll , ['llAll_cba_boost_loose_signal']),
                              ('LL SR BOOST(Tight)',ll , ['llAll_cba_boost_tight_signal']),
                              ('LL SR INC LFV'     ,ll , ['llAll_lfv_inc_signal']),
                              ('LH SR BOOST(Loose)',lh , ['mhAll_cba_boost_loose_signal', 'ehAll_cba_boost_loose_signal']),
                              ('LH SR BOOST(Tight)',lh , ['mhAll_cba_boost_tight_signal', 'ehAll_cba_boost_tight_signal']),
                              ('HH SR BOOST(Loose)',hh , ['hhAll_cba_boost_loose_signal']),
                              ('HH SR BOOST(Tight)',hh , ['hhAll_cba_boost_tight_signal']),
                              ('LL Top BOOST'      ,ll , ['llAll_cba_boost_top']),
                              ('LL Zll BOOST'      ,ll , ['llAll_cba_boost_zll']),
                              ('LL Ztt INC LFV'    ,ll , ['llAll_lfv_inc_ztt']),
                              ('LL Top INC LFV'    ,ll , ['llAll_lfv_inc_top']),
                              ('LH Top BOOST'      ,lh , ['mhAll_cba_boost_top', 'ehAll_cba_boost_top']),
                              ],
  
                  'vbf' :    [('LL SR VBF(Loose)'  ,ll , ['llAll_cba_vbf_loose_signal']),
                              ('LL SR VBF(Tight)'  ,ll , ['llAll_cba_vbf_tight_signal']),
                              ('LL SR VBF LFV'     ,ll , ['llAll_lfv_vbf_signal']),
                              ('LH SR VBF(Loose)'  ,lh , ['mhAll_cba_vbf_loose_signal', 'ehAll_cba_vbf_loose_signal']),
                              ('LH SR VBF(Tight)'  ,lh , ['mhAll_cba_vbf_tight_signal', 'ehAll_cba_vbf_tight_signal']),
                              ('HH SR VBF(high-pT)',hh , ['hhAll_cba_vbf_lowdr_signal']),
                              ('HH SR VBF(Loose)'  ,hh , ['hhAll_cba_vbf_highdr_loose_signal']),
                              ('HH SR VBF(Tight)'  ,hh , ['hhAll_cba_vbf_highdr_tight_signal']),
                              ('LL Top VBF'        ,ll , ['llAll_cba_vbf_top']),
                              ('LL Zll VBF'        ,ll , ['llAll_cba_vbf_zll']),
                              ('LL Ztt VBF LFV'    ,ll , ['llAll_lfv_vbf_ztt']),
                              ('LL Top VBF LFV'    ,ll , ['llAll_lfv_vbf_top']),                              
                              ('LH Top VBF'        ,lh , ['mhAll_cba_vbf_top', 'ehAll_cba_vbf_top']),
                              ],
                              
                  'boost_mva' :  [('LL MVA SR BOOST SF',ll , ['llAll_mva_boost_sf_signal']),
                                  ('LL MVA SR BOOST DF',ll , ['llAll_mva_boost_df_signal']),
                                  ('LH MVA SR BOOST'   ,lh , ['mhAll_mva_boost_signal', 'ehAll_mva_boost_signal']),
                                  ('HH MVA SR BOOST'   ,hh , ['hhAll_mva_boost_signal']),
                                  ('LL Top BOOST'      ,ll , ['llAll_mva_boost_top']),
                                  ('LL Zll BOOST'      ,ll , ['llAll_mva_boost_zll']),
                                  ('LH Top BOOST'      ,lh , ['mhAll_mva_boost_top', 'ehAll_mva_boost_top']),
                                  ],                              
                                  
                  'vbf_mva' : [('LL MVA SR VBF'     ,ll , ['llAll_mva_vbf_signal']),
                               ('LH MVA SR VBF'     ,lh , ['mhAll_mva_vbf_signal', 'ehAll_mva_vbf_signal']),
                               ('HH MVA SR VBF'     ,hh , ['hhAll_mva_vbf_signal']),
                               ('LL Top VBF'        ,ll , ['llAll_mva_vbf_top']),
                               ('LL Zll VBF'        ,ll , ['llAll_mva_vbf_zll']),
                               ('LH Top VBF'        ,lh , ['mhAll_mva_vbf_top', 'ehAll_mva_vbf_top']),
                              ],
                              }
  
  ## TODO: Find out, how is it with the "hhAll_mva_restmmc_control"? Is it a category of it's own? In that case we do not need to include it here
  
  # overall syst file content
  lines = []
  
  # loop over categories
  for category in categories:
    
    # define which regions are used in the fit
    regions = regionsDict[category]
    
    # loop over variation (CKKW, QSF, etc)
    for variation in variations:
      # initialize the global normalization factors
      totalYield     = 0
      totalYieldHigh = 0
      totalYieldLow  = 0
      
      #initialize the per-channel normalization factors
      channelYield     = [0, 0, 0]
      channelYieldHigh = [0, 0, 0]
      channelYieldLow  = [0, 0, 0]
      
  
      # loop over regions
      allRegionsAvailable = True
      for name,ch,subRegions in regions:
        #initialize variables to hold yields of the sub-categories
        nominal = 0  # nominal yield in the region
        varHigh = 0   # up-variation yield in the region
        varLow  = 0   # down-variation yield in the region
        
        # loop over sub-regions and sum up the yields
        # this is needed because in lep-had inputs the folders are split by flavor and have to be combined
        for region in subRegions:
          # retrieve plots
          if region in tree:
            plotNominal = tree[region][sample]['nominal']
            if '{}_high'.format(variation) in tree[region][sample]:
              plotHigh    = tree[region][sample]['{}_high'.format(variation)]
            if '{}_low'.format(variation) in tree[region][sample]:
              plotLow     = tree[region][sample]['{}_low'.format(variation)]
          
            # calculate integral yields
            nominal += plotNominal.Integral()
            if '{}_high'.format(variation) in tree[region][sample]:
              varHigh += plotHigh.Integral()
            if '{}_low'.format(variation) in tree[region][sample]:
              varLow  += plotLow.Integral()
          
          else:
            allRegionsAvailable = False
          
        # add the yields up to get the total yield accross all the regions
        totalYield     += nominal
        totalYieldHigh += varHigh
        totalYieldLow  += varLow
        
        # add the yields up to get the "per-channel" yields
        # N.B. we are only using both signal and control regions to get the totalYield
        channelYield    [ch] += nominal
        channelYieldHigh[ch] += varHigh
        channelYieldLow [ch] += varLow
      
      # calculate relative variations between channels
      if allRegionsAvailable:
        print "Global yield variations of {} in {} category".format(variation, category)
        for ch in [ll,lh,hh]:
          print "  {}_{}_{}_high {}".format(chNames[ch], category, variation, (channelYieldHigh[ch] / channelYield[ch] * totalYield / totalYieldHigh))
          print "  {}_{}_{}_low  {}".format(chNames[ch], category, variation, (channelYieldLow[ch]  / channelYield[ch] * totalYield / totalYieldLow))
          
          #also store in the txt file
          lines += [ "{}_{}_{}_high {}\n".format(chNames[ch], category, variation, (channelYieldHigh[ch] / channelYield[ch] * totalYield / totalYieldHigh)) ]
          lines += [ "{}_{}_{}_low  {}\n".format(chNames[ch], category, variation, (channelYieldLow[ch]  / channelYield[ch] * totalYield / totalYieldLow)) ]
        
      # Rescale the original variation hostograms in the WS inputs so that they do not change the total yield in each channel
      for name,ch,subRegions in regions:
        for region in subRegions:
          if region in tree:
            # clone the original histograms
            if '{}_high'.format(variation) in tree[region][sample]:
              newPlotHigh = tree[region][sample]['{}_high'.format(variation)].Clone('{}_constrained_high'.format(variation))
            if '{}_low'.format(variation) in tree[region][sample]:
              newPlotLow  = tree[region][sample]['{}_low' .format(variation)].Clone('{}_constrained_low' .format(variation))
            
            # now do the actual rescaling (N.B. different scale used for different channels)
            if '{}_high'.format(variation) in tree[region][sample]:
              newPlotHigh.Scale(  channelYield[ch]/channelYieldHigh[ch] )
            if '{}_low'.format(variation) in tree[region][sample]:
              newPlotLow .Scale(  channelYield[ch]/channelYieldLow [ch] )
            
            # store the new plots in the WS input folder tree
            if '{}_high'.format(variation) in tree[region][sample]:
              tree[region][sample][newPlotHigh.GetName()] = newPlotHigh
            if '{}_low'.format(variation) in tree[region][sample]:
              tree[region][sample][newPlotLow .GetName()] = newPlotLow
  
  # perform sanity checks: check if the channel yield after the constrain match the nominal
  print "Sanity check for the rescaled variation histograms: (N.B. constrained variation yields in each channel should be the same as nominal)"
  for category in categories:
    regions = regionsDict[category]
    for variation in variations:
      channelYield                = [0, 0, 0]
      channelYieldConstrainedHigh = [0, 0, 0]
      channelYieldConstrainedLow  = [0, 0, 0]
      for name,ch,subRegions in regions:
        nominal = 0; varHigh = 0; varLow  = 0
        for region in subRegions:
          if region in tree:
            nominal += tree[region][sample]['nominal'].Integral()
            if '{}_constrained_high'.format(variation) in tree[region][sample]:
              varHigh += tree[region][sample]['{}_constrained_high'.format(variation)].Integral()
            if '{}_constrained_low'.format(variation) in tree[region][sample]:
              varLow  += tree[region][sample]['{}_constrained_low'.format(variation)].Integral()
        channelYield               [ch] += nominal
        channelYieldConstrainedHigh[ch] += varHigh
        channelYieldConstrainedLow [ch] += varLow
      for ch in [ll,lh,hh]:
        if channelYield[ch]!=0:
          print "{} {}: nominal {}, {}_constrained_high {}, {}_constrained_low {}".format(category, chNames[ch], channelYield[ch], variation, channelYieldConstrainedHigh[ch], variation, channelYieldConstrainedLow [ch])
      
  
  # finally, save the modified WS input tree and overallsyst file:
  if '.root' in args.wsInputFile:
    newFileName = args.wsInputFile.replace('.root','_zttTheoryConstrained.root')
  else:
    newFileName = args.wsInputFile + '_zttTheoryConstrained'
  
  print "Creating the processed WS input file {} ...".format(newFileName)
  fOut = TFile.Open(newFileName, 'recreate')
  save(tree, fOut)
  print "...done"
  
  if len(lines)!=0:
    print "Creating the overall sys file {} ...".format(args.overallSysFileName)
    with open(args.overallSysFileName,"w") as ff:
      for line in lines:
        ff.write(line)
       
    print "...done"

except SystemExit as e:
  pass
except Exception:
  raise


# the end
