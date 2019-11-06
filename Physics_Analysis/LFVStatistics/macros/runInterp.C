// Author: Aaron Armbruster
// Date:   2011-11-16
//
// Description:
//
// Interpolate nominal and systematic histograms for signal points where we have no MC

#include "TFile.h"
#include "TH1D.h"

#include "config_2011/ggf.C"
#include "config_2011/vbf.C"
#include "config_2011/wh.C"
#include "config_2011/zh.C"
#include "config_2011/br.C"

#include "macros/runChain.C"
// #include "macros/makeNorms.C"
// #include "macros/writeXML.C"
// #include "macros/setup.C"
#include "macros/th1dmorph.C"

#include "RooMomentMorph.h"
#include "RooDataHist.h"
#include "RooHistPdf.h"
#include "RooRealVar.h"
#include "RooConstVar.h"
#include "RooGlobalFunc.h"
#include "RooPlot.h"

#include <set>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <algorithm>

using namespace std;
using namespace RooFit;

// morphing modes
// 0 = RooMomentMorph: Linear
// 1 = RooMomentMorph: NonLinear
// 2 = RooMomentMorph: NonLinearPosFractions
// 3 = RooMomentMorph: NonLinearLinFractions
// 4 = Alex Read algo
int interpMode=0;

bool runningInterpLooper = 0;
string dtos(double d);
TH1D* momentMorph(string name, TH1D* hist_lo, TH1D* hist_hi, double pointLo, double pointHi, double point);
void momentMorph2(string sampleName, string name, double pointLo, double pointHi, double mass, string version, string folderName, TFile* outFile);
TH1D* getHist(string sampleName, string sysName, string fileName, double mass, string version, string regionName, string salt, TFile*& file);
double interp(double x, double x1, double x2, double y1, double y2);

// dummy function for compiling
void runInterp()
{

}

