#include <WorkspaceBuilder/Channel.h>
#include <WorkspaceBuilder/Sample.h>
#include <WorkspaceBuilder/Systematic.h>
#include <RooStats/HistFactory/Measurement.h>
#include <RooStats/HistFactory/Channel.h>
#include <RooStats/HistFactory/Sample.h>
#include "boost/algorithm/string.hpp"

namespace WorkspaceBuilder{

  //-------------------------------------------------------------------------------
  std::string PruneUtil::getCriteriaName(int pruneType){
    if     (pruneType==EmptyHist          ) return "Empty histogram    ";
    else if(pruneType==LowStat            ) return "Low stat for shape ";
    else if(pruneType==Chi2               ) return "Shape ChiSquared   ";
    else if(pruneType==MaxVar             ) return "Max Variation      ";
    else if(pruneType==DiffUpDownSig      ) return "DiffUpDownSig      ";
    else if(pruneType==FlatPrune          ) return "FlatPrune          ";
    else if(pruneType==OneSideSmall       ) return "OneSideSmall       ";
    else if(pruneType==OneSideSmallDiff   ) return "OneSideSmallDiff   ";
    else if(pruneType==OneSideSmallAbsDiff) return "OneSideSmallAbsDiff";
    else if(pruneType==ZttPrune           ) return "ZttPrune           ";
    else if(pruneType==ZttPruneRetain     ) return "ZttPruneRetain     ";
    else if(pruneType==OtherReason        ) return "OtherReason        ";
    else                                    return "Not defined        ";
    return "";
  }

  //-------------------------------------------------------------------------------
  bool Systematic::readHistogramsFromRootFile(UserTools* usertools){
    if(m_upinputhistogram!=0) delete m_upinputhistogram; m_upinputhistogram=0;
    if(m_downinputhistogram!=0) delete m_downinputhistogram; m_downinputhistogram=0;
    if(m_upfinalhistogram!=0) delete m_upfinalhistogram; m_upfinalhistogram=0;
    if(m_downfinalhistogram!=0) delete m_downfinalhistogram; m_downfinalhistogram=0;
    m_upnormalisation=1.;
    m_downnormalisation=1.;

    //read the histograms for this sample
    for(unsigned int iinputchannel=0; iinputchannel<m_channel->inputChannels().size(); iinputchannel++){
      InputChannel* inputchannel=m_channel->inputChannels()[iinputchannel];
      for(unsigned int iinputsample=0; iinputsample<m_sample->inputSamples().size(); iinputsample++){
	if(m_sample->inputSamples()[iinputsample]->applyToInputChannel(inputchannel)){
	  InputSample* inputsample=m_sample->inputSamples()[iinputsample];
	  std::string nameinfileforsample=m_sample->inputSamples()[iinputsample]->nameInFile(inputchannel);
	  TH1F *histup=0;
	  TH1F *histdown=0;
	  if(applyToInputSampleAndChannel(inputsample, inputchannel)){
	    std::string nameinfileforsystematic=getSystematicNameInFile(inputsample, inputchannel);
	    if(m_forceoverall){
	      TH1F* nominal=usertools->getHistogram(inputchannel->nameInFile(),inputsample->nameInFile(inputchannel), "nominal", 0); //0 as this is not a systematic
	      if(m_onesided){
		double upnorm=usertools->getOverallSys(inputchannel->nameInFile(),inputsample->nameInFile(inputchannel), nameinfileforsystematic, 1);
		histup=(TH1F*)nominal->Clone();
		histup->Scale(upnorm);
		histdown=nominal;
	      }
	      else{

		double upnorm=usertools->getOverallSys(inputchannel->nameInFile(),inputsample->nameInFile(inputchannel), nameinfileforsystematic, 1);
		double downnorm=usertools->getOverallSys(inputchannel->nameInFile(),inputsample->nameInFile(inputchannel), nameinfileforsystematic, -1);
		histup=(TH1F*)nominal->Clone();
		histup->Scale(upnorm);
		histdown=(TH1F*)nominal->Clone();
		histdown->Scale(downnorm);
	      }
	    }
	    else{
	      if(m_onesided){
		histup=usertools->getHistogram(inputchannel->nameInFile(),inputsample->nameInFile(inputchannel), nameinfileforsystematic, 1); //our convention is that the _high histogram is used
		histdown=usertools->getHistogram(inputchannel->nameInFile(),inputsample->nameInFile(inputchannel), "nominal", 0); //0 as this is not a systematic
	      }
	      else{
		histup=usertools->getHistogram(inputchannel->nameInFile(),inputsample->nameInFile(inputchannel), nameinfileforsystematic, 1);
		histdown=usertools->getHistogram(inputchannel->nameInFile(),inputsample->nameInFile(inputchannel), nameinfileforsystematic, -1);
	      }
	    }
	  }
	  else{ // if this systematic does not apply to a given subsample or subchannel, use the nominal for this sample and channel
	    histup=usertools->getHistogram(inputchannel->nameInFile(),inputsample->nameInFile(inputchannel), "nominal", 0); //0 as this is not a systematic
	    histdown=(TH1F*)histup->Clone();
	  }
          // Scale by pseudo-random number
          //bool ish125 = TString(m_name).Contains("H125");
          //bool ishww  = TString(m_name).Contains("HWW");
	  //if( ish125 || ishww ){
          //   srand(10);
          //   histup->Scale(rand()%10);
          //   histdown->Scale(rand()%10);
          //}
	  if(m_upinputhistogram==0)
	    m_upinputhistogram=histup;
	  else
	    m_upinputhistogram->Add(histup);

	  if(m_downinputhistogram==0)
	    m_downinputhistogram=histdown;
	  else
	    m_downinputhistogram->Add(histdown);
	}
      }
    }
    
    if(m_upinputhistogram==0 || m_downinputhistogram==0){
      std::cout << "Did not find input histogram for systematic \"" << m_name << "\" "
		<< "channel \"" << m_channelname << "\" and sample \"" << m_samplename << "\"." << std::endl;
      return false;
    }
    
    return true;
  }

