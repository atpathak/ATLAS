#include <WorkspaceBuilder/HTauTauLFVUserTools.h>
#include <iostream>
#include <TH1F.h>
#include <TRandom3.h>

namespace WorkspaceBuilder{

  //-------------------------------------------------------------------------------
  HTauTauLFVUserTools::HTauTauLFVUserTools(std::string statanalysisname, std::string outputpath, std::string version, std::vector<std::string> rootpaths, std::vector<std::string> normfactortxtfilenames, std::vector<std::string> overallsystematicfilenames, std::map<std::string,std::string> parameters, bool unblind, bool info): HTauTauUserTools(statanalysisname, outputpath, version, rootpaths, normfactortxtfilenames,overallsystematicfilenames, parameters, unblind, info) {
    
  }

  //-------------------------------------------------------------------------------
  HTauTauLFVUserTools::~HTauTauLFVUserTools(void){
  
  }
  
  //-------------------------------------------------------------------------------
  std::string HTauTauLFVUserTools::description(){
    TString description=HTauTauUserTools::description();
    description+="\n\tBlindFullWithFloat blind all of the signal regions with floating point (instead of integer data)\n\tBlindFullWithInt blind all of the signal regions with integer\n\tBlindFullWithRandomInt blind all of the signal regions with a random integer (according to Poission(expected float MC))\n\tBlindWithBackgroundOnly blind all of the signal regions with background-only (no Higgs signal) used as sum of MC.\n\tBlindWithZero blinds with 0. data content (mainly useful for debugging)\n";
    return description.Data();
  }

  //-------------------------------------------------------------------------------
  TH1F* HTauTauLFVUserTools::rebinHistogram(TH1F* input, std::string /*channel*/, std::string /*sample*/, std::string /*systematic*/, int /*up*/){
    TH1F* outhist=(TH1F*)input->Clone();
    outhist->SetDirectory(0);

    return outhist;
  }


  //-------------------------------------------------------------------------------
  bool HTauTauLFVUserTools::blindData(TH1F* input, Channel* channel, float /*lumi*/){
    if(!m_unblind){
      std::map<std::string,double> parameters;
      parameters.insert(std::pair<std::string,double>("SigXsecOverSM",1.));
      parameters.insert(std::pair<std::string,double>("SigXsecOverSM_WW",1.));
      if(m_parameters.find("BlindWithBackgroundOnly")!=m_parameters.end()){
	parameters.find("SigXsecOverSM")->second=0;
	parameters.find("SigXsecOverSM_WW")->second=0;
	//parameters.insert(std::pair<std::string,double>("SigXsecOverSM",0.));
	//parameters.insert(std::pair<std::string,double>("SigXsecOverSM_WW",0.));
      }
     
      TH1F* sumofmc=channel->getSumOfMCHistogram(parameters);
      //TH1F* signal=channel->getSignalHistogram(parameters);
      //double lowedge=sumofmc->GetXaxis()->GetBinLowEdge(1);
      //double upedge=sumofmc->GetXaxis()->GetBinUpEdge(sumofmc->GetNbinsX());
      double blindfrom=-1E100;
      double blindto=1E100;
      /* change the blinding region here if necessary */

      std::cout << "INFO: Blinding data from " << blindfrom << " to " << blindto << " in channel " << channel->name() << std::endl; 

      for(int ibin=0; ibin<input->GetNbinsX()+2; ibin++){
	
	if(input->GetXaxis()->GetBinUpEdge(ibin)>=blindfrom && input->GetXaxis()->GetBinLowEdge(ibin)<=blindto){
	  double bincontent=(int)(sumofmc->GetBinContent(ibin)+0.5);
	  if(m_parameters.find("BlindWithZero")!=m_parameters.end()){
	    bincontent=0.;
	  }
	  if(m_parameters.find("BlindFullWithFloat")!=m_parameters.end()){
	    bincontent=sumofmc->GetBinContent(ibin);
	  }
	  if(m_parameters.find("BlindFullWithRandomInt")!=m_parameters.end()){
	    TRandom3 rnd(TString(channel->name()).Hash()+ibin);
	    bincontent=rnd.Poisson(sumofmc->GetBinContent(ibin));
	  }
	  //std::cout << "Setting bin " << ibin << " from " << input->GetBinContent(ibin) << " to " << bincontent  << " sumofmc: " << sumofmc->GetBinContent(ibin)<< std::endl;
	  input->SetBinContent(ibin,bincontent);//add a ghost event
	  input->SetBinError(ibin,sqrt(bincontent));

	}
	if(ibin!=0 && ibin!=input->GetNbinsX()+1 && input->GetBinContent(ibin)<=0){
	  std::cout << "WARNING: In channel \"" << channel->name() << "\" the data (possibly fake data due to blinding) in bin " << ibin << "<=0: " << input->GetBinContent(ibin) << ", this will likely lead to problems due to an ill-defined likelihood function."<< std::endl;
	}
      }
    }


    return true;
  }



} // namespace
