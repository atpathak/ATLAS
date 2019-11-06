#!/usr/bin/env python2

import sys
import os


def splitPath(filepath):

    dirname, filename = os.path.split(filepath)
    name = os.path.splitext(filename)[0]
    return dirname, name


def makeCorrMatrix(filepath):

    corrMatrix = ["CorrelationMatrixS","CorrelationMatrixB"]
    dirname, name = splitPath(filepath)
    file = TFile(filepath)
    for matrix in corrMatrix:
        canvas = TCanvas(matrix,"")
        histo = file.Get(matrix)
        histo.Draw("col text")
        canvas.SaveAs(dirname+"/"+name+"_"+matrix+".png")
    file.Close()


def makeInputVarPlots(filepath):

    dirname, name = splitPath(filepath)
    file = TFile(filepath)
    gStyle.SetOptTitle(0)
    
    tree = file.Get("TrainTree")
    branches = [b.GetName() for b in tree.GetListOfBranches()]
    inputVar = [branches[i] for i in range(2,len(branches)-2)]

    for var in inputVar:
        canvas = TCanvas(var,"")
        signal = file.Get("Method_BDT/"+name+"/"+var+"__Signal")
        background = file.Get("Method_BDT/"+name+"/"+var+"__Background")

        signal.SetLineColor(2)
        signal.SetMarkerColor(2)
        signal.SetFillColor(2)
        background.SetLineColor(4)
        background.SetMarkerColor(4)
        background.SetFillColor(4)
        signal.SetFillStyle(3001)
        background.SetFillStyle(3001)

        signal.Scale(1./signal.Integral())
        background.Scale(1./background.Integral())
        ymax = max([h.GetMaximum() for h in [signal,background] ]) * 1.2
        signal.SetMaximum(ymax)
        signal.GetYaxis().SetTitle("Normalized")
        signal.Draw("hist")
        background.Draw("hist same")
        signal.SetTitle("Signal")
        background.SetTitle("Background")
        canvas.cd(1).BuildLegend(0.7, 0.72, 0.87, 0.88).SetFillColor(0)
        canvas.SaveAs(dirname+"/"+name+"_"+var+".png")
        canvas.Close()
    file.Close()


def makeOverTrainCheckPlots(filepath):

    dirname, name = splitPath(filepath)
    file = TFile(filepath)

    trainSignal     = TH1D('trainSignal','Signal (Train)',40,-1.0,1.0) 
    trainBackground = TH1D('trainBackground','Background (Train)',40,-1.0,1.0) 
    testSignal      = TH1D('testSignal','Signal (Test)',40,-1.0,1.0) 
    testBackground  = TH1D('testBackground','Background (Test)',40,-1.0,1.0) 

    trainSignal.Sumw2()
    trainBackground.Sumw2()
    testSignal.Sumw2()
    testBackground.Sumw2()

    trainTree = file.Get("TrainTree") 
    testTree = file.Get("TestTree") 

    signalCut = 'classID==0'
    backgroundCut = 'classID>0'

    trainTree.Project("trainSignal",name,signalCut)
    trainTree.Project("trainBackground",name,backgroundCut)
    testTree.Project("testSignal",name,signalCut)
    testTree.Project("testBackground",name,backgroundCut)

    trainSignal.SetLineColor(2)
    trainSignal.SetFillColor(2)
    testSignal.SetLineColor(2)
    testSignal.SetMarkerColor(2)
    testSignal.SetFillColor(2)
 
    trainBackground.SetLineColor(4)
    trainBackground.SetFillColor(4)
    testBackground.SetLineColor(4)
    testBackground.SetMarkerColor(4)
    testBackground.SetFillColor(4)
 
    trainSignal.SetFillStyle(3001)
    trainBackground.SetFillStyle(3001)
    testSignal.SetFillStyle(0)
    testBackground.SetFillStyle(0)

    testSignal.SetMarkerStyle(20)
    testBackground.SetMarkerStyle(20)
 
    trainSignal.GetXaxis().SetTitle("BDT response")
    trainSignal.GetYaxis().SetTitle("Normalized")
 
    canvas = TCanvas("","")
    gStyle.SetOptTitle(0)

    trainSignal.Scale(1./trainSignal.Integral())
    trainBackground.Scale(1./trainBackground.Integral())
    testSignal.Scale(1./testSignal.Integral())
    testBackground.Scale(1./testBackground.Integral())

    trainSignal.Draw("hist")
    trainBackground.Draw("hist same")
    testSignal.Draw("ep same")
    testBackground.Draw("ep same")

    ymax = max([h.GetMaximum() for h in [trainSignal,trainBackground,testSignal,testBackground] ])
    ymax *=1.2
    trainSignal.SetMaximum(ymax)
 
    canvas.cd(1).BuildLegend(0.42, 0.72, 0.57, 0.88).SetFillColor(0)

    text=TLatex()
    text.SetNDC();
    kS = trainSignal.KolmogorovTest(testSignal)
    kB = trainBackground.KolmogorovTest(testBackground)
    resultString = "KS-test for signal(background): %.2f (%.2f)" %(kS,kB)
    text.DrawLatex(0.15,0.93,resultString)

    canvas.SaveAs(dirname+"/"+name+"_overtraining.png")
    canvas.Close()
    file.Close()


def plotROC(histoArray,dirname):

    gStyle.SetOptTitle(0)
    canvas = TCanvas("","")
    for i,histo in enumerate(histoArray):
        histo.SetLineColor(i+1)
        histo.SetFillColor(0)
        histo.SetFillStyle(0)
        if i == 0:
            histo.GetXaxis().SetTitle("Signal efficiency")
            histo.GetYaxis().SetTitle("Background rejection")
            histo.Draw("hist")
        else:
            histo.Draw("same hist")
    canvas.cd(1).BuildLegend(0.22, 0.22, 0.57, 0.47).SetFillColor(0)
    canvas.SaveAs(dirname+"/ROC.png")
    canvas.Close()


def makeROC(filepathArray):

    histoArray = []
    for filepath in filepathArray:
        file = TFile(filepath)
        dirname, name = splitPath(filepath)
        histo = file.Get("Method_BDT/"+name+"/MVA_"+name+"_trainingRejBvsS")
        histo.SetDirectory(0)
        histo.SetTitle(name[:-2])
        histoArray.append(histo)
        file.Close()
    plotROC(histoArray,dirname)


def main(args):
    print "Creating plots from BDT output root file"

    for path in args:
        makeCorrMatrix(path)
        makeInputVarPlots(path)
        makeOverTrainCheckPlots(path)
    makeROC(args)


if __name__ == "__main__":

    args = sys.argv[1].split(',')

    from ROOT import *

    gROOT.SetBatch(True)
    gStyle.SetOptStat(0)

    main(args)