  //-------------------------------------------------------------------------------
  bool Systematic::deriveHistograms(UserTools* usertools, TH1F* nominalhistogram, float lumi){
    if(m_upfinalhistogram!=0) delete m_upfinalhistogram; m_upfinalhistogram=0;
    if(m_downfinalhistogram!=0) delete m_downfinalhistogram; m_downfinalhistogram=0;

    m_upfinalhistogram=usertools->rebinHistogram(m_upinputhistogram, m_channelname, m_samplename,m_name,1);
    m_upfinalhistogram->SetDirectory(0);
    usertools->produceEquidistantHistogram(m_upfinalhistogram);
    if(lumi>0.)
      m_upfinalhistogram->Scale(lumi/m_channel->lumi());

    //usertools->smoothHistogram(m_upfinalhistogram, m_channelname, m_samplename,m_name,1); //done later

    m_downfinalhistogram=usertools->rebinHistogram(m_downinputhistogram, m_channelname, m_samplename,m_name,1);
    m_downfinalhistogram->SetDirectory(0);
    usertools->produceEquidistantHistogram(m_downfinalhistogram);
    if(lumi>0.)
      m_downfinalhistogram->Scale(lumi/m_channel->lumi());

    //usertools->smoothHistogram(m_downfinalhistogram, m_channelname, m_samplename,m_name,1); //done later
    // Remove bins with negative content (set to 1E-6)
    for(int ibin=0; ibin<m_upfinalhistogram->GetNbinsX()+2; ibin++){
        if (m_upfinalhistogram->GetBinContent(ibin) < 0){
            std::cout << "WARNING! Systematic::deriveHistograms() - SETTING TO 1E-6 (AS VALUE IS NEGATIVE) BIN " 
		      << ibin << " FOR UP-SYST " << m_name << " IN CHANNEL " << m_channelname << " FOR SAMPLE " << m_samplename << std::endl;
            m_upfinalhistogram->SetBinContent(ibin, 1E-6);
        }
        if (m_downfinalhistogram->GetBinContent(ibin) < 0){
            std::cout << "WARNING! Systematic::deriveHistograms() - SETTING TO 1E-6 (AS VALUE IS NEGATIVE) BIN " 
		      << ibin << " FOR DOWN-SYST " << m_name << " IN CHANNEL " << m_channelname << " FOR SAMPLE " << m_samplename << std::endl;
            m_downfinalhistogram->SetBinContent(ibin, 1E-6);
        }
    }

    if(m_symmetrize){
	for(int ibin=0; ibin<m_upfinalhistogram->GetNbinsX()+2; ibin++){
	  double upbin=m_upfinalhistogram->GetBinContent(ibin);
	  double downbin=m_downfinalhistogram->GetBinContent(ibin);
	  double nombin=nominalhistogram->GetBinContent(ibin);
          //if(m_onesided){
          //  std::cout << "Systematic : " << m_name << std::endl;
          //  std::cout << "Start Values" << std::endl;
          //  std::cout << m_upfinalhistogram->Integral() << std::endl;
          //  std::cout << m_downfinalhistogram->Integral() << std::endl;
          //  std::cout << "up : " << upbin << std::endl;
          //  std::cout << "nom : " << nombin << std::endl;
          //  std::cout << "down : " << downbin << std::endl;
          //}
	  double sigma=fabs(upbin-downbin)/2.; //set sigma to the maximum difference between any combination of nombin, upbin and down (should almost always be upbin-downbin)
	  if(fabs(upbin-nombin)>sigma)
	    sigma=fabs(upbin-nombin);// /2.;
	  if(fabs(downbin-nombin)>sigma)
	    sigma=fabs(downbin-nombin);// /2.;

	  if(upbin>nombin){
	      m_upfinalhistogram->SetBinContent(ibin,nombin+sigma);
	      if(nombin-sigma<0)
		m_downfinalhistogram->SetBinContent(ibin,1E-6);
	      else
		m_downfinalhistogram->SetBinContent(ibin,nombin-sigma);
	    }
	    else{
	      m_downfinalhistogram->SetBinContent(ibin,nombin+sigma);
	      if(nombin-sigma<0)
		m_upfinalhistogram->SetBinContent(ibin,1E-6);
	      else
		m_upfinalhistogram->SetBinContent(ibin,nombin-sigma);
	    }
	
            //if(m_onesided){
            //  std::cout << "Final Values" << std::endl;
            //  std::cout << "up : "   << m_upfinalhistogram->GetBinContent(ibin)   << std::endl;
            //  std::cout << "nom : "  << nominalhistogram->GetBinContent(ibin)  << std::endl;
            //  std::cout << "down : " << m_downfinalhistogram->GetBinContent(ibin) << std::endl;
            //}
	  
	}//loop over bins
      // Remove bins with negative content (set to 1E-6)
      for(int ibin=0; ibin<m_upfinalhistogram->GetNbinsX()+2; ibin++){
          if (m_upfinalhistogram->GetBinContent(ibin) < 0) m_upfinalhistogram->SetBinContent(ibin, 1E-6);
          if (m_downfinalhistogram->GetBinContent(ibin) < 0) m_downfinalhistogram->SetBinContent(ibin, 1E-6);
      }
      //for(int ibin=0; ibin<m_upfinalhistogram->GetNbinsX()+2; ibin++){
      //  std::cout << m_channelname << " ibin "<<ibin << " up=" << m_upfinalhistogram->GetBinContent(ibin) << " down=" << m_downfinalhistogram->GetBinContent(ibin) << " upnorm=" << m_upfinalhistogram->GetSumOfWeights()/nominalhistogram->GetSumOfWeights() << " downnorm=" << m_downfinalhistogram->GetSumOfWeights()/nominalhistogram->GetSumOfWeights() << "up*norm=" << (m_upfinalhistogram->GetSumOfWeights()/nominalhistogram->GetSumOfWeights())*m_upfinalhistogram->GetBinContent(ibin) << " down*norm=" << (m_downfinalhistogram->GetSumOfWeights()/nominalhistogram->GetSumOfWeights())*m_downfinalhistogram->GetBinContent(ibin) << std::endl;
      //}
    }

    if(nominalhistogram->GetSumOfWeights()!=0){
      m_upnormalisation=m_upfinalhistogram->GetSumOfWeights()/nominalhistogram->GetSumOfWeights();
      m_upfinalhistogram->Scale(1/m_upnormalisation);
    }
    else{
      m_upnormalisation=1.;
    }

    if(nominalhistogram->GetSumOfWeights()!=0){
      m_downnormalisation=m_downfinalhistogram->GetSumOfWeights()/nominalhistogram->GetSumOfWeights();
      m_downfinalhistogram->Scale(1/m_downnormalisation);
    }
    else{
      m_downnormalisation=1.;
    }

    //if(m_name=="ATLAS_JES_EffectiveNP_1" && m_samplename=="Fake" && m_channelname=="Htt_year2015_chanhh_catvbflowdr_regsig_selCBA")
    //std::cout << "JULIANderive: down=" << m_downnormalisation << " up=" << m_upnormalisation << " upfinalhist=" << m_upfinalhistogram->GetSumOfWeights() << " downfinalhist=" << m_downfinalhistogram->GetSumOfWeights() << "nomfinalhist="<< m_sample->finalhist()->GetSumOfWeights() << " uphist=" << m_upinputhistogram->GetSumOfWeights() << " downhist=" << m_downinputhistogram->GetSumOfWeights() << " nomhist=" << m_sample->inputhist()->GetSumOfWeights() << std::endl;

    return true;
  }

