// Macro to convert analysis root files to stat format hists



#include <utility>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include "TSystem.h"
#include "TString.h"
#include "TObjArray.h"
#include "TFile.h"



bool do2011 = false;
bool doCPS = false;



void setupSampleHist(std::vector<std::string> &samples, std::vector<std::string> &paths, std::vector<std::string> &histograms)
{

  // Set samples and paths

  samples.push_back(std::string("data")); paths.push_back(std::string("data/X"));

  samples.push_back(std::string("ggww")); paths.push_back(std::string("bkg/X/diboson/WW/ggWW"));
  samples.push_back(std::string("qqww")); paths.push_back(std::string("bkg/X/diboson/WW/qqWW"));
  samples.push_back(std::string("ttbar")); paths.push_back(std::string("bkg/X/top/ttbar"));
  samples.push_back(std::string("st")); paths.push_back(std::string("bkg/X/top/singletop"));
  samples.push_back(std::string("wg")); paths.push_back(std::string("bkg/X/diboson/NonWW/Wgamma/gammaWincl/Wgamma"));
  samples.push_back(std::string("wgs")); paths.push_back(std::string("bkg/X/diboson/NonWW/Wgamma/gammaWincl/WgammaStar"));
  samples.push_back(std::string("wzzz")); paths.push_back(std::string("bkg/X/diboson/NonWW/ZW + bkg/X/diboson/NonWW/ZZ"));
  samples.push_back(std::string("ztautau")); paths.push_back(std::string("bkg/X/Zjets/Nom/?/tt"));
  samples.push_back(std::string("zleplep")); paths.push_back(std::string("bkg/X/Zjets/Nom/?/ee + bkg/X/Zjets/Nom/?/mm"));
  samples.push_back(std::string("wjets")); paths.push_back(std::string("bkg/X/Wjets"));

  samples.push_back(std::string("ggfMASS")); paths.push_back(std::string("sig/X/mhMASS/ggf"));
  samples.push_back(std::string("vbfMASS")); paths.push_back(std::string("sig/X/mhMASS/vbf"));
  samples.push_back(std::string("zhMASS")); paths.push_back(std::string("sig/X/mhMASS/ZH"));
  samples.push_back(std::string("whMASS")); paths.push_back(std::string("sig/X/mhMASS/WH"));

  // Histograms
  histograms.push_back("em_signalLike1_0j");
  histograms.push_back("me_signalLike1_0j");
  histograms.push_back("em_signalLike1_1j");
  histograms.push_back("me_signalLike1_1j");
  histograms.push_back("em_signalLike2_0j");
  histograms.push_back("me_signalLike2_0j");
  histograms.push_back("em_signalLike2_1j");
  histograms.push_back("me_signalLike2_1j");
  histograms.push_back("OF_ASR_0j");
  histograms.push_back("OF_ASR_1j");
  histograms.push_back("SF_ASR_0j");
  histograms.push_back("SF_ASR_1j");
  histograms.push_back("OF_AfrecSR_0j");
  histograms.push_back("OF_AfrecSR_1j");
  histograms.push_back("SF_AfrecSR_0j");
  histograms.push_back("SF_AfrecSR_1j");
  histograms.push_back("OF_mainControl_0j");
  histograms.push_back("OF_mainControl_1j");
  histograms.push_back("OF_topbox_1j");
  histograms.push_back("OF_CZpeak_0j");
  histograms.push_back("OF_CZpeak_1j");
  histograms.push_back("SF_CZpeak_0j");
  histograms.push_back("SF_CZpeak_1j");
  histograms.push_back("OF_CfrecZpeak_0j");
  histograms.push_back("OF_CfrecZpeak_1j");
  histograms.push_back("SF_CfrecZpeak_0j");
  histograms.push_back("SF_CfrecZpeak_1j");

  histograms.push_back("em_signalLike0301015_0j");
  histograms.push_back("me_signalLike0301015_0j");
  histograms.push_back("em_signalLike0301015_1j");
  histograms.push_back("me_signalLike0301015_1j");
  histograms.push_back("em_signalLike0301520_0j");
  histograms.push_back("me_signalLike0301520_0j");
  histograms.push_back("em_signalLike0301520_1j");
  histograms.push_back("me_signalLike0301520_1j");
  histograms.push_back("em_signalLike030201000_0j");
  histograms.push_back("me_signalLike030201000_0j");
  histograms.push_back("em_signalLike030201000_1j");
  histograms.push_back("me_signalLike030201000_1j");
  histograms.push_back("em_signalLike30501015_0j");
  histograms.push_back("me_signalLike30501015_0j");
  histograms.push_back("em_signalLike30501015_1j");
  histograms.push_back("me_signalLike30501015_1j");
  histograms.push_back("em_signalLike30501520_0j");
  histograms.push_back("me_signalLike30501520_0j");
  histograms.push_back("em_signalLike30501520_1j");
  histograms.push_back("me_signalLike30501520_1j");
  histograms.push_back("em_signalLike3050201000_0j");
  histograms.push_back("me_signalLike3050201000_0j");
  histograms.push_back("em_signalLike3050201000_1j");
  histograms.push_back("me_signalLike3050201000_1j");


}