void runInterp(double mass, string version)
{
  double scale = -1.0; //20.0;

  // these should probably go into a common file
  // overriden = 1;
  // massBoundary=200;
  // nrBins = 5;
  // if (!runningLooper)
  // {
  //   doee = 1;
  //   domm = 1;
  //   doem = 1;
  //   do0j = 1;
  //   do1j = 1;
  // }
  
  // useDetSys         = 1;
  // useTheoryWW       = 0 || (mass >= massBoundary);
  // useTheoryTop      = 0;
  // useShape          = 1 && useDetSys && mode != 0;
  // useThSys          = 1;
  // scaleZ            = 1;
  // scaleTop          = 1;
  // conditionalAsimov = 1;
  // useHighPt         = 1;
  // useLowPt          = 0;
  
  // setup(mass);
  
  // find endpoint masses
  int nrBasePoints = 43;
  // int nrBasePoints = 37;
  double thisMass = 90;
  double step = 5;
  double pointLo = 90;
  double pointHi = 600;

  if (version.find("2011") != string::npos)
  {
    nrBasePoints = 39;
    thisMass = 110;
    pointLo = 110;
    pointHi = 600;
  }

  double* baseMassPoints = new double[nrBasePoints];

  for (int i=0;i<nrBasePoints;i++)
  {
    // if (thisMass == 480)
    // {
    //   thisMass += 20;
    //   i--;
    //   continue;
    // }

    // if (thisMass == 170 && exclude170)
    // {
    //   thisMass += 5;
    //   i--;
    //   continue;
    // }

    baseMassPoints[i] = thisMass;
    // if (thisMass >= 200) step = 20;
    if (thisMass >= 200 && thisMass < 300) step = 20;

    thisMass += step;

    if (mass == baseMassPoints[i]) 
    {
      cout << "Mass point already known. Skipping interpolation." << endl;
      return;
    }

    if (baseMassPoints[i] < mass) pointLo = baseMassPoints[i];
    if (baseMassPoints[i] > mass && pointHi == 600) pointHi = baseMassPoints[i];
  }

  // alt if we had to apply low mass selections to an otherwise high mass point
  bool alt = /*pointHi == 170 || pointHi == 220*/ pointLo == massBoundary || pointLo == massBoundary2;

  if (!runningInterpLooper) runChain(mass, version, alt, 0, 1);

  stringstream massStr;
  massStr << mass;
  string smass = massStr.str();

  stringstream altStr;
  if (alt) altStr << "_alt";
  string salt = altStr.str();
  cout << "interp region = [" << pointLo << ", " << pointHi << "]" << endl;
  cout << "salt: " << salt << endl;

  stringstream massStr_lo;
  massStr_lo << pointLo;
  string smass_lo = massStr_lo.str();

  stringstream massStr_hi;
  massStr_hi << pointHi;
  string smass_hi = massStr_hi.str();

  // get xs and br for all three points
  // vh cross section is pretty flat, so just let the algo linearly interpolate.
  // otherwise have do deal with wh/zh mixing
  double br = get_br(mass);
  double br_lo = get_br(pointLo);
  double br_hi = get_br(pointHi);
  double xs_ggf = get_ggf(mass);
  double xs_vbf = get_vbf(mass);
  double xs_wh = get_wh(mass);
  double xs_zh = get_zh(mass);
  double xs_ggf_lo = get_ggf(pointLo);
  double xs_vbf_lo = get_vbf(pointLo);
  double xs_wh_lo = get_wh(pointLo);
  double xs_zh_lo = get_zh(pointLo);
  double xs_ggf_hi = get_ggf(pointHi);
  double xs_vbf_hi = get_vbf(pointHi);
  double xs_wh_hi = get_wh(pointHi);
  double xs_zh_hi = get_zh(pointHi);

  int nrRegions = regions->size();
  int nrSys = fileNames->size();

  cout << "nrSys = " << nrSys << endl;

  vector<string> keywords;
  keywords.push_back("ggf");
  keywords.push_back("vbf");
  keywords.push_back("wh");
  keywords.push_back("zh");

  // yet another giant nested loop
  for (int isys=0;isys<nrSys;isys++)
  {
    Sys* s = &(*fileNames)[isys];

    cout << "On sys: " << s->folder << endl;

    set<string> folderNames; // don't duplicate folders
    folderNames.insert(s->fileUp);
    folderNames.insert(s->fileDown);

    bool match = false;
    for (set<string>::iterator itr1=s->sampleNames.begin();itr1!=s->sampleNames.end();itr1++)
    {
      for (int ik=0;ik<(int)keywords.size();ik++)
      {
        if (itr1->find(keywords[ik]) != string::npos)
        {
          match = true;
          break;
        }
      }
      if (match) break;
    }
    if (!match) continue;

    for (set<string>::iterator file = folderNames.begin();file!=folderNames.end();file++)
    {
      stringstream baseFile;
      baseFile << "rev/" << version << "/hists/" << "/" << s->folder << "/" << *file;
      system(("mkdir -vp " + baseFile.str()).c_str());

      cout << "Opening TFiles" << endl;
      stringstream fileName_ggf;
      fileName_ggf << baseFile.str() << "/ggf" << mass << ".root";
      TFile* outFile_ggf = new TFile(fileName_ggf.str().c_str(), "recreate");

      stringstream fileName_vbf;
      fileName_vbf << baseFile.str() << "/vbf" << mass << ".root";
      TFile* outFile_vbf = new TFile(fileName_vbf.str().c_str(), "recreate");

      stringstream fileName_wh;
      fileName_wh << baseFile.str() << "/wh" << mass << ".root";
      TFile* outFile_wh = new TFile(fileName_wh.str().c_str(), "recreate");

      stringstream fileName_zh;
      fileName_zh << baseFile.str() << "/zh" << mass << ".root";
      TFile* outFile_zh = new TFile(fileName_zh.str().c_str(), "recreate");

      cout << "Looping over regions, file = " << *file << ", sys = " << s->folder << endl;
      for (int ireg=0;ireg<nrRegions;ireg++)
      {
        cout << "Starting region loop" << endl;
        Region* r = &(*regions)[ireg];

        cout << "On reg: " << r->name << endl;
        int nrChannels = r->channels.size();
        for (int ichan=0;ichan<nrChannels;ichan++)
        {
          cout << "Start chan loop" << endl;
          Channel* c = &r->channels[ichan];

          bool skipLoop = skipRegion(r, c->name, c->jetName);
          if (skipLoop) continue;

          cout << "On chan: " << c->name << endl;

          string regionName = c->name + "_" + r->name + "_" + c->jetName;
          TString name = regionName;

          if (interpMode == 0 || interpMode == 4)
          {
            cout << "Grabbing ggf files" << endl;
          
            // prepare histograms for interpolation
            TFile* file_ggf_lo;
            TH1D* hist_ggf_lo = getHist("ggf"+smass_lo, s->folder, *file, pointLo, version, regionName, salt, file_ggf_lo);

            if (scale > 0 && hist_ggf_lo->Integral() > 0) // useful for interp tests
            {
              hist_ggf_lo->Scale(scale/hist_ggf_lo->Integral());
            }
            else
            {
              hist_ggf_lo->Scale(1./(xs_ggf_lo*br_lo)); // first scale by 1 / xs*br (already interpolated) to retrieve effective efficiency
            }

            TFile* file_ggf_hi;
            TH1D* hist_ggf_hi = getHist("ggf"+smass_hi, s->folder, *file, pointHi, version, regionName, "", file_ggf_hi);

            if (scale > 0 && hist_ggf_hi->Integral() > 0)
            {
              hist_ggf_hi->Scale(scale/hist_ggf_hi->Integral());
            }
            else
            {
              hist_ggf_hi->Scale(1./(xs_ggf_hi*br_hi)); // same for hi variation
            }

            outFile_ggf->cd();
            TH1D* hist_ggf;
            if (interpMode != 4)
            {
              hist_ggf = momentMorph(regionName, hist_ggf_lo, hist_ggf_hi, pointLo, pointHi, mass);
            }
            else
            {
              hist_ggf = th1dmorph(regionName, regionName, hist_ggf_lo, hist_ggf_hi, pointLo, pointHi, mass); //interpolate
            }

            if (scale <= 0)
            {
              hist_ggf->Scale(xs_ggf*br); // scale by already interpolated xs*br
            }

            cout << "Grabbing vbf files" << endl;
            // repeat for vbf
            TFile* file_vbf_lo;
            TH1D* hist_vbf_lo = getHist("vbf"+smass_lo, s->folder, *file, pointLo, version, regionName, salt, file_vbf_lo);
            if (scale > 0 && hist_vbf_lo->Integral() > 0)
            {
              hist_vbf_lo->Scale(scale/hist_vbf_lo->Integral());
            }
            else
            {
              hist_vbf_lo->Scale(1./(xs_vbf_lo*br_lo));
            }

            TFile* file_vbf_hi;
            TH1D* hist_vbf_hi = getHist("vbf"+smass_hi, s->folder, *file, pointHi, version, regionName, "", file_vbf_hi);
            if (scale > 0 && hist_vbf_hi->Integral() > 0)
            {
              hist_vbf_hi->Scale(scale/hist_vbf_hi->Integral());
            }
            else
            {
              hist_vbf_hi->Scale(1./(xs_vbf_hi*br_hi));
            }
      
            outFile_vbf->cd();
            TH1D* hist_vbf;
            if (interpMode != 4)
            {
              hist_vbf = momentMorph(regionName, hist_vbf_lo, hist_vbf_hi, pointLo, pointHi, mass);
            }
            else
            {
              hist_vbf = th1dmorph(regionName, regionName, hist_vbf_lo, hist_vbf_hi, pointLo, pointHi, mass);
            }
            if (scale <= 0)
            {
              hist_vbf->Scale(xs_vbf*br);
            }
            cout << "Done vbf" << endl;

            cout << "Grabbing wh files" << endl;
            // repeat for wh
            TFile* file_wh_lo;
            TFile* file_wh_hi;
            if (dowh)
            {
              TH1D* hist_wh_lo = getHist("wh"+smass_lo, s->folder, *file, pointLo, version, regionName, salt, file_wh_lo);
              if (mass <= 300)
              {
                if (!hist_wh_lo)
                {
                  cout << "ERROR::WH lo hist doesn't exist" << endl;
                  exit(1);
                }
                if (scale > 0 && hist_wh_lo->Integral() > 0)
                {
                  hist_wh_lo->Scale(scale/hist_wh_lo->Integral());
                }
                else
                {
                  hist_wh_lo->Scale(1./(xs_wh_lo*br_lo));
                }
              }
              TH1D* hist_wh_hi = getHist("wh"+smass_hi, s->folder, *file, pointHi, version, regionName, "", file_wh_hi);
              if (mass <= 300)
              {
                if (!hist_wh_hi)
                {
                  cout << "ERROR::WH hi hist doesn't exist" << endl;
                  exit(1);
                }
                if (scale > 0 && hist_wh_hi->Integral() > 0)
                {
                  hist_wh_hi->Scale(scale/hist_wh_hi->Integral());
                }
                else
                {
                  hist_wh_hi->Scale(1./(xs_wh_hi*br_hi));
                }
      
                outFile_wh->cd();
                TH1D* hist_wh;
                if (interpMode != 4)
                {
                  hist_wh = momentMorph(regionName, hist_wh_lo, hist_wh_hi, pointLo, pointHi, mass);
                }
                else
                {
                  hist_wh = th1dmorph(regionName, regionName, hist_wh_lo, hist_wh_hi, pointLo, pointHi, mass);
                }
                if (scale <= 0)
                {
                  hist_wh->Scale(xs_wh*br);
                }
              }
              cout << "Done wh" << endl;
            }

            cout << "Grabbing zh files" << endl;
            // repeat for zh
            TFile* file_zh_lo;
            TH1D* hist_zh_lo = getHist("zh"+smass_lo, s->folder, *file, pointLo, version, regionName, salt, file_zh_lo);
            if (mass <= 300)
            {
              if (!hist_zh_lo)
              {
                cout << "ERROR::ZH lo hist doesn't exist" << endl;
                exit(1);
              }
              if (scale > 0 && hist_zh_lo->Integral() > 0)
              {
                hist_zh_lo->Scale(scale/hist_zh_lo->Integral());
              }
              else
              {
                hist_zh_lo->Scale(1./(xs_zh_lo*br_lo));
              }
            }
            TFile* file_zh_hi;
            TH1D* hist_zh_hi = getHist("zh"+smass_hi, s->folder, *file, pointHi, version, regionName, "", file_zh_hi);
            if (mass <= 300)
            {
            if (!hist_zh_hi)
            {
              cout << "ERROR::ZH hi hist doesn't exist" << endl;
              exit(1);
            }
            if (scale > 0 && hist_zh_hi->Integral() > 0)
            {
              hist_zh_hi->Scale(scale/hist_zh_hi->Integral());
            }
            else
            {
              hist_zh_hi->Scale(1./(xs_zh_hi*br_hi));
            }
      
            outFile_zh->cd();
            TH1D* hist_zh;
            if (interpMode != 4)
            {
              hist_zh = momentMorph(regionName, hist_zh_lo, hist_zh_hi, pointLo, pointHi, mass);
            }
            else
            {
              hist_zh = th1dmorph(regionName, regionName, hist_zh_lo, hist_zh_hi, pointLo, pointHi, mass);
            }
            if (scale <= 0)
            {
              hist_zh->Scale(xs_zh*br);
            }
            }
            cout << "Done zh" << endl;

            file_ggf_lo->Close();
            file_ggf_hi->Close();
            file_vbf_lo->Close();
            file_vbf_hi->Close();
            if (dowh)
            {
              file_wh_lo->Close();
              file_wh_hi->Close();
            }
            file_zh_lo->Close();
            file_zh_hi->Close();

            cout << "Files closed" << endl;
          }
          else
          {
            cout << "ERROR::Fix me" << endl;
            exit(1);
            
            momentMorph2("ggf", regionName, pointLo, pointHi, mass, version, s->folder+"/"+*file, outFile_ggf);
            momentMorph2("vbf", regionName, pointLo, pointHi, mass, version, s->folder+"/"+*file, outFile_vbf);
            momentMorph2("wh", regionName, pointLo, pointHi, mass, version, s->folder+"/"+*file, outFile_wh);
            momentMorph2("zh", regionName, pointLo, pointHi, mass, version, s->folder+"/"+*file, outFile_zh);
          }
        }
      }

      cout << "Writing" << endl;
      outFile_ggf->Write();
      outFile_ggf->Close();

      outFile_vbf->Write();
      outFile_vbf->Close();
      cout << "Written" << endl;
      if (dowh)
      {
        outFile_wh->Write();
        outFile_wh->Close();
        cout << "Written" << endl;
      }
      outFile_zh->Write();
      outFile_zh->Close();
      cout << "Written" << endl;
    }
  }

  // now run the normalization script, but only on the histograms hot off the interpolation skillet
  makeNorms(mass, version, false, true);

  // write the xml
  writeXML(mass, version);
}

