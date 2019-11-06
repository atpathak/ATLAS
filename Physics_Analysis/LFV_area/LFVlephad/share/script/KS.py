#!/usr/bin/env python

from QFramework import *
from ROOT import *
import argparse
import sys
from time import sleep
from copy import deepcopy
import math

train_color = [38, 46]
test_color = [kBlue+3, kRed+3]
n = len(train_color)

E = 1.7 
all_KS = []

def main(args):

  for cat in args.category:
    for trained in args.trained:
      new = deepcopy(args)
      new.trained = trained
      new.category = cat

      single(new)


  mean = sum(all_KS) / len(all_KS)
  print "Average proability:", mean, "+-", math.sqrt(sum([(mean - x)**2 for x in all_KS]) / (len(all_KS) - 1))

def single(args):
  fine  = "Fine" if args.fine else ""
  file = TFile.Open(args.data)
  samplefolder = file.Get("samples")

  gStyle.SetOptStat(0);
  c = TCanvas()
  if args.log:
    c.SetLogy()

  legend = TLegend(0.25, 0.60, 0.75, 0.9)

  paths = []
  scales = []
  if args.background or not args.signal:
    paths.append("bkg")
    scales.append(args.background_scale)

  if args.signal or not args.background:
    paths.append("sig")
    scales.append(args.signal_scale)

  limits = []
  for i, (path, scale) in enumerate(zip(paths, scales)):
    training_hist = "Cut%s%s/BDT%s%s%s" % (args.category, args.trained, args.trained.upper(), args.category, fine)
    testing_on = "Even" if args.trained == "Odd" else "Odd" 
    test_hist = "Cut%s%s/BDT%s%s%s" % (args.category, testing_on, args.trained.upper(), args.category, fine)

    training = samplefolder.getHistogram(path, training_hist)
    test = samplefolder.getHistogram(path, test_hist)

    if not training: print("Training histogram '%s' not found." % training_hist)
    if not test: print("Test histogram '%s'not found." % test_hist)

    if not test or not training:
      sys.exit(1)

    training.SetTitle("")
    training.Scale(scale)
    training.SetFillColorAlpha(train_color[i %n], 0.090)
    training.SetFillStyle(3003)
    training.SetLineColor(test_color[i % n])
    training.Draw("HIST same")
    limits.append(training.GetMaximum())
    training.SetMaximum(max(limits) * E)

    test.Scale(scale)
    test.SetLineColor(kBlack)
    test.SetMarkerStyle(20)
    test.SetMarkerSize(1.3)
    test.SetMarkerColor(test_color[i % n])
    test.Draw("same")
    limits.append(test.GetMaximum())
    test.SetMaximum(max(limits) * E)

    prob = training.KolmogorovTest(test)
    print "Kolmogorov probability = %f" % prob
    all_KS.append(prob)

    scaleIndicator = " x %d" % scale if scale != 1 else ""

    legend.AddEntry(training, "%s%s training set (%s)" % (path, scaleIndicator, args.trained), "pf")
    legend.AddEntry(test, "%s%s test set (%s)" % (path, scaleIndicator, testing_on))
    legend.AddEntry("", "%s KS prob = %.4f" % (path, prob), "")

  legend.Draw()
  if "/" not in args.data:
    output_file = "./"
  else:
    output_file = args.data.rsplit("/", 1)[0]


  output_file += "/ks-%s_%s_%s_%s_%s.eps"  % (fine, "log" if args.log else "lin", args.category, args.trained, "".join(paths))
  gPad.Print(output_file)

if __name__ == "__main__":
  parser = argparse.ArgumentParser(
    description="Calculate KS from the readAnalysis output. On the training "
    "histogram the ROOT's KolmogorovTest method is called.",
    formatter_class=argparse.ArgumentDefaultsHelpFormatter)

  parser.add_argument("data", metavar="DATA", 
    help="Root file containing the output of readAnalsys.")

  parser.add_argument("-c", "--category", metavar="CAT", choices=("VBF", "Boosted"),
  nargs="+", default=["VBF", "Boosted"],
    help="Category, i.e Vbf or Boosted")

  parser.add_argument("-t", "--trained", metavar="TRAINED_ON", choices=("Even", "Odd"),
  nargs="+", default=["Even", "Odd"],
    help="The set on which the BDT was trained, i.e. Even or Odd")

  parser.add_argument("-l", "--log", action="store_true", default=False,
    help="use log y axis")
  
  # parser.add_argument("training", metavar="TRAINING",
  #   help="Cut and histogram of the training set.")

  # parser.add_argument("test", metavar="TEST",
  #   help="Cut and histogram of the test set.")

  parser.add_argument("-x", "--signal-scale", help="scale signal",
    default=100)

  parser.add_argument("-X", "--background-scale", help="scale background",
    default=1)

  parser.add_argument("-s", "--signal", help="plot signal only",
    action="store_true", default=False)

  parser.add_argument("-b", "--background", help="plot background only",
    action="store_true", default=False)

  parser.add_argument("-f", "--fine", help="plot fine histogram",
    action="store_true", default=False)

  args = parser.parse_args()
  
  main(args)
