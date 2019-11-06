// Author: Aaron Armbruster
// Date:   2011-11-16
//
// Description:
//
// Normalize systematic histograms and write out the total overall variations in rates to txt file

#include "macros/setup.C"

#include "TFile.h"
#include "TH1D.h"
#include "TSystem.h"

#include <sstream>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <set>
#include <math.h>

using namespace std;

struct SysInfo
{
  string id;
  double hi;
  double lo;
};

void makeNorms(double mass = 0, string version = "test", bool alt = false, bool interpSigOnly = false)
{
  LOG(logINFO) << "Normalizing systematic histograms";

  setup(mass, alt);

  stringstream massStr;
  massStr << mass;
  string smass = massStr.str();

  stringstream altStr;
  if (alt) altStr << "_alt";
  string salt = altStr.str();

  int nrBasePoints = 43;
  double* baseMassPoints = new double[nrBasePoints];
  double thisMass = 90;
  double step = 5;
  for (int i=0;i<nrBasePoints;i++)
  {
    baseMassPoints[i] = thisMass;
    if (thisMass >= 200) step = 20;

    thisMass += step;
  }

  // int nrBasePoints = 39;
  // int* baseMassPoints = new int[nrBasePoints];
  // int thisMass = 110;
  // int step = 5;
  // for (int i=0;i<nrBasePoints;i++)
  // {
  //   baseMassPoints[i] = thisMass;
  //   if (thisMass >= 200) step = 20;
  //   thisMass += step;
  // }

  // unnecessary
  int nrPoints = 1;
  double* massPoints = new double[nrPoints];
  massPoints[0] = mass;

  int nrSys = fileNames->size();

  // prepare sample and channel names
  vector<string> sampleNames;
  if (!interpSigOnly)
  {
    if (dost)    sampleNames.push_back("st");
    if (dottbar) sampleNames.push_back("ttbar");
    if (doww)
    {
      if (!splitww)
      {
        sampleNames.push_back("ww");
      }
      else
      {
        sampleNames.push_back("ggww");
        sampleNames.push_back("qqww");
      }
    }
    if(!doOSmSS){//Nina
      if (dowwew)  sampleNames.push_back("wwew");
      if (dowzzz)  sampleNames.push_back("wzzz");
      if (dowzzzew)sampleNames.push_back("wzzzew");
      if (doWgs)   sampleNames.push_back("wgs");
      if (doWg)    sampleNames.push_back("wg");
    }
    if (dozjets)
    {
      if (!splitzjets)
      {
        sampleNames.push_back("zjets");
      } else
      {
        sampleNames.push_back("zleplep");
        sampleNames.push_back("ztautau");
      }
    }
    if (dozjetsew)
    {
      if (!splitzjets)
      {
        sampleNames.push_back("zjetsew");
      } else
      {
        sampleNames.push_back("zleplepew");
        sampleNames.push_back("ztautauew");
      }
    }
    if(doOSmSS){//Nina
      sampleNames.push_back("wjetsminusss");
      sampleNames.push_back("ss");
    }else{
      if (doWjets) sampleNames.push_back("wjets"); 
    }

  }
  for (int imass=0;imass<nrPoints;imass++)
  {
    bool found = false;
    for (int ibase=0;ibase<nrBasePoints;ibase++)
    {
      if (baseMassPoints[ibase] == massPoints[imass])
      {
        found = true;
        break;
      }
    }

    if (!found && !interpSigOnly) continue;
    if (found && interpSigOnly) continue;

    stringstream ggfName;
    ggfName << "ggf" << massPoints[imass];
    
    stringstream ggfName0;
    if(useJHUspin0)
    {
      ggfName0 << "ggH" << massPoints[imass] << "spin0p";
      // ggfName << "ggH" << massPoints[imass] << "spin0p";
    }  
    else
    {
      ggfName0 << "ggf" << massPoints[imass];
      // ggfName << "ggf" << massPoints[imass];
    }

    stringstream ggfName2;
    if(doSpin2plus)  ggfName2 << "ggH" << massPoints[imass] << "spin2p";
    else if(doSpin1plus)  ggfName2 << "ggH" << massPoints[imass] << "spin1p";
    else if(doSpin1minus)  ggfName2 << "ggH" << massPoints[imass] << "spin1m";
    else if(doSpin0minus)  ggfName2 << "ggH" << massPoints[imass] << "spin0m";

    stringstream vbfName;
    vbfName << "vbf" << massPoints[imass];

    stringstream whName;
    whName << "wh" << massPoints[imass];

    stringstream zhName;
    zhName << "zh" << massPoints[imass];

    if (include125BG)
    {
      sampleNames.push_back("ggf125bg");
      sampleNames.push_back("vbf125bg");
      sampleNames.push_back("wh125bg");
      sampleNames.push_back("zh125bg");
    }

    if (!doSpin)
    {
      //if (!doFermi && doggf && !doVBF2j) sampleNames.push_back(ggfName.str());
      //else sampleNames.push_back("ggf125");
      if (doggf) sampleNames.push_back(ggfName.str());
    } else
    {
      if (doggf) sampleNames.push_back(ggfName0.str());
      if (doggf) sampleNames.push_back(ggfName2.str());
    }
    if (dovbf) sampleNames.push_back(vbfName.str());
    if (mass <= 300 && doVH)
    {
      if (dowh) sampleNames.push_back(whName.str());
      if (dozh) sampleNames.push_back(zhName.str());
    }
  }
  delete massPoints;
  delete baseMassPoints;
  int nrSamples = sampleNames.size();
  if (nrSamples == 0) return;

  vector<string> channelNames;
  if (doee) channelNames.push_back("ee");
  if (doem) channelNames.push_back("em");
  if (splitem && dome) channelNames.push_back("me");
  if (domm) channelNames.push_back("mm");
  if (combineCRs) channelNames.push_back("OF");
  if (combineSFCRs) channelNames.push_back("SF");
  int nrChannels = channelNames.size();

  vector<string> jetNames;
  if (do0j) jetNames.push_back("0j");
  if (do1j) jetNames.push_back("1j");
  if (do2j) jetNames.push_back("2j");
  int nrJets = jetNames.size();

  int nrRegions = regions->size();

  TFile** files = new TFile*[nrSamples];

  //save nominal values
  LOG(logINFO) << "Saving nominal values";
  map<string, double> nominals_tot;
  map<string, pair<double, double> > nominals;
  for (int isam=0;isam<nrSamples;isam++)
  {
    stringstream fileName;
    fileName << "rev/" << version << "/hists/Nominal/Normal/" << sampleNames[isam] << ".root";
    TFile* f = new TFile(fileName.str().c_str());
    files[isam] = f;
    for (int ichan=0;ichan<nrChannels;ichan++)
    {
      for (int ijet=0;ijet<nrJets;ijet++)
      {
        for (int ireg=0;ireg<nrRegions;ireg++)
        {
          LOG(logINFO) << "ireg = " << ireg << " / " << nrRegions;
          string regionName = (*regions)[ireg].name;
          Region* r = &(*regions)[ireg];

          bool skipLoop = skipRegion(r, channelNames[ichan], jetNames[ijet]);
          if (skipLoop) continue;

          LOG(logDEBUG) << "Made it through the gauntlet";

          stringstream histName;
          histName << channelNames[ichan] << "_" << (*regions)[ireg].name << "_" << jetNames[ijet];
          TH1D* hist = (TH1D*)f->Get(histName.str().c_str());
          LOG(logDEBUG) << "hist: " << hist;
          
          if (!hist)
          {
            LOG(logERROR) << "Couldn't find hist: " << histName.str();
            exit(1);
          }

          string nomName = sampleNames[isam] + "_" + histName.str();
          double integral, error;
          integral = hist->IntegralAndError(0, hist->GetNbinsX()+1, error);
          nominals[nomName] = make_pair(integral, error);

          if (isam == 0) nominals_tot[histName.str()] = 0;
          nominals_tot[histName.str()] += integral;

          LOG(logINFO) << "Exiting loop";
        }
      }
    }
  }

  //big loop that does the normalization
  LOG(logINFO) << "Normalizing histograms / computing norms";
  vector<SysInfo> removed;
  vector<SysInfo> warn;
  stringstream outFileName;
  system(("mkdir -vp rev/"+version+"/normHists").c_str());
  ofstream outFile(("rev/"+version+"/normHists/norms_"+smass+salt+(interpSigOnly?"_sig":"")+".txt").c_str());
  for (int isys=0;isys<nrSys;isys++)
  {
    Sys* sys = &(*fileNames)[isys];
    bool first = true;
    LOG(logINFO) << "Processing sys: " << sys->folder;

    bool sym = sys->fileUp == sys->fileDown; // are the variations symmetric?
    for (int isam=0;isam<nrSamples;isam++)
    {
      if (sys->sampleNames.find(sampleNames[isam]) == sys->sampleNames.end()) continue;

      string baseFolder = sys->folder+"/";
      system(("mkdir -vp rev/"+version+"/normHists/"+baseFolder+sys->fileUp).c_str());
      if (!sym) system(("mkdir -vp rev/"+version+"/normHists/"+baseFolder+sys->fileDown).c_str());

      bool doHi = 1;
      bool doLo = 1;

      if (!gSystem->AccessPathName(("rev/"+version+"/normHists/"+baseFolder+sys->fileUp+"/"+sampleNames[isam]+".root").c_str(), kFileExists)) {
	while (!gSystem->AccessPathName(("rev/"+version+"/normHists/"+baseFolder+sys->fileUp+"/"+sampleNames[isam]+".root.lock").c_str(), kFileExists))
	  sleep(30);
	doHi = 0;
      }
      else
	if (sys->folder != "Nominal")
	  gSystem->Exec(("touch rev/"+version+"/normHists/"+baseFolder+sys->fileUp+"/"+sampleNames[isam]+".root.lock").c_str());
      if (!gSystem->AccessPathName(("rev/"+version+"/normHists/"+baseFolder+sys->fileDown+"/"+sampleNames[isam]+".root").c_str(), kFileExists)) {
	while (!gSystem->AccessPathName(("rev/"+version+"/normHists/"+baseFolder+sys->fileDown+"/"+sampleNames[isam]+".root.lock").c_str(), kFileExists))
	  sleep(30);
	doLo = 0;
      }
      else
	if (sys->folder != "Nominal")
	  gSystem->Exec(("touch rev/"+version+"/normHists/"+baseFolder+sys->fileDown+"/"+sampleNames[isam]+".root.lock").c_str());
      if (doHi) system(("cp rev/"+version+"/hists/"+baseFolder+sys->fileUp+"/"+sampleNames[isam]+".root rev/"+version+"/normHists/"+baseFolder+sys->fileUp+"/"+sampleNames[isam]+".root").c_str());
      if (!sym && doLo) system(("cp rev/"+version+"/hists/"+baseFolder+sys->fileDown+"/"+sampleNames[isam]+".root rev/"+version+"/normHists/"+baseFolder+sys->fileDown+"/"+sampleNames[isam]+".root").c_str());

      if (sys->folder == "Nominal") continue;

      TFile* f_hi = new TFile(("rev/"+version+string(doHi?"/normHists/":"/hists/")+baseFolder+sys->fileUp+"/"+sampleNames[isam]+".root").c_str(),doHi?"update":"read");
      TFile* f_lo = sym ? NULL : new TFile(("rev/"+version+string(doLo?"/normHists/":"/hists/")+baseFolder+sys->fileDown+"/"+sampleNames[isam]+".root").c_str(),doLo?"update":"read");
      
      for (int ichan=0;ichan<nrChannels;ichan++)
      {
        for (int ijet=0;ijet<nrJets;ijet++)
        {
          for (int ireg=0;ireg<nrRegions;ireg++)
          {
            Region* r = &(*regions)[ireg];

            bool skipLoop = skipRegion(r, channelNames[ichan], jetNames[ijet]);
            if (skipLoop) continue;

            string histName = channelNames[ichan]+"_"+(*regions)[ireg].name+"_"+jetNames[ijet];
            TH1D* h_hi = (TH1D*)f_hi->Get(histName.c_str());
            TH1D* h_lo = sym ? NULL : (TH1D*)f_lo->Get(histName.c_str());

            if (!h_hi || (!sym && !h_lo))
            {
              LOG(logWARNING) << "Hist doesn't exist: " << histName << ", for sys ";
              continue;
            }

            double hi_int = h_hi->Integral();
            double lo_int = sym ? 0 : h_lo->Integral();

            double nom = nominals[sampleNames[isam]+"_"+histName].first;

            LOG(logDEBUG) << "Testing nominal for " << sampleNames[isam] << "_" << histName << ": " << nom;
            if (nom < 0.0001)
            {
              continue;
            }

            double var_up = hi_int/nom - 1; // get the variations
            double var_down = sym ? var_up : lo_int/nom - 1;
            //double var_down = sym ? -var_up/(1+var_up) : lo_int/nom - 1;

            string id = sampleNames[isam] + "_" + sys->folder + "_" + histName;

            if (fabs(var_up) < 0.001) var_up = 0;
            if (fabs(var_down) < 0.001) var_down = 0;

            bool remove = false;

            set<string> flagged; //flag spurious systematics that result from low MC stat
            //       flagged.insert("");
            //       flagged.insert("");
            flagged.insert("");
            if (flagged.find(id) != flagged.end()) remove = true;
            if (((id.find("ttbar") != string::npos || id.find("st") != string::npos) &&
                 /*id.find("JES") != string::npos &&*/ id.find("0j") != string::npos)) remove = true;
            if(doABCD2j && (id.find("SF") != string::npos || id.find("mm") != string::npos || id.find("ee") != string::npos) && id.find("signalLike") != string::npos && id.find("2j") != string::npos && (id.find("ztautau") != string::npos || id.find("zjets") != string::npos || id.find("zleplep") != string::npos) && id.find("ew") == string::npos) remove = true;

            //pick up any spurious systematics not otherwise flagged
            //if (var_up <= -0.99 || var_down <= -0.99 || var_up > 2 || var_down > 2) remove = true;
            if (var_up <= -0.8 || var_down <= -0.8 || var_up > 1.5 || var_down > 1.5) remove = true;


            if (remove) // remove by setting to zero
            {
              SysInfo info;
              info.id=id;
              info.hi=var_up;
              info.lo=var_down;
              removed.push_back(info);

              var_up=0;
              var_down=0;
            }


            if (fabs(var_up) > 0.5 || fabs(var_down) > 0.5)
            {
              SysInfo info;
              info.id=id;
              info.hi=var_up;
              info.lo=var_down;
              warn.push_back(info);
            }


            //no need to consider very small systematics. they do nothing but slow down computations
            if ((fabs(var_up) > 0.005 || fabs(var_down) > 0.005) && sys->isNorm )
            {
              if (first)
              {

                outFile << "seed " << sys->folder << "\n"
                << "range -5 5\n"
                << "type bifur_gaus\n";
                first=false;
              }

              outFile << "uncert " << id << " " << var_up << " " << var_down << "\n";

            }

            //normalize
            h_hi->Scale(nom/hi_int);
            if (h_lo) h_lo->Scale(nom/lo_int);

            //determine if it's negligible
            if (sys->isShape)
            {
              TH1D* nomHist = (TH1D*)files[isam]->Get(histName.c_str());
              bool isNegligible = true;
              int nrBins = h_hi->GetNbinsX();
              for (int ib=0;ib<nrBins;ib++)
              {
                double nom_content = nomHist->GetBinContent(ib+1);
                if (nom_content < 0.00001) continue;
                double hi_content = h_hi->GetBinContent(ib+1);
                double lo_content = h_lo ? h_lo->GetBinContent(ib+1) : nom_content;
                double var_hi = hi_content / nom_content - 1;
                double var_lo = lo_content / nom_content - 1;
                if (fabs(var_hi) > 0.01 || fabs(var_lo) > 0.01)
                {
                  isNegligible = false;
                  break;
                }
              }

              if (isNegligible)
              {
                LOG(logWARNING) << "Negligible shape systematic: " << sampleNames[isam] << "_" << histName;
                sys->veto.insert(sampleNames[isam]+"_"+histName);
              }
            }
          }
        }
      }

      //write
      if (doHi) {
	f_hi->Write();
	gSystem->Exec(("rm -f rev/"+version+"/normHists/"+baseFolder+sys->fileUp+"/"+sampleNames[isam]+".root.lock").c_str());
      }
      f_hi->Close();
      if (f_lo)
      {
        if (doLo) {
	  f_lo->Write();
	  gSystem->Exec(("rm -f rev/"+version+"/normHists/"+baseFolder+sys->fileDown+"/"+sampleNames[isam]+".root.lock").c_str());
	}
        f_lo->Close();
	delete f_lo;
      }
      delete f_hi;
    }

    outFile << "\n";
  }
  outFile.close();

  for (int isam=0;isam<nrSamples;isam++)
  {
    files[isam]->Close();
  }
  delete files;

  //print warnings of very large systematics
  int nrRemove=removed.size();
  for (int i=0;i<nrRemove;i++)
  {
    SysInfo* info = &removed[i];
    LOG(logWARNING) << "Removing spurious systematic: " << info->id << " " << info->hi << " " << info->lo;
  }

  ofstream warnFile(("rev/"+version+"/normHists/"+"warned_"+smass+salt+".txt").c_str());

  int nrWarn=warn.size();
  for (int i=0;i<nrWarn;i++)
  {
    SysInfo* info = &warn[i];
    LOG(logWARNING) << "WARNING::Spurious systematic remaining: " << info->id << " " << info->hi << " " << info->lo;
    warnFile << "flagged.insert(\"" << info->id << "\");\n";
  }

  if (interpSigOnly) return;

  //save statistical uncertainties and wjets systematics
  if (doData) system(("cp rev/"+version+"/hists/Nominal/Normal/data.root rev/"+version+"/normHists/Nominal/Normal/data.root").c_str());

  return;
}
