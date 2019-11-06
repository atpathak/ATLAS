// Author: Aaron Armbruster
// Date:   2011-11-16
//
// Description:
// Write XML, run histfactory, make asimov data, and edit and write workspace to file

#include "TFile.h"

#include "RooPoisson.h"

#include "macros/makeAsimovData.C"
#include "macros/setup.C"
#include "macros/printRand.C"
#include "RooStats/HistFactory/FlexibleInterpVar.h"
#include "RooStats/HistFactory/PiecewiseInterpolation.h"
#include "RooStats/HistFactory/Measurement.h"
#include "RooStats/HistFactory/MakeModelAndMeasurementsFast.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <stdlib.h>
#include <sstream>

using namespace std;
using namespace RooFit;
using namespace RooStats;
// using namespace HistFactory;

struct WSInfo
{
  string name;
  vector<string> regionNames;
};

void joinSystematics(vector<vector<Response> >& vecResponse, vector<Response>& joined);
void writeXML2(string outFileName, double mass = 130, string version = "test", bool alt = false);
void writeXML(double mass = 130, string version = "test", bool alt = false);

void writeXML(double mass, string version, bool alt)
{
  setup(mass, alt);

  if (useLowPt && !useHighPt && (useHighMass || useHighMass2)) return;

  stringstream massStr;
  massStr << mass;
  string smass = massStr.str();

  stringstream altStr;
  if (alt) altStr << "_alt";
  string salt = altStr.str();

  // Write out infos for different regions, settings, periods, ...
  vector<Region>* all_regions = new vector<Region>;
  *all_regions = *regions;

  vector<WSInfo> infos;

  // Signal
  WSInfo srs;
  srs.name="signalRegions";
  if (dosignalregion)
  {
    if (mode < 6 && !(mode == 1 && splitmjj))
    {
      srs.regionNames.push_back("signalLike");
    }
    else if (mode == 6 || (mode == 1 && splitmjj)) 
    {
      srs.regionNames.push_back("signalLike1");
      srs.regionNames.push_back("signalLike2");
      if (doSpin) srs.regionNames.push_back("signalLike3");
    }
    else if (mode == 7)
    {
      for (int i=0;i<8;i++)
      {
        stringstream regionName;
        regionName << "signalLike" << i;
        srs.regionNames.push_back(regionName.str());
      }
    }
    else if (mode == 8)
    {
      // parse boundary strings
      vector<string> parsedsubleadptbounds = parseString(subleadptbounds, ",");
      vector<string> parsedmllbounds = parseString(mllbounds, ",");

      for (int i_mll = 0; i_mll < parsedmllbounds.size()-1; i_mll++) {
        for (int i_subleadpt = 0; i_subleadpt < parsedsubleadptbounds.size()-1; i_subleadpt++) {

          stringstream regionName;
          regionName << "signalLike" << parsedmllbounds[i_mll] << parsedmllbounds[i_mll+1] << parsedsubleadptbounds[i_subleadpt] << parsedsubleadptbounds[i_subleadpt+1];
     
          srs.regionNames.push_back(regionName.str());
        }
      }
    }
    if((ZMode == 2 || doPacman2j) && (doee || domm))
    {
      srs.regionNames.push_back("ASR");
      srs.regionNames.push_back("AfrecSR");
    }
    if (useHighPt) infos.push_back(srs);
  }
  
  // Low pT signal
  WSInfo srs_lowpt;
  srs_lowpt.name="lowPtRegions";
  if (dosignalregion)
  {
    srs_lowpt.regionNames.push_back("lowPt");
    if (useLowPt) infos.push_back(srs_lowpt);
  }

  // Control regions
  WSInfo crs;
  WSInfo zcrs;
  crs.name="controlRegions";
  zcrs.name="PacmanControlRegions";
  if (!doMVA)
  {
    if (!(massMode == 3 && overrideCuts) && !useHighMass2 && mode != 7) crs.regionNames.push_back("mainControl");
  }
  else if (doWWCR_MVA)
  {
    crs.regionNames.push_back("mainControl");
  }

  if (!doSpin)
  {
    if(splitmjjtopCR){
      crs.regionNames.push_back("topbox1");
      crs.regionNames.push_back("topbox2");
    }
    else crs.regionNames.push_back("topbox");
    LOG(logDEBUG) << "Pushed back topbox";
  }
  if (doSpin)
  {
    if( doZCR_MVA) crs.regionNames.push_back("zbox");
  }
  if ((ZMode == 2 || doPacman2j) && (doee || domm))
    {
      zcrs.regionNames.push_back("CZpeak");
      zcrs.regionNames.push_back("CfrecZpeak");
      if (!(massMode == 3 && overrideCuts) && !useHighMass2)zcrs.regionNames.push_back("EWWCR");
      if (!(massMode == 3 && overrideCuts) && !useHighMass2)zcrs.regionNames.push_back("EfrecWWCR");
    }
  
  LOG(logDEBUG) << "Configured regions.";
  
  if (!doMVA)
  { 
    LOG(logDEBUG) << "do0j = " << do0j;
    LOG(logDEBUG) << "do1j = " << do1j;
    if (do1j || do2j)
    {
      infos.push_back(crs);
      LOG(logDEBUG) << "Pushed back CRs";
    }
    else if (!useHighMass2)
    {
      infos.push_back(crs);
    }
    infos.push_back(zcrs);
  }
  else if (doMVA && (do0j && doWWCR_MVA))
  { //for 0j there is no top CR, so if push_back CR only if doWWCR==true
    infos.push_back(crs);
  }
  else if (doMVA && (do1j || do2j))
  { //for 1j or 2j there is top CR so push_back CR
    infos.push_back(crs);
  }

  int nrInfos = infos.size();

  RooArgSet obs;
  RooArgSet globs;
  RooArgSet nuis;
  RooArgSet poi;
  map<string, RooAbsPdf*> pdf_map;
  map<string, RooDataSet*> data_map;
  RooCategory* merged_cat = new RooCategory("merged_cat","merged_cat");

  // Write individual workspaces
  bool master_do0j = do0j;
  bool master_do1j = do1j;
  bool master_do2j = do2j;
  for (int ii=0;ii<nrInfos;ii++)
  {
    // this is the only solution i could think of to put in variable bin numbers for different jet multiplicities
    do0j = master_do0j;
    do1j = master_do1j;
    do2j = master_do2j;

    int nrLoop = 3;
    //********** if (infos[ii].name != "signalRegions") nrLoop = 1;
    if (infos[ii].name == "lowPtRegions")
    {
      do2j = 0;
      if (!do0j && !do1j) continue;
    }
    if (infos[ii].name == "controlRegions")
    {
      if (do0j && !do1j && !do2j && useHighMass2) continue;
    }
    for (int jj = 0; jj < nrLoop; jj++)
    {
      //********** if (infos[ii].name == "signalRegions")
      if (infos[ii].name == "signalRegions" || infos[ii].name == "signalRegions2" || infos[ii].name == "controlRegions" || infos[ii].name == "PacmanControlRegions")
      {
        if (jj == 0)
        {
          if (!master_do0j) continue;
          if (infos[ii].name == "controlRegions" && mode == 7) continue;
          do0j = 1;
          do1j = 0;
          do2j = 0;
        }
        else if (jj == 1)
        {
          if (!master_do1j) continue;
          do0j=0;
          do1j=1;
          do2j=0;
        }
        else if (jj == 2)
        {
          if (!master_do2j) continue;
          do0j = 0;
          do1j = 0;
          do2j = 1;
        }
      }
      regions->clear();

      vector<string> regionNames = infos[ii].regionNames;

      int nrRegions = regionNames.size();
      for (int ir = 0; ir < nrRegions; ir++)
      {
        bool found = false;
        for (int ir_a = 0; ir_a < (int)all_regions->size(); ir_a++)
        {
          Region* r_a = &(*all_regions)[ir_a];
          if (r_a->name == regionNames[ir])
          {
            regions->push_back(*r_a);
            found = true;
            break;
          }
        }
        if (!found)
        {
          LOG(logERROR) << "Couldn't find region " << regionNames[ir];
          exit(1);
        }
      }

      stringstream subfolder;
      subfolder << infos[ii].name;
      //********** if (infos[ii].name == "signalRegions")
      if (infos[ii].name == "signalRegions" || infos[ii].name == "signalRegions2" || infos[ii].name == "controlRegions" || infos[ii].name == "PacmanControlRegions")
      {
        subfolder << "_" << jj << "j";
      }

      LOG(logDEBUG) << "subfolder = " << subfolder.str();
      LOG(logDEBUG) << "doee = " << doee;
      LOG(logDEBUG) << "domm = " << domm;
      LOG(logDEBUG) << "doem = " << doem;
      LOG(logDEBUG) << "dome = " << dome;
      
      if ((useHighMass2) && jj==0 && infos[ii].name == "controlRegions") continue;
      if (doSFonly && (doem || dome) && jj==0 && infos[ii].name == "controlRegions") continue;
      if (infos[ii].name == "PacmanControlRegions" && ((!domm || !doee) || (!doPacman2j && jj==2))) continue;

      LOG(logDEBUG) << "Passed, subfolder is still " << subfolder.str();

      // Call writeXML2
      writeXML2(subfolder.str(), mass, version, alt);

      RooFormulaVar* epsilon_spin2   = NULL;
      RooRealVar* epsilon_spin0      = NULL;

      RooRealVar* f_recoil_NDY       = NULL;
      RooFormulaVar* r_recoil_NDY    = NULL;
      RooRealVar* f_recoil_NDY_WW    = NULL;
      RooFormulaVar* r_recoil_NDY_WW = NULL;
      RooRealVar* f_recoil_NDY_SR    = NULL;
      RooFormulaVar* r_recoil_NDY_SR = NULL;
      RooRealVar* f_recoil_NDY_ZP    = NULL;
      RooFormulaVar* r_recoil_NDY_ZP = NULL;
      RooRealVar* f_recoil_DY        = NULL;
      RooFormulaVar* r_recoil_DY     = NULL;
      RooFormulaVar* f_recoil_NDY_S  = NULL;
      RooFormulaVar* r_recoil_NDY_S  = NULL;
      RooRealVar *f                  = NULL;
      RooRealVar *epsilon0           = NULL;
      RooRealVar *theta_SR           = NULL;

      //grab the pdf, data, and sets
      TFile* file      = new TFile(("rev/"+version+"/xml/"+smass+salt+"/"+subfolder.str()+"/output_combined_lvlv_model.root").c_str());
      RooWorkspace* w  = (RooWorkspace*)file->Get("combined");
      ModelConfig* mc  = (ModelConfig*)w->obj("ModelConfig");
      RooDataSet* data = (RooDataSet*)w->data("obsData");
      obs.add(*mc->GetObservables());
      globs.add(*mc->GetGlobalObservables());
      nuis.add(*mc->GetNuisanceParameters());
      if (!useLumiAsPOI)
      {
        // Use mu as nuisance parameter and spin0/spin2 fraction as poi
        if (doSpin)
        {
          LOG(logWARNING) << "Replacing POI for spin";
          RooRealVar* mu = (RooRealVar*)mc->GetParametersOfInterest()->first();
          mu->setVal(1);
          mu->setConstant(0);
          nuis.add(*mu);
          poi.remove(*mu);

          RooRealVar* epsilon = w->var("ATLAS_epsilon");
          epsilon->setRange(0, 1);
          poi.add(*w->var("ATLAS_epsilon"));
          nuis.remove(*w->var("ATLAS_epsilon"));
          nuis.remove(*w->var("ATLAS_epsilon_rejected"));
        }
        else if (doCRVRcheck)
        {
          LOG(logWARNING) << "Replacing POI for CR/VR test";
          RooRealVar* mu = (RooRealVar*)mc->GetParametersOfInterest()->first();
          mu->setVal(1);
          mu->setConstant(1);
          nuis.add(*mu);
          poi.remove(*mu);
          
          if (w->var("CRVR_HWW"))
          {
            RooRealVar* alpha = w->var("CRVR_HWW");
            alpha->setVal(1);
            alpha->setConstant(0);
            nuis.remove(*alpha);
            poi.add(*alpha);
          }
          else
          {
            LOG(logWARNING) << "CRVR_HWW does not exist";
          }
        } 
        else
        {
          stringstream ggfMass;
          ggfMass << 125;
          string fixmass = ggfMass.str();
          poi.add(*mc->GetParametersOfInterest());
          //if (doggf && !doVBF2j) poi.add(*w->var(("ATLAS_sampleNorm_ggf"+smass).c_str()));
          //if (doggf && doVBF2j) poi.add(*w->var(("ATLAS_sampleNorm_ggf125"+fixmass).c_str()));
          if (doggf) poi.add(*w->var(("ATLAS_sampleNorm_ggf"+smass).c_str()));
          if (dovbf) poi.add(*w->var(("ATLAS_sampleNorm_vbf"+smass).c_str()));
          if (dowh)  poi.add(*w->var(("ATLAS_sampleNorm_wh"+smass).c_str()));
          if (dozh)  poi.add(*w->var(("ATLAS_sampleNorm_zh"+smass).c_str()));
        }
      }
      else
      {
        RooRealVar* mu = (RooRealVar*)mc->GetParametersOfInterest()->first();
        mu->setVal(1);
        mu->setConstant(1);

        RooRealVar* lumi = w->var("Lumi");
        lumi->setRange(0.1, 20);
        poi.add(*w->var("Lumi"));
      }

      RooSimultaneous* simPdf = (RooSimultaneous*)mc->GetPdf();
      RooCategory* cat = (RooCategory*)&simPdf->indexCat();
      RooCatType* tt = NULL;
      TIterator* itr = cat->typeIterator();

      // Make replacements for spin measurement
      bool hasSpin2_fraction = false;
      if (!epsilon_spin0 && w->var("ATLAS_epsilon_rejected"))
      {
        hasSpin2_fraction = true;
        epsilon_spin0 = w->var("ATLAS_epsilon");
        RooArgList formSet(*epsilon_spin0);
        epsilon_spin2 = new RooFormulaVar("ATLAS_epsilon_rejected_form","ATLAS_epsilon_rejected_form","1-@0",formSet);
        w->import(*epsilon_spin2);
      }

      // Make replacements for Pacman
      if (jj == 0)
      {
        f        = new RooRealVar("f","f",ratio_S_NDY_SR_0j);
        epsilon0 = new RooRealVar("epsilon0","epsilon0",epsilon0_data_0j);
      } else if (jj == 1)
      {
        f        = new RooRealVar("f","f",ratio_S_NDY_SR_1j);
        epsilon0 = new RooRealVar("epsilon0","epsilon0",epsilon0_data_1j);
      } else if (jj == 2)
      {
	f        = new RooRealVar("f","f",ratio_S_NDY_SR_2j);
	epsilon0 = new RooRealVar("epsilon0","epsilon0",epsilon0_data_2j);
      }

      string s_jj;
      stringstream sstream_jj;
      sstream_jj << jj;
      s_jj = sstream_jj.str();

      // NDY efficiency accross all regions
      bool hasRecoil_rejected_NDY = false;
      if (!f_recoil_NDY && w->var(("PM_EFF_f_recoil_NDY"+s_jj+"j").c_str()))
      {
        hasRecoil_rejected_NDY = true;
        f_recoil_NDY = w->var(("PM_EFF_f_recoil_NDY"+s_jj+"j").c_str());
        RooArgList formSet(*f_recoil_NDY);
        r_recoil_NDY = new RooFormulaVar(("PM_EFF_f_recoil_rejected_NDY"+s_jj+"j_form").c_str(),("PM_EFF_f_recoil_rejected_NDY"+s_jj+"j_form").c_str(),"1-@0",formSet);
        w->import(*r_recoil_NDY);
        nuis.add(*w->var(("PM_EFF_f_recoil_NDY"+s_jj+"j").c_str()));
      }
      // NDY efficiency in the SR
      bool hasRecoil_rejected_NDY_SR = false;
      if (!f_recoil_NDY_SR && w->var(("PM_EFF_f_recoil_NDY_SR"+s_jj+"j").c_str()))
      {
        hasRecoil_rejected_NDY_SR = true;
        f_recoil_NDY_SR = w->var(("PM_EFF_f_recoil_NDY_SR"+s_jj+"j").c_str());
        RooArgList formSet(*f_recoil_NDY_SR);
        r_recoil_NDY_SR = new RooFormulaVar(("PM_EFF_f_recoil_rejected_NDY_SR"+s_jj+"j_form").c_str(),("PM_EFF_f_recoil_rejected_NDY_SR"+s_jj+"j_form").c_str(),"1-@0",formSet);
        w->import(*r_recoil_NDY_SR);
        nuis.add(*w->var(("PM_EFF_f_recoil_NDY_SR"+s_jj+"j").c_str()));
      }

      // NDY efficiency in the Z peak
      bool hasRecoil_rejected_NDY_ZP = false;
      if (!f_recoil_NDY_ZP && w->var(("PM_EFF_f_recoil_NDY_ZP"+s_jj+"j").c_str()))
      {
        hasRecoil_rejected_NDY_ZP = true;
        f_recoil_NDY_ZP = w->var(("PM_EFF_f_recoil_NDY_ZP"+s_jj+"j").c_str());
        RooArgList formSet(*f_recoil_NDY_ZP);
        r_recoil_NDY_ZP = new RooFormulaVar(("PM_EFF_f_recoil_rejected_NDY_ZP"+s_jj+"j_form").c_str(),("PM_EFF_f_recoil_rejected_NDY_ZP"+s_jj+"j_form").c_str(),"1-@0",formSet);
        w->import(*r_recoil_NDY_ZP);
        nuis.add(*w->var(("PM_EFF_f_recoil_NDY_ZP"+s_jj+"j").c_str()));
      }

      // NDY efficiency in the WW CR
      bool hasRecoil_rejected_NDY_WW = false;
      if (!f_recoil_NDY_WW && w->var(("PM_EFF_f_recoil_NDY_WW"+s_jj+"j").c_str()))
      {
        hasRecoil_rejected_NDY_WW = true;
        f_recoil_NDY_WW = w->var(("PM_EFF_f_recoil_NDY_WW"+s_jj+"j").c_str());
        RooArgList formSet(*f_recoil_NDY_WW);
        r_recoil_NDY_WW = new RooFormulaVar(("PM_EFF_f_recoil_rejected_NDY_WW"+s_jj+"j_form").c_str(),("PM_EFF_f_recoil_rejected_NDY_WW"+s_jj+"j_form").c_str(),"1-@0",formSet);
        w->import(*r_recoil_NDY_WW);
        nuis.add(*w->var(("PM_EFF_f_recoil_NDY_WW"+s_jj+"j").c_str()));
      }

      // DY efficiency accross all regions
      bool hasRecoil_rejected_DY = false;
      if (!f_recoil_DY && w->var(("PM_EFF_f_recoil_DY"+s_jj+"j").c_str()))
      {
        hasRecoil_rejected_DY = true;
        f_recoil_DY = w->var(("PM_EFF_f_recoil_DY"+s_jj+"j").c_str());
        RooArgList formSet(*f_recoil_DY);
        r_recoil_DY = new RooFormulaVar(("PM_EFF_f_recoil_rejected_DY"+s_jj+"j_form").c_str(),("PM_EFF_f_recoil_rejected_DY"+s_jj+"j_form").c_str(),"1-@0",formSet);
        w->import(*r_recoil_DY);
        nuis.add(*w->var(("PM_EFF_f_recoil_DY"+s_jj+"j").c_str()));
      }

      // NDY efficiency for the signal
      bool hasSignalEfficiency = false;
      if (!f_recoil_NDY_S && w->var(("PM_EFF_f_recoil_NDY_S"+s_jj+"j").c_str()))
      {
        LOG(logDEBUG) << "Pacman signal efficiency replacement";
        hasSignalEfficiency = true;
        if (!splitEfficiencies) f_recoil_NDY = w->var(("PM_EFF_f_recoil_NDY"+s_jj+"j").c_str());
        else f_recoil_NDY = w->var(("PM_EFF_f_recoil_NDY_SR"+s_jj+"j").c_str());
        if (w->var(("alpha_PM_theta_SR"+s_jj+"j").c_str()))
        {
          theta_SR = w->var(("alpha_PM_theta_SR"+s_jj+"j").c_str());
        } else
        {
          LOG(logDEBUG) << "DEBUG::" << subfolder;
          LOG(logERROR) << "ERROR::PM_theta_SR not in workspace";
          exit(1);
        }
        RooArgList formSet2(*f_recoil_NDY,*f,*epsilon0,*theta_SR);
        f_recoil_NDY_S = new RooFormulaVar(("PM_EFF_f_recoil_NDY_S"+s_jj+"j_form").c_str(),("PM_EFF_f_recoil_NDY_S"+s_jj+"j_form").c_str(),"(1-((1-@1)*((@0*(1-@0))/(@2*(1-@2)))))*pow((1+((@0*(1-@0))/(@2*(1-@2)))*0.1),@3)*@0",formSet2);
        w->import(*f_recoil_NDY_S);
        if (!splitEfficiencies) nuis.add(*w->var(("PM_EFF_f_recoil_NDY"+s_jj+"j").c_str()));
        else                    nuis.add(*w->var(("PM_EFF_f_recoil_NDY_SR"+s_jj+"j").c_str()));
        nuis.add(*w->var(("alpha_PM_theta_SR"+s_jj+"j").c_str()));
      }


      bool hasSignalEfficiency_rejected = false;
      if (!r_recoil_NDY_S && w->var(("PM_EFF_f_recoil_NDY_S"+s_jj+"j").c_str()))
      {
        LOG(logDEBUG) << "Pacman signal rejection replacement";
        hasSignalEfficiency_rejected = true;
        if (!splitEfficiencies) f_recoil_NDY = w->var(("PM_EFF_f_recoil_NDY"+s_jj+"j").c_str());
        else f_recoil_NDY = w->var(("PM_EFF_f_recoil_NDY_SR"+s_jj+"j").c_str());
        if (w->var(("alpha_PM_theta_SR"+s_jj+"j").c_str()))
        {
          theta_SR = w->var(("alpha_PM_theta_SR"+s_jj+"j").c_str());
        } else
        {
          LOG(logDEBUG) << "" << subfolder;
          LOG(logERROR) << "PM_theta_SR not in workspace";
          exit(1);
        }
        RooArgList formSet(*f_recoil_NDY,*f,*epsilon0,*theta_SR);
        r_recoil_NDY_S = new RooFormulaVar(("PM_EFF_f_recoil_rejected_NDY_S"+s_jj+"j_form").c_str(),("PM_EFF_f_recoil_rejected_NDY_S"+s_jj+"j_form").c_str(),"1-((1-((1-@1)*((@0*(1-@0))/(@2*(1-@2)))))*pow((1+((@0*(1-@0))/(@2*(1-@2)))*0.1),@3)*@0)",formSet);
        w->import(*r_recoil_NDY_S);
        if (!splitEfficiencies) nuis.add(*w->var(("PM_EFF_f_recoil_NDY"+s_jj+"j").c_str()));
        else                    nuis.add(*w->var(("PM_EFF_f_recoil_NDY_SR"+s_jj+"j").c_str()));
        nuis.add(*w->var(("alpha_PM_theta_SR"+s_jj+"j").c_str()));
      }

      TList* data_list = doData ? data->split(*cat) : NULL;
      int nrData = doData ? data_list->GetEntries() : 0;

      if (hasSignalEfficiency)
      {
        LOG(logDEBUG) << "hasSignalEfficiency is true";
      }
      if (hasRecoil_rejected_NDY_WW)
      {
        LOG(logDEBUG) << "hasRecoil_rejected_NDY_WW is true";
      }
      if (hasRecoil_rejected_NDY_ZP)
      {
        LOG(logDEBUG) << "hasRecoil_rejected_NDY_ZP is true";
      }
      if (hasRecoil_rejected_NDY_SR)
      {
        LOG(logDEBUG) << "hasRecoil_rejected_NDY_SR is true";
      }

      obs.remove(*cat);
      while ((tt = (RooCatType*)itr->Next()))
      {
        merged_cat->defineType(tt->GetName());
        RooAbsPdf* pdf = simPdf->getPdf(tt->GetName());

        LOG(logDEBUG) << "tt is " << tt->GetName();
        
        if (hasSignalEfficiency)
        {
          LOG(logDEBUG) << "tt::hasSignalEfficiency is true";
        }
        if (hasRecoil_rejected_NDY_WW)
        {
          LOG(logDEBUG) << "tt::hasRecoil_rejected_NDY_WW is true";
        }
        if (hasRecoil_rejected_NDY_ZP)
        {
          LOG(logDEBUG) << "tt::hasRecoil_rejected_NDY_ZP is true";
        }
        if (hasRecoil_rejected_NDY_SR)
        {
          LOG(logDEBUG) << "tt::hasRecoil_rejected_NDY_SR is true";
        }

        // Do the actual workspace editing
        if (hasSpin2_fraction || hasRecoil_rejected_NDY || hasRecoil_rejected_NDY_WW || hasRecoil_rejected_NDY_ZP || hasRecoil_rejected_NDY_SR || hasRecoil_rejected_DY || hasSignalEfficiency || hasSignalEfficiency_rejected)
        {
          stringstream editStr;
          editStr << "EDIT::" << pdf->GetName() << "_edit(" << pdf->GetName();
          if (hasSpin2_fraction            && pdf->dependsOn(*w->var("ATLAS_epsilon_rejected")))                         editStr << ",ATLAS_epsilon_rejected=ATLAS_epsilon_rejected_form";
          if (hasRecoil_rejected_NDY       && pdf->dependsOn(*w->var(("PM_EFF_f_recoil_rejected_NDY"+s_jj+"j").c_str())))    editStr << (",PM_EFF_f_recoil_rejected_NDY"+s_jj+"j=PM_EFF_f_recoil_rejected_NDY"+s_jj+"j_form").c_str();
          if (hasRecoil_rejected_NDY_WW    && pdf->dependsOn(*w->var(("PM_EFF_f_recoil_rejected_NDY_WW"+s_jj+"j").c_str()))) editStr << (",PM_EFF_f_recoil_rejected_NDY_WW"+s_jj+"j=PM_EFF_f_recoil_rejected_NDY_WW"+s_jj+"j_form").c_str();
          if (hasRecoil_rejected_NDY_ZP    && pdf->dependsOn(*w->var(("PM_EFF_f_recoil_rejected_NDY_ZP"+s_jj+"j").c_str()))) editStr << (",PM_EFF_f_recoil_rejected_NDY_ZP"+s_jj+"j=PM_EFF_f_recoil_rejected_NDY_ZP"+s_jj+"j_form").c_str();
          if (hasRecoil_rejected_NDY_SR    && pdf->dependsOn(*w->var(("PM_EFF_f_recoil_rejected_NDY_SR"+s_jj+"j").c_str()))) editStr << (",PM_EFF_f_recoil_rejected_NDY_SR"+s_jj+"j=PM_EFF_f_recoil_rejected_NDY_SR"+s_jj+"j_form").c_str();
          if (hasRecoil_rejected_DY        && pdf->dependsOn(*w->var(("PM_EFF_f_recoil_rejected_DY"+s_jj+"j").c_str())))     editStr << (",PM_EFF_f_recoil_rejected_DY"+s_jj+"j=PM_EFF_f_recoil_rejected_DY"+s_jj+"j_form").c_str();
          if (hasSignalEfficiency          && pdf->dependsOn(*w->var(("PM_EFF_f_recoil_NDY_S"+s_jj+"j").c_str())))           editStr << (",PM_EFF_f_recoil_NDY_S"+s_jj+"j=PM_EFF_f_recoil_NDY_S"+s_jj+"j_form").c_str();
          if (hasSignalEfficiency_rejected && pdf->dependsOn(*w->var(("PM_EFF_f_recoil_rejected_NDY_S"+s_jj+"j").c_str())))  editStr << (",PM_EFF_f_recoil_rejected_NDY_S"+s_jj+"j=PM_EFF_f_recoil_rejected_NDY_S"+s_jj+"j_form").c_str();
          editStr << ")";
          LOG(logINFO) << "editStr is " << editStr.str().c_str();
          if (editStr.str().find("=") != string::npos)
          {
            w->factory(editStr.str().c_str());
            pdf = w->pdf((string(pdf->GetName())+"_edit").c_str());
            if (!pdf)
            {
              LOG(logERROR) << "Something went wrong in variable edit";
              LOG(logERROR) << "editStr was " << editStr.str().c_str();
              exit(1);
            }
          }
        }

        pdf_map[tt->GetName()] = pdf;

        bool found = false;
        for (int id = 0; id < nrData; id++)
        {
          RooDataSet* thisData = (RooDataSet*)data_list->At(id);
          if (string(thisData->GetName()).find(string(tt->GetName())) != string::npos)
          {
            data_map[tt->GetName()] = thisData;
            found = true;
            break;
          }
        }
        if (!found && doData)
        {
          LOG(logERROR) << "Couldn't find data for cat " << tt->GetName();
          exit(1);
        }
      }
      epsilon_spin2   = NULL;
      epsilon_spin0   = NULL;
      f_recoil_NDY    = NULL;
      r_recoil_NDY    = NULL;
      f_recoil_NDY_WW = NULL;
      r_recoil_NDY_WW = NULL;
      f_recoil_NDY_SR = NULL;
      r_recoil_NDY_SR = NULL;
      f_recoil_NDY_ZP = NULL;
      r_recoil_NDY_ZP = NULL;
      f_recoil_DY     = NULL;
      r_recoil_DY     = NULL;
      f_recoil_NDY_S  = NULL;
      r_recoil_NDY_S  = NULL;
      f               = NULL;
      epsilon0        = NULL;
      theta_SR        = NULL;
    }
  }
  RooRealVar* weightVar = (RooRealVar*)obs.find("weightVar");
  obs.add(*merged_cat);
  if (!weightVar)
  {
    LOG(logERROR) << "Couldn't find weightVar in any of the input files.";
    exit(1);
  }

  // Merge models
  LOG(logINFO) << "Making pdf";
  RooSimultaneous* merged_pdf = new RooSimultaneous("mergedPdf","mergedPdf", pdf_map, *merged_cat);

  LOG(logINFO) << "Making data";
  RooDataSet* merged_data = doData ? new RooDataSet("obsData","obsData",obs,Index(*merged_cat), Import(data_map), WeightVar(*weightVar)) : NULL;

  // Get rid of constant parameters
  vector<string> constParams;
  constParams.push_back("Z_scaleF_ee0j");
  constParams.push_back("Z_scaleF_mm0j");
  constParams.push_back("Z_scaleF_em0j");
  constParams.push_back("Z_scaleF_ee1j");
  constParams.push_back("Z_scaleF_mm1j");
  constParams.push_back("Z_scaleF_ee2j");
  constParams.push_back("Z_scaleF_mm2j");
  constParams.push_back("Top_scaleF_0j");
  constParams.push_back("Lumi");
  int nrSamples = samples->size();
  for (int i = 0; i < nrSamples; i++)
  {
    constParams.push_back("ATLAS_sampleNorm_"+(*samples)[i].name);
  }
  constParams.push_back("mu_BR_WW");
  constParams.push_back("PM_EFF_f_recoil_rejected_DY0j");
  constParams.push_back("PM_EFF_f_recoil_rejected_DY1j");
  if (doPacman2j) constParams.push_back("PM_EFF_f_recoil_rejected_DY2j");
  if (!splitEfficiencies)
  {
    constParams.push_back("PM_EFF_f_recoil_rejected_NDY0j");
    constParams.push_back("PM_EFF_f_recoil_rejected_NDY1j");
    if(doPacman2j) constParams.push_back("PM_EFF_f_recoil_rejected_NDY2j");
  }
  else
  {
    constParams.push_back("PM_EFF_f_recoil_rejected_NDY_SR0j");
    constParams.push_back("PM_EFF_f_recoil_rejected_NDY_ZP0j");
    constParams.push_back("PM_EFF_f_recoil_rejected_NDY_WW0j");
    constParams.push_back("PM_EFF_f_recoil_rejected_NDY_SR1j");
    constParams.push_back("PM_EFF_f_recoil_rejected_NDY_ZP1j");
    constParams.push_back("PM_EFF_f_recoil_rejected_NDY_WW1j");
    if (doPacman2j){
      constParams.push_back("PM_EFF_f_recoil_rejected_NDY_SR2j");
      constParams.push_back("PM_EFF_f_recoil_rejected_NDY_ZP2j");
      constParams.push_back("PM_EFF_f_recoil_rejected_NDY_WW2j");
    }
  }
  constParams.push_back("PM_EFF_f_recoil_NDY_S0j");
  constParams.push_back("PM_EFF_f_recoil_rejected_NDY_S0j");
  constParams.push_back("PM_EFF_f_recoil_NDY_S1j");
  constParams.push_back("PM_EFF_f_recoil_rejected_NDY_S1j");
  if (doPacman2j){
    constParams.push_back("PM_EFF_f_recoil_NDY_S2j");
    constParams.push_back("PM_EFF_f_recoil_rejected_NDY_S2j");
  }
  // constParams.push_back("ATLAS_epsilon_rejected");
  // constParams.push_back("ATLAS_epsilon");
  // constParams.push_back("ATLAS_norm_WW0j");
  // constParams.push_back("ATLAS_norm_WW1j");
  // constParams.push_back("ATLAS_norm_Top1j");
  // constParams.push_back(("alpha_PM_theta_SR"+s_jj+"j").c_str());
  int nrConst = constParams.size();

  LOG(logWARNING) << "Removing constant nuisance parameters.";
  for (int i=0;i<nrConst;i++)
  {
    RooRealVar* param = (RooRealVar*)nuis.find(constParams[i].c_str());
    if (param)
    {
      param->setConstant(1);
      LOG(logWARNING) << "Setting param to const: " << constParams[i];

      nuis.remove(*param);
    }
  }

  globs.remove(*globs.find("nominalLumi"));
  nuis.sort();
  globs.sort();

  LOG(logINFO) << "Building workspaces and model config";
  RooWorkspace merged_ws("combined");
  ModelConfig mc("ModelConfig",&merged_ws);
  mc.SetPdf(*merged_pdf);
  mc.SetNuisanceParameters(nuis);
  mc.SetGlobalObservables(globs);
  mc.SetObservables(obs);
  mc.SetParametersOfInterest(poi);

  merged_ws.import(mc);
  if (doData) merged_ws.import(*merged_data);

  // For some reason these aren't being set to const in the above lines
  for (int i = 0; i < nrConst; i++)
  {
    RooRealVar* param = (RooRealVar*)merged_ws.var(constParams[i].c_str());
    if (param)
    {
      param->setConstant(1);
      LOG(logWARNING) << "Setting param to const again: " << constParams[i];
    }
  }

  ModelConfig* mcInWs = (ModelConfig*)merged_ws.obj("ModelConfig");

  // Bugfix for w->factory noRounding not working in histfactory
  TIterator* nItr = mcInWs->GetNuisanceParameters()->createIterator();
  RooRealVar* var;
  while ((var = (RooRealVar*)nItr->Next()))
  {
    string poisName = string(var->GetName()) + "_constraint";
    if (poisName.find("gamma_stat") == string::npos) continue;
    RooPoisson* pois = (RooPoisson*)merged_ws.pdf(poisName.c_str());
    if (!pois)
    {
      LOG(logERROR) << "ERROR::Couldn't find corresponding poisson for var " << var->GetName();
      exit(1);
    }
    pois->setNoRounding(true);
  }

  // Make asimov data
  RooArgSet funcs = merged_ws.allFunctions();
  TIterator* it = funcs.createIterator();
  TObject* tempObj = 0;
  while((tempObj=it->Next()))
  {
    HistFactory::FlexibleInterpVar* flex = dynamic_cast<HistFactory::FlexibleInterpVar*>(tempObj);
    if(flex)
    {
      flex->setAllInterpCodes(flatInterpCode);
    }
    PiecewiseInterpolation* piece = dynamic_cast<PiecewiseInterpolation*>(tempObj);
    if(piece)
    {
      piece->setAllInterpCodes(shapeInterpCode);
    }
  }

  LOG(logINFO) << "Making asimov data";
  RooDataSet* dataInWs = (RooDataSet*)merged_ws.data("obsData");
  makeAsimovData(mcInWs, conditionalAsimov && doData, &merged_ws, mcInWs->GetPdf(), dataInWs, 0);
  makeAsimovData(mcInWs, conditionalAsimov && doData, &merged_ws, mcInWs->GetPdf(), dataInWs, 1);
  makeAsimovData(mcInWs, conditionalAsimov && doData, &merged_ws, mcInWs->GetPdf(), dataInWs, 2);

  // merged_ws.Print();

  LOG(logINFO) << "Writing to file";
  system(("mkdir -vp workspaces/"+version).c_str());
  merged_ws.writeToFile(("workspaces/"+version+"/"+smass+salt+".root").c_str(), true); // write

  merged_ws.Print("t");

  printRand();
}

