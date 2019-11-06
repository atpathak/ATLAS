#include <WorkspaceBuilder/Channel.h>
#include <RooStats/HistFactory/Measurement.h>
#include <RooStats/HistFactory/Channel.h>
#include <RooStats/HistFactory/Sample.h>

namespace WorkspaceBuilder{

  //-------------------------------------------------------------------------------
  bool Sample::readHistogramsFromRootFile(UserTools* usertools){
    if(m_inputhist!=0) delete m_inputhist; m_inputhist=0;

    //read the histograms for this sample
    for(unsigned int iinputchannel=0; iinputchannel<m_channel->inputChannels().size(); iinputchannel++){
      InputChannel* inputchannel=m_channel->inputChannels()[iinputchannel];
      for(unsigned int iinputsample=0; iinputsample<m_inputsamples.size(); iinputsample++){
        if(m_inputsamples[iinputsample]->applyToInputChannel(inputchannel)){
          std::string nameinfileforsample=m_inputsamples[iinputsample]->nameInFile(inputchannel);
          TH1F *tmphist=usertools->getHistogram(inputchannel->nameInFile(), nameinfileforsample, "nominal", 0);
          // Scale by pseudo-random number
          // bool ish125 = TString(m_name).Contains("H125");
          // bool ishww  = TString(m_name).Contains("HWW");
          //if( ish125 || ishww ){
          //   srand(10);
          //   tmphist->Scale(rand()%10);
          //}
          if(m_inputhist==0) m_inputhist=tmphist;
          else{
            if(m_inputhist->GetNbinsX()!=tmphist->GetNbinsX()){
              std::cout << "In Sample "<<m_name<<" for channel " << m_channelname << " trying to add inputchannel " << inputchannel->nameInFile();
              std::cout<< " and input sample" <<m_inputsamples[iinputsample]->nameInFile(inputchannel) << " for readHistogramsFromRootFile() method, but hist->GetNbinsX()="<<m_inputhist->GetNbinsX();
              std::cout <<"!="<<tmphist->GetNbinsX() << "=tmphist->GetNbinsX()!" <<std::endl;
            }
            m_inputhist->Add(tmphist);

          }
        }
      }
    }

    if(m_inputhist==0 ){
      std::cout << "Did not find input histogram for sample \"" << m_name << "\" in channel \"" << m_channelname << "\"."<< std::endl;
      return false;
    }

    for(unsigned int isystematic=0; isystematic<m_systematics.size(); isystematic++)
      if(!m_systematics.at(isystematic)->readHistogramsFromRootFile(usertools))
        return false;

    for(unsigned int inormfactor=0; inormfactor<m_normfactors.size(); inormfactor++)
      if(!m_normfactors.at(inormfactor)->readNumbers(usertools))
        return false;

    return true;
  }