  //-------------------------------------------------------------------------------
  bool Systematic::deriveFinalHistograms(UserTools* usertools, TH1F* nominalhistogram){

    if(this->isHistoSys()){
      m_upfinalhistogram->Scale(m_upnormalisation);

      //Kyle-Fix and smoothing is needed only for histograms used as shape systematics.
      //(Otherwise smoothing and Kyle-Fix will change the OverallSys as well)
      bool ret=usertools->kyleFixHistogram(m_upfinalhistogram, m_channelname, m_samplename,m_name,+1, m_sample->isSignal());
      if(nominalhistogram->GetSumOfWeights()!=0){
        m_upnormalisation=m_upfinalhistogram->GetSumOfWeights()/nominalhistogram->GetSumOfWeights();//Update OverallSys after smoothing
      }
      m_upfinalhistogram->Scale(1./m_upnormalisation);
      if(!ret){
        std::cerr<<"ERROR! Systematic::deriveFinalHistograms() - Problem with Kyle-Fix for channel "<<m_channelname<<" sample "<<m_samplename<<" +1sigma"<<std::endl;
        return false;
      }
      m_downfinalhistogram->Scale(m_downnormalisation);
      ret=usertools->kyleFixHistogram(m_downfinalhistogram, m_channelname, m_samplename,m_name,-1, m_sample->isSignal());
      if(nominalhistogram->GetSumOfWeights()!=0){
        m_downnormalisation=m_downfinalhistogram->GetSumOfWeights()/nominalhistogram->GetSumOfWeights();//Update OverallSys after smoothing
      }
      m_downfinalhistogram->Scale(1./m_downnormalisation);
      if(!ret){
        std::cerr<<"ERROR! Systematic::deriveFinalHistograms() - Problem with Kyle-Fix for channel "<<m_channelname<<" sample "<<m_samplename<<" -1sigma"<<std::endl;
        return false;
      }

      //enable smoothing code
      bool smoothShapes = true;
      if(smoothShapes){
        //apply smoothing on "up"
        m_upfinalhistogram->Scale(m_upnormalisation);
        ret=usertools->smoothHistogram(m_upfinalhistogram, nominalhistogram, m_channelname, m_samplename, m_name,+1);
        //
       if(nominalhistogram->GetSumOfWeights()!=0){
          m_upnormalisation=m_upfinalhistogram->GetSumOfWeights()/nominalhistogram->GetSumOfWeights();//Update OverallSys after smoothing
        }
        m_upfinalhistogram->Scale(1./m_upnormalisation);
        if(!ret){
          std::cerr<<"ERROR! Systematic::deriveFinalHistograms() - Problem with smoothing "<<m_channelname<<" sample "<<m_samplename<<" +1sigma"<<std::endl;
          return false;
        }
        //apply smoothing on "down"
        m_downfinalhistogram->Scale(m_downnormalisation);
        ret=usertools->smoothHistogram(m_downfinalhistogram, nominalhistogram, m_channelname, m_samplename, m_name,-1);
        if(nominalhistogram->GetSumOfWeights()!=0){
          m_downnormalisation=m_downfinalhistogram->GetSumOfWeights()/nominalhistogram->GetSumOfWeights();//Update OverallSys after smoothing
        }
        m_downfinalhistogram->Scale(1./m_downnormalisation);
        if(!ret){
          std::cerr<<"ERROR! Systematic::deriveFinalHistograms() - Problem with smoothing "<<m_channelname<<" sample "<<m_samplename<<" -1sigma"<<std::endl;
          return false;
        }
        //

      }
    }

    //////////////////////////////////////////////////////
    //So-called aggressive symmetrisation (default off)
    //////////////////////////////////////////////////////
    static uint aggressive_symmetrisation_notification(0);
    this->setIsAggressiveSymm(false);//Initialisation
    Bool_t applyLocalSymm = false;
    Bool_t applyGlobalSymm = false;
    if     (usertools->hasParameter("AggressiveSymmLocal" )) applyLocalSymm  = true;
    else if(usertools->hasParameter("AggressiveSymmGlobal")) applyGlobalSymm = true;
    else if(!aggressive_symmetrisation_notification++){
      std::cout<<"INFO: no aggressive symmetrisation will be applied since LocalSymmetrization = "
               <<applyLocalSymm<<" and GlobalSymmetrization = "<<applyGlobalSymm<<std::endl;
    }

    if(applyLocalSymm || applyGlobalSymm){
      if(this->isOverallSys() || this->isHistoSys()){
        // Check if histo should be "aggressively symmetrised"
        int doAggressive = 0;
        TH1F* myuphistogram   = (TH1F*)m_upfinalhistogram  ->Clone();
        TH1F* mydownhistogram = (TH1F*)m_downfinalhistogram->Clone();
        myuphistogram  ->Scale(m_upnormalisation  );
        mydownhistogram->Scale(m_downnormalisation);

        Long64_t originalNEntries = nominalhistogram->GetEntries();
        // std::cout<<std::endl;
        // std::cout<<"upnorm="<<m_upnormalisation<<", downnorm="<<m_downnormalisation<<", normWeight="<<nominalhistogram->GetSumOfWeights()<<std::endl;
        // std::cout<<"UpNorm="<<m_upnormalisation<<", DownNorm="<<m_downnormalisation<<std::endl;
        // std::cout<<"This NP will be used for "<<(isOverallSys()?"Overall":"none")<<", "<<(isHistoSys()?"Hist":"none")<<std::endl;

        for(int ibin=0; ibin<myuphistogram->GetNbinsX()+2; ibin++){
          double upbin  =myuphistogram   ->GetBinContent(ibin);
          double downbin=mydownhistogram ->GetBinContent(ibin);
          double nombin =nominalhistogram->GetBinContent(ibin);
          // std::cout<<upbin<<"(up), "<<downbin<<"(down), "<<nombin<<"(nom), "<<std::endl;
          if(upbin>nombin && downbin>nombin) doAggressive=1;
          if(upbin<nombin && downbin<nombin) doAggressive=1;
        }

        if(doAggressive){
          this->setIsOverallSys(true); //Force retaing OverallSys so that shape after symmetrisation has meaningful information
          this->setIsAggressiveSymm(true);

          //Global symmetrisation
          if(applyGlobalSymm){
            Double_t upyield   = myuphistogram  ->GetSumOfWeights();
            Double_t downyield = mydownhistogram->GetSumOfWeights();
            for(int ibin=0; ibin<myuphistogram->GetNbinsX()+2; ibin++){
              double upbin  =myuphistogram   ->GetBinContent(ibin);
              double downbin=mydownhistogram ->GetBinContent(ibin);
              double nombin =nominalhistogram->GetBinContent(ibin);
              //Get max-variation
              double maxvar = TMath::Max(fabs(upbin-nombin),fabs(downbin-nombin));
              if(nombin==0. && maxvar==0.) continue;
              if(upyield>downyield){
                // Set bin of up-histo to nom+maxvar, and down-histo to nom-maxvar
                myuphistogram  ->SetBinContent(ibin,TMath::Max(nombin+maxvar,0.0001));
                mydownhistogram->SetBinContent(ibin,TMath::Max(nombin-maxvar,0.0001));
              }else{
                // Set bin of up-histo to nom-maxvar, and down-histo to nom+maxvar
                myuphistogram  ->SetBinContent(ibin,TMath::Max(nombin-maxvar,0.0001));
                mydownhistogram->SetBinContent(ibin,TMath::Max(nombin+maxvar,0.0001));
              }
            }
          }
          //Local symmetrisation
          if(applyLocalSymm){
            for(int ibin=0; ibin<myuphistogram->GetNbinsX()+2; ibin++){
              double upbin  =myuphistogram   ->GetBinContent(ibin);
              double downbin=mydownhistogram ->GetBinContent(ibin);
              double nombin =nominalhistogram->GetBinContent(ibin);
              double upsig   = upbin   - nombin;
              double downsig = downbin - nombin;
              if((nombin-upbin)*(nombin-downbin) > 0){
                if(fabs(upsig)>fabs(downsig)){
                  myuphistogram->SetBinContent(ibin,nombin+upsig);
                  if(nombin-upsig<0) mydownhistogram->SetBinContent(ibin,1E-6);
                  else               mydownhistogram->SetBinContent(ibin,nombin-upsig);
                }
                else{
                  mydownhistogram->SetBinContent(ibin,nombin+downsig);
                  if(nombin-downsig<0) myuphistogram->SetBinContent(ibin,1E-6);
                  else                 myuphistogram->SetBinContent(ibin,nombin-downsig);
                }
              }
            }
          }

          //Checking bin contents after symmetrisation
          for(int ibin=0; ibin<myuphistogram->GetNbinsX()+2; ibin++){
            double upbin  =myuphistogram   ->GetBinContent(ibin);
            double downbin=mydownhistogram ->GetBinContent(ibin);
            double nombin =nominalhistogram->GetBinContent(ibin);
            // std::cout<<"After: "<<upbin<<"(up), "<<downbin<<"(down), "<<nombin<<"(nom), "<<std::endl;
            if(upbin>nombin && downbin>nombin) doAggressive=1;
            if(upbin<nombin && downbin<nombin) doAggressive=1;
          }

          //Set #entries since it was changed by SetBinContents()
          myuphistogram  ->SetEntries(originalNEntries);
          mydownhistogram->SetEntries(originalNEntries);

          //Set m_upfinalhistogram/m_downfinalhistogram to be myuphistogram/mydownhistogram
          m_upfinalhistogram  ->Delete();
          m_downfinalhistogram->Delete();
          m_upfinalhistogram   = myuphistogram  ;
          m_downfinalhistogram = mydownhistogram;

          //Normalise the histograms so that they again are shape only
          if(nominalhistogram->GetSumOfWeights()!=0){
            //          Double_t originalUpNorm   = m_upnormalisation;
            //          Double_t originalDownNorm = m_downnormalisation;
            m_upnormalisation  =m_upfinalhistogram  ->GetSumOfWeights()/nominalhistogram->GetSumOfWeights();
            m_downnormalisation=m_downfinalhistogram->GetSumOfWeights()/nominalhistogram->GetSumOfWeights();
            m_upfinalhistogram  ->Scale(1/m_upnormalisation  );
            m_downfinalhistogram->Scale(1/m_downnormalisation);
            //          m_upnormalisation  =originalUpNorm;
            //          m_downnormalisation=originalDownNorm;
          }else{
            m_upnormalisation=1.;
            m_downnormalisation=1.;
          }

          //Recode which histograms are aggressively symmetrised (To be implemented to save in a file.)
          std::cout<<"INFO: "<<m_channelname<<", "<<m_samplename<<", "<<m_name<<" are aggressively symmetrised."<<std::endl;
        }
      }
    }
    //finished special treatment for SMHtautau

    if (m_downnormalisation<=0){
      std::cout << "WARNING: Important problem with systematic \""<<m_name <<"\" in sample \""<<m_samplename<<"\" in channel\""<<m_channelname<<"\" as down normalisation="<<m_downnormalisation << " (up normalisation="<<m_upnormalisation<<"). If this is put like this in the likelihood function, its value will be n.a.n., I am setting the down normalisation to 0.00001, but this should not happen."<<std::endl;
      m_downnormalisation=0.00001;
    }

    if (m_upnormalisation<=0){
      std::cout << "WARNING: Important problem with systematic \""<<m_name <<"\" in sample \""<<m_samplename<<"\" in channel\""<<m_channelname<<"\" as up normalisation="<<m_upnormalisation << " (down normalisation="<<m_downnormalisation<<"). If this is put like this in the likelihood function, its value will be n.a.n., I am setting the up normalisation to 0.00001, but this should not happen."<<std::endl;
      m_upnormalisation=0.00001;
    }

    return true;
  }