void caf2statHists(std::string anaOutputDir = "", // Where are the output .root files
		   std::string version = "test", 
		   std::string syst="Normal",
		   std::string prefix = "", // prefixes need a trailing underscore, and no other
		   std::string mass = "125",
		   std::string pathToLibrary = "HWWAnalysisCode/lib/libQFramework.so", 
		   std::string statDir = "./")
{

  if (anaOutputDir == "")
  {
    std::cout<<"Please specifiy anaOutputDir!"<<std::endl;
    return;
  }
    
  gSystem->Load(pathToLibrary.c_str()); // load tqlibrary

  std::vector<std::string> samples;
  std::vector<std::string> paths;

  std::vector<std::string> histograms;
  setupSampleHist(samples, paths, histograms);

  if (samples.size() != paths.size() )
  {
    std::cout<< "samples and paths are not the same size!" << std::endl;
    return;
  }
  

  // Load analysis file
  std::string anaName = syst;
  if (syst == "Normal") anaName = "Nominal";
  std::string inputfile = anaOutputDir + "samples_hww_analysis_" + version + "_" + anaName + ".root:samples_" + anaName;
  std::cout << "Will get histograms from " << inputfile.c_str() << std::endl;
  TQSampleFolder *samplesFolder = TQSampleFolder::loadSampleFolder(inputfile.c_str() );


  if (!samplesFolder)      {
    std::cout<<"Couldn't load: " << inputfile + "  Continuing" << std::endl;
    return;
  }

  //Deal with special cases where CAF name is different from stat variation name
  if (syst=="_BJetWeightUp")
    syst = "BtagUp";
  else if (syst=="_BJetWeightDown")
    syst = "BtagDown";
  else if (syst=="_CTJetWeightUp")
    syst = "CtagUp";
  else if (syst=="_CTJetWeightDown")
    syst = "CtagDown";
  else if (syst=="_MisTagJetWeightUp")
    syst = "MtagUp";
  else if (syst=="_MisTagJetWeightDown")
    syst = "MtagDown";
  else if (syst=="_lepIsoUp")
    syst = "IsoUp";
  else if (syst=="_lepIsoDown")
    syst = "IsoDown";
  else if (syst=="_lepTriggerSFup")
    syst = "TriggerUp";
  else if (syst=="_lepTriggerSFdown")
    syst = "TriggerDown";
  else if (syst=="_MTShapeUp")
    syst = "PowhegShapeUp";
  else if (syst=="_MTShapeDown")
    syst = "PowhegShapeDown";


  //Renaming the syst in case it is weight-related
  if (syst.at(0)=='_')
    syst = syst.substr(1);

  std::string statsystname = syst;

  for (unsigned int iSample = 0; iSample < samples.size(); iSample++)      
  {
    // Get parent syst folder from systsList.txt
    std::string parentfolder = "";
    std::fstream systsList;
    std::string line, one, two;
    std::stringstream ss;
    if (syst=="Nominal") {
      parentfolder="Nominal";
      statsystname="Normal";
    }
    else if (syst=="ElecFakeWeightUp") {
      parentfolder="FakeRate_EL_HWW";
      statsystname="SysFakeUp";
    }
    else if (syst=="ElecFakeWeightDown") {
      parentfolder="FakeRate_EL_HWW";
      statsystname="SysFakeDown";
    }
    else if (syst=="MuonFakeWeightUp") {
      parentfolder="FakeRate_MU_HWW";
      statsystname="SysFakeUp";
    }
    else if (syst=="MuonFakeWeightDown") {
      parentfolder="FakeRate_MU_HWW";
      statsystname="SysFakeDown";
    }
    else {
      systsList.open( ("rev/" + version + "/systsList.txt").c_str(), std::ios::in );
      if (systsList.is_open())
	{
	  while(getline(systsList, line))
	    {
	      ss.str("");
	      ss.clear();
	      ss << line;
	      ss >> one >> two;

	      if (two == statsystname)
		{
		  if (statsystname.find("FlavComp") != std::string::npos)
		    {
		      if (samples[iSample] == "qqww" || samples[iSample] == "ggww")
			parentfolder = "ATLAS_JES_FlavComp_HWW_WW";
		      else if (samples[iSample] == "ttbar" || samples[iSample] == "st")
			parentfolder = "ATLAS_JES_FlavComp_HWW_tt";
		      else
			parentfolder = "ATLAS_JES_FlavComp_HWW_other";
		      break;
		    }
		  else{
		    parentfolder = one;
		    break;
		  }
		}
	    }
	  if (parentfolder == "")
	    {
	      std::cout <<"Failed to find parent folder"<<std::endl;
	      return;
	    }
	}
      else
	{
	  std::cout<< "Failed to open " << "rev/" << version << "/systsList.txt" <<std::endl;
	  return;
	}
      systsList.close();
    }//end of looking up for parentfolder

    std::cout << "Will write histograms into " << parentfolder.c_str() << "/" << statsystname.c_str() << std::endl;

    if (samples[iSample] == "data" && parentfolder != "Nominal") continue; //only nominal for data

    system( ("mkdir -vp " + statDir + "/rev/" + version + "/hists/" + parentfolder + "/" +statsystname ).c_str() );

    string sampleName = TString(samples[iSample]).ReplaceAll("MASS", mass);
    TFile * tempOutput = new TFile(std::string(statDir + "/rev/" + version + "/hists/" + parentfolder + "/" +statsystname + "/" + sampleName + ".root").c_str(), "RECREATE");

    // Grab the histograms
    for (unsigned int iHist=0; iHist < histograms.size(); iHist++)	{
      TObjArray * histTokens = TString(prefix + histograms[iHist]).Tokenize("_");
      TString name="";

      int flavors = 0;
      if (prefix != "" ) flavors = 1;
      for (int a = flavors + 1 ; a < histTokens->GetEntriesFast(); a++)	  {
	  name += histTokens->At(a)->GetName();
	  if (a != histTokens->GetEntriesFast() -1) name += "_";	 
      }
      


      TString path = paths[iSample];
      if (TString(histTokens->At(flavors)->GetName()).Contains("em"))
	path.ReplaceAll("X", "em");
      else if (TString(histTokens->At(flavors)->GetName()) == "me")
	path.ReplaceAll("X", "me");
      else if (TString(histTokens->At(flavors)->GetName()) == "OF")
	path = path.ReplaceAll("X", "em") + " + " + TString(paths[iSample]).ReplaceAll("X", "me");
      else if (TString(histTokens->At(flavors)->GetName()) == "SF")
	path = path.ReplaceAll("X", "ee") + " + " + TString(paths[iSample]).ReplaceAll("X", "mm");

	  
      path.ReplaceAll("MASS", mass);

      TString compname = name + "/MT_Remapped";
      std::cout << " Getting samplesFolder->getHistogram(" << path.Data() << ", " << compname.Data() << ")" << std::endl;
      TH1 * tempHist = samplesFolder->getHistogram(path.Data(), compname.Data());
      if (tempHist) {
	TString newname = histTokens->At(flavors)->GetName();
	newname += "_"; newname += name;
	tempHist->SetName(newname);
	std::cout << "   written " << newname.Data() << std::endl;
	tempHist->Write();
      }
    }


    tempOutput->Write();
    tempOutput->Close();
  }
    
  
}
