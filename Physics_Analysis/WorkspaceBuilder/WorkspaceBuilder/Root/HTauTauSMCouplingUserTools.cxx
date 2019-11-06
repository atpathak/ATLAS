#include <WorkspaceBuilder/HTauTauSMCouplingUserTools.h>
#include <fstream>
#include <iostream>
#include <TH1F.h>
#include <TRandom3.h>
#include <TMath.h>

namespace WorkspaceBuilder{

  //-------------------------------------------------------------------------------
  HTauTauSMCouplingUserTools::HTauTauSMCouplingUserTools(std::string statanalysisname, std::string outputpath, std::string version, std::vector<std::string> rootpaths, std::vector<std::string> normfactortxtfilenames, std::vector<std::string> overallsystematicfilenames, std::map<std::string,std::string> parameters, bool unblind, bool info): HTauTauUserTools(statanalysisname, outputpath, version, rootpaths, normfactortxtfilenames, overallsystematicfilenames, parameters, unblind, info) {

  }

  //-------------------------------------------------------------------------------
  HTauTauSMCouplingUserTools::~HTauTauSMCouplingUserTools(void){
  
  }
  
  //-------------------------------------------------------------------------------
  std::string HTauTauSMCouplingUserTools::description(){
    TString description=HTauTauUserTools::description();
    description+="\n\tRebinToSingleBin rebin histograms in each channel to a single bin\n\tBlindFull blind all of the signal regions (i.e not only in blinded region)\n\tBlindWithInt blind with integer (instead of floating-point data)\n\tBlindWithRandomInt blind all of the signal regions with a random integer (according to Poission(expected float MC))\n\tBlindWithBackgroundOnly blind all of the signal regions with background-only (no Higgs signal) used as sum of MC.\n\tBlindWithZero blinds with 0. data content (mainly useful for debugging)\n\tBlindWithMu=ABC\n";
    return description.Data();
  }


  //-------------------------------------------------------------------------------
  
  double HTauTauSMCouplingUserTools::getLumiInInvPb(std::string channel){
    for(unsigned int ifile=0; ifile<m_rootfiles.size(); ifile++){
      m_rootfiles.at(ifile)->cd();
      TDirectory* channeldir=m_rootfiles.at(ifile)->GetDirectory(channel.c_str());
      if(channeldir!=0){
	TH1F* hist=(TH1F*)channeldir->Get("lumiininvpb");
	  if(hist!=0){
	    return hist->GetBinContent(1); 
	  }
      }
    }
    //if(TString(channel).Contains("15"))
    //  return 3193.6;
    //if(TString(channel).Contains("16"))
    //  return 5118.3;
    //if(TString(channel).Contains("All"))
    //  return 8311.9;

    std::cout << "ERROR: Could not find luminosity information in histogram lumiininvpb for channel \"" << channel << "\"." << std::endl;
    return HTauTauSMCouplingUserTools::getLumiInInvPb(channel);
  }


