#!/usr/bin/env python2

def main():
  hist = TH1F("hist","hist",100,-2,2);
  hist.SetDirectory(0);
  hist.FillRandom("gaus");
  hist2=TQHistogramUtils.copyHistogram(hist);
  newbins = vector("int")()
  for b in [10,50,90]:
    newbins.push_back(b)
   
  edges = TQHistogramUtils.getBinLowEdges(hist,newbins);
  hist3 = TQHistogramUtils.getRebinned(hist,newbins,False);
  hist4 = TQHistogramUtils.getRebinned(hist,edges,False);

  TQHistogramUtils.rebin(hist,newbins);
  TQHistogramUtils.rebin(hist2,edges);
  
  ok1 = TQHistogramUtils.areEqual(hist,hist2);
  ok2 = TQHistogramUtils.areEqual(hist3,hist4);
  ok3 = TQHistogramUtils.areEqual(hist,hist3);
  ok = ok1 and ok2 and ok3;

if __name__ == "__main__":
  # parse the CLI arguments
  from QFramework import *
  from ROOT import *
  TQLibrary.getQLibrary().setApplicationName("testRebinning")
  main()

