#include <iostream>
#include <fstream>
#include <string>

#include "TSystem.h"
#include "TFile.h"
#include "TH1.h"


void remapBinningFromCAFhists(std::string rebinningListFile, std::string version) {
  //Opening the file containing the "unbinned" histograms
  gSystem->Load("HWWAnalysisCode/lib/libQFramework.so");
  std::string unbinnedhistfile="HWWAnalysisCode/analysis/HWWlvlv_2012/samples_hww_analysis_Nominal_unbinnedhists_" + version + ".root";
  TFile f(unbinnedhistfile.c_str());

  //Opening the "histograms.txt" file 
  std::string rebinnedHistFile=std::string("HWWAnalysisCode/analysis/HWWlvlv_2012/histograms_remappedbinning_" + version + ".txt");
  ifstream iRebinnedHistFile;
  iRebinnedHistFile.open (rebinnedHistFile.c_str(), ifstream::in);
  ofstream oRebinnedHistFile(rebinnedHistFile.c_str());

  //Reading the binning configuration file and deal with binning remapping
  std::cout << "Reading in rebinning configuration file: " << rebinningListFile << std::endl;
  ifstream inFile(rebinningListFile.c_str());
  if (inFile.fail())  {
    std::cout << "ERROR::Couldn't open file: " << rebinningListFile.c_str() << std::endl;
    exit(1);
  }
  //For each remapping to do
  string channels;
  string region;
  unsigned int nbins;
  while (!inFile.eof())  {
    inFile >> channels;
    inFile >> region;
    inFile >> nbins;

    std::string channelsCAFname;
    std::string chan;
    if (channels=="OF") {
      channelsCAFname="bkg/em+bkg/me";
      chan="em,me";
    }
    else if (channels=="SF") {
      channelsCAFname="bkg/ee+bkg/mm";
      chan="ee,mm";
    }
    else {
      channelsCAFname="bkg/"+channels;
      chan=channels;
    }

    std::string formula="";
    if (nbins>1) {
      std::vector<Int_t> * borders = new std::vector<Int_t> ;
      TQSampleFolder * samples_Nominal = f.Get("samples_Nominal");
      int res = samples_Nominal->createRemappedHistogramFlat(region+"/MT", nbins, channelsCAFname, region+"/MT_Remapped", borders);
      TH1 * hist = samples_Nominal->getHistogram(channelsCAFname,region+"/MT_Remapped");
      if (!hist) 
	std::cout << "Remapping of " << region+"/MT for " << channelsCAFname << " failed !" << std::endl;
      else { //printing the binning
	hist = samples_Nominal->getHistogram(channelsCAFname,region+"/MT");
	std::cout << "Remapping binning of " << region+"/MT for " << channelsCAFname << " is :" << std::endl;
	formula = "-0.1 ";
	for (unsigned int ibin=0; ibin<borders->size(); ibin++) {
	  float lowbound = ((ibin>0) ? hist->GetBinLowEdge(borders->at(ibin-1))*1000. : 0.);
	  float upbound = hist->GetBinLowEdge(borders->at(ibin))*1000.;
	  formula += "+";
	  formula += TString::Format("%.3f*(MT>%.0f)*(MT<%.0f)",(ibin+1)*(1./nbins),lowbound,upbound).Data();
	}
	formula += TString::Format(" +1.0*(MT>%.0f)",hist->GetBinLowEdge(borders->at(borders->size()-1))*1000.).Data();
      }
    }//end of if remapping necessary (not a single bin)
    else
      formula="0.5";
   
    if (formula!="") {
      std::cout << formula << std::endl;

      //and dumping it into the histogram file      
      oRebinnedHistFile << "# ================================================================================= \n";
      oRebinnedHistFile << "	% cuts = '" << region << "*', channel = '" << chan << "' \n";
      oRebinnedHistFile << "# ================================================================================= \n";
      oRebinnedHistFile << "TH1F('MT_Remapped',''," << nbins << ",0.,1.) << ( ( " << formula << " ) : 'Mapped m_{T} [arbitrary]' ) \n";
      oRebinnedHistFile << "\n\n";
    }

  }

  std::cout << "Remapped binning written in " << rebinnedHistFile.c_str() << std::endl;
  //closing the histogram.txt file
  oRebinnedHistFile.close();

}
