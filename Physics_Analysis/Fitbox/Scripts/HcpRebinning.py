import sys 
from ROOT import TFile, TH1, gROOT

# Yet another rebinning script
#
# python Scripts/HcpRebinning.py <plots file> <binning file from WsBuilder> <output name>

if __name__ == '__main__':
    
    if len(sys.argv) < 4:
        print 'Too few arguments:', sys.argv
        exit(-1)
    
    fplots = TFile(sys.argv[1])             # Plots file
    fbinning = TFile(sys.argv[2])           # Binning file
    fout = TFile(sys.argv[3], 'recreate')   # Output file
    
    for key in fplots.GetListOfKeys():
        
        name = key.GetName()
        
        # Pick only the category name
        name = 'bins_' + name[name.find('hh'):]
        
        if not gROOT.GetClass(key.GetClassName()).InheritsFrom("TH1"):
            print 'Skipping', key.GetClassName(), ':', name
            continue
        
        print 'Processing', name
        
        # Get hist from plots file
        hplot = key.ReadObj()
        
        # Get hist from binning file
        hbinning = fbinning.Get('binning/%s' % name)
        nbins = hbinning.GetNbinsX()
        
        # Adjust bins 
        bins = [0] * (nbins+1)
        bins = array('d', bins)
        hbinning.GetXaxis().GetLowEdge(bins)
        bins[nbins] = hbinning.GetBinLowEdge(nbins) + hbinning.GetBinWidth(nbins)
        
        hplot.SetBins(nbins, bins)
        #hplot.GetXaxis().SetRange(bins[0], bins[nbins])
        hplot.GetXaxis().SetRange()
        
        # Normalise by bin width
        for iBin in range(1, hplot.GetNbinsX() + 1 ):
            binWidth = hplot.GetBinWidth(iBin)
            if binWidth > 0.:
                hplot.SetBinContent(iBin, hplot.GetBinContent(iBin) / binWidth)
                hplot.SetBinError(iBin, hplot.GetBinError(iBin) / binWidth)
        
        fout.cd()
        hplot.Write()
    
    fplots.Close()
    fbinning.Close()
    fout.Close()