  //-------------------------------------------------------------------------------
  bool Sample::deriveHistograms(UserTools* usertools, float lumi){
    if(m_finalhist!=0) delete m_finalhist; m_finalhist=0;

    m_finalhist=usertools->rebinHistogram(m_inputhist, m_channelname, m_name,"nominal",0);
    m_finalhist->SetDirectory(0);
    m_finalAxis = (TAxis*)(m_finalhist->GetXaxis()->Clone("m_finalAxis"));//To keep binning information before making equidistant bins
    usertools->produceEquidistantHistogram(m_finalhist);
    if(lumi>0.)
      m_finalhist->Scale(lumi/m_channel->lumi());
    for(int ibin=0; ibin<m_finalhist->GetNbinsX()+2; ibin++){
      if (m_finalhist->GetBinContent(ibin) < 0){
        std::cout << "WARNING! Sample::deriveHistograms() -  SETTING TO 1E-6 (AS VALUE IS NEGATIVE) BIN "
                  << ibin << " IN CHANNEL " << m_channelname << " FOR SAMPLE " << m_name << std::endl;
        m_finalhist->SetBinContent(ibin, 1E-6);
      }
    }

    //don't symmetrize the nominal histogram
    bool ret=true;
    //ret=usertools->smoothHistogram(m_finalhist, 0,  m_channelname, m_name,"nominal",0);
    //if(!ret)
    //return false;

    for(unsigned int isystematic=0; isystematic<m_systematics.size(); isystematic++)
      if(!m_systematics.at(isystematic)->deriveHistograms(usertools, m_finalhist, lumi)){
        std::cerr<<"Sample::deriveHistograms() - ERROR! Cannot derive histograms for "
                 <<m_channelname<<" sample "
                 <<m_name<<" systematic "
                 <<m_systematics.at(isystematic)->name()<<std::endl;
        return false;
      }

    for(unsigned int isyst=0; isyst<m_systematics.size(); isyst++){
      bool ret=usertools->symmetrizeSystematic(m_systematics[isyst], m_finalhist);
      if(!ret){
        std::cerr<<"Sample::deriveHistograms() - ERROR! Cannot symmetrize histograms for "
                 <<m_channelname<<" sample "
                 <<m_name<<" systematic "
                 <<m_systematics.at(isyst)->name()<<std::endl;

        return false;
      }
    }

    ret=usertools->kyleFixHistogram(m_finalhist, m_channelname, m_name,"nominal",0, isSignal());
    if(!ret){
      std::cerr<<"Sample::deriveHistograms() - ERROR! Problem with Kyle-Fix for channel "<<m_channelname<<" sample "<<m_name<<" nominal..."<<std::endl;
      return false;
    }

    return true;
  }

  //-------------------------------------------------------------------------------
  bool Sample::deriveFinalHistograms(UserTools* usertools){
    for(unsigned int isyst=0; isyst<m_systematics.size(); isyst++){
      bool ret=m_systematics[isyst]->deriveFinalHistograms(usertools,m_finalhist);
      if(!ret)
        return false;
    }

    return true;
  }

  //-------------------------------------------------------------------------------

  bool Sample::pruneSystematics(UserTools* usertools){
    std::cout<<"INFO: Sample::pruneSystematics() - Pruning for sample '"<<this->name()<<"' in channel '"<<m_channel->name()<<"'..."<<std::endl;
    for(unsigned int isyst=0; isyst<m_systematics.size(); isyst++){
      int ret=usertools->pruneSystematic(m_systematics[isyst], m_channel, this);
      if(ret==0){//Everything Pruned
        m_systematics[isyst]->setIsPrunedAway(true);
        m_systematics[isyst]->setIsOverallSys(false);
        m_systematics[isyst]->setIsHistoSys(false);
      }
      else if(ret==1){//Shape Pruned
        m_systematics[isyst]->setIsPrunedAway(false);
        m_systematics[isyst]->setIsOverallSys(true);
        m_systematics[isyst]->setIsHistoSys(false);
      }
      else if(ret==2){//Overall Pruned
        m_systematics[isyst]->setIsPrunedAway(false);
        m_systematics[isyst]->setIsOverallSys(false);
        m_systematics[isyst]->setIsHistoSys(true);
      }
      else if(ret==3){//Nothing pruned
        m_systematics[isyst]->setIsPrunedAway(false);
        m_systematics[isyst]->setIsOverallSys(true);
        m_systematics[isyst]->setIsHistoSys(true);
      }
      else{
        std::cout << "Sample::pruneSystematics() - ERROR! For systematic \"" << m_systematics[isyst]->name()
                  << "\", sample \"" << m_name
                  << "\" and channel \""<<m_channelname
                  <<"\" an unknown return value \""
                  <<ret << "\" was obtained while pruning." << std::endl;
        throw UnknownPruningReturnValue(m_channelname, m_name,m_systematics[isyst]->name(), ret );
      }
    }

    return true;
  }

