from ROOT import TH1F
from array import array
from math import sqrt
#------------------------
# imports the WS intut content into the dict structure
def readFolderTree(dir):
  tree = dict()
  keys = dir.GetListOfKeys()
  for key in keys:
    if 'TDirectory' in `type(dir.Get(key.GetName()))`:
      tree[key.GetName()] = readFolderTree(dir.Get(key.GetName()))
    elif 'TH' in `type(dir.Get(key.GetName()))`:
      h = dir.Get(key.GetName())
      h.SetDirectory(0)
      tree[key.GetName()] = h
    else:
      raise TypeError('Unknown type %s' % `type(dir.Get(key.GetName()))`)
  
  return tree

      
#------------------------
# lists the WS intut content
def ls(tree, prefix = ""):
  sorted = [x for x in tree.iteritems()]
  sorted.sort()
  for key, val in sorted:
    if type(val)==dict:
      print
      print prefix,key
      ls(val, prefix+'  ')
    else:
      print prefix,key,"[",type(val),"]"

#------------------------
## saves the tree back into the file
def save(tree, file, folderName = ""):
  if folderName != "":
    file.mkdir(folderName)
    file.cd(folderName)
  
  sorted = [x for x in tree.iteritems()]
  sorted.sort()
  for key, val in sorted:
    if type(val)==dict:
      save(val, file, folderName+key+"/")
    else:
      file.cd(folderName)
      val.Write()
          
#------------------------
## Rebin with variable binning
def rebin(histogram, bin_edges, name):
  newbins = len(bin_edges)-1
  
  histogram_new = TH1F(name, histogram.GetTitle(), newbins, array('d',bin_edges));
  histogram_new.GetXaxis().SetTitle(histogram.GetXaxis().GetTitle())
  histogram_new.GetYaxis().SetTitle(histogram.GetYaxis().GetTitle())

  oldbin = 1
  for newbin in range(1, newbins+1):
    bincont = 0
    binerrsqr = 0
    i = 0
    while histogram.GetBinLowEdge(oldbin+i) < bin_edges[newbin]:
      bincont += histogram.GetBinContent(oldbin+i)
      binerrsqr += (histogram.GetBinError(oldbin+i)) ** 2
      #print binerrsqr
      i += 1
    histogram_new.SetBinContent(newbin, bincont)
    histogram_new.SetBinError(newbin, sqrt(binerrsqr))
    oldbin += i
  
  histogram_new.SetEntries(histogram.GetEntries())
  
  return histogram_new
#------------------------
