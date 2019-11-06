#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>
#include <math.h>

#include "TFile.h"
#include "TH1D.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TPad.h"

using namespace std;

struct Sys
{

  Sys()
  {
    folder="";
    fileUp="";
    fileDown="";
  }

  Sys(string _folder, string _fileUp, string _fileDown)
  {
    folder = _folder;
    fileUp = _fileUp;
    fileDown = _fileDown;
  }

  string folder;
  string fileUp;
  string fileDown;
};

bool plotAll=0;
void plotVariation(int mass, string version, bool signalOnly = false);
void plotVariation(string sampleName, string version, int mass, string sysName,string regionName, bool normalize = true);
void plotVariation(string version)
{
  const int nrPoints = 24;
  int massPoints[nrPoints] = {110,115,120,125,130,135,140,145,150,155,160,165,170,175,180,185,190,195,200,220,240,260,280,300};
  for (int i=0;i<nrPoints;i++)
  {
    bool signalOnly = false;
    if (i != 0) signalOnly = true;
    plotVariation(massPoints[i], version, signalOnly);
  }
}

void plotVariation(int mass, string version, bool signalOnly)
{
  plotAll=1;

  stringstream massStr;
  massStr << mass;
  string smass = massStr.str();

  vector<string> sampleNames;
  sampleNames.push_back("ggf"+smass);
  if (!signalOnly)
  {
    sampleNames.push_back("vbf"+smass);
    sampleNames.push_back("ttbar");
    sampleNames.push_back("st");
    //sampleNames.push_back("wjets");
    sampleNames.push_back("zjets");
    sampleNames.push_back("ww");
    sampleNames.push_back("wzzz");
  }

  vector<string> sysNames;
  sysNames.push_back("Nom");
  sysNames.push_back("B_EFF");
  sysNames.push_back("C_EFF");
  sysNames.push_back("L_EFF");
  sysNames.push_back("E_EFF");
  sysNames.push_back("E_RES");
  sysNames.push_back("E_SCALE");
  sysNames.push_back("M_SCALE");
  sysNames.push_back("MS_RES");
  sysNames.push_back("ID_RES");
  sysNames.push_back("JER");
  sysNames.push_back("JES");
  sysNames.push_back("CLUSTER");
  //sysNames.push_back("SOFTJETS");
  sysNames.push_back("PILEUP");
  sysNames.push_back("Fake_Rate");
  int nrSys = sysNames.size();

  vector<string> regionNames;
  regionNames.push_back("signalLike");
  regionNames.push_back("mainControl");
  regionNames.push_back("topbox");
  int nrRegions = regionNames.size();

  int nrSamples = sampleNames.size();
  for (int i=0;i<nrSamples;i++)
  {
    for (int isys=0;isys<nrSys;isys++)
    {
      for (int ireg=0;ireg<nrRegions;ireg++)
      {
	plotVariation(sampleNames[i], version, mass, sysNames[isys], regionNames[ireg], 1);
      }
    }
  }
}