  //-------------------------------------------------------------------------------
  bool Sample::writeHistogramsToRootFile(TDirectory* channeldir){
    channeldir->cd();
    TDirectory* sampledir=channeldir->mkdir(m_name.c_str());
    sampledir->cd();

    // cross-check if there are <0 bin contents <- should not happen with KyleFix
    double firstbinwidth=m_finalhist->GetXaxis()->GetBinUpEdge(1)-m_finalhist->GetXaxis()->GetBinLowEdge(1);
    for(int i=1; i<m_finalhist->GetNbinsX()+1; i++){
      if(m_finalhist->GetBinContent(i)<0.)
        std::cout << "WARNING: For channel \"" << m_channelname << "\" and sample \"" << m_name << "\" the bin content in bin " << i << " <0: " <<  m_finalhist->GetBinContent(i) << std::endl;
      double binwidth=m_finalhist->GetXaxis()->GetBinUpEdge(i)-m_finalhist->GetXaxis()->GetBinLowEdge(i);
      if(fabs((binwidth-firstbinwidth)/binwidth)>0.001)
        std::cout << "WARNING: For channel \"" << m_channelname << "\" and sample \""<< m_name<<"\" the bin width in bin " << i << " \""<<binwidth <<"\" is different from the width in the first bin \"" <<  firstbinwidth << "\". Non equi-distant binning will likely lead to problems with HistFactory."<< std::endl;
    }

    TH1F* hist=(TH1F*)m_finalhist->Clone();
    hist->SetDirectory(sampledir);
    hist->SetName("nominal");
    hist->Write();
    delete hist;
    for(unsigned int isystematic=0; isystematic<m_systematics.size(); isystematic++)
      if(!m_systematics.at(isystematic)->writeHistogramsToRootFile(sampledir))
        return false;

    return true;
  }

  //-------------------------------------------------------------------------------
  TH1F* Sample::getHistogram(std::map<std::string,double> parameters){
    if(m_finalhist ==0){
      std::cout << "ERROR: getHistogram failed for sample \"" << m_name << "\" as input histogram were not correctly initialized." << std::endl;
      throw SampleNotInitialized(m_channelname, m_name);
    }

    TH1F* hist=(TH1F*)m_finalhist->Clone();
    hist->SetDirectory(0);

    //take care of systematics
    for(unsigned int isystematic=0; isystematic<m_systematics.size(); isystematic++){
      TH1F *addhist=m_systematics[isystematic]->getHistoSysHistogramChange(parameters, m_finalhist);
      if(addhist!=0){ //==0 if nuisance parameter =0
        for(int ibin=0; ibin<addhist->GetNbinsX()+2; ibin++){
          double sf=1.;
          if(hist->GetBinContent(ibin)!=0){
            sf+=addhist->GetBinContent(ibin)/hist->GetBinContent(ibin);
          }
          hist->SetBinContent(ibin, hist->GetBinContent(ibin)*sf);
          hist->SetBinError(ibin, hist->GetBinError(ibin)*sf); // on purpose so that MC stat error of systematics is not considered (just as in likelihood function) hist->Add(addhist) would lead to wrong result.
        }
        delete addhist;
        addhist=0;
      }
    }

    for(unsigned int isystematic=0; isystematic<m_systematics.size(); isystematic++){
      double sf=m_systematics[isystematic]->getOverallSysScaleFactor(parameters);
      hist->Scale(sf);
    }

    //take care of normfactors
    for(unsigned int inormfactor=0; inormfactor<m_normfactors.size(); inormfactor++){
      double nf=m_normfactors[inormfactor]->getNormFactorValue(parameters);
      hist->Scale(nf);
    }

    //take care of shapefactors and MC stat gamma_stat_* parameters
    for(int ibin=1; ibin<hist->GetNbinsX()+1; ibin++){
      double sf=getGammaScaleFactor(parameters,ibin); //get statistical gamma_* factor
      for(unsigned int ishapefactor=0; ishapefactor<m_shapefactors.size(); ishapefactor++){
        sf*=m_shapefactors[ishapefactor]->getGammaScaleFactor(parameters, ibin);
        hist->SetBinContent(ibin,sf*hist->GetBinContent(ibin));
        hist->SetBinError(ibin,sf*hist->GetBinError(ibin));
      }
    }

    return hist;
  }