  //-------------------------------------------------------------------------------

  bool Systematic::writeHistogramsToRootFile(TDirectory* sampledir){

    sampledir->cd();
    if(!isPrunedAway() || isHistoSys()){
      double firstbinwidth=m_upfinalhistogram->GetXaxis()->GetBinUpEdge(1)-m_upfinalhistogram->GetXaxis()->GetBinLowEdge(1);
      // cross-check if there are <0 bin contents <- should never happen with KyleFix
      for(int i=1; i<m_upfinalhistogram->GetNbinsX()+1; i++){
	if(m_upfinalhistogram->GetBinContent(i)<0. || m_downfinalhistogram->GetBinContent(i)<0.){
	  std::cout << "WARNING: For channel \"" << m_channelname << "\", sample \"" << m_samplename << "\" and systematic \"" << m_name << "\" the bin content in bin " << i << " <0: up=" <<  m_upfinalhistogram->GetBinContent(i) << " down="<<m_downfinalhistogram->GetBinContent(i) <<  std::endl;
	}
	double binwidth=m_upfinalhistogram->GetXaxis()->GetBinUpEdge(i)-m_upfinalhistogram->GetXaxis()->GetBinLowEdge(i);
	if(fabs((binwidth-firstbinwidth)/binwidth)>0.001)
	  std::cout << "WARNING: For channel \"" << m_channelname << "\", sample \""<< m_samplename<<"\" and systematic \"" << m_name << "\" (up) the bin width in bin " << i << " \""<<binwidth <<"\" is different from the width in the first bin \"" <<  firstbinwidth << "\". Non equi-distant binning will likely lead to problems with HistFactory."<< std::endl;
      }


      TH1F* histup=(TH1F*)m_upfinalhistogram->Clone();
      histup->SetDirectory(sampledir);
      TString systnameup=m_name+"_high";
      histup->SetName(systnameup);
      histup->Write();
      delete histup;

      // cross-check if there are <0 bin contents
      firstbinwidth=m_downfinalhistogram->GetXaxis()->GetBinUpEdge(1)-m_downfinalhistogram->GetXaxis()->GetBinLowEdge(1);
      for(int i=1; i<m_downfinalhistogram->GetNbinsX()+1; i++){
	if(m_downfinalhistogram->GetBinContent(i)<0.)
	  std::cout << "WARNING: For channel \"" << m_channelname << "\" and sample \"" << m_samplename << "\" and systematic \"" << m_name << "\" the bin content in bin " << i << " <0: " <<  m_downfinalhistogram->GetBinContent(i) << std::endl;
	double binwidth=m_downfinalhistogram->GetXaxis()->GetBinUpEdge(i)-m_downfinalhistogram->GetXaxis()->GetBinLowEdge(i);
	if(fabs((binwidth-firstbinwidth)/binwidth)>0.001)
	  std::cout << "WARNING: For channel \"" << m_channelname << "\", sample \""<< m_samplename<<"\" and systematic \"" << m_name << "\" (down) the bin width in bin " << i << " \""<<binwidth <<"\" is different from the width in the first bin \"" <<  firstbinwidth << "\". Non equi-distant binning will likely lead to problems with HistFactory."<< std::endl;
      }

      TH1F* histdown=(TH1F*)m_downfinalhistogram->Clone();
      histdown->SetDirectory(sampledir);
      TString systnamedown=m_name+"_low";
      histdown->SetName(systnamedown);
      histdown->Write();
      delete histdown;
    }
    return true;
  }