// previously used for number counting
double interp(double x, double x1, double x2, double y1, double y2)
{
  double m = (y2-y1)/(x2-x1);
  double b = y2 - m*x2;
  return m*x+b;
}

// grab the histogram
TH1D* getHist(string sampleName, string sysName, string fileName, double mass, string version, string regionName, string salt, TFile*& file)
{
  cout<< "Getting hist for sample " << sampleName << ", sys " << sysName << ", in file " << fileName << ", for mass " << mass << ", version " << version << ", region " << regionName << ", alt ? " << salt << endl;
  stringstream inFileName;
  inFileName << "rev/" << version << "/hists/" << sysName << "/" << fileName << "/" << sampleName << ".root";
  file = new TFile(inFileName.str().c_str());
  TH1D* hist = (TH1D*)file->Get(regionName.c_str());
  return hist;
}

TH1D* momentMorph(string name, TH1D* hist_lo, TH1D* hist_hi, double pointLo, double pointHi, double point)
{
  bool justLinear=1;
  cout << "Morphing" << endl;

  // linearly interpolate the normalization
  double norm_lo = hist_lo->Integral();
  double norm_hi = hist_hi->Integral();

  double slope = (norm_hi-norm_lo)/(pointHi-pointLo);
  double b = norm_hi-slope*pointHi;
  double norm = slope*point+b;

  int overSample = 1000; // oversample and rebin later. this alleviates uneven binning of RooMomentMorph

  hist_lo->Scale(1./norm_lo);
  hist_hi->Scale(1./norm_hi);

  TH1D* morphD;

  if (!justLinear)
  {
    // setup mass reference variables
    RooRealVar m("mass","mass",point);
    RooArgList mref(RooConst(pointLo),RooConst(pointHi));

    // setup pdfs
    RooRealVar obs("obs","obs",hist_lo->GetXaxis()->GetXmin(), hist_lo->GetXaxis()->GetXmin(), hist_lo->GetXaxis()->GetXmax());
    obs.setBins(hist_lo->GetNbinsX()*overSample);

    RooArgList lobs1(obs);
    RooDataHist dh1((string(hist_lo->GetName())+"-dh1").c_str(), (string(hist_lo->GetName())+"-dh1").c_str(), obs, hist_lo);
    RooHistPdf pdf1((string(hist_lo->GetName())+"-pdf1").c_str(), (string(hist_lo->GetName())+"-pdf1").c_str(), obs, obs, dh1);

    // RooRealVar obs2("obs2","obs2",hist_hi->GetXaxis()->GetXmin(), hist_hi->GetXaxis()->GetXmin(), hist_hi->GetXaxis()->GetXmax());
    RooArgList lobs2(obs);
    RooDataHist dh2((string(hist_hi->GetName())+"-dh2").c_str(), (string(hist_hi->GetName())+"-dh2").c_str(), obs, hist_hi);
    RooHistPdf pdf2((string(hist_hi->GetName())+"-pdf2").c_str(), (string(hist_hi->GetName())+"-pdf2").c_str(), obs, obs, dh2);
  
    // build pdf interpolator
    RooArgList lobs(obs, obs);
    RooArgList lpdf(pdf1, pdf2);
    RooMomentMorph morph("morph", "morph", m, lobs, lpdf,  mref, 
       (interpMode == 0 ? RooMomentMorph::Linear :
        interpMode == 1 ? RooMomentMorph::NonLinear :
        interpMode == 2 ? RooMomentMorph::NonLinearPosFractions :
        RooMomentMorph::NonLinearLinFractions));

    // get the histogram
    TH1F* morphF = (TH1F*)morph.createHistogram((name+"_F").c_str(), obs/*, Binning(hist_lo->GetNbinsX())*/);
    morphF->Rebin(overSample);

    // convert to a TH1D
    int nrBins = morphF->GetNbinsX();
    morphD = new TH1D(name.c_str(),name.c_str(),nrBins,morphF->GetXaxis()->GetXmin(),morphF->GetXaxis()->GetXmax());
    for (int i=1;i<=nrBins;i++)
    {
      morphD->SetBinContent(i, morphF->GetBinContent(i));
    }
  }
  else
  {
    int nrBins = hist_lo->GetNbinsX();
    double x_lo = hist_lo->GetXaxis()->GetXmin();
    double x_hi = hist_lo->GetXaxis()->GetXmax();
    morphD = new TH1D(name.c_str(),name.c_str(),nrBins,x_lo,x_hi);
    for (int i=1;i<=nrBins;i++)
    {
      double interp_content = interp(point, pointLo, pointHi, hist_lo->GetBinContent(i), hist_hi->GetBinContent(i));
      morphD->SetBinContent(i, interp_content);
    }
  }

  hist_lo->Scale(norm_lo);
  hist_hi->Scale(norm_hi);
  morphD->Scale(norm);

  cout << "Done" << endl;
  return morphD;
}