  //-------------------------------------------------------------------------------

  double Sample::getGammaScaleFactor(std::map<std::string,double> parameters, int ibin){
    if(ibin<=0){
      std::cout << "ERROR: Sample::getGammaScaleFactor called with ibin<=0, but only bins in visible range have gamma factors, ibin="<< ibin << std::endl;
      return 1.;
    }

    double sf=1.;

    TString parname="gamma_stat_"+m_channelname+"_bin_";
    parname+=(ibin-1);
    std::map<std::string,double>::iterator it=parameters.find(parname.Data());
    if(it!=parameters.end())
      return it->second;

    return sf;
  }

  //-------------------------------------------------------------------------------

  RooStats::HistFactory::Sample Sample::getHistFactorySample(std::string rootfilename,std::string channel){
    if(m_finalhist ==0){
      std::cout << "ERROR: getHistFactorySample failed for sample \"" << m_name << "\" as input histogram were not correctly initialized." << std::endl;
      throw SampleNotInitialized(m_channelname, m_name);
    }

    RooStats::HistFactory::Sample sample(m_name);
    sample.SetNormalizeByTheory(m_normalizebytheory);
    if(m_staterror){
      sample.ActivateStatError();
    }
    sample.SetInputFile(rootfilename);
    sample.SetHistoName("nominal");
    sample.SetHistoPath(channel+"/"+m_name+"/");
    //sample->ActivateStatError();
    for(unsigned int isystematic=0; isystematic<m_systematics.size(); isystematic++){
      if(!m_systematics[isystematic]->isPrunedAway()){
        if(m_systematics[isystematic]->isHistoSys() && !m_systematics[isystematic]->forceOverall())
          sample.AddHistoSys(m_systematics[isystematic]->getHistFactoryHistoSys(channel,m_name, rootfilename));
        //the overall part is always used
        if(m_systematics[isystematic]->isOverallSys() && !m_systematics[isystematic]->shapeOnly()){
          sample.AddOverallSys(m_systematics[isystematic]->getHistFactoryOverallSys());
        }
      }
    }

    for(unsigned int inormfactor=0; inormfactor<m_normfactors.size(); inormfactor++){
      sample.AddNormFactor(m_normfactors[inormfactor]->getHistFactoryNormFactor());
    }

    for(unsigned int ishapefactor=0; ishapefactor<m_shapefactors.size(); ishapefactor++){
      sample.AddShapeFactor(m_shapefactors[ishapefactor]->getHistFactoryShapeFactor());
    }

    return sample;
  }

  //-------------------------------------------------------------------------------

  bool Sample::hasEffect(){
    
    for (auto itag : m_tags) {
        if (itag == "DoNotWriteToWs") {
            std::cout << "INFO: Sample \"" << m_name << "\" in \""<<m_channelname<< "\" will be skipped " << std::endl;
            return false;
        }
    }
    
    if(m_finalhist->GetSumOfWeights()>0.01) return true;
    else  {
      std::cout << "INFO: Sample \"" << m_name << "\" in \""<<m_channelname<< "\" has <0.01 events and will not be used in fit model. sumofweights="<<m_finalhist->GetSumOfWeights() << std::endl;
      return false;
    }

    for(unsigned int isyst=0; isyst<m_systematics.size(); isyst++)
      if(!m_systematics[isyst]->isPrunedAway())
        return true;
    if(m_shapefactors.size()>0)
      return true;
    return false;
  }

} // namespace
