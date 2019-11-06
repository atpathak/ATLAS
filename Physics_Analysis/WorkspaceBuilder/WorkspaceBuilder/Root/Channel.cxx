#include <WorkspaceBuilder/Channel.h>
#include <WorkspaceBuilder/StatAnalysis.h>
#include <RooStats/HistFactory/Measurement.h>
#include <RooStats/HistFactory/Channel.h>
#include <RooStats/HistFactory/Sample.h>

namespace WorkspaceBuilder{

  //-------------------------------------------------------------------------------
  bool Channel::readHistogramsFromRootFile(UserTools* usertools){
    //first set the lumi
    m_lumi=0.;
    for(unsigned int isubchannel=0; isubchannel<inputChannels().size(); isubchannel++){
      if(inputChannels()[isubchannel]->considerLumi()){
	double lumi=usertools->getLumiInInvPb(inputChannels()[isubchannel]->nameInFile());
	m_lumi+=lumi;
      }
    // std::cout<<"======================================================"<<std::endl;
    // std::cout<<this->name()<<std::endl;
    // std::cout<<isubchannel<<": "<<inputChannels()[isubchannel]->name()<<std::endl;
    // std::cout<<m_lumi<<", "<<std::endl;
    // std::cout<<"======================================================"<<std::endl;
    // getchar();

    }
    //    m_lumi=13200;

    if(!m_data->readHistogramsFromRootFile(usertools))
      return false;

    for(unsigned int isample=0; isample<m_samples.size(); isample++)
      if(!m_samples.at(isample)->readHistogramsFromRootFile(usertools))
	return false;
    return true;
  }

  //-------------------------------------------------------------------------------
  bool Channel::deriveHistograms(UserTools* usertools, float lumi){

    for(unsigned int isample=0; isample<m_samples.size(); isample++)
      if(!m_samples.at(isample)->deriveHistograms(usertools, lumi)){
	std::cerr<<"ERROR! Channel::deriveHistograms() - Unable to derive histograms for sample "<<m_samples.at(isample)->name()<<"..."<<std::endl;
	return false;
      }

    if(!m_data->deriveHistograms(usertools, lumi)){
      std::cerr<<"ERROR! Channel::deriveHistograms() - Unable to derive histograms for DATA sample ..."<<std::endl;
      return false;
    }

    return true;
  }

  //-------------------------------------------------------------------------------
  bool Channel::deriveFinalHistograms(UserTools* usertools){

    for(unsigned int isample=0; isample<m_samples.size(); isample++)
      if(!m_samples.at(isample)->deriveFinalHistograms(usertools))
	return false;
    
    if(!m_data->deriveFinalHistograms(usertools))
      return false;
    return true;
  }

  //-------------------------------------------------------------------------------

  bool Channel::pruneSystematics(UserTools* usertools){
    for(unsigned int isample=0; isample<m_samples.size(); isample++)
      if(!m_samples.at(isample)->pruneSystematics(usertools))
	return false;
    return true;
  }

  //-------------------------------------------------------------------------------
  bool Channel::writeHistogramsToRootFile(TFile *file){
    file->cd();
    TDirectory* channeldir=file->mkdir(m_name.c_str());
    if(!m_data->writeHistogramsToRootFile(channeldir))
      return false;
    for(unsigned int isample=0; isample<m_samples.size(); isample++)
      if(!m_samples.at(isample)->writeHistogramsToRootFile(channeldir))
	return false;
    return true;
  }

  //-------------------------------------------------------------------------------
  TH1F* Channel::getSignalHistogram(std::map<std::string,double> parameters){
    TH1F * hist=0;
    for(unsigned int isample=0; isample<m_samples.size(); isample++){
      bool issignal=m_samples[isample]->isSignal();
      if(issignal){
	TH1F* tmphist=m_samples[isample]->getHistogram(parameters);
	if(hist==0)
	  hist=tmphist;
	else{
	  if(hist->GetNbinsX()!=tmphist->GetNbinsX())
	    std::cout << "In Channel " << m_name << " trying to add sample " << m_samples[isample]->name() << " for getSignalHistogram() method, but hist->GetNbinsX()="<<hist->GetNbinsX()<<"("<<m_samples[0]->name()<<")!="<<tmphist->GetNbinsX() << "=tmphist->GetNbinsX()!" <<std::endl;
	  hist->Add(tmphist);
	}
      }
    }
    
    return hist;
  }

  //-------------------------------------------------------------------------------
  TH1F* Channel::getSumOfMCHistogram(std::map<std::string,double> parameters){
    TH1F * hist=0;
    for(unsigned int isample=0; isample<m_samples.size(); isample++){
      TH1F* tmphist=m_samples[isample]->getHistogram(parameters);
      if(hist==0)
	hist=tmphist;
      else{
	  if(hist->GetNbinsX()!=tmphist->GetNbinsX())
	    std::cout << "In Channel " << m_name << " trying to add sample " << m_samples[isample]->name() << " for getSumOfMCHistogram() method, but hist->GetNbinsX()="<<hist->GetNbinsX()<<"("<<m_samples[0]->name()<<")!="<<tmphist->GetNbinsX() << "=tmphist->GetNbinsX()!" <<std::endl;
	  hist->Add(tmphist);
      }
    }
    
    return hist;
  }

  //-------------------------------------------------------------------------------
  RooStats::HistFactory::Channel Channel::getHistFactoryChannel(std::string outputpath, std::string version, std::string statanalysisname){
    TString rootfilename=outputpath;
    rootfilename+=version;
    rootfilename+="/xml/";
    rootfilename+=statanalysisname;
    rootfilename+="_histofile.root";
  
    RooStats::HistFactory::Channel channel(m_name,rootfilename.Data());
    channel.SetStatErrorConfig(staterrorthreshold, RooStats::HistFactory::Constraint::Poisson);
    channel.SetData(m_data->getHistFactoryData(rootfilename.Data(),m_name));

    // add the samples
    for(unsigned int isample=0; isample<m_samples.size(); isample++){
      if(m_samples[isample]->hasEffect()){
	channel.AddSample(m_samples[isample]->getHistFactorySample(rootfilename.Data(), m_name));
      }
    }
    
    return channel;
  }

} // namespace