void momentMorph2(string sampleName, string name, double pointLo, double pointHi, double mass, string version, string folderName, TFile* outFile)
{
  double scaleLo = sampleName == "ggf" ? get_ggf(pointLo)*get_br(pointLo) : get_vbf(pointLo)*get_br(pointLo);
  double scaleHi = sampleName == "ggf" ? get_ggf(pointHi)*get_br(pointHi) : get_vbf(pointHi)*get_br(pointHi);
  double scale   = sampleName == "ggf" ? get_ggf(mass)*get_br(mass)       : get_vbf(mass)*get_br(mass);

  // int nrBasePoints = 39;
  int nrPoints = mass < 200 ? 23 : 6;
  int* massPoints = new int[nrPoints];
  if (mass < 200)
  {
    int counter=0;
    for (int i=90;i<=200;i+=5) massPoints[counter++] = i;
  }
  else
  {
    int counter=0;
    for (int i=200;i<=300;i+=20) massPoints[counter++] = i;
  }
  bool alt = false;
  if (mass < 200) alt = true;

  double norm_lo=0,norm_hi=0;
  int overSample = 1000; // oversample and rebin later. this alleviates uneven binning of RooMomentMorph

  RooRealVar m("mass","mass",mass);
  RooArgList mref;
  RooArgList obss;
  RooArgList pdfs;
  RooRealVar obs("obs","obs",0,0,1);

  TFile** files = new TFile*[nrPoints];
  TFile* tmpFile = new TFile(("rev/"+version+"/normHists/"+folderName+"/"+sampleName+dtos(massPoints[0])+".root").c_str());
  TH1D* tmpHist = (TH1D*)tmpFile->Get(name.c_str());
  obs.setBins(tmpHist->GetNbinsX()*overSample);
  tmpFile->Close();

  for (int im=0;im<nrPoints;im++)
  {
    int thisMass = massPoints[im];
    string smass=dtos(thisMass);
    string salt = thisMass==200&&mass<200?"_alt":"";

    TFile* file = new TFile(("rev/"+version+"/normHists/"+folderName+"/"+sampleName+smass+".root").c_str());
    TH1D* hist = (TH1D*)file->Get(name.c_str());

    if (thisMass == pointLo)
    {
      norm_lo = hist->Integral()/scaleLo;
    }
    if (thisMass == pointHi)
    {
      norm_hi = hist->Integral()/scaleHi;
    }

    RooDataHist* dh = new RooDataHist((string(hist->GetName())+"-dh-"+smass).c_str(), (string(hist->GetName())+"-dh1-"+smass).c_str(), obs, hist);
    RooHistPdf* pdf = new RooHistPdf((string(hist->GetName())+"-pdf-"+smass).c_str(), (string(hist->GetName())+"-pdf-"+smass).c_str(), obs, obs, *dh);

    mref.add(RooConst(thisMass));
    obss.add(obs);
    pdfs.add(*pdf);
    files[im] = file;
  }

  RooMomentMorph morph("morph", "morph", m, obss, pdfs,  mref,
           (interpMode == 0 ? RooMomentMorph::Linear :
            interpMode == 1 ? RooMomentMorph::NonLinear :
            interpMode == 2 ? RooMomentMorph::NonLinearPosFractions :
            RooMomentMorph::NonLinearLinFractions));

  // get the histogram
  TH1F* morphF = (TH1F*)morph.createHistogram((name+"_F").c_str(), obs/*, Binning(hist_lo->GetNbinsX())*/);
  morphF->Rebin(overSample);

  outFile->cd();
  // convert to a TH1D
  int nrBins = morphF->GetNbinsX();
  TH1D* morphD = new TH1D(name.c_str(),name.c_str(),nrBins,morphF->GetXaxis()->GetXmin(),morphF->GetXaxis()->GetXmax());
  for (int i=1;i<=nrBins;i++)
  {
    morphD->SetBinContent(i, morphF->GetBinContent(i));
  }

  double slope = (norm_hi-norm_lo)/(pointHi-pointLo);
  double b = norm_hi-slope*pointHi;
  double norm = slope*mass+b;

  morphD->Scale(norm*scale);

  // cleanup, return
  delete massPoints;
  for (int im=0;im<nrPoints;im++) files[im]->Close();
  delete files;
}

string dtos(double d)
{
  stringstream s;
  s << d;
  return s.str();
}
