#include <WorkspaceBuilder/Channel.h>
#include <RooStats/HistFactory/Measurement.h>
#include <RooStats/HistFactory/Channel.h>
#include <RooStats/HistFactory/Sample.h>

namespace WorkspaceBuilder{

  //-------------------------------------------------------------------------------
  bool Data::readHistogramsFromRootFile(UserTools* usertools){
    if(m_inputhist!=0) delete m_inputhist; m_inputhist=0;

    //read the histograms for this data
    for(unsigned int i=0; i<m_inputdata.size(); i++){
      for(unsigned isubchannel=0; isubchannel<m_channel->inputChannels().size(); isubchannel++){
	//first check if for this dataname a tag is found
	bool channeltagfound=m_channel->inputChannels()[isubchannel]->hasTag(m_inputdatachannel[i]);
	if(channeltagfound || m_inputdatachannel[i]=="All" || m_inputdatachannel[i]==m_channel->inputChannels()[isubchannel]->name()){
	  if(m_inputhist==0) m_inputhist=usertools->getHistogram(m_channel->inputChannels()[isubchannel]->nameInFile(), m_inputdata[i], "nominal", 0); //0 as this is not a systematic
	  else{
	    TH1F* tmphist=usertools->getHistogram(m_channel->inputChannels()[isubchannel]->nameInFile(), m_inputdata[i], "nominal", 0);
	    if(m_inputhist->GetNbinsX()!=tmphist->GetNbinsX()){
	      std::cout << "In Data for channel " << m_channelname << " trying to add inputchannel " << m_channel->inputChannels()[isubchannel]->nameInFile()<< " and input data" << m_inputdata[i] << " for readHistogramsFromRootFile() method, but hist->GetNbinsX()=" << m_inputhist->GetNbinsX()<<" ("<<m_channel->inputChannels()[isubchannel]->nameInFile()<<", "<<m_inputdata[i] << ")!="<<tmphist->GetNbinsX() << "=tmphist->GetNbinsX()!" <<std::endl;
	    }
	    m_inputhist->Add(tmphist);
	  }
	}
	// possibly consider more than one input data
      }
    }

    if(m_inputhist==0 ){
      std::cout << "Did not find input histogram for data in "
		<< "channel \"" << m_channelname << "\"." << std::endl;
      return false;
    }

    return true;
  }

  //-------------------------------------------------------------------------------

  bool Data::deriveHistograms(UserTools* usertools, float lumi){
    if(m_finalhist!=0) delete m_finalhist; m_finalhist=0;

    m_finalhist=usertools->rebinHistogram(m_inputhist, m_channelname, m_name,"nominal",0);
    m_finalhist->SetDirectory(0);

    //no smoothing for data
    //usertools->smoothHistogram(m_finalhist, m_channelname, m_name,"nominal",0);

    //blind the data (the user tools can decide to blind the data or not)
    bool ret=usertools->blindData(m_finalhist, m_channel, lumi);
    if(!ret){
      std::cerr<<"ERROR! Data::deriveHistograms() - Problem with blinding DATA..."<<std::endl;
      return false;
    }

    //produce an empty histogram with the correct binning, so that Hbb framework can produce plots with right binning
    m_binninghistogram=(TH1F*)m_finalhist->Clone();
    m_binninghistogram->SetName(TString("bins_"+m_channelname).Data());
    m_binninghistogram->SetDirectory(0);
    m_binninghistogram->SetEntries(0);
    for(int i=0; i<m_binninghistogram->GetNbinsX()+2; i++){
      m_binninghistogram->SetBinContent(i,0.);
      m_binninghistogram->SetBinError(i,0.);
    }

    usertools->produceEquidistantHistogram(m_finalhist);

    return true;
  }

 //-------------------------------------------------------------------------------

  bool Data::deriveFinalHistograms(UserTools* /*usertools*/){
    //bool ret=usertools->blindData(m_finalhist, m_channel);
    //if(!ret)
    //return false;
    
    return true;
  }

 //-------------------------------------------------------------------------------

  bool Data::writeHistogramsToRootFile(TDirectory* channeldir){
    channeldir->cd();
    TDirectory* sampledir=channeldir->mkdir(m_name.c_str());
    sampledir->cd();

    // cross-check if there are <0 bin contents
    double firstbinwidth=m_finalhist->GetXaxis()->GetBinUpEdge(1)-m_finalhist->GetXaxis()->GetBinLowEdge(1);
    for(int i=1; i<m_finalhist->GetNbinsX()+1; i++){
      if(m_finalhist->GetBinContent(i)<0.)
	std::cout << "WARNING: For channel \"" << m_channelname << "\" and data the bin content in bin " << i << " <0: " <<  m_finalhist->GetBinContent(i) << std::endl;
      double binwidth=m_finalhist->GetXaxis()->GetBinUpEdge(i)-m_finalhist->GetXaxis()->GetBinLowEdge(i);
      if(fabs((binwidth-firstbinwidth)/binwidth)>0.001)
	std::cout << "WARNING: For channel \"" << m_channelname << "\" and data the bin width in bin " << i << " \""<<binwidth <<"\" is different from the width in the first bin \"" <<  firstbinwidth << "\". Non equi-distant binning will likely lead to problems with HistFactory."<< std::endl;
    }

    TH1F* hist=(TH1F*)m_finalhist->Clone();
    hist->SetDirectory(sampledir);
    hist->SetName("nominal");
    hist->Write();
    delete hist;

    return true;
  }

  //-------------------------------------------------------------------------------

  RooStats::HistFactory::Data Data::getHistFactoryData(std::string rootfilename,std::string channel){
    RooStats::HistFactory::Data data;
    data.SetName(m_name);
    data.SetInputFile(rootfilename);
    data.SetHistoName("nominal");
    data.SetHistoPath(channel+"/"+m_name+"/");

    return data;
  }

} // namespace