  //-------------------------------------------------------------------------------
  double Systematic::getOverallSysScaleFactor(std::map<std::string,double> parameters){
    TString npname="alpha_";
    npname+=m_name;
    std::map<std::string,double>::iterator it=parameters.find(npname.Data());
    if(it==parameters.end())
      return 1.;
    
    double alpha=it->second;

    if(m_upnormalisation <0.|| m_downnormalisation<0.){
      std::cout << "ERROR: getOverallSysScaleFactor failed for systematic \"" << m_name << "\" as normalisation doubles were not correctly initialized." << std::endl;
      throw SystematicNotInitialized(m_channelname, m_samplename, m_name);
    }

    it=parameters.find("ConsiderPrune");
    Bool_t considerPrune = it==parameters.end() ? kFALSE : kTRUE;
    if( (considerPrune && this->isOverallSys()) || considerPrune==kFALSE){
      if(alpha>0.) return pow(m_upnormalisation,alpha);
      else         return pow(m_downnormalisation,fabs(alpha));
    }

    return 1.;
  }

  //-------------------------------------------------------------------------------
  TH1F* Systematic::getHistoSysHistogramChange(std::map<std::string,double> parameters, TH1F* nominal){
    TString npname="alpha_";
    npname+=m_name;
    std::map<std::string,double>::iterator it=parameters.find(npname.Data());
    if(it==parameters.end())
      return 0;
    
    double alpha=it->second;

    if(m_upfinalhistogram ==0|| m_downfinalhistogram==0){
      std::cout << "ERROR: getHistoSysHistogramChange failed for systematic \"" << m_name << "\" as up/down histograms were not correctly initialized." << std::endl;
      throw SystematicNotInitialized(m_channelname, m_samplename, m_name);
    }
    if(alpha>0){
      TH1F* ret=(TH1F*)m_upfinalhistogram->Clone();
      ret->SetDirectory(0);
      //      m_upfinalhistogram->Scale(nominal->GetSumOfWeights()/m_upfinalhistogram->GetSumOfWeights()); //should have been done before, here just done for security
      ret->Add(nominal,-1.);
      ret->Scale(alpha);
      return ret;
    }
    else{
      TH1F* ret=(TH1F*)m_downfinalhistogram->Clone();
      //      m_downfinalhistogram->Scale(nominal->GetSumOfWeights()/m_downfinalhistogram->GetSumOfWeights()); //should have been done before, here just done for security
      ret->SetDirectory(0);
      ret->Add(nominal,-1.);
      ret->Scale(fabs(alpha));
      return ret;
    }

    return 0;
  }