  //-------------------------------------------------------------------------------
  TH1F* HTauTauSMCouplingUserTools::getHistogram(std::string channel, std::string sample, std::string systematic, int up){
    /*if(sample=="Fake" && TString(channel).Contains("ll"))
      sample="Fakes";*/
    if(systematic=="MadgraphVsSherpa"){
      TH1F *sherpa;
      /* not needed any more
      if(TString(channel).Contains("eh15") || TString(channel).Contains("eh16") || TString(channel).Contains("mh15") || TString(channel).Contains("mh16")){
	TString newsample= TString(sample)+TString("Sherpa");
	sherpa=HTauTauSMCouplingUserTools::getHistogram(channel,newsample.Data(), "nominal", 0);
      }else{
	TString newsample= TString(sample)+TString("Sh");
	sherpa=HTauTauSMCouplingUserTools::getHistogram(channel, newsample.Data(), "nominal", 0);
	}*/
      TString newsample= TString(sample)+TString("Sh");                                                                                                                            
      sherpa=HTauTauSMCouplingUserTools::getHistogram(channel, newsample.Data(), "nominal", 0);

      //TH1F *madgraph=HTauTauSMCouplingUserTools::getHistogram(channel, sample, "nominal", 0);
      //sherpa->Scale(madgraph->GetSumOfWeights()/sherpa->GetSumOfWeights());
      return sherpa;
    }
    
    /*if(TString(channel).Contains("eh15") || TString(channel).Contains("eh16") || TString(channel).Contains("mh15") || TString(channel).Contains("mh16")){
      if(sample=="Zttewk" || sample=="ZllEWK"){
	TH1F *zttewk=HTauTauSMCouplingUserTools::getHistogram(channel, "Ztt", systematic, up);
	zttewk->SetName(TString(zttewk->GetName())+"zttewk");
	zttewk->SetEntries(0);
	for(int i=0; i<zttewk->GetNbinsX()+2; i++){
	  zttewk->SetBinContent(i,0.);
	  zttewk->SetBinError(i,0.);
	}
	return zttewk;
      }*/

  //   if(TString(channel).Contains("_ztt") || TString(channel).Contains("_zll") ){
  //     if(sample=="ZllEWK"){
  // TH1F *zllewk=HTauTauSMCouplingUserTools::getHistogram(channel, "Zll", systematic, up);
  // zllewk->SetName(TString(zllewk->GetName())+"zllewk");
  // zllewk->SetEntries(0);
  // for(int i=0; i<zllewk->GetNbinsX()+2; i++){
  //   zllewk->SetBinContent(i,0.);
  //   zllewk->SetBinError(i,0.);
  // }
  // return zllewk;
  //     }
  //   }


    //Special treatment needed for Run1 "reverse engineered" inputs, by Julian
    //Overallsys are saved as a histogram of 1 bin with range -0.5 to 0.5 instead of in a txt
    //Deal with that...
    if( m_parameters.find("Run1Inputs")!=m_parameters.end()){
      TH1F* sys = HTauTauUserTools::getHistogram(channel, sample, systematic, up);
      if(sys->GetNbinsX()==1 && sys->GetBinLowEdge(1)==-0.5 && sys->GetBinLowEdge(sys->GetNbinsX()+1)==0.5){
        Double_t osys = sys->Integral();
        std::cout<<"INFO: Found overall sys-style histogram for "<<systematic<<" and up "<<up<<" in sample "<<sample<<"and channel "<<channel<<". Treating as overallsys with value"<<osys<<std::endl;
        TH1F* nominal=HTauTauUserTools::getHistogram(channel,sample, "nominal", 0);
        sys =(TH1F*)nominal->Clone();
        sys->Scale(osys/nominal->Integral());
      }
      return sys;
    }



    return HTauTauUserTools::getHistogram(channel, sample, systematic, up);
  }

