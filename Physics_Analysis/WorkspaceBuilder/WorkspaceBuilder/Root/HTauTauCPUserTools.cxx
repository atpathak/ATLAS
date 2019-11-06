#include <WorkspaceBuilder/HTauTauCPUserTools.h>
#include <iostream>
#include <TH1F.h>
#include <TRandom3.h>

namespace WorkspaceBuilder{

  //-------------------------------------------------------------------------------
  HTauTauCPUserTools::HTauTauCPUserTools(std::string statanalysisname, std::string outputpath, std::string version, std::vector<std::string> rootpaths, std::vector<std::string> normfactortxtfilenames, std::vector<std::string> overallsystematicfilenames, std::map<std::string,std::string> parameters, bool unblind, bool info): HTauTauUserTools(statanalysisname, outputpath, version, rootpaths, normfactortxtfilenames, overallsystematicfilenames, parameters, unblind, info) {
    
    if (m_parameters.find("Theta") == m_parameters.end() || m_parameters.find("Theta")->second == "") {
        std::cout << "ERROR: \'--cp\' option set but no theta value specified (use \'-p Theta=xx\')" << std::endl;
        throw;  // TODO: implement a better exception
    }
  }

  //-------------------------------------------------------------------------------
  HTauTauCPUserTools::~HTauTauCPUserTools(void){
  
  }
  
  //-------------------------------------------------------------------------------
  std::string HTauTauCPUserTools::description(){
    TString description=HTauTauUserTools::description();
    description += "\n\tBlindFullWithFloat blind all of the signal regions with floating point (instead of integer data)\n";
    description += "\tBlindFullWithInt blind all of the signal regions with integer\n";
    description += "\tBlindFullWithRandomInt blind all of the signal regions with a random integer (according to Poission(expected float MC))\n";
    description += "\tBlindWithBackgroundOnly blind all of the signal regions with background-only (no Higgs signal) used as sum of MC.\n";
    description += "\tBlindWithZero blinds with 0. data content (mainly useful for debugging)\n";
    description += "\tTheta The mixing angle to use for CP model\n";
    description += "\tFlatMultijet Replace Fakes histogram by uniform distribution\n";
    description += "\tToyMultijet Replace Fakes histogram by toy MC sampled from uniform distribution\n";
    return description.Data();
  }

  //-------------------------------------------------------------------------------
  TH1F* HTauTauCPUserTools::getHistogram(std::string channel, std::string sample, std::string systematic, int up) {
    
    // Get the correct CP-mix histograms for the given theta angle
    if (sample == "VBF_theta_X") {
        sample = "VBF_theta_" + m_parameters.find("Theta")->second + "deg";
    }
    else if (sample == "ggH_theta_X") {
        sample = "ggH_theta_" + m_parameters.find("Theta")->second + "deg";
    }
    
    //std::cout << "In getHistogram: channel " << channel << ", sample " << sample << std::endl;
    return HTauTauUserTools::getHistogram(channel, sample, systematic, up);
  }

  //-------------------------------------------------------------------------------
  TH1F* HTauTauCPUserTools::rebinHistogram(TH1F* input, std::string /*channel*/, std::string sample, std::string /*systematic*/, int /*up*/){
    
    TH1F* outhist=(TH1F*)input->Clone();
    outhist->SetDirectory(0);
    //outhist->Sumw2();
    
    // Replace the multijet histogram with either a flat distribution or toy MC
    // sampled from a flat distribution
    if (sample == "Fake") {
        
        if (m_parameters.find("FlatMultijet") != m_parameters.end()) {
            
            std::cout << "INFO: Using flat distribution for multijet background" << std::endl;
            
            float bincontent = 0;
            float integralPerUnit = outhist->Integral()/(outhist->GetXaxis()->GetXmax());
            
            for (int i = 1; i <= outhist->GetNbinsX(); i++) {
                float binWidth = outhist->GetBinWidth(i);
                bincontent = integralPerUnit*binWidth;
                outhist->SetBinContent(i, bincontent);
                outhist->SetBinError(i, sqrt(bincontent));
                // std::cout << "Bin width: " << binWidth << std::endl;
                // std::cout << "Setting bin " << i << " to " << bincontent << std::endl;
            }
        }
        else if (m_parameters.find("ToyMultijet") != m_parameters.end()) {
            
            std::cout << "INFO: Using toy MC for multijet background" << std::endl;
            float bincontent = outhist->Integral()/outhist->GetNbinsX();
            TRandom3 rnd;
            for (int i = 1; i <= outhist->GetNbinsX(); i++) {
                outhist->SetBinContent(i, rnd.Poisson(bincontent));
                outhist->SetBinError(i, sqrt(outhist->GetBinContent(i)));
                // std::cout << "Setting bin " << i << " to " << outhist->GetBinContent(i) << std::endl;
            }
        }
    }

    return outhist;
  }


  //-------------------------------------------------------------------------------
  bool HTauTauCPUserTools::blindData(TH1F* input, Channel* channel, float /*lumi*/){
    
    // Blind the Higgs regions, but unblind the Z regions
    if (channel->name().find("hh_Ztt") != std::string::npos) m_unblind = true;
    else m_unblind = false;
      
    if(!m_unblind){
      std::map<std::string,double> parameters;
      parameters.insert(std::pair<std::string,double>("ATLAS_epsilon", 1.));
      parameters.insert(std::pair<std::string,double>("ATLAS_epsilon_rejected", 0.));
      //parameters.insert(std::pair<std::string,double>("SigXsecOverSM_WW",1.));
      if(m_parameters.find("BlindWithBackgroundOnly")!=m_parameters.end()){
	        parameters.find("ATLAS_epsilon")->second=0;
            parameters.find("ATLAS_epsilon_rejected")->second=0;
	        //parameters.find("SigXsecOverSM_WW")->second=0;
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
      
      // temporary hack: blind MMC distr
      if (channel->name().find("mmc") != std::string::npos) {
        
        blindfrom = 100;
        blindto = 150;
      }
      

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