  //-------------------------------------------------------------------------------
  RooStats::HistFactory::OverallSys Systematic::getHistFactoryOverallSys(){
    if(m_upnormalisation <0.|| m_downnormalisation<0.){
      std::cout << "ERROR: getHistFactoryOverAllSys failed for systematic \"" << m_name << "\" in sample \""<<m_samplename<<"\" and channel \""<<m_channelname<< "\"as normalisation doubles were not correctly initialized. u=" << m_upnormalisation << ", d=" <<m_downnormalisation << std::endl;
      throw SystematicNotInitialized(m_channelname, m_samplename, m_name);
    }

    RooStats::HistFactory::OverallSys overallsys;
    overallsys.SetName(m_name);
    overallsys.SetLow(m_downnormalisation);
    overallsys.SetHigh(m_upnormalisation);
    return overallsys;
  }

  //-------------------------------------------------------------------------------
  RooStats::HistFactory::HistoSys Systematic::getHistFactoryHistoSys(std::string channel, std::string sample, std::string rootfilename){
    if(m_upfinalhistogram ==0|| m_downfinalhistogram==0){
      std::cout << "ERROR: getHistFactoryHistoSys failed for systematic \"" << m_name << "\" as up/down histograms were not correctly initialized." << std::endl;
      throw SystematicNotInitialized(m_channelname, m_samplename, m_name);
    }
    RooStats::HistFactory::HistoSys histosys;
    histosys.SetName(m_name);
    histosys.SetInputFileLow(rootfilename);
    histosys.SetInputFileHigh(rootfilename);
    histosys.SetHistoPathLow(channel+"/"+sample+"/");
    histosys.SetHistoNameLow(m_name+"_low");
    histosys.SetHistoPathHigh(channel+"/"+sample+"/");
    histosys.SetHistoNameHigh(m_name+"_high");
    return histosys;
  }

