import ROOT



ROOT.gROOT.Macro('$ROOTCOREDIR/scripts/load_packages.C')
ROOT.SIG.DummyStruct()
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument('rfile')
args = parser.parse_args()

rfile = args.rfile
hist = ROOT.SIG.runSig(
    rfile, 
    "combined", 
    "ModelConfig", 
    "obsData", 
    "asimovData_1", 
    "conditionalGlobs_1", 
    "nominalGlobs", 
    "125", 
    "test", 0)

for ibin in range(0, hist.GetNbinsX() + 1):
    print hist.GetXaxis().GetBinLabel(ibin), ': ', hist.GetBinContent(ibin)