  //-------------------------------------------------------------------------------
  TH1F* HTauTauSMCouplingUserTools::rebinHistogram(TH1F* input, std::string channel, std::string sample, std::string systematic, int up){
    TH1F* outhist=0;
    TString histname=channel+"_"+sample+"_"+systematic+"_"+TString(up);

    if(m_parameters.find("RebinToSingleBin")!=m_parameters.end() || TString(channel).Contains("regztt") ){
      //    if(m_parameters.find("RebinToSingleBin")!=m_parameters.end() ){
      outhist = new TH1F(histname,histname, 1, -0.5,0.5);
      outhist->SetDirectory(0);
      outhist->Sumw2();
      double sumw=0.;
      double sumw2=0.;
      for(int i=1; i<input->GetNbinsX()+1; i++){
        sumw+=input->GetBinContent(i);
        sumw2+=pow(input->GetBinError(i),2.);
      }
      outhist->SetBinContent(1,sumw);
      outhist->SetBinError(1,sqrt(sumw2));
      outhist->SetEntries(input->GetEntries());
      return outhist;
    }

    //Making side-band histogram for the fitting scrutiny
    //    Bool_t debug=(TString(sample).Contains("Ztt") && TString(systematic).Contains("JER"));
    Bool_t debug=false;
    if(m_parameters.find("UseSideBandForFit")!=m_parameters.end() && TString(channel).Contains("regsig")){
      if(debug)std::cout<<channel<<", "<<sample<<", "<<systematic<<", "<<up<<std::endl;
      Int_t nExpBins = 0;
      //Hard coded threshold for the signal region. To be configurable by "-p" options.
      Double_t LowerBoundOfSR = 100.;
      //Double_t UpperBoundOfSR = 150.;
      Double_t UpperBoundOfSR = TMath::Infinity(); //Exclude all data points above LowerBoundOfSR.
      //Loop to compute #bins for the new histogram
      for(Int_t ibin=1; ibin<input->GetNbinsX()+1; ibin++){//Don't consider under/overflow bins
        Double_t lowEdge =input->GetBinLowEdge(ibin);
        Double_t highEdge=input->GetBinLowEdge(ibin)+input->GetBinWidth(ibin);
        if(debug)std::cout<<"Bin="<<ibin<<": LowEdge="<<lowEdge<<", HighEdge="<<highEdge<<", "<<input->GetBinContent(ibin)<<" +/- "<<input->GetBinError(ibin);
        if((UpperBoundOfSR<=lowEdge) || (highEdge<=LowerBoundOfSR)){
          nExpBins++;
        }else{
          if(debug)std::cout<<" -- This bin should be excluded!!";
        }
        if(debug)std::cout<<std::endl;
      }

      outhist = new TH1F(histname,histname, nExpBins, 0.,nExpBins);//Making histogram with equidistant bin.
      outhist->SetDirectory(0);
      outhist->Sumw2();
      //Loop to fill the new histogram
      Int_t binIndex = 1;
      for(Int_t ibin=1; ibin<input->GetNbinsX()+1; ibin++){//Don't consider under/overflow bins
        Double_t lowEdge =input->GetBinLowEdge(ibin);
        Double_t highEdge=input->GetBinLowEdge(ibin)+input->GetBinWidth(ibin);
        if((UpperBoundOfSR<=lowEdge) || (highEdge<=LowerBoundOfSR)){
          outhist->SetBinContent(binIndex,input->GetBinContent(ibin));
          outhist->SetBinError  (binIndex,input->GetBinError  (ibin));
          binIndex++;
        }
      }
      outhist->SetBinContent(0,input->GetBinContent(0));
      outhist->SetBinError  (0,input->GetBinError  (0));
      outhist->SetBinContent(outhist->GetNbinsX()+1,input->GetBinContent(input->GetNbinsX()+1));
      outhist->SetBinError  (outhist->GetNbinsX()+1,input->GetBinError  (input->GetNbinsX()+1));
      outhist->SetEntries(input->GetEntries());//This will lead you underestimated Kyle-Fix weight, but no proper solution in principle...
      // //Contents check
      // for(Int_t jbin=0; jbin<outhist->GetNbinsX()+2; jbin++){
      //   std::cout<<"outhist: Bin="<<jbin<<", "<<outhist->GetBinContent(jbin)<<" +/- "<<outhist->GetBinError(jbin)<<std::endl;
      // }
      return outhist;
      getchar();
    }

    outhist=(TH1F*)input->Clone();
    outhist->SetDirectory(0);
    //outhist->Rebin(outhist->GetNbinsX());

    return outhist;
  }

  //-------------------------------------------------------------------------------

  bool HTauTauSMCouplingUserTools::smoothHistogram(TH1F* input, TH1F* nominal, std::string channel, std::string sample, std::string systematic, int up){
    if(!TString(channel).Contains("year2011") && 
       !TString(channel).Contains("year2012") )
      return HTauTauUserTools::smoothHistogram(input, nominal, channel, sample, systematic, up);
    else
      return true;

  }

  //-------------------------------------------------------------------------------