  void Systematic::removeHistSys(int &pruneStatus){
    if     (pruneStatus == OnlyHistSys) pruneStatus = PrunedAway;
    else if(pruneStatus == NotPruned  ) pruneStatus = OnlyOverallSys;
    return;
  }

  void Systematic::removeOverallSys(int &pruneStatus){
    if     (pruneStatus == OnlyOverallSys) pruneStatus = PrunedAway;
    else if(pruneStatus == NotPruned     ) pruneStatus = OnlyHistSys;
    return;
  }

  void Systematic::addHistSys(int &pruneStatus){
    if     (pruneStatus == PrunedAway    ) pruneStatus = OnlyHistSys;
    else if(pruneStatus == OnlyOverallSys) pruneStatus = NotPruned;
    return;
  }

  void Systematic::addOverallSys(int &pruneStatus){
    if     (pruneStatus == PrunedAway ) pruneStatus = OnlyOverallSys;
    else if(pruneStatus == OnlyHistSys) pruneStatus = NotPruned;
    return;
  }

  bool Systematic::hasHistSys(int pruneStatus){
    bool retval = false;
    if(pruneStatus == OnlyHistSys || pruneStatus == NotPruned) retval = true;
    return retval;
  }

  bool Systematic::hasOverallSys(int pruneStatus){
    bool retval = false;
    if(pruneStatus == OnlyOverallSys || pruneStatus == NotPruned) retval = true;
    return retval;
  }

}
