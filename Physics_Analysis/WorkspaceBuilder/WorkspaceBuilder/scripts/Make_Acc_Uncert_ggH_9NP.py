## Create acceptance uncertainties for the ggH QCD uncertainties (i.e. the so called 9NP scheme)
## by renormalizing the default inputs by total-xsection variation:
##
##   acc_variation = variation.Scale( sum_of_weights_nominal / sum_of_weights_variation )
##
## where sum_of_weights_nominal and sum_of_weights_variation are sums of events weights 
## of all generated events.
##
## We use the following assumptons:
##  1. ggH for the lep-lep channel consists only from the DSID 345120
##  2. ggH for the had-had channel consists only from the DSID 345123
##  3. ggH for the lep-had channel consists only from the DSIDs 345121 and 345121
##  4. There is no difference in acceptance between positive and negative lepton channels in lep-had

try:
  # command line agrument parser
  import argparse
  parser = argparse.ArgumentParser()
  parser.add_argument('-ws', '--wsInputFile', help='Name of the combined WS input root file')
  args = parser.parse_args()

  # have to import root after the parser
  from ROOT import TFile
  from wsPostProcess import *

  # open the WS inpt file
  f    = TFile.Open(args.wsInputFile)
  
  print "Loading WS input folder tree..."
  tree = readFolderTree(f)
  print "...done"
  
  # sample
  sample = 'ggH'
  
  # normalization factors:
  ll_sum_of_weights_nominal = 53957603.57
  ll_sum_of_weights = [ ('theory_sig_qcd_0_high', 56492715.88),	
                        ('theory_sig_qcd_1_high', 55185917.43),	
                        ('theory_sig_qcd_2_high', 54041298.10),	
                        ('theory_sig_qcd_3_high', 54047280.43),	
                        ('theory_sig_qcd_4_high', 54121620.68),	
                        ('theory_sig_qcd_5_high', 53956505.47),	
                        ('theory_sig_qcd_6_high', 54076640.70),	
                        ('theory_sig_qcd_7_high', 54182534.31),	
                        ('theory_sig_qcd_8_high', 54092313.60),
                      ]
  
  lh_sum_of_weights_nominal = 42976109.88
  lh_sum_of_weights = [ ('theory_sig_qcd_0_high', 44991163.54),	
                        ('theory_sig_qcd_1_high', 43941580.52),	
                        ('theory_sig_qcd_2_high', 43010396.20),	
                        ('theory_sig_qcd_3_high', 43060393.68),	
                        ('theory_sig_qcd_4_high', 43104010.47),	
                        ('theory_sig_qcd_5_high', 42974999.18),	
                        ('theory_sig_qcd_6_high', 43069327.90),	
                        ('theory_sig_qcd_7_high', 43171852.50),	
                        ('theory_sig_qcd_8_high', 43095802.77),
                      ]
  
  hh_sum_of_weights_nominal = 53538670.72
  hh_sum_of_weights = [ ('theory_sig_qcd_0_high', 56042603.54),	
                        ('theory_sig_qcd_1_high', 54729386.89),	
                        ('theory_sig_qcd_2_high', 53568183.13),	
                        ('theory_sig_qcd_3_high', 53620581.91),	
                        ('theory_sig_qcd_4_high', 53695584.28),	
                        ('theory_sig_qcd_5_high', 53540003.79),	
                        ('theory_sig_qcd_6_high', 53631218.96),	
                        ('theory_sig_qcd_7_high', 53751977.36),	
                        ('theory_sig_qcd_8_high', 53671009.16),
                      ]
	
	
	
  # loop over regions
  for regionName,region in tree.iteritems():
    if type(region) == dict and sample in region:
      # pull out the appropriate normalzation factors for this channels
      if 'llAll' in regionName:
        sum_of_weights_nominal = ll_sum_of_weights_nominal
        sum_of_weights = ll_sum_of_weights
      elif 'ehAll' in regionName or 'mhAll' in regionName:
        sum_of_weights_nominal = lh_sum_of_weights_nominal
        sum_of_weights = lh_sum_of_weights
      elif 'hhAll' in regionName:
        sum_of_weights_nominal = hh_sum_of_weights_nominal
        sum_of_weights = hh_sum_of_weights
      else:
        raise ValueError("Cannot determine channel for region {}".format(regionName))
      
      print "{}/{}".format(regionName, sample)
      
      # modify the variations by a correction factor sum_of_weights_nominal / sum_of_weights_variation
      for histName, sum_of_weights_var in sum_of_weights:
        print "  {}".format(histName)
        if histName not in region[sample] or region[sample][histName].Integral() == 0:
          print "    not available or has 0 intergal" 
        else:
          # create a renormalized variation
          histNameNew = histName.replace("_high", "_acceptance_high")
          new_var = region[sample][histName].Clone(histNameNew)
          new_var.Scale( sum_of_weights_nominal / sum_of_weights_var )
          
          # store rescaled histograms in the tree:
          region[sample][new_var.GetName()] = new_var
          
          # debug printout
          print "    original overall variation {}".format(region[sample][histName].Integral() / region[sample]['nominal'].Integral() )          
          print "    rescaled overall variation {} (was rescaled by {})".format(region[sample][histNameNew].Integral() / region[sample]['nominal'].Integral(), sum_of_weights_nominal / sum_of_weights_var )
          
          
  # save the updated tree
  if '.root' in args.wsInputFile:
    newFileName = args.wsInputFile.replace('.root','_accGGH9NP.root')
  else:
    newFileName = args.wsInputFile + '_accGGH9NP'
  
  print "Creating the processed WS input file {} ...".format(newFileName)
  fOut = TFile.Open(newFileName, 'recreate')
  save(tree, fOut)
  print "...done"
  


except SystemExit as e:
  pass
except Exception:
  raise


# the end