  bool HTauTauSMCouplingUserTools::kyleFixHistogram(TH1F* input, std::string channel, std::string sample, std::string systematic, int up, bool issignal){
    if(!TString(channel).Contains("year2011") && 
       !TString(channel).Contains("year2012") )
      return HTauTauUserTools::kyleFixHistogram(input, channel, sample, systematic, up, issignal);
    else
      return true;
  }

  //-------------------------------------------------------------------------------

  bool HTauTauSMCouplingUserTools::blindData(TH1F* input, Channel* channel, float /*lumi*/){
 
    // blind between 100 GeV and 150 GeV
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
      std::map<std::string,std::string>::iterator it=m_parameters.find("BlindWithMu");
      if(it!=m_parameters.end()){
	parameters.find("SigXsecOverSM")->second=atof(it->second.c_str());
        std::cout << "SigXsecOverSM blinding : " << it->second.c_str() << std::endl;
	parameters.find("SigXsecOverSM_WW")->second=0;
      }
      if( m_parameters.find("RunRandomMu")!=m_parameters.end()){
        std::cout << "Warning!! =============================================================================" << std::endl;
        std::cout << "Data will be unblinded with randomly scaled signal histograms!!"                         << std::endl;
        std::cout << "Output WS gives non-sense significance. But still do not to see post-fit plots with it"  << std::endl;
        std::cout << "so that people cannot get any insight for the real data in the critical region...."      << std::endl;
        std::cout << "=======================================================================================" << std::endl;
        srand(m_rndmseed);
        int rndval = rand() % 100 + 1; //random number [1, 100]
        double rndf = rndval*0.1; //random number [1, 100]
        //        std::cout<<"RNDM = "<<rndf<<std::endl;
        parameters.find("SigXsecOverSM")->second=rndf;
        parameters.find("SigXsecOverSM_WW")->second=rndf;
        std::cout << "SigXsecOverSM blinding : " << it->second.c_str() << std::endl;
      }
      TH1F* sumofmc=channel->getSumOfMCHistogram(parameters);
      TH1F* signal=channel->getSignalHistogram(parameters);
      double lowedge=sumofmc->GetXaxis()->GetBinLowEdge(1);
      double upedge=sumofmc->GetXaxis()->GetBinUpEdge(sumofmc->GetNbinsX());
      double blindfrom=-1E100;
      double blindto=1E100;     
      if (m_parameters.find("BlindFull")==m_parameters.end() && m_parameters.find("BlindFullWithFloat")==m_parameters.end() && m_parameters.find("BlindFullWithInt")==m_parameters.end() && m_parameters.find("BlindFullWithRandomInt")==m_parameters.end() ){
	std::cout<<"Looking at producing Hybrid Dataset"<<std::endl;
	if((channel->name()=="Htt_year2015_chanhh_catboost_regsig_selMVA" || channel->name()=="Htt_year2015_chanhh_catvbf_regsig_selMVA" || channel->name()=="Htt_year2015_chanlh_catboost_regsig_selMVA" || channel->name()=="Htt_year2015_chanlh_catvbf_regsig_selMVA" ) && fabs(lowedge-50)<0.001 && fabs(upedge-200.)<0.001){ //MMC hh or lh -> blind between 100 GeV and 150 GeV
	  blindfrom=100.;
	  blindto=150.;
	}
	else if((channel->name()=="Htt_year2015_chanll_catvbf_regsig_selMVA" || channel->name()=="Htt_year2015_chanll_catboost_regsig_selMVA") && fabs(lowedge/1000.-0.)<0.001 && fabs(upedge/1000.-300.)<0.001){ //MMC ll -> blind between 100 GeV and 150 GeV (ll uses MeV for the histograms)
	  blindfrom=100.;
	  blindto=150.;
	}
	else if((channel->name()=="Htt_year2015_chanll_catvbf_regsig_selMVA" || channel->name()=="Htt_year2015_chanll_catboost_regsig_selMVA") && fabs(lowedge-0.)<0.001 && fabs(upedge-300.)<0.001){ //MMC ll -> blind between 100 GeV and 150 GeV (just to be sure as they usually use MeV)
	  blindfrom=100000.;
	  blindto=150000.;
	}
	// blind rebinnings for May combination
	else if(channel->name()=="Htt_year2015_chanhh_catrest_regcontrol_selMVA" && fabs(lowedge+0.5)<0.001 && fabs(upedge-22.5)<0.001){ // hh rest
	  blindfrom=9.5;
	  blindto=19.5;
	}
	else if(channel->name()=="Htt_year2015_chanhh_catboost_regsig_selMVA" && fabs(lowedge+0.5)<0.001 && fabs(upedge-28.5)<0.001){ // hh boost
	  blindfrom=9.5;
	  blindto=19.5;
	}
	else if(channel->name()=="Htt_year2015_chanhh_catvbf_regsig_selMVA" && fabs(lowedge+0.5)<0.001 && fabs(upedge-20.5)<0.001){ // hh vbf
	  blindfrom=9.5;
	  blindto=19.5;
	}
	else if((channel->name()=="Htt_year2015_chanll_catboost_regsig_selMVA" || channel->name()=="Htt_year2015_chanll_catvbf_regsig_selMVA") && fabs(lowedge+0.5)<0.001 && fabs(upedge-15.5)<0.001){ // ll_boost_signal and ll_vbf_signal
	  blindfrom=2.5;
	  blindto=5.5;
	}
	else if((channel->name()=="Htt_year2015_chanll_catboost_regtop_selMVA" || channel->name()=="Htt_year2015_chanll_catvbf_regtop_selMVA") && fabs(lowedge+0.5)<0.001 && fabs(upedge-0.5)<0.001){ // ll_boost_{top/zll} and ll_vbf_{top/zll}
	  blindfrom=1E100;
	  blindto=1E100;//no blinding
	}
	else if((TString(channel->name()).Contains("selCBA") ) &&  !(TString(channel->name()).Contains("regsig"))  ){
	  blindfrom=1E100;
	  blindto=1E100;//no blinding
	}
	else if( (TString(channel->name()).Contains("selCBA") ) && (TString(channel->name()).Contains("chanll")) && (fabs(lowedge/1000.-0.)<0.001 && fabs(upedge/1000.-300.)<0.001) ){
	  //MMC ll -> blind between 100 GeV and 150 GeV (just to be sure as they usually use MeV)        
          blindfrom=100000.;
          blindto=150000.;
	}
	else if( TString(channel->name()).Contains("selCBA") ){
          blindfrom=100.;
          blindto=150.;	  
	}
	else if(fabs(lowedge+1.)<0.001 && fabs(upedge-1.)<0.001){ //BDT -> blind the region with 70% of the signal
	  int lowbin=input->GetNbinsX()+1;
	  blindto=10;
	  double allsignal=signal->GetSumOfWeights();
	  double suminblindedregion=0.;
	  while(lowbin>0 && suminblindedregion/allsignal<0.7){
	    suminblindedregion+=signal->GetBinContent(lowbin);
	    blindfrom=input->GetXaxis()->GetBinLowEdge(lowbin);
	    lowbin--;
	  }
	}
	else{
	  std::cout << "WARNING: Unknown histogram type (could not recognize it as MMC or BDT distribution). I am blinding the whole data in channel \"" <<channel->name()<<"\" now, this could lead to unexpected results (lowedge=" << lowedge <<", upedge="<<upedge<<")." << std::endl; 
	}
      }

      //Special for Random-mu test
      if(m_parameters.find("RunRandomMu")!=m_parameters.end()){
        std::cout << "Set blinding range blindfrom=blindto=1E100 " << std::endl;
        blindfrom = 1E100;
        blindto   = 1E100;
      }

      //Special for m_vis
      if(m_parameters.find("RunWithMvis")!=m_parameters.end()){
        // For the moment m_vis blinding is only applied for leplep-channel
        if((TString(channel->name()).Contains("selCBA") ) && (TString(channel->name()).Contains("chanll"))){
          std::cout << "Set blinding range for blindfrom=40. & blindto=60 for m_vis" << std::endl;
          blindfrom = 40.;
          blindto   = 60.;
        }
      }

      std::cout << "INFO: Blinding data from " << blindfrom << " to " << blindto << " in channel " << channel->name() << std::endl; 

      for(int ibin=0; ibin<input->GetNbinsX()+2; ibin++){
	
	if(input->GetXaxis()->GetBinUpEdge(ibin)>=blindfrom && input->GetXaxis()->GetBinLowEdge(ibin)<=blindto){
	  double bincontent=sumofmc->GetBinContent(ibin);
	  if(m_parameters.find("BlindWithZero")!=m_parameters.end()){
	    bincontent=0.;
	  }
	  if( m_parameters.find("BlindWithInt")!=m_parameters.end()){
	    bincontent=(int)(sumofmc->GetBinContent(ibin)+0.5);
	  }
	  if(m_parameters.find("BlindFullWithRandomInt")!=m_parameters.end() || m_parameters.find("BlindWithRandomInt")!=m_parameters.end()){
	    TRandom3 rnd(TString(channel->name()).Hash()+ibin);
	    bincontent=rnd.Poisson(sumofmc->GetBinContent(ibin));
	  }
	  //std::cout << "Setting bin " << ibin << " from " << input->GetBinContent(ibin) << " to " << bincontent  << " sumofmc: " << sumofmc->GetBinContent(ibin)<< std::endl;
	  input->SetBinContent(ibin,bincontent);//add a ghost event
	  input->SetBinError(ibin,sqrt(bincontent));

	}
	if(ibin!=0 && ibin!=input->GetNbinsX()+1 && input->GetBinContent(ibin)<=0){
	  std::cout << "WARNING: In channel \"" << channel->name() << "\" the data (possibly fake data due to blinding) in bin " << ibin << "<=0: " << input->GetBinContent(ibin) << ", this will likely lead to problems due to an ill-defined likelihood function."<< std::endl;
	  for(int i=0; i<input->GetNbinsX()+2; i++){
	    std::cout << "INFO: Printing the histogram: bin " << i << " -> " << input->GetBinContent(i) << " ["<<input->GetXaxis()->GetBinLowEdge(i)<<","<<input->GetXaxis()->GetBinUpEdge(i)<<"]"<< std::endl;
	  }
	}
      }
    }