// writeXML2
void writeXML2(string subfolder, double mass, string version, bool alt)
{
  LOG(logINFO) << "Writing XML: " << mass << " " << version << " alt?" << alt;

  setup(mass, alt);

  stringstream massStr;
  massStr << mass;
  string smass = massStr.str();

  stringstream altStr;
  if (alt) altStr << "_alt";
  string salt = altStr.str();

  // if (version.find("Pacman") != string::npos && (!doee || !domm)) return;

  if (rewriteXML)
  {
    if (!overriden) // baseline config
    {
      useDetSys    = 1;
      useShape     = 1 && useDetSys;
      useThSys     = 1;
      useStatSys   = 1;
      useTheoryWW  = 0;
      useTheoryTop = 0;
      scaleZ       = 1;
      scaleTop     = 1;
    }

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
    int nrSamples = samples->size();
    int nrSys = fileNames->size();

    // load shape variations
    vector<Response> detR;
    if (useShape)
    {
      for (int isys = 0; isys < nrSys; isys++)
      {
        Sys* s = &(*fileNames)[isys];
        if (s->folder == "Nominal") continue;
        if (!s->isShape) continue;

        for (int isam = 0; isam < nrSamples; isam++)
        {
          // if ((*samples)[isam].name == "wjets") continue;

          if (s->sampleNames.find((*samples)[isam].name) == s->sampleNames.end()) continue;

          string hiFileName = "rev/"+version+"/normHists/" + s->folder + "/" + s->fileUp   + "/" + (*samples)[isam].name + ".root";
          string loFileName = "rev/"+version+"/normHists/" + s->folder + "/" + s->fileDown + "/" + (*samples)[isam].name + ".root";
          for (int ij = 0; ij < nrJets; ij++)
          {
            for (int ic = 0; ic < nrChannels; ic++)
            {
              for (int ir = 0; ir < nrRegions; ir++)
              {
                if (s->veto.find((*samples)[isam].name+"_"+channelNames[ic]+"_"+(*regions)[ir].name+"_"+jetNames[ij]) != s->veto.end()) continue;

                string name = s->folder;

                Response res;
                res.name = name;
                res.channel = channelNames[ic];
                res.jet = jetNames[ij];
                res.sample = (*samples)[isam].name;
                res.region = (*regions)[ir].name;
                res.hiFileName = hiFileName;
                res.loFileName = loFileName;
                res.hiHistName = channelNames[ic]+"_"+(*regions)[ir].name+"_"+jetNames[ij];
                res.loHistName = channelNames[ic]+"_"+(*regions)[ir].name+"_"+jetNames[ij];
                detR.push_back(res);
              }
            }
          }
        }
      }
    }

    // Load flat systematics
    vector<Response> normR;
    if (useDetSys)
    {
      readInUncerts("rev/"+version+"/normHists/norms_"+smass+salt+".txt", normR, mass, 0, 0, bool(ZMode!=0?1:0));
    }

    // Load flat systematics for interpolated signal (was done at later stage)
    bool found = false;

    int nrBasePoints = 43;
    double* baseMassPoints = new double[nrBasePoints];
    double thisMass = 90;
    double step = 5;
    for (int i=0;i<nrBasePoints;i++)
    {
      baseMassPoints[i] = thisMass;
      if (baseMassPoints[i] == mass)
      {
        found = true;
        break;
      }
      if (thisMass >= 200) step = 20;

      thisMass += step;
    }

    if (!found) readInUncerts("rev/"+version+"/normHists/norms_"+smass+salt+"_sig.txt", normR, mass);

    // Load theory uncertainties
    vector<Response> thR;
    if (useThSys)
    {
      if (mode < 7) {
        if (!doWWCR2j) {
          readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"theory_constraints_other.txt", thR, mass, 0, 0, bool(ZMode!=0?1:0));
        } else if (doWWCR2j) {
          readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"theory_constraints_other_noww.txt", thR, mass, 0, 0, bool(ZMode!=0?1:0));
        }
      } else if (mode == 8) {
        readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"theory_constraints_other_3D.txt", thR, mass, 0, 0, bool(ZMode!=0?1:0));
      }

      if (mode == 0 && !doABCD2j) // FIXME: do we still need SF method?
      {
        readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"zjets_2j_cuts.txt", thR, mass, 0, 0);
      }
      else if(!doABCD2j)
      {
        readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"zjets_2j.txt", thR, mass, 0, 0);
      }

      if (useTheoryWW) readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"theory_constraints_ww.txt", thR, mass, 0, 0); // FIXME
      else if (mode != 7) 
      {
        if (useAltCRs) // FIXME: default CRs now, get rid of old definition
        {
          if (!doCRVRcheck)
          {
            if (skipSFWWCR) {
              if (mode == 8) readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"alpha_uncerts_ww_alt_3D_skipSF.txt", thR, mass, 0, 0);
              else readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"alpha_uncerts_ww_alt_skipSF.txt", thR, mass, 0, 0);
            }
            else {
              readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"alpha_uncerts_ww_alt.txt", thR, mass, 0, 0);
            }
          }
          else readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"alpha_uncerts_ww_alt_CRVR.txt", thR, mass, 0, 0);
        }
        else
        {
        //else readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"alpha_uncerts_ww.txt", thR, mass, 0, 0);
        if(!doSpin) 
    	  {
    	    readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"alpha_uncerts_ww.txt", thR, mass, 0, 0);
    	  }
    	  else 
    	  {
    	    readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"alpha_uncerts_ww_spin.txt", thR, mass, 0, 0);        
    	  }
        }
      }
      else if (mode == 7)
      {
        readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"mll_ratios.txt", thR, mass, 0, 0);
      }

      if (useTheoryTop) readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"theory_constraints_top.txt", thR, mass, 0, 0);
      else
      {
        // make sure that 2j numbers in both files
        if (skipSFtopCR) {
          if (mode == 8) readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"alpha_uncerts_top_3D_skipSF.txt", thR, mass, 0, 0);
          else readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"alpha_uncerts_top_skipSF.txt", thR, mass, 0, 0);
        }
        else readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"alpha_uncerts_top.txt", thR, mass, 0, 0);
      }
    }

    // Load signal cross section uncertainties
    stringstream xsFileName;
    xsFileName << "config"+string(do2012?"_2012/":"_2011/")+"xs_files/xs_" << mass << ".txt";
    if (useThSys) readInUncerts(xsFileName.str(), thR, mass);

    // Load signal branching ratio uncertainties
    stringstream brFileName;
    brFileName << "config"+string(do2012?"_2012/":"_2011/")+"br_files/br_" << mass << ".txt";
    if (useThSys) readInUncerts(brFileName.str(), thR, mass);

    // Load top0j and zjets uncerts
    if (ZMode == 2 && (doee || domm))
    {
      readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"zjets_pacman.txt", normR, mass);
    }

    if (doPacman2j && (doee || domm))
    {
      readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"zjets_pacman2j.txt", normR, mass);
    }

    if (splitzjets && useDetSys)
    {
      if(doPacman2j)
      { 
        readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"ztautau_uncerts.txt", normR, mass);
        readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"zjets_uncerts_2j.txt", normR, mass);
      }
      else if(doABCD2j)
      {
        readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"ztautau_uncerts_withabcd.txt", normR, mass); 
        readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"zjets_abcd_uncerts_2j.txt", normR, mass);
      }
      else{
	readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"ztautau_uncerts.txt", normR, mass);
	readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"zjets_uncerts_2j.txt", normR, mass);
      }
      //if(doABCD2j && do2012) readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"zjets_abcd_uncerts_2j.txt", normR, mass);
      //else if(!doABCD2j && do2012) readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"zjets_uncerts_2j.txt", normR, mass);
    }

    if (useDetSys)
    {
      if (mode != 8) readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"top0j_uncerts.txt", normR, mass);
      else if (mode == 8) readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"top0j_uncerts_3D.txt", normR, mass);
      if (ZMode == 1)
      {
        if (useHighPt)
      {
          stringstream fileName;
          fileName << "config"+string(do2012?"_2012/":"_2011/")+"zjets_abcdef";
          if (!useHighMass && !useHighMass2) fileName << "_lowm";
          else fileName << "_highm";
          fileName << ".txt";
          readInUncerts(fileName.str(), normR, mass);
        }

        if (useLowPt)
        {
          stringstream fileName;
          fileName << "config"+string(do2012?"_2012/":"_2011/")+"zjets_lowpt_abcdef.txt";

          readInUncerts(fileName.str(), normR, mass);
        }
      }
      else if (ZMode == 0)
      {
        readInUncerts("config"+string(do2012?"_2012/":"_2011/")+"zjets_scalefs.txt", normR, mass);
      }
    }

    // Merge flat systemaitcs into one vector
    vector<vector<Response> > vecFlatSys;
    vecFlatSys.push_back(normR);
    // vecFlatSys.push_back(WjR); // now read from norms.txt
    vecFlatSys.push_back(thR);

    vector<Response> flatSys;
    joinSystematics(vecFlatSys, flatSys);

    int nrDetR = detR.size();
    int nrFlat = flatSys.size();

    // Prepare directory structures
    system(("mkdir -vp rev/"+version+"/xml/"+smass+salt+"/"+subfolder).c_str());
    // system(("cp config"+string(do2012?"_2012/":"_2011/")+"HistFactorySchema.dtd rev/"+version+"/xml/"+smass+salt+"/"+subfolder).c_str());
    
    RooStats::HistFactory::Measurement meas("lvlv", "lvlv");
    meas.SetOutputFilePrefix("./rev/"+version+"/xml/"+smass+salt+"/"+subfolder+"/output");
    meas.SetExportOnly(1);
    meas.SetPOI("SigXsecOverSM_HWW");
    meas.SetLumi(1.0);
    meas.SetLumiRelErr(0.037);

    // Main loop to make XML for each channel
    for (int ichan = 0; ichan < nrChannels; ichan++)
    {
      for (int ijet = 0; ijet < nrJets; ijet++)
      {
        for (int ireg = 0; ireg < nrRegions; ireg++)
        {
          Region* reg = &(*regions)[ireg];


          bool skipLoop = skipRegion(reg, channelNames[ichan], jetNames[ijet]);
          if (skipLoop) continue;
          
          if (!doWWCR_MVA && reg->name.find("mainControl") != string::npos && ((jetNames[ijet] == "2j" && !doWWCR2j) || (doMVA == 1 && jetNames[ijet] == "0j"))) continue;

          string channelName = channelNames[ichan]+"_"+(*regions)[ireg].name+"_"+jetNames[ijet];
          string channelName_plusyear = channelName+(do2012?"_2012":"");
          string folder = "rev/"+version+"/normHists/Nominal/Normal/";

          RooStats::HistFactory::Channel chan(channelName_plusyear);

          string constraintType;
          if (statMode == 0)
          {
            constraintType = "Poisson";
          }
          else if (statMode == 1)
          {
            constraintType = "Guassian";
          }
          else
          {
            LOG(logERROR) << "Undefined stat mode: " << statMode;
            exit(1);
          }
          if (useStatSys) chan.SetStatErrorConfig(stat_cutoff, constraintType);            

          if (doData) chan.SetData(channelName, folder+"data.root");

          for (int isam = 0; isam < nrSamples; isam++)
          {

            if (jetNames[ijet] != "2j" && ((*samples)[isam] == "wwew" || (*samples)[isam] == "wzzzew" || (*samples)[isam] == "zjetsew" || (*samples)[isam] == "zleplepew" || (*samples)[isam] == "ztautauew")) continue;

            if ((*samples)[isam].type == "signal" || ((*samples)[isam].name.find("ggf") != string::npos && doVBF2j))
            {
              if ((channelNames[ichan] == "em" && (*regions)[ireg].name.find("AfrecSR")  != string::npos) ||
                (channelNames[ichan] == "em" && (*regions)[ireg].name.find("ASR")        != string::npos) ||
                (channelNames[ichan] == "em" && (*regions)[ireg].name.find("EfrecWWCR")  != string::npos) ||
                (channelNames[ichan] == "em" && (*regions)[ireg].name.find("EWWCR")      != string::npos) ||
                (channelNames[ichan] == "em" && (*regions)[ireg].name.find("CfrecZpeak") != string::npos) ||
                (channelNames[ichan] == "em" && (*regions)[ireg].name.find("CZpeak")     != string::npos) ||
                (channelNames[ichan] == "me" && (*regions)[ireg].name.find("AfrecSR")    != string::npos) ||
                (channelNames[ichan] == "me" && (*regions)[ireg].name.find("ASR")        != string::npos) ||
                (channelNames[ichan] == "me" && (*regions)[ireg].name.find("EfrecWWCR")  != string::npos) ||
                (channelNames[ichan] == "me" && (*regions)[ireg].name.find("EWWCR")      != string::npos) ||
                (channelNames[ichan] == "me" && (*regions)[ireg].name.find("CfrecZpeak") != string::npos) ||
                (channelNames[ichan] == "me" && (*regions)[ireg].name.find("CZpeak")     != string::npos) ||
                (channelNames[ichan] == "OF" && (*regions)[ireg].name.find("AfrecSR")    != string::npos) ||
                (channelNames[ichan] == "OF" && (*regions)[ireg].name.find("ASR")        != string::npos) ||
                (channelNames[ichan] == "OF" && (*regions)[ireg].name.find("EfrecWWCR")  != string::npos) ||
                (channelNames[ichan] == "OF" && (*regions)[ireg].name.find("EWWCR")      != string::npos) ||
                (channelNames[ichan] == "OF" && (*regions)[ireg].name.find("CfrecZpeak") != string::npos) ||
                (channelNames[ichan] == "OF" && (*regions)[ireg].name.find("CZpeak")     != string::npos))
              {
                continue;
              }
            }

            string normByTheory = "True"; // apply lumi?
            if (((!useTheoryWW && ((*samples)[isam] == "ww" || (*samples)[isam] == "ggww" || (*samples)[isam] == "qqww") && (jetNames[ijet] == "1j" || jetNames[ijet] == "0j")) ||
                 (!useTheoryTop && ((*samples)[isam] == "ttbar" || (*samples)[isam] == "st") && (jetNames[ijet] == "1j" || jetNames[ijet] == "2j")) ||
                 ((*samples)[isam] == "wjets" || (*samples)[isam] == "wjetsminusss" || (*samples)[isam] == "ss" ) || //Nina
                 ((ZMode != 0 || doPacman2j) && ((*samples)[isam] == "zjets" || (*samples)[isam] == "zleplep" || (*samples)[isam] == "ztautau")) ||
                 (((*samples)[isam] == "ttbar" || (*samples)[isam] == "st") && (jetNames[ijet] == "0j")))) normByTheory = "False";
            if(doABCD2j && ((*samples)[isam] == "zleplep" || (*samples)[isam] == "ztautau" || (*samples)[isam] == "zjets") && (*regions)[ireg].name.find("signalLike") != string::npos && jetNames[ijet] == "2j") normByTheory = "False";

            // Use normByTheory when applying lumi sys later. here keep hf's lumi for possible scale tests

            RooStats::HistFactory::Sample sample((*samples)[isam].name, channelName, folder+(*samples)[isam].name+".root");

            if (!(channelNames[ichan] == "em" && (*regions)[ireg].name.find("AfrecSR")    != string::npos) &&
                !(channelNames[ichan] == "em" && (*regions)[ireg].name.find("ASR")        != string::npos) &&
                !(channelNames[ichan] == "em" && (*regions)[ireg].name.find("EfrecWWCR")  != string::npos) &&
                !(channelNames[ichan] == "em" && (*regions)[ireg].name.find("EWWCR")      != string::npos) &&
                !(channelNames[ichan] == "em" && (*regions)[ireg].name.find("CfrecZpeak") != string::npos) &&
                !(channelNames[ichan] == "em" && (*regions)[ireg].name.find("CZpeak")     != string::npos) &&
                !(channelNames[ichan] == "me" && (*regions)[ireg].name.find("AfrecSR")    != string::npos) &&
                !(channelNames[ichan] == "me" && (*regions)[ireg].name.find("ASR")        != string::npos) &&
                !(channelNames[ichan] == "me" && (*regions)[ireg].name.find("EfrecWWCR")  != string::npos) &&
                !(channelNames[ichan] == "me" && (*regions)[ireg].name.find("EWWCR")      != string::npos) &&
                !(channelNames[ichan] == "me" && (*regions)[ireg].name.find("CfrecZpeak") != string::npos) &&
                !(channelNames[ichan] == "me" && (*regions)[ireg].name.find("CZpeak")     != string::npos) &&
                !(channelNames[ichan] == "OF" && (*regions)[ireg].name.find("AfrecSR")    != string::npos) &&
                !(channelNames[ichan] == "OF" && (*regions)[ireg].name.find("ASR")        != string::npos) &&
                !(channelNames[ichan] == "OF" && (*regions)[ireg].name.find("EfrecWWCR")  != string::npos) &&
                !(channelNames[ichan] == "OF" && (*regions)[ireg].name.find("EWWCR")      != string::npos) &&
                !(channelNames[ichan] == "OF" && (*regions)[ireg].name.find("CfrecZpeak") != string::npos) &&
                !(channelNames[ichan] == "OF" && (*regions)[ireg].name.find("CZpeak")     != string::npos))
            {
              for (int ir = 0; ir < nrDetR; ir++) // write shape systematics
              {
                Response* r = &detR[ir];
                if (r->match(channelNames[ichan], jetNames[ijet], (*samples)[isam].name, (*regions)[ireg].name))
                {
                  sample.AddHistoSys(r->name, r->loHistName, r->loFileName, "", r->hiHistName, r->hiFileName, "");
                }
              }

              for (int ir = 0; ir < nrFlat; ir++) // write flat systematics
              {
                Response* r = &flatSys[ir];
                if (r->match(channelNames[ichan], jetNames[ijet], (*samples)[isam].name, (*regions)[ireg].name))
                {
                  sample.AddOverallSys(r->name, 1 + r->lo, 1 + r->hi);
                }
              }

              if (useDetSys && normByTheory == "True")
              {
                double lumi_val = 0.018;
                if (do2012) lumi_val = 0.036; // <- new
                sample.AddOverallSys("LUMI"+string(do2012?"_2012":"_2011"), 1./(1 + lumi_val), 1 + lumi_val);
              }

              if ((*samples)[isam].type == "signal")
              {
                sample.AddNormFactor("SigXsecOverSM_HWW", 1.0, 0.0, 50, 1);
              }
              else if (include125BG && (*samples)[isam].name.find("125bg") != string::npos)
              {
                sample.AddNormFactor("SigXsecOverSM_125_HWW", 1.0, 0.0, 50);
              }

              if (!useTheoryWW && ((*samples)[isam] == "ww" || (*samples)[isam] == "ggww" || (*samples)[isam] == "qqww" || (*samples)[isam] == "wwew") && (jetNames[ijet] != "2j" || (doWWCR2j && jetNames[ijet] == "2j")))
              {
                if(doWWCR_MVA || (!doSpin)) sample.AddNormFactor("ATLAS_norm_WW"+jetNames[ijet], 1.0, 0.0, 10.0);
              }

              if (!useTheoryTop && ((*samples)[isam] == "ttbar" || (*samples)[isam] == "st") && jetNames[ijet] != "0j")
              {
		if(splitmjjtopCR && !((*regions)[ireg].name.find("signalLike1") != string::npos) && !((*regions)[ireg].name.find("topbox1") != string::npos)){
                  sample.AddNormFactor("ATLAS_norm_Top2"+jetNames[ijet], 1.0, 0.0, 10.0);
		}
                else if(splitmjjtopCR && !((*regions)[ireg].name.find("signalLike2") != string::npos) && !((*regions)[ireg].name.find("topbox2") != string::npos)){
		  sample.AddNormFactor("ATLAS_norm_Top1"+jetNames[ijet], 1.0, 0.0, 10.0);
		}
		else sample.AddNormFactor("ATLAS_norm_Top"+jetNames[ijet], 1.0, 0.0, 10.0);
              }

              if (profileggf && doVBF2j && (*samples)[isam] == ("ggf"+smass).c_str())
              {
                sample.AddNormFactor("ATLAS_norm_ggf", 1.0, 0.0, 10.0);
              }

              if (doCRVRcheck && (*regions)[ireg].name.find("signalLike") != string::npos && ((*samples)[isam] == "ww" || (*samples)[isam] == "ggww" || (*samples)[isam] == "qqww"))
              {
                sample.AddNormFactor("CRVR_HWW", 1.0, 0.0, 50, 1);
              }

              // Spin0/Spin2 fraction epsilon
              if (doSpin)
              {
                if (((*samples)[isam].name.find("spin0p") != string::npos && useJHUspin0) || ((*samples)[isam].name.find("ggf") != string::npos && !useJHUspin0))
                {
                  sample.AddNormFactor("ATLAS_epsilon", 0.5, -5.0, 5.0, 1);
                }
                else if ((*samples)[isam].name.find("spin2p") != string::npos || (*samples)[isam].name.find("spin1p") != string::npos || (*samples)[isam].name.find("spin1m") != string::npos || (*samples)[isam].name.find("spin0m") != string::npos)
                {
                  sample.AddNormFactor("ATLAS_epsilon_rejected", 0.5, -5.0, 5.0, 1);
                }
              }

              if (doSpin && doMVA && ((*samples)[isam] == "zjets" || (*samples)[isam] == "zleplep" || (*samples)[isam] == "ztautau"))
              {
                if(doZCR_MVA) sample.AddNormFactor("ATLAS_norm_Z"+jetNames[ijet], 1.0, 0.0, 10.0);
              }
            }

            if ( ((*regions)[ireg].name.find("AfrecSR") != string::npos || (*regions)[ireg].name.find("ASR") != string::npos || ((*regions)[ireg].name.find("signalLike") != string::npos && jetNames[ijet] == "2j") && doPacman2j) && (channelNames[ichan] == "ee" || channelNames[ichan] == "mm" || channelNames[ichan] == "SF"))
            {
              if ((*samples)[isam].type == "signal" || ((*samples)[isam].name.find("ggf") != string::npos && doVBF2j))
              {
                sample.AddOverallSys("PM_theta_SR"+jetNames[ijet], 1.0, 1.0);
              }
            }

            // Pacman control regions and nuisance parameters
            if ((ZMode == 2 && (jetNames[ijet] == "0j" || jetNames[ijet] == "1j")) || (doPacman2j && jetNames[ijet] == "2j"))
            {
              if ((*samples)[isam] == "zjets" || (*samples)[isam] == "zleplep" || ((*samples)[isam] == "ztautau" && (jetNames[ijet] != "2j") ))
              {
                if (channelNames[ichan] == "ee" || channelNames[ichan] == "mm" || channelNames[ichan] == "SF")
                {
                  if ((*regions)[ireg].name == "AfrecSR" || ((*regions)[ireg].name.find("signalLike") != string::npos && jetNames[ijet] == "2j"))
		  // if ((*regions)[ireg].name == "AfrecSR")
		  // if ((*regions)[ireg].name.find("signalLike") != string::npos && jetNames[ijet] == "2j")
                  {
                    sample.AddNormFactor("ATLAS_norm_SF_MUSR_DY"+string(splitPacmanNFs?"_"+channelNames[ichan]:"")+jetNames[ijet], 1.0, 0.0, 50.0);
                    //sample.AddNormFactor("PM_EFF_f_recoil_DY"+jetNames[ijet], jetNames[ijet]=="0j"?f_DY_all_0j:f_DY_all_1j, 0.0, 1.0);
		    sample.AddNormFactor("PM_EFF_f_recoil_DY"+jetNames[ijet], jetNames[ijet]=="0j"?f_DY_all_0j:(jetNames[ijet]=="1j"?f_DY_all_1j:f_DY_all_2j), 0.0, 1.0);
                  }

                  if ((*regions)[ireg].name == "ASR")
                  {
                    sample.AddNormFactor("ATLAS_norm_SF_MUSR_DY"+string(splitPacmanNFs?"_"+channelNames[ichan]:"")+jetNames[ijet], 1.0, 0.0, 50.0);
                    //sample.AddNormFactor("PM_EFF_f_recoil_rejected_DY"+jetNames[ijet], jetNames[ijet]=="0j"?1-f_DY_all_0j:1-f_DY_all_1j, 0.0, 1.0);
		    sample.AddNormFactor("PM_EFF_f_recoil_rejected_DY"+jetNames[ijet], jetNames[ijet]=="0j"?1-f_DY_all_0j:(jetNames[ijet]=="1j"?1-f_DY_all_1j:1-f_DY_all_2j), 0.0, 1.0);
                  }

                  if ((*regions)[ireg].name.find("CfrecZpeak") != string::npos)
                  {
                    sample.AddNormFactor("ATLAS_norm_SF_MU_DY"+jetNames[ijet], 1.0, 0.0, 50.0);
                    //sample.AddNormFactor("PM_EFF_f_recoil_DY"+jetNames[ijet], jetNames[ijet]=="0j"?f_DY_all_0j:f_DY_all_1j, 0.0, 1.0);
		    sample.AddNormFactor("PM_EFF_f_recoil_DY"+jetNames[ijet], jetNames[ijet]=="0j"?f_DY_all_0j:(jetNames[ijet]=="1j"?f_DY_all_1j:f_DY_all_2j), 0.0, 1.0);
                  }

                  if ((*regions)[ireg].name.find("CZpeak") != string::npos)
                  {
                    sample.AddNormFactor("ATLAS_norm_SF_MU_DY"+jetNames[ijet], 1.0, 0.0, 50.0);
                    //sample.AddNormFactor("PM_EFF_f_recoil_rejected_DY"+jetNames[ijet], jetNames[ijet]=="0j"?1-f_DY_all_0j:1-f_DY_all_1j, 0.0, 1.0);
		    sample.AddNormFactor("PM_EFF_f_recoil_rejected_DY"+jetNames[ijet], jetNames[ijet]=="0j"?1-f_DY_all_0j:(jetNames[ijet]=="1j"?1-f_DY_all_1j:1-f_DY_all_2j), 0.0, 1.0);
                  }

                  if ((*regions)[ireg].name == "EfrecWWCR")
                  {
                    sample.AddNormFactor("ATLAS_norm_SF_MU_DY"+jetNames[ijet], 1.0, 0.0, 50.0);
                    //sample.AddNormFactor("PM_EFF_f_recoil_DY"+jetNames[ijet], jetNames[ijet]=="0j"?f_DY_all_0j:f_DY_all_1j, 0.0, 1.0);
		    sample.AddNormFactor("PM_EFF_f_recoil_DY"+jetNames[ijet], jetNames[ijet]=="0j"?f_DY_all_0j:(jetNames[ijet]=="1j"?f_DY_all_1j:f_DY_all_2j), 0.0, 1.0);
                  }

                  if ((*regions)[ireg].name == "EWWCR")
                  {
                    sample.AddNormFactor("ATLAS_norm_SF_MU_DY"+jetNames[ijet], 1.0, 0.0, 50.0);
                    //sample.AddNormFactor("PM_EFF_f_recoil_rejected_DY"+jetNames[ijet], jetNames[ijet]=="0j"?1-f_DY_all_0j:1-f_DY_all_1j, 0.0, 1.0);
		    sample.AddNormFactor("PM_EFF_f_recoil_rejected_DY"+jetNames[ijet], jetNames[ijet]=="0j"?1-f_DY_all_0j:(jetNames[ijet]=="1j"?1-f_DY_all_1j:1-f_DY_all_2j), 0.0, 1.0);
                  }
                }
              }
              else
              {
                if ((*regions)[ireg].name.find("EfrecWWCR") != string::npos)
                {
                  //if (splitEfficiencies) sample.AddNormFactor("PM_EFF_f_recoil_NDY_WW"+jetNames[ijet], jetNames[ijet]=="0j"?f_NDY_WWCR_0j:f_NDY_WWCR_1j, 0.0, 1.0);
		  if (splitEfficiencies) sample.AddNormFactor("PM_EFF_f_recoil_NDY_WW"+jetNames[ijet], jetNames[ijet]=="0j"?f_NDY_WWCR_0j:(jetNames[ijet]=="1j"?f_NDY_WWCR_1j:f_NDY_WWCR_2j), 0.0, 1.0);
                  else                   sample.AddNormFactor("PM_EFF_f_recoil_NDY"+jetNames[ijet], f_NDY_all, 0.0, 1.0);
                }

                if ((*regions)[ireg].name.find("EWWCR") != string::npos)
                {
                  //if (splitEfficiencies) sample.AddNormFactor("PM_EFF_f_recoil_rejected_NDY_WW"+jetNames[ijet], jetNames[ijet]=="0j"?1-f_NDY_WWCR_0j:1-f_NDY_WWCR_1j, 0.0, 1.0);
		  if (splitEfficiencies) sample.AddNormFactor("PM_EFF_f_recoil_rejected_NDY_WW"+jetNames[ijet], jetNames[ijet]=="0j"?1-f_NDY_WWCR_0j:(jetNames[ijet]=="1j"?1-f_NDY_WWCR_1j:1-f_NDY_WWCR_2j), 0.0, 1.0);
                  else                   sample.AddNormFactor("PM_EFF_f_recoil_rejected_NDY"+jetNames[ijet], 1-f_NDY_all, 0.0, 1.0);
                }

                if ((*regions)[ireg].name.find("CfrecZpeak") != string::npos)
                {
                  //if (splitEfficiencies) sample.AddNormFactor("PM_EFF_f_recoil_NDY_ZP"+jetNames[ijet], jetNames[ijet]=="0j"?f_NDY_ZP_0j:f_NDY_ZP_1j, 0.0, 1.0);
		  if (splitEfficiencies) sample.AddNormFactor("PM_EFF_f_recoil_NDY_ZP"+jetNames[ijet], jetNames[ijet]=="0j"?f_NDY_ZP_0j:(jetNames[ijet]=="1j"?f_NDY_ZP_1j:f_NDY_ZP_2j), 0.0, 1.0);
                  else                   sample.AddNormFactor("PM_EFF_f_recoil_NDY"+jetNames[ijet], f_NDY_all, 0.0, 1.0);
                }

                if ((*regions)[ireg].name.find("CZpeak") != string::npos)
                {
                  //if (splitEfficiencies) sample.AddNormFactor("PM_EFF_f_recoil_rejected_NDY_ZP"+jetNames[ijet], jetNames[ijet]=="0j"?1-f_NDY_ZP_0j:1-f_NDY_ZP_1j, 0.0, 1.0);
		  if (splitEfficiencies) sample.AddNormFactor("PM_EFF_f_recoil_rejected_NDY_ZP"+jetNames[ijet], jetNames[ijet]=="0j"?1-f_NDY_ZP_0j:(jetNames[ijet]=="1j"?1-f_NDY_ZP_1j:1-f_NDY_ZP_2j), 0.0, 1.0);
                  else                   sample.AddNormFactor("PM_EFF_f_recoil_rejected_NDY"+jetNames[ijet], 1-f_NDY_all, 0.0, 1.0);
                }

                if (((*regions)[ireg].name.find("AfrecSR") != string::npos) || ((*regions)[ireg].name.find("signalLike") != string::npos && jetNames[ijet] == "2j"))
		// if (((*regions)[ireg].name.find("AfrecSR") != string::npos))
		  //if((*regions)[ireg].name.find("signalLike") != string::npos && jetNames[ijet] == "2j")
                {
                  if (((*samples)[isam].type == "signal" || ((*samples)[isam].name.find("ggf") != string::npos && doVBF2j)) && (channelNames[ichan] == "ee" || channelNames[ichan] == "mm" || channelNames[ichan] == "SF"))
                  {
                    sample.AddNormFactor("PM_EFF_f_recoil_NDY_S"+jetNames[ijet], 1.0, 0.0, 1.0);
                  }
                  else
                  {
                    //if (splitEfficiencies) sample.AddNormFactor("PM_EFF_f_recoil_NDY_SR"+jetNames[ijet], jetNames[ijet]=="0j"?f_NDY_SR_0j:f_NDY_SR_1j, 0.0, 1.0);
		    if (splitEfficiencies) sample.AddNormFactor("PM_EFF_f_recoil_NDY_SR"+jetNames[ijet], jetNames[ijet]=="0j"?f_NDY_SR_0j:(jetNames[ijet]=="1j"?f_NDY_SR_1j:f_NDY_SR_2j), 0.0, 1.0);
                    else                   sample.AddNormFactor("PM_EFF_f_recoil_NDY"+jetNames[ijet], f_NDY_all, 0.0, 1.0);
                  }
                }

                if ((*regions)[ireg].name.find("ASR") != string::npos)
                {
                  if (((*samples)[isam].type == "signal" || ((*samples)[isam].name.find("ggf") != string::npos && doVBF2j)) && (channelNames[ichan] == "ee" || channelNames[ichan] == "mm" || channelNames[ichan] == "SF"))
                  {
                    sample.AddNormFactor("PM_EFF_f_recoil_rejected_NDY_S"+jetNames[ijet], 1.0, 0.0, 1.0);
                  }
                  else
                  {
                    //if (splitEfficiencies) sample.AddNormFactor("PM_EFF_f_recoil_rejected_NDY_SR"+jetNames[ijet], jetNames[ijet]=="0j"?1-f_NDY_SR_0j:1-f_NDY_SR_1j, 0.0, 1.0);
		    if (splitEfficiencies) sample.AddNormFactor("PM_EFF_f_recoil_rejected_NDY_SR"+jetNames[ijet], jetNames[ijet]=="0j"?1-f_NDY_SR_0j:(jetNames[ijet]=="1j"?1-f_NDY_SR_1j:1-f_NDY_SR_2j), 0.0, 1.0);
                    else                   sample.AddNormFactor("PM_EFF_f_recoil_rejected_NDY"+jetNames[ijet], 1-f_NDY_all, 0.0, 1.0);
                  }
                }
              }
            }

            if ((channelNames[ichan] == "ee" || channelNames[ichan] == "mm" || channelNames[ichan] == "SF") && ((ZMode == 2 && (jetNames[ijet] == "0j" || jetNames[ijet] == "1j")) || (doPacman2j && jetNames[ijet] == "2j")))
            {
              if ((*samples)[isam] == "zjets" || (*samples)[isam] == "zleplep" || (*samples)[isam] == "ztautau") // DY
              {
                if ((*regions)[ireg].name == "topbox")
                {
                  sample.AddNormFactor("ATLAS_norm_SF_MU_DY"+jetNames[ijet], 1.0, 0.0, 50.0);
                }
              }
            }

            // Normalization parameter for individual samples (various uses)
            sample.AddNormFactor("ATLAS_sampleNorm_"+(*samples)[isam].name, 1.0, 0.0, 10.0, 1);

            // Normalization parameter for BR(HWW)
            if ((*samples)[isam].type == "signal" || ((*samples)[isam].name.find("ggf") != string::npos && doVBF2j))
            {
              sample.AddNormFactor("mu_BR_WW", 1.0, 0.0, 10.0, 1);
            }

            bool activate = 1;
            if ((*samples)[isam].type == "signal" || ((*samples)[isam].name.find("ggf") != string::npos && doVBF2j)) activate = 0;
            if (((*samples)[isam].name == "zjets" || (*samples)[isam].name == "zleplep" || (*samples)[isam].name == "ztautau") && (ZMode == 1 || (ZMode == 2 && doABCD2j && jetNames[ijet] == "2j")) && channelNames[ichan] != "em") activate = 0;
            if (useStatSys && activate) sample.ActivateStatError();

            chan.AddSample(sample);

          }

          meas.AddChannel(chan);

        }
      }
    }

    meas.CollectHistograms();
    meas.PrintTree();
    cout<<"before xml"<<endl;
    meas.PrintXML("./rev/"+version+"/xml/"+smass+salt+"/"+subfolder, meas.GetOutputFilePrefix());
    cout<<"after xml"<<endl;
    MakeModelAndMeasurementFast(meas);

  }

  LOG(logINFO) << "Done making workspace: " << subfolder;
}

// join vectors, removing duplicates
void joinSystematics(vector<vector<Response> >& vecResponse, vector<Response>& joined)
{
  int nrVec = vecResponse.size();
  for (int i=0;i<nrVec;i++)
  {
    vector<Response>* vecR = &vecResponse[i];
    int nrVec2 = vecR->size();
    for (int j=0;j<nrVec2;j++)
    {
      Response* thisR = &vecResponse[i][j];
      Response* r = NULL;
      int nrJoined = joined.size();
      for (int k=0;k<nrJoined;k++)
      {
        Response* res = &joined[k];
        if (res->match(thisR->channel, thisR->jet, thisR->sample, thisR->region) && res->name == thisR->name)
        {
          r = res;
          break;
        }
      }

      if (!r)
      {
        Response newR = *thisR;
        newR.hi += 1;
        newR.lo += 1;
        joined.push_back(newR);
      }
      else
      {
        r->hi *= 1+thisR->hi;
        r->lo *= 1+thisR->lo;
      }
    }
  }

  int nrJoined = joined.size();
  for (int i=0;i<nrJoined;i++)
  {
    joined[i].hi -= 1;
    joined[i].lo -= 1;
  }
}
