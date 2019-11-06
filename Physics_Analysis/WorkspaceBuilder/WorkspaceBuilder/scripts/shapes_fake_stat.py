import os
import sys
import argparse
import ROOT
#from wsPostProcess import readFolderTree, combineFakeStat, save

#------------------------
def envelopePlots(nominalPlot, varPlots, newName):
  
  plotUp   = nominalPlot.Clone(newName+"_high")
  plotDown = nominalPlot.Clone(newName+"_low")

  for b in range(1, nominalPlot.GetNbinsX()+1):
    # set var plots to 0
    plotUp  .SetBinError(b,0)
    plotUp  .SetBinContent(b,0)
    plotDown.SetBinError(b,0)
    plotDown.SetBinContent(b,0)

    # sum the variations in quadrature
    for varPlot in varPlots:
      if varPlot.GetBinContent(b)>nominalPlot.GetBinContent(b): 
        plotUp.SetBinContent(b,plotUp.GetBinContent(b) + (varPlot.GetBinContent(b)-nominalPlot.GetBinContent(b))**2)
      else:
        plotDown.SetBinContent(b,plotDown.GetBinContent(b) + (varPlot.GetBinContent(b)-nominalPlot.GetBinContent(b))**2)
              
    # calculate square-root in each bin
    plotUp  .SetBinContent(b,ROOT.TMath.Sqrt(plotUp  .GetBinContent(b)))
    plotDown.SetBinContent(b,ROOT.TMath.Sqrt(plotDown.GetBinContent(b)))

  
  # construct absolute variations
  plotUp.Add(nominalPlot)
  plotDown.Scale(-1.)
  plotDown.Add(nominalPlot)

  return [plotUp, plotDown]

#------------------------
def readFolderTree(dir, folderName = ''):
  tree = dict()
  tree[folderName] = []
  keys = dir.GetListOfKeys()
  for key in keys:
    if 'TDirectory' in `type(dir.Get(key.GetName()))`:
      tree[folderName] += [ readFolderTree(dir.Get(key.GetName()), key.GetName()) ]
    elif 'TH' in `type(dir.Get(key.GetName()))`:
      h = dir.Get(key.GetName())
      h.SetDirectory(0)
      tree[folderName] += [ h ]
    else:
      raise TypeError('Unknown type %s' % `type(dir.Get(key.GetName()))`)
  
  return tree

## make a copy of the original tree, but with all the fake stat uncertainties combined into one
def combineFakeStat(tree, newName = "ll_fake_stat", fakeUncert = ["ll_fake_ee_set_low",  
                                                                  "ll_fake_ee_set_high", 
                                                                  "ll_fake_ee_det_low",  
                                                                  "ll_fake_ee_det_high", 
                                                                  "ll_fake_mm_smt_low",  
                                                                  "ll_fake_mm_smt_high", 
                                                                  "ll_fake_mm_dmt_low",  
                                                                  "ll_fake_mm_dmt_high", 
                                                                  "ll_fake_em_set_low",  
                                                                  "ll_fake_em_set_high", 
                                                                  "ll_fake_em_smt_low",  
                                                                  "ll_fake_em_smt_high", 
                                                                  "ll_fake_em_emt_low",  
                                                                  "ll_fake_em_emt_high", 
                                                                  "ll_fake_me_set_low",  
                                                                  "ll_fake_me_set_high", 
                                                                  "ll_fake_me_smt_low",  
                                                                  "ll_fake_me_smt_high", 
                                                                  "ll_fake_me_emt_low",  
                                                                  "ll_fake_me_emt_high"]):
  newTree = dict()
  for key, vals in tree.iteritems():
    newTree[key] = []
    fakePlots = []
    nominalPlot = None
    for val in vals:
      if type(val) == dict:
        newTree[key] += [ combineFakeStat( val, newName, fakeUncert) ]
      elif val.GetName() not in fakeUncert:
        newTree[key] += [ val ]
        if val.GetName() == 'nominal':
          nominalPlot = val
      else:
        fakePlots += [ val ]
    
    # create envelope plots, if needed
    if len(fakePlots)!=0 and nominalPlot:
      newTree[key] += envelopePlots(nominalPlot, fakePlots, newName)
   
  return newTree

#------------------------
## saves the tree back into the file
def save(tree, file, folderName = ""):
  for key, vals in tree.iteritems():
    if folderName!="" and key!="":
      folderName += "/" + key
    elif folderName=="" and key!="":
      folderName = key
    
    if folderName != "":
      file.mkdir(folderName)
      file.cd(folderName)
    
    # save the plots
    for val in vals:
      if type(val) == dict:
        save(val, file, folderName)
      else:
        file.cd(folderName)
        val.Write()
        
  


parser = argparse.ArgumentParser()
#parser.add_argument('-rebin', default=2)
parser.add_argument('ifile1')
parser.add_argument('ifile2')
#parser.add_argument('ofile')
args=parser.parse_args()

f = ROOT.TFile.Open(args.ifile1)
if f and not f.IsZombie():
    # load WS file
    tree = readFolderTree(f)
    
    # apply post-processing
    tree = combineFakeStat(tree)

    #print tree, tree.GetName()
    
    # create the output file of the same name.
    file = ROOT.TFile.Open(args.ifile2,'recreate')
    file.cd()
    
    # save the converted WS inputs
    save(tree, file)
    
#close
file.Close()