void plotVariation(string sampleName, string version, int mass, string sysName,string regionName, bool normalize)
{
  stringstream massStr;
  massStr << mass;
  string smass = massStr.str();

  bool doNorm = true;
  string folder = "hists";
  if (doNorm) folder = "normHists";

  map<string, Sys> fileNames; // pair is <up, down>
  fileNames["Nom"] = Sys("Nominal","Normal","Normal");
  fileNames["B_EFF"] = Sys("ATLAS_B_EFF","BtagUp","BtagDown");
  fileNames["C_EFF"] = Sys("ATLAS_C_EFF","CtagUp","CtagDown");
  fileNames["L_EFF"] = Sys("ATLAS_L_EFF","MtagUp","MtagDown");
  fileNames["E_EFF"] = Sys("ATLAS_E_EFF","ElecEffUp","ElecEffDown");
  fileNames["M_EFF"] = Sys("ATLAS_M_EFF","MuonEffUp","MuonEffDown");
  fileNames["E_RES"] = Sys("ATLAS_E_RES","ElecResolutionUp","ElecResolutionDown");
  fileNames["E_SCALE"] = Sys("ATLAS_E_SCALE","ElecScaleUp","ElecScaleDown");
  fileNames["M_SCALE"] = Sys("ATLAS_M_SCALE","MuonScale","MuonScale");
  fileNames["ID_RES"] = Sys("ATLAS_ID_RES","IDUP","IDLOW");
  fileNames["MS_RES"] = Sys("ATLAS_MS_RES","MSUP","MSLOW");
  fileNames["JER"] = Sys("ATLAS_JER","JERUp","JERUp");
  fileNames["JES"] = Sys("ATLAS_JES","JESUp","JESDown");
  //fileNames["SOFTJETS"] = Sys("ATLAS_SOFTJETS","SoftJetsUp","SoftJetsDown");
  fileNames["PILEUP"] = Sys("ATLAS_PILEUP","PileUpUp","PileUpDown");
//   fileNames["CELLOUT"] = Sys("ATLAS_CELLOUT","CellOutEflowUp","CellOutEflowDown");
  fileNames["CLUSTER"] = Sys("ATLAS_CLUSTER","AllClustersUp","AllClustersDown");
  fileNames["MU_RESCALE"] = Sys("ATLAS_MU_RESCALE","MuRescaleUp","MuRescaleDown");
  fileNames["Fake_Rate"] = Sys("Fake_Rate","SysFakeUp","SysFakeDown");
  fileNames["TRIGGER"] = Sys("ATLAS_TRIGGER","TriggerUp","TriggerDown");
  fileNames["ISO"] = Sys("ATLAS_ISO","IsoUp","IsoDown");

  vector<string> channelNames;
  channelNames.push_back("ee");
  channelNames.push_back("em");
  channelNames.push_back("mm");
  int nrChannels = channelNames.size();

  vector<string> jetNames;
  jetNames.push_back("0j");
  jetNames.push_back("1j");
  int nrJets = jetNames.size();

  TCanvas* c1 = new TCanvas("c1","c1",1024,768);
  c1->Divide(nrChannels, nrJets);


  stringstream nomFileName;
  nomFileName << "rev/" << version << "/"+folder+"/" << mass << "/Nominal/Normal/" << sampleName << ".root";
  TFile* nomFile = new TFile(nomFileName.str().c_str());

  stringstream hiFileName;
  hiFileName << "rev/" << version << "/"+folder+"/" << mass << "/"+fileNames[sysName].folder+"/"+fileNames[sysName].fileUp+"/" << sampleName << ".root";
  TFile* hiFile = new TFile(hiFileName.str().c_str());

  stringstream loFileName;
  loFileName << "rev/" << version << "/"+folder+"/" << mass << "/"+fileNames[sysName].folder+"/"+fileNames[sysName].fileDown+"/" << sampleName << ".root";
  TFile* loFile = new TFile(loFileName.str().c_str());

  TH1D* hiTemplate,*loTemplate,*nomTemplate;

  TLine l;
  TLatex t;
  t.SetNDC();
  t.SetTextSize(0.05);
  for (int ij=0;ij<nrJets;ij++)
  {
    for (int ic=0;ic<nrChannels;ic++)
    {
      int padNr = ij * nrChannels + ic + 1;
      TPad* pad = (TPad*)c1->cd(padNr);

      stringstream histName;
      histName << channelNames[ic] << "_" << regionName << "_" << jetNames[ij];
      TH1D* nomHist = (TH1D*)nomFile->Get(histName.str().c_str());
      TH1D* hiHist = (TH1D*)hiFile->Get(histName.str().c_str());
      TH1D* loHist = (TH1D*)loFile->Get(histName.str().c_str());

      if (!nomHist || !hiHist || !loHist)
      {
	cout << "ERROR::Hist doesn't exist: " << histName.str() << endl;
	continue;
      }

      hiTemplate=hiHist;
      loTemplate=loHist;
      nomTemplate=nomHist;

      int nrBins = nomHist->GetNbinsX();
      double xmin = nomHist->GetXaxis()->GetXmin();
      double xmax = nomHist->GetXaxis()->GetXmax();

      double maxVal = -10e9;
      double minVal = 10e9;

      double nomIntegral = nomHist->Integral();
      double hiIntegral = hiHist->Integral();
      double loIntegral = loHist->Integral();

      hiHist->Scale(nomIntegral/hiIntegral);
      loHist->Scale(nomIntegral/loIntegral);

      if (normalize)
      {
	for (int ib=0;ib<nrBins;ib++)
	{
	  double nomContent = nomHist->GetBinContent(ib+1);
	  double hiContent = hiHist->GetBinContent(ib+1);
	  double loContent = loHist->GetBinContent(ib+1);

	  cout << setprecision(3);
	  double varHi = (hiContent-nomContent)/nomContent;
	  double varLo = (loContent-nomContent)/nomContent;
	  if (varHi <= -0.999 || varLo <= -0.999)
	  {
	    cout << "WARNING::Variation [up, down] = [" << varHi << ", " << varLo << "] for bin " << ib << " of " << sysName << " in hist " << histName.str() << " for sample " << sampleName << endl;
	  }

	  if (fabs(varHi) < 10 && fabs(varHi) > pow(10., -9)) 
	  {
	    hiHist->SetBinContent(ib+1, varHi);
	  }
	  else
	  {
	    hiHist->SetBinContent(ib+1, 0);
	  } 

	  if (fabs(varLo) < 10 && fabs(varLo) > pow(10., -9)) 
	  {
	    loHist->SetBinContent(ib+1, varLo);
	  }
	  else
	  {
	    loHist->SetBinContent(ib+1, 0);
	  } 
	}
	//hiHist->Scale(1./nomIntegral);
	//loHist->Scale(1./nomIntegral);
      }
      hiHist->GetYaxis()->SetTitle("Expected Events");
      if (normalize) 
      {
	hiHist->GetYaxis()->SetTitleOffset(1.6);
	hiHist->GetYaxis()->SetTitle("( Var - Nom ) / Nom");
      }
      hiHist->GetXaxis()->SetTitle("Mapped M_{T}");

      hiHist->SetStats(0);
      hiHist->SetTitle("");

      hiHist->SetLineColor(kBlue);
      hiHist->SetMarkerColor(kBlue);

      loHist->SetLineColor(kRed);
      loHist->SetMarkerColor(kRed);

      maxVal = max(maxVal, hiHist->GetMaximum());
      maxVal = max(maxVal, loHist->GetMaximum());

      minVal = min(minVal, hiHist->GetMinimum());
      minVal = min(minVal, loHist->GetMinimum());

      if (normalize)
      {
	hiHist->SetMinimum(minVal*1.1);
	hiHist->SetMaximum(maxVal*1.1);
	hiHist->Draw("");
      }
      else
      {
	maxVal = max(maxVal, nomHist->GetMaximum());
	hiHist->SetMinimum(0);
	hiHist->SetMaximum(maxVal*1.1);
	hiHist->Draw("");
	nomHist->Draw("same");
	//pad->SetLogy(1);
      }
      loHist->Draw("same");
      l.DrawLine(0, xmin, 0, xmax); 
      t.DrawLatex(0.3,0.96,histName.str().c_str());
    }
  }

  c1->cd(1);

  double xmin_leg = 0.56;
  double xdiff_leg = 0.22;
  double ymax_leg = 0.5;
  double ydiff_leg = 0.2;
  if (normalize) ydiff_leg = 0.15;

  TLegend* leg = new TLegend(xmin_leg, ymax_leg-ydiff_leg, xmin_leg+xdiff_leg, ymax_leg, "", "NDC");
  leg->SetFillStyle(0);
  leg->SetTextSize(0.05);
  leg->SetBorderSize(0);
  leg->SetFillColor(0);

  if (!normalize) leg->AddEntry(nomTemplate, "Nominal", "l");
  leg->AddEntry(hiTemplate, "+1#sigma", "l");
  leg->AddEntry(loTemplate, "-1#sigma", "l");
  leg->Draw();


  t.DrawLatex(xmin_leg, ymax_leg-0.24, (smass + ", " + sysName).c_str());
  t.DrawLatex(xmin_leg, ymax_leg-0.29, sampleName.c_str());


  stringstream saveName;
  saveName << "variations/" << version << "/" << mass;
  system(("mkdir -vp " + saveName.str()).c_str());
  saveName << "/" << sysName << "_" << sampleName << "_" << regionName << "_" << normalize << ".eps";
  c1->SaveAs(saveName.str().c_str());

  if (plotAll)
  {
    nomFile->Close();
    hiFile->Close();
    loFile->Close();
  }
}