    return true;
  }

  //-------------------------------------------------------------------------------

  int HTauTauSMCouplingUserTools::pruneSystematic(Systematic* systematic, Channel* channel, Sample* sample){

    if(TString(channel->name()).Contains("year2011") ||
       TString(channel->name()).Contains("year2012") ){
      int retval=3;
      if(systematic->getUpHistogram()->GetNbinsX()<=3 &&
	 systematic->getDownHistogram()->GetNbinsX()<=3 ){
	if(fabs(systematic->getUpNormalisation()-1.)<1E-6 &&
	   fabs(systematic->getDownNormalisation()-1.)<1E-6 )
	  retval=0; //prune away
	else
	  retval=1; //consider as overallsys
      }
      //return 2 if low and high=1
      else if(fabs(systematic->getUpNormalisation()-1.)<1E-6 &&
	      fabs(systematic->getDownNormalisation()-1.)<1E-6 )
	retval=2;//consider as histosys only

      if((retval==2||retval==3) && (systematic->getUpHistogram()->GetNbinsX()<=3 || systematic->getDownHistogram()->GetNbinsX()<=3 ))
	 std::cout << "ERROR: histosys with too little bins found" << systematic->getUpHistogram()->GetNbinsX() << " or " << systematic->getDownHistogram()->GetNbinsX() << std::endl;

      return retval; // consider as overall and histosys
    }
    /////////////////////////////////////////////////
    // Pruning for year 2015/2016
    /////////////////////////////////////////////////
    else{
      ////////////////////////////////////////////////////
      // Common pruning criteria applied first
      ////////////////////////////////////////////////////
      int retvalue = Systematic::NotPruned;
      retvalue = HTauTauUserTools::pruneSystematic(systematic, channel, sample);

      ////////////////////////////////////////////////////
      // VERY SPECIFIC SM Htt PRUNING
      ////////////////////////////////////////////////////

      // Retain Overall systs if forceOverall()
      if (systematic->forceOverall() && (retvalue == 0 || retvalue == 2)){
        //std::cout << "Keeping : " << systematic->name() << " in sample : "  << sample->name() << " and channel : " << channel->name() << std::endl;
        if (retvalue == 0) retvalue = 1;
        else if (retvalue == 2) retvalue = 3;
      }

      bool doMGvsSHremovalinZtt = true;
      if (doMGvsSHremovalinZtt){
        if (TString(systematic->name()).Contains("MadgraphVsSherpa") && TString(channel->name()).Contains("regztt")){
          systematic->setSysPrunedBy(PruneUtil::OtherReason);
          Systematic::removeOverallSys(retvalue);
        }
      }

      //document one-sided systematics into a txt file
      if(Systematic::hasHistSys(retvalue)){
        //for shape systematics check if any bin is one-sided
        for(int ibin=1; ibin<systematic->getUpHistogram()->GetNbinsX()+1; ibin++){
          double diffup=systematic->getUpHistogram()->GetBinContent(ibin)-sample->getFinalHistogram()->GetBinContent(ibin);
          double diffdown=systematic->getDownHistogram()->GetBinContent(ibin)-sample->getFinalHistogram()->GetBinContent(ibin);
          if(diffdown*diffup>0.){
            m_onesidedsystematicsfile << "One-sided systematic " << channel->name() << " \t " << sample->name() << " \t " << systematic->name() << " \t " << ibin << " \t " << sample->getFinalHistogram()->GetBinContent(ibin) << " \t " << systematic->getUpHistogram()->GetBinContent(ibin) << " \t " << systematic->getDownHistogram()->GetBinContent(ibin) << std::endl;
          }
        }
      }
      if(Systematic::hasOverallSys(retvalue)){
        double diffup=systematic->getUpNormalisation()-1.;
        double diffdown=systematic->getDownNormalisation()-1.;
        if(diffdown*diffup>0.){
          m_onesidedsystematicsfile << "One-sided systematic " << channel->name() << " \t " << sample->name() << " \t " << systematic->name() << " \t NORM \t " << 1. << " \t " << systematic->getUpNormalisation() << " \t " << systematic->getDownNormalisation();
          if(retvalue==3) m_onesidedsystematicsfile << "\t SHAPE";
          if(sample->getYield()/channel->data()->getYield()>0.1 && fabs(diffup)>0.01 && fabs(diffdown)>0.01)
            m_onesidedsystematicsfile << "\t IMPORTANT";
          m_onesidedsystematicsfile << std::endl;
        }
      }

      //Force pruning for JES
      // if (!TString(systematic->name()).Contains("ATLAS_JES")) retvalue = Systematic::PrunedAway;

      //Force pruning HistSys for Ztt theory
      // if(TString(systematic->name()).Contains("Theo_Ztt")){
      //   Systematic::removeHistSys(retvalue);
      // }

      // //Force pruning some HistSys for JER
      // if(TString(systematic->name()).Contains("JER")){
      //   if(TString(channel->name()).Contains("chanll_catboost") || TString(channel->name()).Contains("chanlh_catboost")){
      //     if(TString(channel->name()).Contains("regsig")){
      //       if(TString(sample->name()).Contains("Ztt")){
      //         std::cout<< channel->name() << " \t " << sample->name() << " \t " << systematic->name() << " \t NORM \t " << 1. << " \t " << systematic->getUpNormalisation() << " \t " << systematic->getDownNormalisation()<<std::endl;
      //         retvalue = Systematic::PrunedAway;
      //       }
      //     }
      //   }
      // }

      //Force pruning OverallSys for JER
      // if(TString(systematic->name()).Contains("JER")) Systematic::removeOverallSys(retvalue);
      // You also have to comment out "this->setIsOverallSys(true);" in Systematic.cxx!!

      //Force pruning HistSys for JER
      // if(TString(systematic->name()).Contains("JER")) Systematic::removeHistSys(retvalue);

      //Force pruning OverallSys for PRW
      // if(TString(systematic->name()).Contains("PRW")) Systematic::removeOverallSys(retvalue);
      // !!! You also have to comment out "this->setIsOverallSys(true);" in Systematic.cxx !!!!

      //Force pruning HistSys for PRW
      // if(TString(systematic->name()).Contains("PRW")) Systematic::removeHistSys(retvalue);

      // //Force pruning for PRW
      // if(TString(systematic->name()).Contains("PRW")){
      //   std::string WhatKept;
      //   if     (retvalue == 0) WhatKept="PrunedAway";
      //   else if(retvalue == 1) WhatKept="OverallSys";
      //   else if(retvalue == 2) WhatKept="Shape";
      //   else if(retvalue == 3) WhatKept="OverallSys+Shape";
      //   Double_t upAbsDiff   = fabs(1.-systematic->getUpNormalisation());
      //   Double_t downAbsDiff = fabs(1.-systematic->getDownNormalisation());
      //   if(upAbsDiff>0.02 || downAbsDiff>0.02){
      //     if( ((upAbsDiff>downAbsDiff) && upAbsDiff/downAbsDiff>10) || ((upAbsDiff<downAbsDiff) && downAbsDiff/upAbsDiff>10) ){
      //       std::cout<<"INFO: PRW for "<<channel->name()<<" of "<<sample->name()<<" returns "<<WhatKept<<"."<<std::endl;
      //       std::cout<<"Up: "<<systematic->getUpNormalisation()<<", Down:"<<systematic->getDownNormalisation()<<std::endl;
      //       Systematic::removeOverallSys(retvalue);
      //     }
      //   }
      //   Systematic::removeHistSys(retvalue);
      // }

      return retvalue;
    }
  }

  //-------------------------------------------------------------------------------

  double HTauTauSMCouplingUserTools::getOverallSys(std::string channel, std::string sample, std::string systematic, int up){
    
    TString systname=systematic;
    if(up>=1)
      systname+="_high";
    else
      systname+="_low";

    TString search="UnknownStringThatWillNotBeInFile";
    if(TString(channel).Contains("ll")) search="ll_";
    if(TString(channel).Contains("eh")) search="lh_";
    if(TString(channel).Contains("mh")) search="lh_";
    if(TString(channel).Contains("hh")) search="hh_";
    if(TString(channel).Contains("boost")) search+="boost";
    if(TString(channel).Contains("vbf")) search+="vbf";

    for(unsigned int i=0; i<m_overallsystematic_name.size(); i++){
      if(m_overallsystematic_name[i]==systname || 
	 m_overallsystematic_name[i]==sample+"_"+systname || 
	 m_overallsystematic_name[i]==channel+"_"+sample+"_"+systname ||
	 m_overallsystematic_name[i]==search+"_"+sample+"_"+systname ){
	return m_overallsystematic_value[i];
      }
    }

    std::cout << "DEBUG search term:" << search+"_"+sample+"_"+systname  << std::endl;
    for(unsigned int i=0; i<m_overallsystematic_name.size(); i++){
      std::cout << "DEBUG available:"<<m_overallsystematic_name[i] << std::endl;
    }

    std::cout << "ERROR: OverallSys for channel \"" << channel << "\", "
	      << "sample \""<< sample << "\" "
	      << "systematic \"" << systematic << "\" "
	      << " and up \"" << up << "\" was not found. Return 1." 
	      << std::endl;
    throw OverallSysNotFound(channel, sample, systematic, up);
    return 1.;
  }

} // namespace
