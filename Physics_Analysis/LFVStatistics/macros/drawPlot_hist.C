#include "TH1D.h"
#include "TFile.h"
#include "TLegend.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TLatex.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <map>
#include <math.h>

using namespace std;

void drawPlot_hist(string histName, string version, int mass = 125, string folder = "Nominal/Normal", bool alt = 0, int rebin = 1)
{
  TCanvas* c1 = new TCanvas("c1","c1",1024,768);

  bool do2011    = 0;
  bool splitww   = 1;
  bool dozjetsew = 1;
  bool dovbfmode = 0;

  stringstream massStr;
  massStr << mass;
  string smass = massStr.str();

  stringstream altStr;
  if (alt && 0)
  {
    altStr << "_alt";
  }
  string salt = altStr.str();

  double xmin_leg = 0.20;
  double xdiff_leg = 0.22;
  double ymax_leg = 0.95;
  double ydiff_leg = 0.4;

  TLegend* leg = new TLegend(xmin_leg, ymax_leg-ydiff_leg, xmin_leg+xdiff_leg, ymax_leg, "", "NDC");
  leg->SetFillStyle(0);
  leg->SetTextSize(0.05);
  leg->SetBorderSize(0);
  leg->SetFillColor(0);

  vector<string> sampleNames;
  sampleNames.push_back("ttbar");
  sampleNames.push_back("st");
  if(!splitww) sampleNames.push_back("ww");
  if(splitww) sampleNames.push_back("ggww");
  if(splitww) sampleNames.push_back("qqww");
  if (!do2011) sampleNames.push_back("wwew");
  sampleNames.push_back("wzzz");
  if (!do2011) sampleNames.push_back("wzzzew");
  sampleNames.push_back("wg");
  sampleNames.push_back("wgs");
  // sampleNames.push_back("zjets");
  sampleNames.push_back("zleplep");
  sampleNames.push_back("ztautau");
  if(dozjetsew) {
    sampleNames.push_back("zleplepew");
    sampleNames.push_back("ztautauew");
  }
  sampleNames.push_back("wjets");
  sampleNames.push_back("data");
  sampleNames.push_back("ggf"+smass);
  // sampleNames.push_back("ggH"+smass+"spin0p");
  // sampleNames.push_back("ggH"+smass+"spin2p");
  sampleNames.push_back("vbf"+smass);
  if (mass <= 300)
  {
    sampleNames.push_back("wh"+smass);
    sampleNames.push_back("zh"+smass);
  }
  int nrSamples = sampleNames.size();

  vector<int> colors;
  colors.push_back(219);
  colors.push_back(218);
  colors.push_back(594);
  colors.push_back(222);
  colors.push_back(224);
  colors.push_back(210);
  colors.push_back(227);
  colors.push_back(228);
  colors.push_back(1);
  colors.push_back(2);
  colors.push_back(3);
  colors.push_back(4);
  colors.push_back(5);
  colors.push_back(6);
  colors.push_back(7);
  colors.push_back(8);
  colors.push_back(9);

  double tot = 0;
  double totS = 0;
  TH1D* dataHist = NULL;
  THStack* stack = new THStack("stack","stack");
  TH1D* errors = NULL;

  double* N = NULL;
  double* tot_s = NULL;
  double* tot_s_err = NULL;
  double* tot_b = NULL;
  double* tot_b_err = NULL;

  int nrBins2 = 0;
  int isam=0;
  for (int i=0;i<nrSamples;i++)
  {
    stringstream fileName;
    if ((sampleNames[i] == "ww" || sampleNames[i] == "ggww" || sampleNames[i] == "qqww") && alt)
    {
      fileName << "rev/"+version+"/normHists/"+"UEPS/PowhegShapeUp"+"/" << sampleNames[i] << ".root";
    }
    else
    {
      fileName << "rev/"+version+"/normHists/"+folder+"/" << sampleNames[i] << ".root";
    }
    TFile* file = new TFile(fileName.str().c_str());
    TH1D* hist = (TH1D*)file->Get(histName.c_str());
    if (rebin != 1) hist->Rebin(rebin);
    if (sampleNames[i] != "data")
    {
      stack->Add(hist);
      hist->SetLineColor(colors[i]);
      hist->SetFillColor(colors[i]);
    }
    else
    {
      dataHist = hist;
    }
    hist->SetTitle("");
    hist->GetXaxis()->SetTitle("Mapped m_{T} [arbitrary]");
    hist->GetYaxis()->SetTitle("Events");
    hist->SetStats(0);

    map<string, map<string, double> > z_sf;
    z_sf["ee"]["0j"] = 0.49;
    z_sf["mm"]["0j"] = 0.49;
    z_sf["em"]["0j"] = 0.66;
    z_sf["ee"]["1j"] = 0.52;
    z_sf["mm"]["1j"] = 0.55;
    z_sf["ee"]["2j"] = 0.47;
    z_sf["mm"]["2j"] = 0.73;
    double top0j_sf = 1.21;

    if (!errors)
    {
      cout << "making errors" << endl;
      errors = new TH1D("errors","errors",hist->GetNbinsX(),hist->GetXaxis()->GetXmin(),hist->GetXaxis()->GetXmax());
      cout << "done" << endl;
    }

    nrBins2 = hist->GetNbinsX();

    if (!N)
    {
      N = new double[nrBins2];
      tot_s = new double[nrBins2];
      tot_s_err = new double[nrBins2];
      tot_b = new double[nrBins2];
      tot_b_err = new double[nrBins2];
      for (int ibin=0;ibin<nrBins2;ibin++)
      {
        N[ibin] = 0;
        tot_s[ibin] = 0;
        tot_s_err[ibin] = 0;
        tot_b[ibin] = 0;
        tot_b_err[ibin] = 0;
      }
    }

    for (int ibin=0;ibin<nrBins2;ibin++)
    {
      if (sampleNames[i] == "data")
      {
        N[ibin] += hist->GetBinContent(ibin+1);
      }
      else if (sampleNames[i].find("vbf") != string::npos || sampleNames[i].find("ggf") != string::npos || sampleNames[i].find("wh") != string::npos || sampleNames[i].find("zh") != string::npos)
      {
        tot_s[ibin] += hist->GetBinContent(ibin+1);
        tot_s_err[ibin] += pow(hist->GetBinError(ibin+1), 2.);
      }
      else
      {
        tot_b[ibin] += hist->GetBinContent(ibin+1);
        tot_b_err[ibin] += pow(hist->GetBinError(ibin+1), 2.);
      }
    }

    string opt = "f";
    if (sampleNames[i] == "data") opt = "p";
    leg->AddEntry(hist, sampleNames[i].c_str(), opt.c_str());

    double err,integral;
    integral = hist->IntegralAndError(0,1000000000,err);
    cout << "Expected " << sampleNames[i] << ": " << integral << " +- " << err << endl;

    if ((!dovbfmode && sampleNames[i].find("ggf") != string::npos) || sampleNames[i].find("vbf") != string::npos || sampleNames[i].find("wh") != string::npos || sampleNames[i].find("zh") != string::npos)
    {
      totS += integral;
    }
    else if (sampleNames[i] != "data")
    {
      tot += integral;
    }

  }
  cout << "Total background: " << tot << endl;
  cout << "Total signal:     " << totS << endl;

  double maxVal = stack->GetMaximum();

  if (dataHist)
  {
    cout << "Has data" << endl;
    maxVal = max(dataHist->GetMaximum(), stack->GetMaximum());
    dataHist->SetMaximum(maxVal*2.5);
    dataHist->SetMinimum(0);
    dataHist->Draw("e");
    stack->Draw("histsame");
    dataHist->Draw("esame");
  }
  else
  {
    stack->SetMaximum(2*maxVal);
    stack->Draw("hist");
  }

  leg->Draw();

  TLatex t;
  t.SetNDC();
  t.DrawLatex(0.6, 0.8, histName.c_str());

  for (int ibin=0;ibin<nrBins2;ibin++)
  {
    tot_s_err[ibin] = sqrt(tot_s_err[ibin]);
    tot_b_err[ibin] = sqrt(tot_b_err[ibin]);
  }

  cout << "Bin by bin expectations" << endl;
  for (int ibin=0;ibin<nrBins2;ibin++)
  {
    cout << ibin << ": S=" << tot_s[ibin] << " +- " << tot_s_err[ibin] << ", B = " << tot_b[ibin] << " +- " << tot_b_err[ibin] << ", Nobs = " << N[ibin] << " +- " << sqrt(N[ibin]) << endl;
  }

  stringstream fileName;
  fileName << "hists/" << version;
  system(("mkdir -vp " + fileName.str()).c_str());
  fileName << "/" << histName;
  c1->SaveAs((fileName.str()+".eps").c_str());
  c1->SaveAs((fileName.str()+".pdf").c_str());
}
