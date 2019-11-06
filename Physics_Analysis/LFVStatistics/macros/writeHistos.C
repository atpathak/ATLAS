#ifndef WRITEHISTOS
#define WRITEHISTOS

// Author: Aaron Armbruster
// Data:   2011-11-16
//
// Description:
//
// Write out all histograms to root files from input trees.
//
// 1) load rates into global objects from the trees
// 2) prepare mT or BDT output mapping
// 3) fill histos

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TSystem.h"

#include "macros/setup.C"
#include "macros/fileHolder.C"
#include "macros/binning.cc"

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <utility>
#include <fstream>
#include <stdio.h>
#include <math.h>

using namespace std;

void getRates(string sampleName, string folder, double mass, bool alt);
//Returns true and fills map_reg_boundaries with values existing in the relevant boundaries file, returns false if this file does not exist
bool getBoundariesFromFile(std::string filename, map<string, set<double> > & map_reg_boundaries);

// MAPPPING MODE
// 0 = nothing
// 1 = map to emu_signalLike_0j range
// 2 = map to flat background
int mappingMode    = 0;

// CONTROL REGION BINNING
bool doSingleBinCR = true;

// FUNCTION TO WRITE OUT HISTOGRAMS
//If offsetSys==-1, hists corresponding to all systematic variations will be done, otherwise only hists corresponding to the syst variation #offsetSys will be done
//If doOnlySignal==false, adding only signal samples to samplesNames so that hists are built only for signal (interesting to get the mass points made in jobs different from the -common- background hists)
void writeHistos(double mass = 0, string version = "test", bool alt = false, int offsetSys=-1, bool doOnlySignal=false)
{
  // setup initial variables, prepare list of sample names
  setup(mass, alt, doOnlySignal);

  double scale = -1.0; // 20.0;

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
  for (int i = 0; i < nrBasePoints; i++)
  {
    baseMassPoints[i] = thisMass;
    if (thisMass >= 200) step = 20;
    thisMass += step;
  }

  int nrPoints = 1;
  double* massPoints = new double[nrPoints];
  massPoints[0] = mass;

  int nrSys = fileNames->size();
  if (offsetSys>=nrSys) {
    LOG(logWARNING) << "Skipping job supposed to be for syst #" << offsetSys << " while only " << nrSys << " exist !";
    return;
  }

  vector<string> sampleNames;
  if (!doOnlySignal) {
    if (dost)    sampleNames.push_back("st");
    if (dottbar) sampleNames.push_back("ttbar");
    if (doww) {
      if (!splitww) {
        sampleNames.push_back("ww");
      }
      else {
        sampleNames.push_back("ggww");
        sampleNames.push_back("qqww");
      }
    }
    if(!doOSmSS){// Nina
      if (doWjets) sampleNames.push_back("wjets");
      if (dowwew)  sampleNames.push_back("wwew"); //Nina... are these samples considered dibosons?
      if (dowzzz)  sampleNames.push_back("wzzz");
      if (dowzzzew)sampleNames.push_back("wzzzew");
      if (doWgs)   sampleNames.push_back("wgs");
      if (doWg)    sampleNames.push_back("wg");
    } else {
      sampleNames.push_back("wjetsminusss");
      sampleNames.push_back("ss");
   }
    if (dozjets) {
      if (!splitzjets) {
        sampleNames.push_back("zjets");
      } else    {
        sampleNames.push_back("zleplep");
        sampleNames.push_back("ztautau");
      }
    }
    if (dozjetsew) {
      if (!splitzjets) {
        sampleNames.push_back("zjetsew");
      } else    {
        sampleNames.push_back("zleplepew");
        sampleNames.push_back("ztautauew");
      }
    }
    //if (doWjets) sampleNames.push_back("wjets");
  }//end of if !doOnlySignal

  for (int imass=0;imass<nrPoints;imass++)  {
    bool found = false;
    for (int ibase = 0; ibase < nrBasePoints; ibase++)
    {
      if (baseMassPoints[ibase] == massPoints[imass])
      {
        found = true;
        break;
      }
    }

    // see if we have MC for this point, if not wait and interpolate at a later stage
    if (!found) continue;

    stringstream ggfName;
    ggfName << "ggf" << massPoints[imass];

    stringstream ggfName0;
    if(useJHUspin0){
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

    if (!doSpin) {
      if (doggf) sampleNames.push_back(ggfName.str());
    } else {
      if (doggf) sampleNames.push_back(ggfName0.str());
      if (doggf) sampleNames.push_back(ggfName2.str());
    }
    if (dovbf) sampleNames.push_back(vbfName.str());
    if (mass <= 300 && doVH)
    {
      if (dowh) sampleNames.push_back(whName.str());
      if (dozh) sampleNames.push_back(zhName.str());
    }
  }// end of loop over nrPoints mass points

  int nrSamples = sampleNames.size();

  // load rates into global variables

  int nrReg = regions->size();
  Region* r_norm = &(*regions)[0]; // for normalizing mt to maximum sr
  // int nrChans = r_norm->channels.size();

  // Trying to get the boundaries if they already exist
  map<string, set<double> > map_reg_boundaries;
  bool doBoundariesExist=false;
  if (mappingMode == 2) {
    std::string boundariesFilename = std::string("rev/"+version+"/hists/boundaries_"+smass+salt+".txt");
    if (mode != 0 && !CutMTandRebin)
      boundariesFilename = std::string("rev/"+version+"/hists/boundaries_125.txt");
    doBoundariesExist = getBoundariesFromFile(boundariesFilename, map_reg_boundaries);
    //In case we need the boundaries to be externally provided and they don't exist yet
    if (doOnlySignal && !doBoundariesExist) {
      LOG(logWARNING) << "Signal hist building job waiting for boundaries (from bkg hists) file (" << boundariesFilename.c_str() << ") to be provided !";
      //we wait for the file to appear
      int iW=0;
      while (iW++ <60 && !doBoundariesExist) {
        LOG(logWARNING) << " waiting 1 minute...";
        sleep(60); //wait 1 minute
        doBoundariesExist = getBoundariesFromFile(boundariesFilename, map_reg_boundaries);
      }
      if (!doBoundariesExist) {
        LOG(logWARNING) << "Leaving job for signal hist building while boundaries (from bkg hists) do not exist yet !";
        return;
      }
    }
    if (doBoundariesExist) {
      LOG(logWARNING) << "Boundary file exists, will reuse it";
      if (offsetSys>0) {
        LOG(logWARNING) << "  ...will thus also skip the building of histograms for 'Nominal'.";
      }
    }
  }//end of mappingMode==2


  // then everything else
  // vector<string> folderNames;
  for (int isys = 0; isys < nrSys; isys++)
  {
    Sys* sys = &(*fileNames)[isys];
    set<string> folders;
    folders.insert(sys->fileUp);
    folders.insert(sys->fileDown);

    if (sys->folder == "Nominal" && doData)
    {
      getRates("data",sys->folder+"/Normal", mass, alt);
    }

    //If we want a specific systematic variation we run only over it and over "Nominal" to insure that the boundaries are done
    if (offsetSys!=-1 && offsetSys!=isys && sys->folder != "Nominal")
      continue;

    for (set<string>::iterator folder = folders.begin(); folder != folders.end(); folder++)
    {
      stringstream command;

      if (!doMVA)
      {
        string base = ntupleFolder; // "output";
        command << "mkdir -vp " << base+"/"+sys->folder+"/"+*folder;
      } else
      {
        command << "mkdir -vp " << basedir+"/H"+smass+"/"+sys->folder+"/"+*folder;
      }
      system(command.str().c_str());
     

      // folderNames.push_back(sys->folder+"/"+*folder);

      for (int isam=0;isam<nrSamples;isam++)
      {
        if (sys->sampleNames.find(sampleNames[isam]) == sys->sampleNames.end()) continue;
        getRates(sampleNames[isam], sys->folder+"/"+*folder, mass, alt);
      }
    }
  }
  // folderNames.push_back("FakeRate_HWW/SysFakeUp");
  // folderNames.push_back("FakeRate_HWW/SysFakeDown");
  // int nrFolders = folderNames.size();

  if (doData && !doOnlySignal) sampleNames.push_back("data");
  nrSamples = sampleNames.size();

  // Prepare mT mapping. Find mT points such that nominal total background is uniform for each channel
  if (!doBoundariesExist && mappingMode == 2)
  {
    for (int ireg = 0; ireg < nrReg; ireg++)
    {
      Region* r = &(*regions)[ireg];
      int nrChannels = r->channels.size();

      for (int ichan = 0; ichan < nrChannels; ichan++)
      {
        Channel* c = &r->channels[ichan];
        bool skipLoop = skipRegion(r, c->name, c->jetName);
        if (skipLoop) continue;

        if ( c->jetName == "0j" && doMVA && !doSmartRemapBDT0j) continue;
        if ( c->jetName == "1j" && doMVA && !doSmartRemapBDT1j) continue;
        if ( c->jetName == "2j" && doMVA && !doSmartRemapBDT2j) continue;

        int theseBins = getNbinsForRegion(r, c);

        // find total background
        for (map<string, map<string, multiset<pair<double, double> > > >::iterator map_map_itr = c->sys_sample_rates.begin(); map_map_itr!=c->sys_sample_rates.end();map_map_itr++)
        {
          if (map_map_itr->first.find("Nominal") == string::npos) continue;

          string name = c->name + "_" + r->name + "_" + c->jetName;

          LOG(logINFO) << "On Channel: " << name;

          map_reg_boundaries[name].clear();

          double tot = 0;
          multiset<pair<double, double> > all_rates;
          for (map<string, multiset<pair<double, double> > >::iterator map_itr = map_map_itr->second.begin();
               map_itr != map_map_itr->second.end();map_itr++)
          {
            if ((map_itr->first.find("ggf")  != string::npos/* && !doVBF2j*/) ||
                (map_itr->first.find("ggH")  != string::npos/* && !doVBF2j*/) ||
                map_itr->first.find("vbf")  != string::npos ||
                map_itr->first.find("wh")   != string::npos ||
                map_itr->first.find("zh")   != string::npos ||
                map_itr->first.find("data") != string::npos)
            {
              if (!(include125BG && map_itr->first.find("125bg") != string::npos)) continue; // sum bg only
            }
            for (multiset<pair<double, double> >::iterator set_itr = map_itr->second.begin();set_itr!=map_itr->second.end();set_itr++)
            {
              tot += set_itr->second;
              all_rates.insert(*set_itr);
            }
          }

          //find the rate we want per bin
          double nrPerBin = tot/theseBins;
          double totOrig = tot;
          double fraction1stBin = 1.0;
          if (c->jetName == "0j") fraction1stBin = fraction1stBin0j;
          if (c->jetName == "1j") fraction1stBin = fraction1stBin1j;
          if (c->jetName == "2j") fraction1stBin = fraction1stBin2j;
          if (doMVA && (doSmartRemapBDT0j || doSmartRemapBDT1j || doSmartRemapBDT2j))
          {
            nrPerBin = tot * fraction1stBin;
          }
          LOG(logINFO) << "Total: " << tot << ", Exp per bin: " << nrPerBin;
          double sum_tot = 0;
          tot = 0;
          for (multiset<pair<double, double> >::iterator set_itr = all_rates.begin();set_itr!=all_rates.end();set_itr++)
          {
            if (c->nj == 2 && (mode == 1 || mode == 6) && variableBin2j)
            {
              vector<double> edge = readBoundaryFile("config"+string(do2012?"_2012/":"_2011/")+"mt_bins_2j.txt");
              for (int ii=0;ii<(int)edge.size();ii++) map_reg_boundaries[name].insert(edge[ii]);
              break;
            }
            sum_tot += set_itr->second;
            tot += set_itr->second;
            if (tot > nrPerBin && int(map_reg_boundaries[name].size()+1) < theseBins && set_itr->second >= 0 /* fu*%! */ && tot < nrPerBin*theseBins) // and find the mT points that satisfy that
            {
              LOG(logINFO) << "tot = " << tot << ", inserting boundary #" << int(map_reg_boundaries[name].size()) << "/" << theseBins-1 << " : " << set_itr->first;
              map_reg_boundaries[name].insert(set_itr->first);
              tot = 0;
              if (doMVA && (doSmartRemapBDT0j || doSmartRemapBDT1j || doSmartRemapBDT2j))
              {
                nrPerBin = totOrig*(1.0-fraction1stBin)/((double)theseBins-1.0);
              }
            }
          }

          LOG(logINFO) << "sum_tot = " << sum_tot;
          LOG(logINFO) << "Found the following boundaries for hist: " << name;

          for (set<double>::iterator itr=map_reg_boundaries[name].begin();itr!=map_reg_boundaries[name].end();itr++)
          {
            LOG(logINFO) << "  -> " << *itr;
          }
          LOG(logINFO) << "";
          break;
        }
      }
    }
  }

  if (!doBoundariesExist && mappingMode == 2) {
    system(("mkdir -vp rev/"+version+"/hists").c_str());
    ofstream bdFile(("rev/"+version+"/hists/boundaries_"+smass+salt+".txt").c_str());
    for (map<string, set<double> >::iterator itr=map_reg_boundaries.begin();itr!=map_reg_boundaries.end();itr++)
      {
	bdFile << itr->first;
	
	set<double> bds = itr->second;
	for (set<double>::iterator itr2=bds.begin();itr2!=bds.end();itr2++)
	  {
	    bdFile << " " << *itr2;
	  }
	bdFile << "\n";
      }
    bdFile.close();
  }//end of if (!doBoundariesExist)

  double minVal_norm = *r_norm->all_rates.begin(); // used in alternate mapping scheme
  double maxVal_norm = *r_norm->all_rates.rbegin(); // (mapping to emu_signalLike_0j range)
  if (mappingMode == 2)
  {
    minVal_norm = 0;
    maxVal_norm = 1;
  }

  if (doMVA && doRemoveEmptyBins && (mappingMode == 0 || (doSmartRemapBDT0j || doSmartRemapBDT1j || doSmartRemapBDT2j) )) // loop to find out last filled bin, to not have any histogram bins
  {
    Region* rtemp0 = &(*regions)[0];
    Region* rtemp1 = &(*regions)[1];

    LOG(logDEBUG) << rtemp0->name << " max=" << *rtemp0->all_rates.rbegin();
    LOG(logDEBUG) << rtemp1->name << " max=" << *rtemp1->all_rates.rbegin();
    LOG(logDEBUG) << "";
  }

  // big nested loop to write all histograms
  // map<string, multiset<pair<double, double> >* > ggf_rates;
  // map<string, multiset<pair<double, double> >* > vbf_rates;
  for (int isys=((offsetSys==-1) ? 0 : offsetSys);isys<((offsetSys==-1) ? nrSys : offsetSys+1);isys++)
  {
    Sys* sys = &(*fileNames)[isys];
    set<string> folders;
    folders.insert(sys->fileUp);
    folders.insert(sys->fileDown);

    for (set<string>::iterator folder_itr=folders.begin();folder_itr!=folders.end();folder_itr++)
    {
      // for (int isys=0;isys<nrFolders;isys++)
      // {
      string folder = sys->folder+"/"+*folder_itr;
      // string folder = smass+salt+"/"+sys->folder+"/"+*folder_itr;
      // string folder = smass+salt+"/"+folderNames[isys];
      LOG(logINFO) << "Writing folder: " << folder;
      for (int isam=0;isam<nrSamples;isam++)
      {
        string sampleName = sampleNames[isam];
        //if (sampleName != "wjets" && folder.find("FakeRate_HWW") != string::npos) continue;
        if (sys->sampleNames.find(sampleName) == sys->sampleNames.end()) continue;

        LOG(logINFO) << " -->Sample = " << sampleName;
        system(("mkdir -vp rev/"+version+"/hists/"+folder).c_str());

        if (!gSystem->AccessPathName(("rev/"+version+"/hists/"+folder+"/"+sampleName+".root").c_str(), kFileExists))
        {
          LOG(logWARNING) << "File " << "rev/" << version << "/hists/" << folder << "/" << sampleName << ".root exists. Continue!";
          continue;
        }

        TFile* outFile = new TFile(("rev/"+version+"/hists/"+folder+"/"+sampleName+".root").c_str(),"recreate");

        for (int ireg=0;ireg<nrReg;ireg++)
        {
          LOG(logINFO) << "ireg = " << ireg << " / " << nrReg;
          Region* r = &(*regions)[ireg];
          LOG(logINFO) << "   |-->Region = " << r->name;

          double minVal = r->all_rates.size() == 0 ? 0 : *r->all_rates.begin();
          double maxVal = r->all_rates.size() == 0 ? 10e9 : *r->all_rates.rbegin();

          LOG(logINFO) << "Done computing max/min";

          if (maxVal <= minVal)
          {
            LOG(logERROR) << "ERROR::Couldn't map mT distribution for region: " << r->name;
            exit(1);
          }

          LOG(logDEBUG) << "computing linear mapping";
          // for mode 1, use linear mT' = mT * sf + offset
          double sf = mappingMode != 0 ? (maxVal_norm - minVal_norm)/(maxVal-minVal) : 1;
          double offset = mappingMode != 0 ? maxVal_norm - sf*maxVal : 0;

          int nrChannels = r->channels.size();
          LOG(logDEBUG) << "entering channel loop";
          for (int ichan=0;ichan<nrChannels;ichan++)
          {
            Channel* c = &r->channels[ichan];
            LOG(logDEBUG) << "channel addy = " << c;
            LOG(logINFO) << "Trying to write histogram " << c->name+c->jetName << " " << r->name;

            bool skipLoop = skipRegion(r, c->name, c->jetName);
            if (skipLoop) continue;

            if (doMVA && c->jetName == "0j" && !doSmartRemapBDT0j)
            {
              minVal_norm = *r_norm->all_rates.begin(); // used in alternate mapping scheme
              maxVal_norm = *r_norm->all_rates.rbegin(); // (mapping to emu_signalLike_0j range)
              sf = 1;
              offset = 0;
            }
            if (doMVA && c->jetName == "1j" && !doSmartRemapBDT1j)
            {
              minVal_norm = *r_norm->all_rates.begin(); // used in alternate mapping scheme
              maxVal_norm = *r_norm->all_rates.rbegin(); // (mapping to emu_signalLike_0j range)
              sf = 1;
              offset = 0;
            }
            if (doMVA && c->jetName == "2j" && !doSmartRemapBDT2j)
            {
              minVal_norm = *r_norm->all_rates.begin(); // used in alternate mapping scheme
              maxVal_norm = *r_norm->all_rates.rbegin(); // (mapping to emu_signalLike_0j range)
              sf = 1;
              offset = 0;
            }

	    int theseBins = getNbinsForRegion(r, c);

            multiset<pair<double, double> >* rates = &c->sys_sample_rates[folder][sampleName];

            stringstream histName;
            histName << c->name << "_" << r->name << "_" << c->jetName;
            stringstream histNameTemp;
            histNameTemp << c->name << "_" << r->name << "_" << c->jetName << "_temp";

            if (mappingMode == 0 || (doMVA && ((c->jetName == "0j" && !doSmartRemapBDT0j) || (c->jetName == "1j" && !doSmartRemapBDT1j) || (c->jetName == "2j" && !doSmartRemapBDT2j))) )
              //if (mappingMode == 0)
            {
              if (!doMVA)
              {
                minVal_norm = *r->all_rates.begin();
                maxVal_norm = *r->all_rates.rbegin();
              }
              else
              {
                //assumes that the MVA output is between -1.0 and +1.0
                minVal_norm = -1.;
                
            if(doDynamicBinning){
              minVal_norm = *r->all_rates.begin();
              maxVal_norm = *r->all_rates.rbegin();
              // minVal_norm = 0.;      maxVal_norm = 1.1;
            }                
                
                if (!doRemoveEmptyBins) maxVal_norm = 1.;
                else
                {
                  int nBinsWanted = theseBins;
                  double BDTmax=-1;
                  for (int n=theseBins;n>1;n--)
                  {
                    if ( (-1.0)+((double)n-1.0)*(2.0/(double)theseBins) > BDTmax ) nBinsWanted--;
                  }
                  maxVal_norm=(-1.0)+nBinsWanted*(2.0/(double)theseBins);
                  LOG(logDEBUG) << BDTmax << " " << nBinsWanted << " " << maxVal_norm;
                  theseBins=nBinsWanted;
                }

                if(doSpin1D && doMVA)
                {
                  minVal_norm = *r->all_rates.begin();
                  maxVal_norm = *r->all_rates.rbegin();
                  LOG(logDEBUG) << "I am setting : minvalue = " << minVal_norm << " and maxvalue = " << maxVal_norm;
                }

                // maxVal_norm = 1;
              }
            }
            TH1D* hist;
            if (doCombineRightmostBin && (r->name.find("signalLike") != string::npos))
            {
              LOG(logINFO) << "in 1";
              hist = new TH1D(histNameTemp.str().c_str(), histNameTemp.str().c_str(), theseBins, minVal_norm, maxVal_norm);
            }
            else
            {
              LOG(logINFO) << "in 2";
              hist = new TH1D(histName.str().c_str(), histName.str().c_str(), theseBins, minVal_norm, maxVal_norm);
            }

            //TH1D* hist = new TH1D(histName.str().c_str(), histName.str().c_str(), theseBins, minVal_norm, maxVal_norm);
            hist->Sumw2();


            //main loop over multiset of mT and event weight, fill hists
            set<double> boundaries = map_reg_boundaries[histName.str()];
            set<double>::iterator bd = boundaries.begin();
            int bin = 1;
            double integral = 0;

            double total_em2jvbf = 0;

            for (multiset<pair<double, double> >::iterator itr = rates->begin();itr != rates->end();itr++)
            {
              if (mappingMode == 0 || mappingMode == 1  || (doMVA && ((c->jetName == "0j" && !doSmartRemapBDT0j) || (c->jetName == "1j" && !doSmartRemapBDT1j) || (c->jetName == "2j" && !doSmartRemapBDT2j))) )
              {
                hist->Fill(sf*itr->first + offset, itr->second);
              }
              else
              {
                if (!(c->jetName == "2j" && doSingleBin2j)) // fixme
                {
                  while (itr->first > *bd && bd != boundaries.end()) // see if we need to move to the next bin
                  {
                    bd++;
                    bin++;
                    //double bound = bd == boundaries.end() ? 10e9 : *bd;
                  }
                }

                if (sampleName.find("vbf") != string::npos && r->name == "signalLike" && c->jetName == "2j") total_em2jvbf += itr->second;

                double center = hist->GetBinCenter(bin); // just fill at the center of the desired bin
                hist->Fill(center, itr->second);
              }
              integral += itr->second;
            }

            if (sampleName.find("vbf") != string::npos && r->name == "signalLike" && c->jetName == "2j")
            {
              LOG(logDEBUG) << "total_em2jvbf=" << total_em2jvbf;
            }

            if (scale > 0 && hist->Integral() > 0) // used for some interpolation tests
            {
              hist->Scale(scale/hist->Integral());
            }

            LOG(logINFO) << "      |-->Channel = " << c->name+c->jetName << ", total = " << integral;

	    cout<<r->name<<endl;
            if (doSingleBinCR)
            {
              if (r->name.find("mainControl") != string::npos ||
                  r->name.find("topbox")      != string::npos ||
                  r->name.find("zbox")        != string::npos ||
                  r->name.find("ASR")         != string::npos ||
                  (r->name.find("AfrecSR")    != string::npos && (c->name == "em" || c->name == "me" || c->name == "OF" )) ||
                  r->name.find("CZpeak")      != string::npos ||
                  r->name.find("CfrecZpeak")  != string::npos ||
                  r->name.find("EWWCR")       != string::npos ||
                  r->name.find("EfrecWWCR")   != string::npos)
              {
                hist->Rebin(hist->GetNbinsX());
              }
            }

            if(do2012 && doSingleBinSF2j && c->jetName == "2j" && merge2jSFSR)  LOG(logDEBUG) << "WARNING doSingleBinSF2j = True and merge2jSFSR = True";
            else if(do2012 && doSingleBinSF2j && c->jetName == "2j" && !merge2jSFSR) // check with stefan
            {
              if(r->name.find("signalLike") != string::npos && (c->name == "mm" || c->name == "ee")) hist->Rebin(hist->GetNbinsX());
            }
            else if(do2011 && c->jetName == "2j" && r->name.find("signalLike") != string::npos) hist->Rebin(hist->GetNbinsX());

            //FIXME: need to check for doSpin?
            if (r->name.find("signalLike2") != string::npos && doSpin) hist->Rebin(hist->GetNbinsX());        
            if (r->name.find("signalLike3") != string::npos && doSpin) hist->Rebin(hist->GetNbinsX());              

            if (r->name.find("signalLike") != string::npos && c->jetName == "2j")
            {
              if (doSingleBin2j)
              {
                hist->Rebin(hist->GetNbinsX());
              }
            }

            if (doCombineRightmostBin && (r->name.find("signalLike") != string::npos))
            {
              TH1D* hist2 = hist;
              // lump last 2 bins
              hist = new TH1D(histName.str().c_str(), histName.str().c_str(), theseBins-1, minVal_norm, maxVal_norm-(hist2->GetBinWidth(theseBins)));
              hist->Sumw2();
              for (int ibin=1;ibin<theseBins-1;ibin++)
              {
                hist->SetBinContent(ibin,hist2->GetBinContent(ibin));
                hist->SetBinError(ibin,hist2->GetBinError(ibin));
              }
              hist->SetBinContent(theseBins-1,hist2->GetBinContent(theseBins-1)+hist2->GetBinContent(theseBins));
              hist->SetBinError(theseBins-1,sqrt(pow(hist2->GetBinError(theseBins-1),2)+pow(hist2->GetBinError(theseBins),2)));
              delete hist2;
            }

            if (sampleName != "data") // if we have zero entries in a bin, just set value to very something small to avoid problems later
            {
              for (int ibin=0;ibin<=theseBins;ibin++)
              {
                if (hist->GetBinContent(ibin+1) <= pow(10,-10)) hist->SetBinContent(ibin+1, pow(10, -18));
                if (hist->GetBinContent(ibin+1) <= pow(10,-10)) hist->SetBinError(ibin+1, 0); //need to reset also its error
                // if (hist->GetBinContent(ibin+1) <= 0) hist->SetBinContent(ibin+1, pow(10, -18));
              }
            }
            
          if(doSpin2plus_75qq){
            hist->SetBinContent(35, 0);
            hist->SetBinError(35, 0); 
          }
          if(doSpin2plus_25qq){
            hist->SetBinContent(31, 0);
            hist->SetBinError(31, 0); 
          }            
          }
        }
        LOG(logINFO) << "Closing file";
        outFile->Write();
        outFile->Close();
      }
    }
  }
  // write the raw signal mt values to a new tree to be used for interpolation
}


//read in rates from trees
void getRates(string sampleName, string folder, double mass, bool alt)
{
  bool applySlope = false;
  if (folder.find("ATLAS_SLOPE") != string::npos)
  {
    applySlope = true;
  }

  stringstream massStr;
  massStr << mass;
  string smass = massStr.str();

  stringstream altStr;
  if (alt) altStr << "_alt";
  string salt = altStr.str();

  LOG(logINFO) << "Preparing rates: " << folder << " :: " << sampleName;

  stringstream inFileName;
  string base = ntupleFolder;//"output";
  if (!doMVA)
  {
    if (applySlope)
    {
      inFileName << base+"/Nominal/Normal/"+sampleName;
    }
    else if (include125BG && sampleName.find("125bg") != string::npos)
    {
      if (sampleName == "ggf125bg") inFileName << base+"/"+folder+"/ggf125";
      if (sampleName == "vbf125bg") inFileName << base+"/"+folder+"/vbf125";
      if (sampleName == "wh125bg")  inFileName << base+"/"+folder+"/wh125";
      if (sampleName == "zh125bg")  inFileName << base+"/"+folder+"/zh125";
    }
    else
    {
      inFileName << base+"/"+folder+"/"+sampleName;
    }
  }
  else
  {
    inFileName << basedir+"/H"+smass+"/"+folder+"/"+sampleName;
  }
  if (sampleName == "wjets" || sampleName == "wjetsminusss") //Nina
  {
    if (wjMode == 1)
    {
      inFileName << "_Z";
    }
    else if (wjMode == 2)
    {
      inFileName << "_Comb";
    }
  }
  inFileName << ".root";

  TFile* file = new TFile(inFileName.str().c_str());
  TTree* tree;

  if (!doMVA)
  {
    tree = (TTree*)file->Get("Output");
  }
  else
  {
    tree = (TTree*)file->Get((TString)treeName);
  }


  if (!tree)
  {
    LOG(logERROR) << "ERROR::Tree doesn't exist for ntuple: " << inFileName.str();
    exit(1);
  }

  float l0pt, l1pt, mll, mtt, mt, metrel, trackmet, ptll, ptlljets, dphi, dphillmet, frecoil, pttot, /*bdphi, bdpsi,*/ met, dYjj, Mjj, cjv_leadPt, metstvf, metrelstvf, BDT_1jet_4vars, isSS, smt_muon_pt;

  unsigned int urun;
  int nj, ne, nm, run, nbt, nbt20, islowpt, isBlinded, ztautaumass, olv;
  float MVA_Mll, MVA_Mtt, MVA_lepPt0, MVA_lepPt1, MVA_MT, MVA_METRel, MVA_TrackMET, MVA_Ptll, MVA_DPhill, MVA_Mjj, MVA_jetEta0, MVA_jetEta1, MVA_BDPhill, MVA_BDPsill, MVA_BPLeadLep, MVA_BESubLeadLep, MVA_BESubLeadNeu, MVA_BPSubLeadLep, lepID0, lepID1, MVA_cjv_leadPt; //this is needed because struct Variable uses doubles, but except for EventWeight and mva_weight, MVATree stores variables as float

  if (!doMVA)
  {
    tree->SetBranchAddress("w",&vars_ptr->w);
    tree->SetBranchAddress("l1pt",&l1pt);
    if (splitem) tree->SetBranchAddress("l0pt",&l0pt);
    tree->SetBranchAddress("mll",&mll);
    tree->SetBranchAddress("mtt",&mtt);
    tree->SetBranchAddress("mjj",&Mjj);
    tree->SetBranchAddress("mt",&mt);
    tree->SetBranchAddress("metrel",&metrel);
    tree->SetBranchAddress("metrelstvf",&metrelstvf);
    tree->SetBranchAddress("met",&met);
    tree->SetBranchAddress("MET_STVF",&metstvf);
    tree->SetBranchAddress("trackmet",&trackmet);
    tree->SetBranchAddress("ptll",&ptll);
    tree->SetBranchAddress("ptlljets",&ptlljets);
    tree->SetBranchAddress("cjv_leadPt",&cjv_leadPt);
    tree->SetBranchAddress("pttot",&pttot);
    tree->SetBranchAddress("dphi",&dphi);
    tree->SetBranchAddress("dYjj",&dYjj);
    tree->SetBranchAddress("dphillmet",&dphillmet);
    tree->SetBranchAddress("frecoil",&frecoil);
    tree->SetBranchAddress("ztautaumass",&ztautaumass);
    // if (bt20GeV)
    // {
    tree->SetBranchAddress("nbt20",&nbt20);
    // }
    // else
    // {
    tree->SetBranchAddress("nbt20",&nbt);
    // }
    tree->SetBranchAddress("nj",&nj);
    tree->SetBranchAddress("ne",&ne);
    tree->SetBranchAddress("nm",&nm);
    tree->SetBranchAddress("run",&urun);
    tree->SetBranchAddress("islowpt",&islowpt);
    tree->SetBranchAddress("olv",&olv);
    tree->SetBranchAddress("isBlinded",&isBlinded);
    tree->SetBranchAddress("BDT_1jet_4vars", &BDT_1jet_4vars);
    tree->SetBranchAddress("isSS", &isSS);
    tree->SetBranchAddress("smt_muon_pt", &smt_muon_pt);
  }
  else
  {
    tree->SetBranchAddress("EventWeight",&vars_ptr->w);
    tree->SetBranchAddress("lepPt0",&MVA_lepPt0);
    if (splitem) tree->SetBranchAddress("lepPt1",&MVA_lepPt1);
    tree->SetBranchAddress("Mll",&MVA_Mll);
    tree->SetBranchAddress("Mtt",&MVA_Mtt);
    tree->SetBranchAddress("MT",&MVA_MT);
    tree->SetBranchAddress("METRel",&MVA_METRel);
    tree->SetBranchAddress("MET_TrackHWW",&MVA_TrackMET);
    tree->SetBranchAddress("Ptll",&MVA_Ptll);
    tree->SetBranchAddress("DPhill",&MVA_DPhill);
    tree->SetBranchAddress("mva_weight",&vars_ptr->mva_weight);
    if (doSpin)
    {
      tree->SetBranchAddress("subtrain_mva_weight",&vars_ptr->subtrain_mva_weight);
      tree->SetBranchAddress("BDPhill",&MVA_BDPhill);
      tree->SetBranchAddress("BDPsill",&MVA_BDPsill);
      tree->SetBranchAddress("BPLeadLep",&MVA_BPLeadLep);
      tree->SetBranchAddress("BPSubLeadLep",&MVA_BPSubLeadLep);
      tree->SetBranchAddress("BESubLeadLep",&MVA_BESubLeadLep);
      tree->SetBranchAddress("BESubLeadNeu",&MVA_BESubLeadNeu);
      tree->SetBranchAddress("lepID0",&lepID0);
      tree->SetBranchAddress("lepID1",&lepID1);
    }
    tree->SetBranchAddress("zjets_mva_weight",&vars_ptr->zjets_mva_weight);
    tree->SetBranchAddress("nbt20",&nbt20);
    tree->SetBranchAddress("nbt",&nbt);
    tree->SetBranchAddress("m_jet_n",&nj);
    tree->SetBranchAddress("m_el_n",&ne);
    tree->SetBranchAddress("m_mu_n",&nm);
    tree->SetBranchAddress("RunNumber",&run);
    tree->SetBranchAddress("Mjj",&MVA_Mjj);
    tree->SetBranchAddress("jetEta0",&MVA_jetEta0);
    tree->SetBranchAddress("jetEta1",&MVA_jetEta1);
    tree->SetBranchAddress("centralJetVeto_leadPt",&MVA_cjv_leadPt);
  }

  BinnedData2D bdthisto2D( -1, 1, -1, 1 );
  TH1D *tmp_hx = new TH1D("tmp_hx", "tmp_hx", nrBins0j_MVA_X, -1., 1.);
  TH1D *tmp_hy = new TH1D("tmp_hy", "tmp_hy", nrBins0j_MVA_Y, -1., 1.);

  if(doDynamicBinning){
    /*   
   bdthisto2D.add_bin( -1., -0.5, -1., -0.5 ); // add a bin with spec'd boundary
    bdthisto2D.add_bin( -0.5, 0., -1., -0.5 );
    bdthisto2D.add_bin( 0., 0.5, -1., -0.5 ); 
    bdthisto2D.add_bin( 0.5, 1., -1., -0.5 );

    bdthisto2D.add_bin( -1., -0.5, -0.5, 0. ); 
    bdthisto2D.add_bin( -0.5, 0., -0.5, 0. ); 
    bdthisto2D.add_bin( 0., 0.5, -0.5, 0. ); 
    bdthisto2D.add_bin( 0.5, 1., -0.5, 0. ); 

    bdthisto2D.add_bin( -1., -0.5, 0., 0.5 ); 
    bdthisto2D.add_bin( -0.5, 0., 0., 0.5 ); 

    bdthisto2D.add_bin( -1., -0.5, 0.5, 1. ); 
    bdthisto2D.add_bin( -0.5, 0., 0.5, 1. ); 
    */

    bdthisto2D.add_bin( -1., 0., -1., 0. );
    bdthisto2D.add_bin( 0., 1., -1., 0. );
    bdthisto2D.add_bin( -1., 0., 0., 1. );

    bdthisto2D.add_bin( 0., 0.2, 0., 0.5 ); 
    bdthisto2D.add_bin( 0.2, 0.4, 0., 0.5 ); 
    bdthisto2D.add_bin( 0.4, 0.6, 0., 0.5 ); 
    bdthisto2D.add_bin( 0.6, 0.8, 0., 0.5 ); 
    bdthisto2D.add_bin( 0.8, 1., 0., 0.5 ); 
 
    bdthisto2D.add_bin( 0., 0.2, 0.5, 1. ); 
    bdthisto2D.add_bin( 0.2, 0.4, 0.5, 1. ); 
    bdthisto2D.add_bin( 0.4, 0.6, 0.5, 1. ); 
    bdthisto2D.add_bin( 0.6, 0.8, 0.5, 1. ); 
    bdthisto2D.add_bin( 0.8, 1., 0.5, 1. ); 

  }

  // folder = smass + salt + "/" + folder;
  int nrEntries = tree->GetEntries();
  int nrReg = regions->size();
  for (int entry=0;entry<nrEntries;entry++)
  {
    // if (entry % 1000 == 0 || entry == nrEntries - 1)
    // {
    //   LOG(logDEBUG) << "->On entry " << entry+1 << " / " << nrEntries;
    // }

    tree->GetEntry(entry);

    if (!doMVA)
    {
      vars_ptr->l0pt = l0pt;
      vars_ptr->l1pt = l1pt;
      vars_ptr->mll = mll;
      vars_ptr->mtt = mtt;
      vars_ptr->Mjj = Mjj;
      vars_ptr->mt = mt;
      vars_ptr->metrel = metrel;
      vars_ptr->metrelstvf = metrelstvf;
      vars_ptr->met = met;
      vars_ptr->trackmet = trackmet;
      vars_ptr->metstvf = metstvf;
      vars_ptr->ptll = ptll;
      vars_ptr->ptlljets = ptlljets;
      vars_ptr->pttot = pttot;
      vars_ptr->dphi = dphi;
      vars_ptr->dYjj = dYjj;
      vars_ptr->cjv_leadPt = cjv_leadPt;
      vars_ptr->dphillmet = dphillmet;
      vars_ptr->run = urun;
      vars_ptr->nbt = nbt;
      vars_ptr->nbt20 = nbt20;
      vars_ptr->islowpt = islowpt;
      vars_ptr->isBlinded = isBlinded;
      vars_ptr->BDT_1jet_4vars = BDT_1jet_4vars;
      vars_ptr->isSS = isSS;
      vars_ptr->smt_muon_pt = smt_muon_pt;
      vars_ptr->olv = olv;
      vars_ptr->frecoil = frecoil;
      vars_ptr->ztautaumass = ztautaumass;
      vars_ptr->mllZ = TMath::Abs(mll - 91.1876);
      if (l0pt > l1pt) {
        vars_ptr->leadpt = l0pt;
        vars_ptr->subleadpt = l1pt;
      }
      else {
        vars_ptr->leadpt = l1pt;
        vars_ptr->subleadpt = l0pt;
      }
    }
    else
    {
      vars_ptr->run = run;
      vars_ptr->nbt = nbt;
      vars_ptr->nbt20 = nbt20;
      vars_ptr->mll = MVA_Mll/1000.0;
      vars_ptr->mtt = MVA_Mtt/1000.0;
      vars_ptr->l0pt = MVA_lepPt0/1000.0;
      vars_ptr->l1pt = MVA_lepPt1/1000.0;
      vars_ptr->mt = MVA_MT/1000.0;
      vars_ptr->metrel = MVA_METRel/1000.0;
      vars_ptr->trackmet = MVA_TrackMET/1000.0;
      vars_ptr->ptll = MVA_Ptll/1000.0;
      vars_ptr->dphi = MVA_DPhill;
      if(doSpin)
      {
        vars_ptr->bdphi = MVA_BDPhill;
        vars_ptr->bdpsi = MVA_BDPsill;
        vars_ptr->bpleadlep = MVA_BPLeadLep/1000.0;
        vars_ptr->bpsubleadlep = MVA_BPSubLeadLep/1000.0;
        //  vars_ptr->besubleadlep = MVA_BESubLeadLep/1000.0;
        vars_ptr->besubleadneu = MVA_BESubLeadNeu/1000.0;
        vars_ptr->esum=(MVA_BPLeadLep+MVA_BESubLeadNeu-0.5*MVA_BPSubLeadLep)/1000.0;
        vars_ptr->llcharge=lepID0*lepID1;
      }
      vars_ptr->Mjj = MVA_Mjj/1000.0;
      vars_ptr->etaProdJJ = MVA_jetEta0*MVA_jetEta1;
      vars_ptr->DeltaEtaJJ = fabs(MVA_jetEta0-MVA_jetEta1);
      vars_ptr->cjv_leadPt = MVA_cjv_leadPt/1000.;
    }

    if (nj > 2) nj = 2;

    bool top0jsfapplied = 0;
    bool zjetssfapplied = 0;
    bool lumiscaled = 0;
    bool DYcorrected2j = 0;

    for (int ireg=0;ireg<nrReg;ireg++)
    {
      Region* r = &(*regions)[ireg];
      int nrChans = r->channels.size();

      bool passedselection = 0;

      for (int ichan=0;ichan<nrChans;ichan++)
      {
        Channel* c = &r->channels[ichan];

        if (combineSFCRs && c->name == "SF")
        {
          if (nm == 2)
          {
            ne = nm;
            nm = 0;
          }
        }

        if (merge2jSFSR && nj == 2 && r->name.find("signalLike") != string::npos && nm == 2)
        {
          ne = nm;
          nm = 0;
        }

        if (c->ne != ne) continue;
        if (c->nm != nm) continue; // make channel selections
        if (c->nj != nj) continue;
        if ((splitem && (nj==0 || nj==1)) || (splitem && !merge2j && nj==2))
        {
          // LOG(logDEBUG) << c->name << " lep0pt = " << vars_ptr->l0pt << " lep1pt = " << vars_ptr->l1pt << " cut = " << me_el_pt_cut;
          if (!combineCRs || !r->isCR)
          {
            if (c->name == "em" && vars_ptr->l0pt < vars_ptr->l1pt) continue; // l0 = electron
            if (c->name == "me" && vars_ptr->l0pt >= vars_ptr->l1pt) continue; // l1 = muon
            if (c->name == "me" && vars_ptr->l0pt < CutPTmeel) continue;
          }
        }

        //if (sampleName.find("ggf") != string::npos || sampleName.find("vbf") != string::npos) vars_ptr->w *= 1.5;
        bool passed = true;
        int nrCuts = r->cuts.size();
        if (!passedselection)
        {
          for (int icut=0;icut<nrCuts;icut++) // apply pre-specified cuts
          {
            if (!(passed = passed && r->cuts[icut].passed(ne, nm, nj, sampleName))) break;
          }
        }

        if (!passed) continue;

        if (combineCRs && nj == 2 && ((merge2jtopCR && r->name.find("topbox") != string::npos) || (merge2jWWCR && r->name.find("mainControl") != string::npos)) && c->name == "OF" && ne == 1 && nm == 1)
        {
          ne = 2;
          nm = 0;
          passedselection = 1;

          if (sampleName == "zjets" || sampleName == "zleplep" || sampleName == "ztautau")
          {
            if (!DYcorrected2j)
            {
              // FIXME hard coded to get the NFs in the merged top CR right
              if (do2011)
              {
                // correct for ztautau
                // if (sampleName == "ztautau") vars_ptr->w /= 2.015076;
                // correct for abcd
                if (sampleName == "zleplep") vars_ptr->w /= 1.38054;
                DYcorrected2j = 1;
              }
              else if (do2012)
              {
                // correct for ztautau
                // if (sampleName == "ztautau") vars_ptr->w /= 1.195;
                // correct for abcd
                if (sampleName == "zleplep") vars_ptr->w /= 1.063974;
                DYcorrected2j = 1;
              }
            }
          }
        }

        if ( doMVA && (sampleName == "zjets" || sampleName == "zleplep" || sampleName == "ztautau") && ZMode == 0) // FIXME someone should check these numbers
        {
            if (ne == 2 && nj == 0) vars_ptr->w *= 1.003; // ok
            if (nm == 2 && nj == 0) vars_ptr->w *= 1.057; // ok
            if (ne == 2 && nj == 1) vars_ptr->w *= 1.052; // 1.060;
            if (nm == 2 && nj == 1) vars_ptr->w *= 0.919; // 0.916;
        }

        if (doblindsignalregion && (sampleName == "zjets" || sampleName == "zleplep" || sampleName == "ztautau") && ZMode == 2 && !doMVA && (ne==2 || nm==2) && !zjetssfapplied)
        {
          if (nj == 0) vars_ptr->w *= NFDY0j;
          else if (nj == 1) vars_ptr->w *= NFDY1j;
          zjetssfapplied = 1;
        }

      if (sampleName == "zjets" && doSpin && doMVA && !doZCR_MVA){
        vars_ptr->w *= 0.9;
      }

      if (sampleName == "ww" && doSpin && doMVA && !doWWCR_MVA){
        vars_ptr->w *= 1.1;
      }

        if ((sampleName == "ttbar" || sampleName == "st") && nj == 0) // top0j
        {
          if (r->name.find("lowPt") != string::npos)
          {
            vars_ptr->w *= NFTop0jLoPT;
          }
          else
          {
            if (!top0jsfapplied)
            {
              vars_ptr->w *= NFTop0jHiPT;
              top0jsfapplied = 1;
            }
          }
        }

        if (scaleLUMI && !lumiscaled)
        {
          vars_ptr->w *= lumiSF;
          lumiscaled = 1;
        }

        if (sampleName != "data" && doPreHCP && !lumiscaled)
        {
          vars_ptr->w /= 20693.7;
          vars_ptr->w *= 13232.9;
          lumiscaled = 1;
        }

        if (sampleName != "data" && doPostHCP && !lumiscaled)
        {
          vars_ptr->w /= 20693.7;
          vars_ptr->w *= 7460.8;
          lumiscaled = 1;
        }

        // ==============================
        // Mjj reweighting, not checked
        // if (doMjjReweight && doVBF2j && (sampleName == "ttbar" || sampleName == "st") && folder.find("Nominal/Normal") != string::npos)
        // {
        //   vars_ptr->w *= 1.009777*(1.02187/cosh((vars_ptr->Mjj*1000)*9.39032e-7-0.0201344));
        //   // vars_ptr->w *= 1;
        // }
        // else if (doMjjReweight && doVBF2j && (sampleName == "ttbar" || sampleName == "st") && folder.find("MjjReweight/Up") != string::npos)
        // {
        //   // Doug Schaefer fluctuate up
        //   // vars_ptr->w *= 0.9664*(1.03476/cosh((vars_ptr->Mjj*1000)*9.10388e-7)+3.65417e-9);
        //   // vars_ptr->w *= 1;
        //   // worst case scenario for "Up"
        //   vars_ptr->w *= 0.885978*(1.31365-1.29343e-6*vars_ptr->Mjj*1000 + 8.49182e-13*vars_ptr->Mjj*1000*vars_ptr->Mjj*1000);
        // }
        // else if (doMjjReweight && doVBF2j && (sampleName == "ttbar" || sampleName == "st") && folder.find("MjjReweight/Down") != string::npos)
        // {
        //   // Doug Schaefer fluctuate down
        //   vars_ptr->w *= 0.9978*(1.06911/cosh((vars_ptr->Mjj*1000)*1.54688e-6)+1.59258e-7); //bc first weight expression from Doug
        //   // vars_ptr->w *= 2.0-(0.9664*(1.03476/cosh((vars_ptr->Mjj*1000)*9.10388e-7)+3.65417e-9));
        // }

        // if (doFlatMjj && doVBF2j && (sampleName == "ttbar" || sampleName == "st") && folder.find("MjjFlat/Up") != string::npos)
        // {
        //   vars_ptr->w *= 1.3;
        // }
        // else if (doFlatMjj && doVBF2j && (sampleName == "ttbar" || sampleName == "st") && folder.find("MjjFlat/Down") != string::npos)
        // {
        //   vars_ptr->w *= 0.7;
        // }
        // ==============================

        if (sampleName == "wjets" || sampleName == "wjetsminusss") vars_ptr->w *= wjetsScale; //Nina Do we want to apply the same scaling to wjets and wjetsminusss? I'm guessing yes

        // if (sampleName.find("ggf") != string::npos || sampleName.find("vbf") != string::npos) vars_ptr->w *= 1.5;

        // spin mapping
        if (doSpin)
        {
          LOG(logDEBUG) << "Start spin mapping";
          LOG(logDEBUG) << "mva_weight = " << vars_ptr->mva_weight;
          LOG(logDEBUG) << "subtrain_mva_weight = " << vars_ptr->subtrain_mva_weight;
          // vars_ptr->output_var_spin = (tmp_hx->GetBinLowEdge((tmp_hx->FindBin(vars_ptr->mva_weight)))) + (1+(tmp_hy->GetBinCenter(tmp_hy->FindBin(vars_ptr->subtrain_mva_weight))))/nrBins0j_MVA_X;
          
          //FIXME: check!
          if(doDynamicBinning) {
            vars_ptr->output_var_spin = (bdthisto2D.get_bin( vars_ptr->mva_weight, vars_ptr->subtrain_mva_weight ) + 1) / 13.;         
          } else  {
            if(doCutBasedSpin) {
              vars_ptr->output_var_spin = (tmp_hx->GetBinLowEdge((tmp_hx->FindBin((2 * (vars_ptr->bdpsi) - 3.141596) / 3.141596)))) + (1 + tmp_hy->GetBinLowEdge(( tmp_hy->FindBin((2 * (vars_ptr->esum) - 100.) / 100. )))) / nrBins0j_MVA_X;
              // (1+(tmp_hy->GetBinCenter(tmp_hy->FindBin((2*(vars_ptr->esum)-100.)/100.))))     
            } else {
              vars_ptr->output_var_spin = (tmp_hx->GetBinLowEdge((tmp_hx->FindBin(vars_ptr->mva_weight)))) + (1 + (tmp_hy->GetBinCenter(tmp_hy->FindBin(vars_ptr->subtrain_mva_weight)))) / nrBins0j_MVA_X;
            }
          }            
          LOG(logDEBUG) << "After adding spin2 : " << vars_ptr->output_var_spin;
        }

        if (applySlope)
        {
          if (r->name.find("mainControl") != string::npos && (sampleName == "ww" || sampleName == "ggww" || sampleName == "qqww"))
          {
            vars_ptr->w *= (1.483 - 0.0024 * vars_ptr->mt);
          }
          else if (r->name.find("signalLike") != string::npos && (sampleName == "ww" || sampleName == "ggww" || sampleName == "qqww"))
          {
            vars_ptr->w *= (1.483 - srSlopeFactor * 0.0024 * vars_ptr->mt);
          }
        }

        /////////////////////////////// !!!!!! F**K'd up UEPS. !!!!!!
        if (applySlopeSpin) {
          if (!doSpin1D) {
            if (sampleName == "ww" && r->name.find("signalLike") != string::npos && folder.find("UEPS_SHAPE") != string::npos ) {
              if (!doCutBasedSpin) {
                double correlationFactor = 0.90275;
          
                double bdty = ( correlationFactor * vars_ptr->mva_weight + vars_ptr->subtrain_mva_weight) / (1 + correlationFactor) ;
                double bdtx = (-correlationFactor * vars_ptr->mva_weight + vars_ptr->subtrain_mva_weight) / (1 + correlationFactor) ;
          
                double delta_uepsy = 1 - (1.025 - 0.0371675 * bdty - 0.090 * bdty * bdty);
                double delta_uepsx = 1 - (1 + 0.040 * bdtx);
          
                if (folder.find( "McatnloShapeUp" ) != string::npos) {
                  vars_ptr->w *= 1 + (vars_ptr->mva_weight > -0.2 && vars_ptr->subtrain_mva_weight > -0.2) * ( delta_uepsy ) ;
                } else {
                  vars_ptr->w *= 1 - (vars_ptr->mva_weight > -0.2 && vars_ptr->subtrain_mva_weight > -0.2) * ( delta_uepsy ) ;
                } 
              } else {
                double delta_ueps = 1 - (0.95 + 0.0315 * vars_ptr->bdpsi);
                if ( folder.find( "McatnloShapeUp" ) != string::npos ) {
                  vars_ptr->w *= ( (1+delta_ueps) ) ;
                } else {
                  vars_ptr->w *= ( (1-delta_ueps) ) ;
                }
              } 
            }
          } else { // spin 1d
            if(sampleName == "ww" && r->name.find("signalLike") != string::npos && folder.find("UEPS_SHAPE") != string::npos ) {
        
            /*
              double bdty = vars_ptr->mva_weight ;
          
              double delta_uepsy = 1 - (1.025 - 0.0371675 * bdty - 0.090 * bdty * bdty);
          
              if ( folder.find( "McatnloShapeUp" ) != string::npos )
              {
                vars_ptr->w *= 1 + (vars_ptr->mva_weight > -0.2 ) * ( delta_uepsy ) ;
              }
              else
              {
                vars_ptr->w *= 1 - (vars_ptr->mva_weight > -0.2 ) * ( delta_uepsy ) ;
              }
            */
          
              double delta_ueps = 1 - (0.95 + 0.0315 * vars_ptr->bdpsi);
              if ( folder.find( "McatnloShapeUp" ) != string::npos ) {
                vars_ptr->w *= ( (1+delta_ueps) ) ;
              } 
              else {
                vars_ptr->w *= ( (1-delta_ueps) ) ;
              }
            }
          }
        } else { // new UEPS treatment
          double theta_angle;
          double bdtx, bdty;
          double delta_ueps_pdf, delta_ueps_ps, delta_ueps_scale;
          if(doSpin1minus) theta_angle= 0.708;  
          if(doSpin1plus) theta_angle = 0.735;
          if(doSpin2plus) theta_angle = 0.740;

          bdty = ( sin(theta_angle) * vars_ptr->mva_weight + cos(theta_angle) * vars_ptr->subtrain_mva_weight);
          bdtx = ( cos(theta_angle) * vars_ptr->mva_weight - sin(theta_angle) * vars_ptr->subtrain_mva_weight);

          if(sampleName == "ww" && r->name.find("signalLike") != string::npos && folder.find("UEPS_SHAPE_PDF") != string::npos ){
            // delta_ueps_pdf = 0.03 * bdtx + 0.03 * bdty; // flat 3%, test
            if(doSpin2plus)  delta_ueps_pdf = TMath::Abs(0.070*bdty - 0.00270*pow(bdty,2));
          
            if ( folder.find( "ShapeUp" ) != string::npos ) {
              if(doSpin2plus)  vars_ptr->w *= 1 + (vars_ptr->mva_weight > -0.2 && vars_ptr->subtrain_mva_weight > -0.2) * ( delta_ueps_pdf + 0.030 * bdty) ;
            } else {
              if(doSpin2plus)   vars_ptr->w *= 1 - (vars_ptr->mva_weight > -0.2 && vars_ptr->subtrain_mva_weight > -0.2) * ( delta_ueps_pdf + 0.030 * bdty) ; // i put another - so - * + = -
            }
          } // pdf
          else if(sampleName == "ww" && r->name.find("signalLike") != string::npos && folder.find("UEPS_SHAPE_PS") != string::npos ){           
            if(doSpin1minus) delta_ueps_ps = 0.063 * bdtx + 0.013 * bdty;
            if(doSpin1plus) delta_ueps_ps = 0.090 * bdtx + 0.010 * bdty;
            if(doSpin2plus) delta_ueps_ps = TMath::Abs(0.0190 * bdtx - 0.030 * bdty);
            // if(doSpin2plus) delta_ueps_ps = 0.0384 * bdtx + 0.007 * bdty;

            if ( folder.find( "ShapeUp" ) != string::npos ){
              vars_ptr->w *= 1 + (vars_ptr->mva_weight > -0.2 && vars_ptr->subtrain_mva_weight > -0.2) * ( delta_ueps_ps) ;
            } else {
              vars_ptr->w *= 1 - (vars_ptr->mva_weight > -0.2 && vars_ptr->subtrain_mva_weight > -0.2) * ( delta_ueps_ps ) ;
            }
          } //ps
          else if(sampleName == "ww" && r->name.find("signalLike") != string::npos && folder.find("UEPS_SHAPE_SCALE") != string::npos ){
            if(doSpin1minus) delta_ueps_scale =  TMath::Abs( TMath::Max( 0.0086 - 0.028*bdtx + 0.014*pow(bdtx,2), 0.0003 - 0.022*bdtx + 0.050*pow(bdtx,2) ) ) +TMath::Abs( TMath::Max( 0.013 - 0.020*bdty + 0.0031*pow(bdty,2) + 0.036*pow(bdty,3),0.017 - 0.014*bdty - 0.0096*pow(bdty,2) + 0.019*pow(bdty,3) ) );
            if(doSpin1plus) delta_ueps_scale =  TMath::Abs( TMath::Max( 0.0049 - 0.033*bdtx + 0.0048*pow(bdtx,2), 0.0001 - 0.013*bdtx + 0.050*pow(bdtx,2) ) ) +TMath::Abs( TMath::Max( 0.013 - 0.029*bdty + 0.0025*pow(bdty,2) + 0.043*pow(bdty,3),0.016 - 0.009*bdty - 0.0092*pow(bdty,2) + 0.014*pow(bdty,3) ) );
            // if(doSpin2plus) delta_ueps_scale =  TMath::Abs( TMath::Max( 0.0076 - 0.035*bdtx + 0.0073*pow(bdtx,2), 0.0057 - 0.011*bdtx + 0.050*pow(bdtx,2) ) ) +TMath::Abs( TMath::Max( 0.010 - 0.020*bdty + 0.0069*pow(bdty,2) + 0.030*pow(bdty,3),0.016 - 0.019*bdty - 0.0096*pow(bdty,2) + 0.025*pow(bdty,3) ) );
            if(doSpin2plus) delta_ueps_scale =  TMath::Abs(  -0.0025 - 0.015*bdtx + 0.050*pow(bdtx,2) ) +TMath::Abs(  0.0160 - 0.0170*bdty - 0.035*pow(bdty,2) + 0.010*pow(bdty,3) );

            if ( folder.find( "ShapeUp" ) != string::npos ) {
              vars_ptr->w *= 1 + (vars_ptr->mva_weight > -0.2 && vars_ptr->subtrain_mva_weight > -0.2)*( delta_ueps_scale);
            } else {
              vars_ptr->w *= 1 - (vars_ptr->mva_weight > -0.2 && vars_ptr->subtrain_mva_weight > -0.2)*( delta_ueps_scale );
            }
          } //scale
        }//the end 

        // load rates into this unnecessarily nested c++ object
        c->sys_sample_rates[folder][sampleName].insert(make_pair(*output_var, vars_ptr->w));
        r->all_rates.insert(*output_var);
        if (doSpin && doMVA)
        {
          LOG(logDEBUG) << "output_var_spin = " << vars_ptr->output_var_spin;
        }
      }
    }
  }
  LOG(logDEBUG) << "->Done with loop";

  delete tmp_hx;
  delete tmp_hy;

  // apply Ztautau NF
  if (sampleName == "ztautau" && splitzjets)
  {
    map<string, double> ztautau_norms;

    // read the txt file
    stringstream fileName;
    fileName << "config"+string(do2012?"_2012/":"_2011/")+"ztautau_norms"+string(mode==8?"_3D":"");
    fileName << ".txt";

    ifstream ztautau_norms_file(fileName.str().c_str());
    if (ztautau_norms_file.bad() || ztautau_norms_file.fail())
    {
      LOG(logERROR) << "ERROR::Couldn't open ztautau norms file: " << fileName.str();
      exit(1);
    }

    while (!ztautau_norms_file.eof())
    {
      string term;
      ztautau_norms_file >> term;
      if (ztautau_norms_file.eof()) break;
      double val;
      ztautau_norms_file >> val;

      ztautau_norms[term] = val;
    }
    ztautau_norms_file.close();

    for (int ireg=0;ireg<nrReg;ireg++)
    {
      Region* r = &(*regions)[ireg];
      int nrChans = r->channels.size();

      for (int ichan=0;ichan<nrChans;ichan++)
      {
        Channel* c = &r->channels[ichan];

        double norm = 0;
        multiset<pair<double, double> >* rates = &c->sys_sample_rates[folder][sampleName];
        for (multiset<pair<double, double> >::iterator itr=rates->begin();itr!=rates->end();itr++)
        {
          norm += itr->second;
        }

        string channelName = c->name+"_"+r->name+"_"+c->jetName;

        if (ztautau_norms.find(channelName) == ztautau_norms.end())
        {
          LOG(logWARNING) << "Didn't find data-driven normalization for channel " << channelName;
          continue;
        }

        double dd_norm = ztautau_norms[channelName];

	cout<<sampleName<<endl;
	cout<<channelName<<endl;
	cout<<"ztautau norm: "<<dd_norm<<endl;

        if (norm < 0)
        {
          LOG(logERROR) << "Scaling Z to be more negative!";
          exit(1);
        }

        if (norm == 0)
        {
          if (dd_norm > 0)
          {
            LOG(logWARNING) << "Data-driven normalization non-zero, but no shape info available for channel " << channelName;
            // exit(1);
          }

          continue;
        }

        double scaleF = dd_norm;///norm;

        multiset<pair<double, double> > new_rates;
        for (multiset<pair<double, double> >::iterator itr=rates->begin();itr!=rates->end();itr++)
        {
          new_rates.insert(make_pair(itr->first, itr->second*scaleF));
        }

        *rates = new_rates;

      }
    }
  }

  // rescale Z to the data-driven rate
  if ((sampleName == "zjets" || sampleName == "zleplep" || sampleName == "ztautau") && (ZMode == 1 || (ZMode == 2 && (doABCD2j || doPacman2j))))
  {
    map<string, double> zjets_norms;
    if (ZMode == 1 && !doPacman2j)
    {
      // read in highpt norms
      stringstream fileName;
      fileName << "config"+string(do2012?"_2012/":"_2011/")+"zjets_norms";
      if (mass <= massBoundary) fileName << "_lowm";
      else fileName << "_highm";
      fileName << ".txt";
  
      ifstream zjets_norms_file(fileName.str().c_str());
      if (zjets_norms_file.bad() || zjets_norms_file.fail())
      {
        LOG(logERROR) << "Couldn't open zjets norms file: " << fileName.str();
        exit(1);
      }
  
      while (!zjets_norms_file.eof())
      {
        string term;
        zjets_norms_file >> term;
        if (zjets_norms_file.eof()) break;
        double val;
        zjets_norms_file >> val;
  
        zjets_norms[term] = val;
      }
      zjets_norms_file.close();
  
      // read in lowpt norms
      stringstream fileName_lopt;
      fileName_lopt << "config"+string(do2012?"_2012":"_2011")+"/"+"zjets_lowpt_norms.txt";
  
      ifstream zjets_norms_file_lopt(fileName_lopt.str().c_str());
      if (zjets_norms_file_lopt.bad() || zjets_norms_file_lopt.fail())
      {
        LOG(logERROR) << "ERROR::Couldn't open zjets norms file: " << fileName_lopt.str();
        exit(1);
      }
  
      while (!zjets_norms_file_lopt.eof())
      {
        string term;
        zjets_norms_file_lopt >> term;
        if (zjets_norms_file_lopt.eof()) break;
        double val;
        zjets_norms_file_lopt >> val;
  
        zjets_norms[term] = val;
      }
      zjets_norms_file_lopt.close();
    }

    if(doABCD2j && !doPacman2j && (sampleName == "zjets" || sampleName == "zleplep"))
    {
      stringstream fileName_2j;
      fileName_2j << "config"+string(do2012?"_2012":"_2011")+"/"+"zjets_abcd_norms_2j.txt";

      ifstream zjets_norms_file_2j(fileName_2j.str().c_str());
      if (zjets_norms_file_2j.bad() || zjets_norms_file_2j.fail())
      {
        LOG(logERROR) << "ERROR::Couldn't open zjets norms file: " << fileName_2j.str();
        exit(1);
      }

      while (!zjets_norms_file_2j.eof())
      {
        string term;
        zjets_norms_file_2j >> term;
        if (zjets_norms_file_2j.eof()) break;
        double val;
        zjets_norms_file_2j >> val;

        zjets_norms[term] = val;
      }
      zjets_norms_file_2j.close();
    }
    else if(!doABCD2j && doPacman2j && (sampleName == "zjets" || sampleName == "zleplep"))
    {
      stringstream fileName_2j;
      fileName_2j << "config"+string(do2012?"_2012":"_2011")+"/"+"zjets_pacman_norms_2j.txt";

      ifstream zjets_norms_file_2j(fileName_2j.str().c_str());
      if (zjets_norms_file_2j.bad() || zjets_norms_file_2j.fail())
      {
        LOG(logERROR) << "ERROR::Couldn't open zjets norms file: " << fileName_2j.str();
        exit(1);
      }

      while (!zjets_norms_file_2j.eof())
      {
        string term;
        zjets_norms_file_2j >> term;
        if (zjets_norms_file_2j.eof()) break;
        double val;
        zjets_norms_file_2j >> val;

        zjets_norms[term] = val;
      }
      zjets_norms_file_2j.close();
    }
    //this zjets NF file is out-of-date
    /*
    else if(!doABCD2j && !doPacman2j && (sampleName == "zjets" || sampleName == "zleplep" || sampleName == "ztautau"))
    {
      stringstream fileName_2j;
      fileName_2j << "config"+string(do2012?"_2012":"_2011")+"/"+"zjets_norms_2j.txt";

      ifstream zjets_norms_file_2j(fileName_2j.str().c_str());
      if (zjets_norms_file_2j.bad() || zjets_norms_file_2j.fail())
      {
        LOG(logERROR) << "ERROR::Couldn't open zjets norms file: " << fileName_2j.str();
        exit(1);
      }

      while (!zjets_norms_file_2j.eof())
      {
        string term;
        zjets_norms_file_2j >> term;
        if (zjets_norms_file_2j.eof()) break;
        double val;
        zjets_norms_file_2j >> val;

        zjets_norms[term] = val;
      }
      zjets_norms_file_2j.close();
    }
    */
    for (int ireg=0;ireg<nrReg;ireg++)
    {
      Region* r = &(*regions)[ireg];
      int nrChans = r->channels.size();

      for (int ichan=0;ichan<nrChans;ichan++)
      {
        Channel* c = &r->channels[ichan];

        double norm = 0;
        multiset<pair<double, double> >* rates = &c->sys_sample_rates[folder][sampleName];
        for (multiset<pair<double, double> >::iterator itr=rates->begin();itr!=rates->end();itr++)
        {
          norm += itr->second;
        }

        string channelName = c->name+"_"+r->name+"_"+c->jetName;

        if (zjets_norms.find(channelName) == zjets_norms.end())
        {
          LOG(logWARNING) << "Didn't find data-driven normalization for channel " << channelName;
          continue;
        }

        double dd_norm = zjets_norms[channelName];
        
        cout<<sampleName<<endl;
	cout<<channelName<<endl;
        cout<<"zleplep norm: "<<dd_norm<<endl;

        if (norm < 0)
        {
          LOG(logERROR) << "ERROR::Scaling Z to be more negative!";
          exit(1);
        }

        if (norm == 0)
        {
          if (dd_norm > 0)
          {
            LOG(logWARNING) << "Data-driven normalization non-zero, but no shape info available for channel " << channelName;
            // exit(1);
          }

          continue;
        }

        double scaleF = dd_norm;///norm;

        multiset<pair<double, double> > new_rates;
        for (multiset<pair<double, double> >::iterator itr=rates->begin();itr!=rates->end();itr++)
        {
          new_rates.insert(make_pair(itr->first, itr->second*scaleF));
        }

        *rates = new_rates;

      }
    }
  }

  file->Close();

}


//Returns true and fills map_reg_boundaries with values existing in the relevant boundaries file, returns false if this file does not exist
bool getBoundariesFromFile(std::string filename, map<string, set<double> > & map_reg_boundaries) {
  //Opening the file and checking it exists
  ifstream bdFile;
  bdFile.open (filename.c_str(), ifstream::in);
  if (!bdFile.is_open() || !bdFile.good())
    return false;

  //Reading boundaries for each region (i.e. line)
  std::string line;
  while (!bdFile.eof()) {
    std::getline (bdFile,line);
    istringstream iss(line);
    std::string first;
    iss >> first;
    set<double> second;
    double bound;
    while (iss) {
      iss >> bound;
      second.insert(bound);
    }
    map_reg_boundaries[first] = second;
  }

  //Closing the file properly
  bdFile.close();

  return true;
}


#endif
