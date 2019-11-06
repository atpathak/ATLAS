#include <WorkspaceBuilder/FakeUserTools.h>
#include <fstream>
#include <iostream>
#include <set>
#include <TCanvas.h>
#include <TROOT.h>
#include <TKey.h>
#include <TRandom3.h>
#include <TClass.h>
#include <sys/stat.h> //to provide mkdir()
#include <fstream> //to write tex file
#include <algorithm> //for max() function
#include <Math/DistFunc.h> //for chi2 cdf function
#include "boost/algorithm/string.hpp"

namespace WorkspaceBuilder{

  //-------------------------------------------------------------------------------
  FakeUserTools::FakeUserTools(std::string statanalysisname,
                             std::string outputpath,
                             std::string version,
                             std::vector<std::string> rootpaths,
                             std::vector<std::string> normfactortxtfilenames,
                             std::vector<std::string> overallsystematicfilenames,
                             std::map<std::string,std::string> parameters,
                             bool unblind,
                             bool info) : UserTools(rootpaths, parameters, unblind, info){
    std::cout << "INFO: "<<statanalysisname<< " ctor" << std::endl;
    setOutputPath(outputpath);
    m_statanalysisname=statanalysisname;
    m_version=version;
    for(unsigned int i=0; i<normfactortxtfilenames.size(); i++){
      std::ifstream txtfile (normfactortxtfilenames.at(i).c_str());
      if (txtfile.is_open()){
        std::string normfactorname="";
        double nominalvalue=0.;
        double lowvalue=0.;
        double highvalue=0.;
        while ( !txtfile.eof() ){
          txtfile >> normfactorname >> nominalvalue >> lowvalue >> highvalue;
          m_normfactor_name.push_back(normfactorname);
          m_normfactor_value.push_back(nominalvalue);
          m_normfactor_low.push_back(lowvalue);
          m_normfactor_high.push_back(highvalue);

        }
        txtfile.close();
      }
    }
    for(unsigned int i=0; i<overallsystematicfilenames.size(); i++){
      std::ifstream txtfile (overallsystematicfilenames.at(i).c_str());
      if (txtfile.is_open()){
        std::string overallsystematicname="";
        double value=0.;
        while ( !txtfile.eof() ){
          txtfile >> overallsystematicname >> value;
          //std::cout << "JULIAN: Using overallsyst: " << overallsystematicname << std::endl;
          m_overallsystematic_name.push_back(overallsystematicname);
          m_overallsystematic_value.push_back(value);
        }
        txtfile.close();
      }
    }
    std::map<std::string,std::string>::iterator it=parameters.find("ConstantNuisanceParameterFile");
    if(it!=parameters.end()){
      std::ifstream txtfile (it->second.c_str());
      if (txtfile.is_open()){
        std::string nuisanceparametername="";
        while ( !txtfile.eof() ){
          txtfile >> nuisanceparametername;
          m_constantnuisanceparameter.push_back(nuisanceparametername);
          std::cout << "INFO: Setting " << nuisanceparametername << " to const." << std::endl;
        }
        txtfile.close();
      }
    }
    it=parameters.find("CorrelatedNuisanceParameterFile");
    if(it!=parameters.end()){
      std::ifstream txtfile (it->second.c_str());
      if (txtfile.is_open()){
        //std::string nuisanceparametername="";
        while ( !txtfile.eof() ){
          std::string line;
          std::getline(txtfile, line);
          std::stringstream  data(line);

          std::string newnpname;
          std::getline(data,newnpname,' ');
          std::string field;
          while(std::getline(data,field,' ')){
            if(field!=""){
              m_correlatednuisanceparameter.insert(std::make_pair<TString,TString>(field,newnpname));
              std::cout << "INFO: Applying correlation for " << field << " -> " << newnpname << std::endl;
            }
          }
        }
        txtfile.close();
      }
    }
    m_pruning_pvaluechi2=new TH1F("pruning_chi2","pruning_chi2",1000,0.,1.);
    m_pruning_pvaluechi2->SetDirectory(0);
    m_pruning_maxvariation=new TH1F("pruning_maxvariation","pruning_maxvariation",100,0.,1.);
    m_pruning_maxvariation->SetDirectory(0);
    m_pruning_maxrelvariation=new TH1F("pruning_maxrelvariation","pruning_maxrelvariation",100,0.,1.);
    m_pruning_maxrelvariation->SetDirectory(0);

    TString dircmd="mkdir -p ";
    dircmd+=m_outputpath;
    dircmd+="/";
    dircmd+=m_version;
    dircmd+="/monitoring/";
    system(dircmd.Data());

    TString filename=m_outputpath;
    filename+="/";
    filename+=m_version;
    filename+="/monitoring/onesidedsystematics_";
    filename+=m_statanalysisname;
    filename+=".txt";

    m_onesidedsystematicsfile.open (filename.Data(), std::ofstream::out);

    filename=m_outputpath;
    filename+="/";
    filename+=m_version;
    filename+="/monitoring/kylefix_";
    filename+=m_statanalysisname;
    filename+=".txt";
    m_kylefixfile.open (filename.Data(), std::ofstream::out);

    m_nkylefixedbins_all=0;
    m_nkylefixedbins_nominal=0;
    m_sumwkylefixedbins_nominal=0;
    //make a list of all histograms in the root files so that later we can check if all of them have been used...
    for(unsigned int ifile=0; ifile<m_rootfiles.size(); ifile++){
      fillUsedHistogramsMap(m_rootfiles[ifile],"");
    }
    //print params
    if(m_parameters.size()){
      std::cout<<"INFO: FakeUserTools::FakeUserTools() Parameters: \n";
      for(const auto param : m_parameters){
        std::cout<<"\t"<<param.first<<" "<<param.second<<"\n";
      }
      std::cout<<std::endl;
    }

  }

  //-------------------------------------------------------------------------------
  FakeUserTools::~FakeUserTools(void){
    TString dircmd="mkdir -p ";
    dircmd+=m_outputpath;
    dircmd+="/";
    dircmd+=m_version;
    dircmd+="/monitoring/";
    system(dircmd.Data());
    TString rootfilename=m_outputpath;
    rootfilename+="/";
    rootfilename+=m_version;
    rootfilename+="/monitoring/";
    rootfilename+=m_statanalysisname;
    rootfilename+=".root";

    TFile *m_monitoringfile=new TFile(rootfilename,"RECREATE");
    TDirectory* dir=m_monitoringfile->mkdir("Pruning");
    m_pruning_maxvariation->SetDirectory(dir);
    m_pruning_maxrelvariation->SetDirectory(dir);
    m_pruning_pvaluechi2->SetDirectory(dir);

    m_monitoringfile->Write();
    std::cout<<"INFO: FakeUserTools::~FakeUserTools() - File '"+rootfilename+"' is created."<<std::endl;
    m_monitoringfile->Close();
    if(m_monitoringfile) delete m_monitoringfile;

    m_onesidedsystematicsfile.close();
    m_kylefixfile.close();

    std::cout << "INFO: " << m_nkylefixedbins_all << " bins have been kyle-fixed." << std::endl;
    std::cout << "INFO: " << m_nkylefixedbins_nominal << " bins have been kyle-fixed for the nominal histograms (sumw="<<m_sumwkylefixedbins_nominal<<")." << std::endl;


    if(m_parameters.find("PrintUnusedHistograms")!=m_parameters.end()){
      std::cout << "INFO: The following histograms have not been used while building the workspace:" << std::endl;
      for(std::map<TString, bool>::iterator it=m_usedhistograms.begin(); it!=m_usedhistograms.end(); it++){
        if(!it->second)
          std::cout << "WARNING: The histogram \"" << it->first << "\" was not used in building this workspace. It might be worth checking if this is intended." << std::endl;
      }
    }
  }

  //-------------------------------------------------------------------------------
  void FakeUserTools::fillUsedHistogramsMap(TDirectory* dir, TString dirname){
    //loop over files

    TIter next(dir->GetListOfKeys());
    TKey *key;
    while ((key = (TKey*)next())){
      if(gROOT->GetClass(key->GetClassName())->InheritsFrom("TDirectory")){
        TObject* obj=key->ReadObj();
        fillUsedHistogramsMap((TDirectory*)obj,dirname+"/"+key->GetName());
      }
      else if(gROOT->GetClass(key->GetClassName())->InheritsFrom("TH1")){
        if(TString(key->GetName())!="lumiininvpb"){
          m_usedhistograms.insert(std::make_pair<TString,bool>(dirname+"/"+key->GetName(),false));
          //std::cout<<"INFO: FakeUserTools::fillUsedHistogramsMap() - registering input: "<<dirname.Data()<<" "<<key->GetName()<<std::endl;
        }
      }
      else{
        std::cout << "WARNING: Found object \"" << key->GetName() << "\" of type \"" << key->GetClassName() << "\" which is neither a TDirectory or a TH1*, will ignore this object." << std::endl;
      }
    }
  }

  //-------------------------------------------------------------------------------
  TH1F* FakeUserTools::getHistogram(std::string channel, std::string sample, std::string systematic, int up){
    for(unsigned int ifile=0; ifile<m_rootfiles.size(); ifile++){
      m_rootfiles.at(ifile)->cd();
      TDirectory* channeldir=m_rootfiles.at(ifile)->GetDirectory(channel.c_str());
      if(channeldir!=0){
        TDirectory* sampledir=channeldir->GetDirectory(sample.c_str());
        if(sampledir!=0){
          TString sysname=systematic;
          if(up<0)
            sysname+="_low";
          if(up>0)
            sysname+="_high";
          TH1F* hist=(TH1F*)sampledir->Get(sysname.Data());
          if(hist!=0){
            if( m_parameters.find("RunRandomMu")!=m_parameters.end()){
              bool isgghxx = boost::algorithm::contains(sample, "ggH");
              bool isvbfxx = boost::algorithm::contains(sample, "VBF");
              if( isgghxx || isvbfxx ){
                srand(m_rndmseed);
                int rndval = rand() % 100 + 1; //random number [1, 100]
                double rndf = rndval*0.1; //random number [1, 100]
                //          std::cout<<"RNDM = "<<rndf<<std::endl;
                //          std::cout << "Rescaling sample : " << sample << std::endl;
                //          std::cout << "Before : " << hist->GetSumOfWeights() << std::endl;
                hist->Scale(rndf);
                //          std::cout << "After : "  << hist->GetSumOfWeights() << std::endl;
              }
            }
            TString histname=channel+"_"+sample+"_"+systematic+"_"+TString(up);
            hist->SetName(histname);
            hist->SetDirectory(0);

            //mark this histogram as being used
            TString nameinfile="/"+channel+"/"+sample+"/"+sysname;
            std::map<TString, bool>::iterator it=m_usedhistograms.find(nameinfile);
            if(it!=m_usedhistograms.end())
              it->second=true;
            else{
              std::cout << "ERROR: The histogram with name \"" << nameinfile << "\" was not found in m_usehistograms." << std::endl;
            }

            //UGLY FIX BY JULIAN NEVER TO BE USED IN PRODUCTION
            if( m_parameters.find("RemoveSignalOutside100150")!=m_parameters.end()){
              if(sample=="ggH" ||
                 sample=="ggHWW" ||
                 sample=="VBFH" ||
                 sample=="VBFHWW" ||
                 sample=="WH" ||
                 sample=="WHWW" ||
                 sample=="ZH" ||
                 sample=="ZHWW" ){
                std::cout << "WARNING: UGLY FIX TO REMOVE SIGNAL in [100,150] GeV SHOULD NEVER BE USED IN PRODUCTION!" << std::endl;
                for(int ibin=0; ibin<hist->GetNbinsX()+2; ibin++){
                  if(hist->GetXaxis()->GetBinUpEdge(ibin)<100.
                     || (hist->GetXaxis()->GetBinLowEdge(ibin)>=150 && hist->GetXaxis()->GetBinUpEdge(ibin)<100000.) ||
                     hist->GetXaxis()->GetBinLowEdge(ibin)>=150000. ){
                    hist->SetBinContent(ibin,0.);
                    hist->SetBinError(ibin,0.);
                  }
                }
              }
            }

            /*if(channel=="lh_boost"){
              std::cout << "WARNING: UGLY FIX BY JULIAN SHOULD NEVER BE USED IN PRODUCTION" << std::endl;
              TH1F* hist2=new TH1F(histname,hist->GetTitle(), hist->GetNbinsX()-1, hist->GetXaxis()->GetBinLowEdge(1), hist->GetXaxis()->GetBinUpEdge(hist->GetNbinsX()));
              for(int i=0; i<hist->GetNbinsX()-1; i++){
              hist2->SetBinContent(i,hist->GetBinContent(i));
              hist2->SetBinError(i,hist->GetBinError(i));
              }
              hist2->SetBinContent(hist->GetNbinsX()-1,hist->GetBinContent(hist->GetNbinsX()-1)+hist->GetBinContent(hist->GetNbinsX()));
              hist2->SetBinError(hist->GetNbinsX()-1,sqrt(pow(hist->GetBinError(hist->GetNbinsX()-1),2.)+pow(hist->GetBinError(hist->GetNbinsX()),2.)));
              hist2->SetBinContent(hist->GetNbinsX(),hist->GetBinContent(hist->GetNbinsX()+1));
              hist2->SetBinError(hist->GetNbinsX(),hist->GetBinError(hist->GetNbinsX()+1));
              return hist2;
              }*/
            //UGLY FIX JULIAN NEVER TO BE USED IN PRODUCTION

            return hist;
          }
        }
      }
    }

    std::cout << "ERROR: Histogram for channel \"" << channel << "\", "
              << "sample \""<< sample << "\" "
              << "systematic \"" << systematic << "\" "
              << " and up \"" << up << "\" was not found. Return 0."
              << std::endl;
    throw HistNotFound(channel, sample, systematic, up);

    return 0;
  }

  //-------------------------------------------------------------------------------

  double FakeUserTools::getOverallSys(std::string channel, std::string sample, std::string systematic, int up){

    TString systname=systematic;
    if(up>=1)
      systname+="_high";
    else
      systname+="_low";

    for(unsigned int i=0; i<m_overallsystematic_name.size(); i++){
      if(m_overallsystematic_name[i]==systname ||
         m_overallsystematic_name[i]==sample+"_"+systname ||
         m_overallsystematic_name[i]==channel+"_"+sample+"_"+systname)
        return m_overallsystematic_value[i];
    }

    std::cout << "ERROR: OverallSys for channel \"" << channel << "\", "
              << "sample \""<< sample << "\" "
              << "systematic \"" << systematic << "\" "
              << " and up \"" << up << "\" was not found. Return 1."
              << std::endl;
    throw OverallSysNotFound(channel, sample, systematic, up);
    return 1.;
  }

  //-------------------------------------------------------------------------------

  double FakeUserTools::getLumiInInvPb(std::string channel){
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

    std::cout << "ERROR: Could not find luminosity information in histogram lumiininvpb for channel \"" << channel << "\"." << std::endl;
    return -1.;
  }

  //-------------------------------------------------------------------------------
  std::string FakeUserTools::description(){
    return "Available options with -p: \n\tNoSmoothing\n\tSymmetrize\n\tNoKyleFix\n\tNoPruning\n\t78TeVPruning for fake pruning to make 7/8 TeV input files usable\n\tPrintUnusedHistograms to print a list of histograms in the root files that are not used\n\tConstantNuisanceParameterFile <filename> to set the nuisance parameters listed in this file to constant (1 column, alpha_ can be omitted)\n\tCorrelatedNuisanceParameterFile <filename> to set the nuisance parameters listed in one line of this file to correlated (1. column: new name of NP, 2./3./4./... input nuisance parameter names)\n\tRemoveSignalOutside100150 sets the signal histograms to 0 outsideof [100,150] GeV.";
  }

  //-------------------------------------------------------------------------------
  double FakeUserTools::getNormFactor(std::string normfactor, int up){
    for(unsigned int inorm=0; inorm<m_normfactor_name.size(); inorm++){
      if(m_normfactor_name[inorm]==normfactor){
        if(up==-1)
          return m_normfactor_low[inorm];
        if(up==0)
          return m_normfactor_value[inorm];
        if(up==1)
          return m_normfactor_high[inorm];
      }
    }

    std::cout << "ERROR: Normfactor with name \"" << normfactor << "\" variation up="<<up<< " was not found in input file." << std::endl;
    throw NormFactorNotFound(normfactor, up);

    return 0;
  }

  //-------------------------------------------------------------------------------
  bool FakeUserTools::isNuisanceParameterConstant(TString npname){

    bool alpha=npname.BeginsWith("alpha_");
    for(unsigned int i=0; i<m_constantnuisanceparameter.size(); i++){
      if(alpha && (npname==m_constantnuisanceparameter[i] || npname==TString("alpha_")+m_constantnuisanceparameter[i])){
        return true;
      }
      if(!alpha && (npname==m_constantnuisanceparameter[i] || TString("alpha_")+npname==m_constantnuisanceparameter[i])){
        return true;
      }
    }

    return false;
  }

  //-------------------------------------------------------------------------------
  TString FakeUserTools::correlatedSystematicName(TString name){
    std::map<TString,TString>::iterator it=m_correlatednuisanceparameter.find(name);
    if(it!=m_correlatednuisanceparameter.end())
      return it->second;

    return name;
  }

  //-------------------------------------------------------------------------------
  bool FakeUserTools::smoothHistogram(TH1F* input, TH1F* nominal, std::string channel, std::string sample, std::string systematic, int up){

    if(m_parameters.find("NoSmoothing")==m_parameters.end()){
      std::cout << "INFO: FakeUserTools::smoothHistogram() - No Smoothing will be applied!" << std::endl;
      return true;
    }

    return true;
  }

  //-------------------------------------------------------------------------------
  bool FakeUserTools::kyleFixHistogram(TH1F* input, std::string channel, std::string sample, std::string systematic, int up, bool issignal){

    if(m_parameters.find("NoKyleFix")==m_parameters.end() && !issignal && input->GetEntries()>0){ // don't kyle-fix signal as this is not conservative and don't kyle fix histograms which have no entry at all as kyle-fixing will fail
      double sumW2TotBin = 0.;

      for(int j=1;j<input->GetNbinsX()+1;j++){ //No OVER/UNDER-FLOW bins considered!
        sumW2TotBin += pow( input->GetBinError(j) , 2);
      }

      double avWeightBin = input->GetSumOfWeights() /(double)input->GetEntries();
      double avW2Bin = sumW2TotBin / (double)input->GetEntries();

      if(input->GetEntries() == 0.0){
        std::cout<<"ERROR! Histogram has 0 entries! Not meaningful to Kyle-fix, please look this over!! Will fail now. We are at channel \""<<channel << "\", sample \"" << sample << "\", systematic \""<<systematic<<"\" ("<<up<<")"<<std::endl;
        return false;
      }

      if(avWeightBin <= 0){
        //std::cout <<  << std::endl;
        std::cout<<"WARNING! Potential Kyle-fix value is NEGATIVE (avWeightBin="<<avWeightBin<<", GetEntries()=" << input->GetEntries() << ", GetSumOfWeights()=" << input->GetSumOfWeights()<<"). This can happen if the integral of the histogram is negative. Clearly pathological, needs to be fixed at an earlier stage... For now setting it to 0.0001, but look into this!!!! We are at channel \""<<channel << "\", sample \"" << sample << "\", systematic \""<<systematic<<"\" ("<<up<<")"<<std::endl;
        if(m_info) std::cout<<"GetSumOfWeights="<<input->GetSumOfWeights()<<" GetEntries="<<input->GetEntries()<<" avWeightBin="<<avWeightBin<<std::endl;
        avWeightBin = 0.0001;
      }

      if(avWeightBin>2){
        //std::cout << "avWeightBin="<<avWeightBin<<", input->GetEntries()==" << input->GetEntries() << ", input->GetSumOfWeights()==" << input->GetSumOfWeights() << std::endl;
        std::cout<<"WARNING! Potential Kyle-fix value is rather large, >2 (avWeightBin="<<avWeightBin<<", GetEntries()=" << input->GetEntries() << ", GetSumOfWeights()=" << input->GetSumOfWeights()<<"). Continuing for now, but you may want to look into it. We are at channel \""<<channel << "\", sample \"" << sample << "\", systematic \""<<systematic<<"\"("<<up<<")"<<std::endl;
      }

      std::vector<double> kylefixedweights;
      for(int j=1;j<input->GetNbinsX()+1;j++){ //No OVER/UNDER-FLOW bins considered!
        if(input->GetBinContent(j) <= 0){
          if(m_info) std::cout<<"INFO: Need to apply KyleFix in channel \""<<channel << "\", sample \"" << sample << "\", systematic \""<<systematic<<"\" ("<<up<<") bin nr"<< j <<"!! Kyle-fixed bin-content: "<<avWeightBin<<" and error: "<<avW2Bin<<" Original bin-content:"<<input->GetBinContent(j)<<std::endl;
          if(channel=="Htt_year2016_chanll_catvbf_regztt_selCBA" && sample=="Ztt" && (systematic=="LumiUnc2016" || systematic=="nominal")){
            std::cout << "JULIAN: Applying kyle-fix in bin " << j << " systematic="<<systematic<<" weight=" << avWeightBin << " sum="<<input->GetSumOfWeights() <<" entries="<<input->GetEntries()<<std::endl;
          }
          input->SetBinContent(j,avWeightBin);
          input->SetBinError(j,avW2Bin);
          kylefixedweights.push_back(avWeightBin);
          m_nkylefixedbins_all++;
          if(systematic=="nominal"){
            m_nkylefixedbins_nominal++;
            m_sumwkylefixedbins_nominal+=avWeightBin;
          }
        }
      }

      if(kylefixedweights.size()>0){
        double sum=0;
        for(unsigned int i=0; i<kylefixedweights.size(); i++) sum+=kylefixedweights[i];
        m_kylefixfile << "Kyle-fix applied \t " << channel << " \t " << sample << " \t " << systematic << " \t in " << kylefixedweights.size() << " bins -> added weight \t " << sum << std::endl;
      }


    }

    return true;
  }

  //-------------------------------------------------------------------------------
  int FakeUserTools::pruneSystematic(Systematic* systematic, Channel* channel, Sample* sample){
    int retvalue=3;
    // Sample class:
    // 0 : Everything pruned
    // 1 : Shape pruned
    // 2 : Normalisation pruned
    // 3 : Nothing pruned

    //no pruning
    if(m_parameters.find("NoPruning")!=m_parameters.end()){
      std::cout<<"INFO: FakeUserTools::pruneSystematic() - No NP pruning will be applied!\n";
      return 3;
    }

    //standard pruning
    std::cout<<"INFO: FakeUserTools::pruneSystematic() - Applying Standard Pruning"<<std::endl;
    ///Pruning 1: A χ^2 test between the upwards and the downwards fluctuated shape with respect to the nominal is performed, for each potential shape systematic NP and for each sample. It should be noted that in this calculation the statistical uncertainty that enters is only the largest of the nominal or varied one, rather than both (since they are typically very strongly correlated). The shape systematic is retained if the result of the χ^2 test is less than a threshold value, for either of the upwards or downwards fluctuated shape, where the exact threshold value differs between the channels. If neither the upward nor the downward variation’s probability is lower than that threshold, the shape variation is considered to not be significant for the given background sample, and the shape NP is not used in the fit (the corresponding normalization uncertainty is still kept however). This pruning criterion is not applied to those systematics whose variation only occurs through the variation of weights of the MC events – these are always kept.

    double chi2up=0.;
    double chi2down=0.;
    for(int ibin=1; ibin<systematic->getUpHistogram()->GetNbinsX()+1; ibin++){
      double diffup=systematic->getUpHistogram()->GetBinContent(ibin)-sample->getFinalHistogram()->GetBinContent(ibin);
      double statuncert=sample->getFinalHistogram()->GetBinError(ibin);
      double statuncertup=systematic->getUpHistogram()->GetBinError(ibin);
      if(diffup!=0 && (statuncertup!=0 || statuncert!=0))
        chi2up+=pow(diffup,2.)/pow(std::max(statuncertup,statuncert),2.);
      double diffdown=systematic->getDownHistogram()->GetBinContent(ibin)-sample->getFinalHistogram()->GetBinContent(ibin);
      double statuncertdown=systematic->getDownHistogram()->GetBinError(ibin);
      if(diffdown!=0 && (statuncertdown!=0 || statuncert!=0))
        chi2down+=pow(diffdown,2.)/pow(std::max(statuncertdown,statuncert),2.);
    }
    //chi2up/=systematic->getUpHistogram()->GetNbinsX()-1;
    //chi2down/=systematic->getDownHistogram()->GetNbinsX()-1;
    if( systematic->hasTag("FourVectorSystematic") ){
      double pvalue_chi2up  =ROOT::Math::chisquared_cdf(chi2up,systematic->getUpHistogram()->GetNbinsX()-1);
      double pvalue_chi2down=ROOT::Math::chisquared_cdf(chi2down,systematic->getUpHistogram()->GetNbinsX()-1);

      double threshold=0.10;

      m_pruning_pvaluechi2->Fill(pvalue_chi2up);
      m_pruning_pvaluechi2->Fill(pvalue_chi2down);
      //double chi2up_pval = (sample->getFinalHistogram())->Chi2Test(systematic->getUpHistogram(),"WW");
      //double chi2dn_pval = (sample->getFinalHistogram())->Chi2Test(systematic->getDownHistogram(),"WW");
      //std::cout << "Chi2 Up : " << pvalue_chi2up   << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
      //std::cout << "Chi2 Dn : " << pvalue_chi2down << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
      //std::cout << "pval Up : " << chi2up_pval << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
      //std::cout << "pval Dn : " << chi2dn_pval << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;

      if(pvalue_chi2up<=threshold && pvalue_chi2down<=threshold){
        //std::cout << "Pruning 1 : " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
        retvalue=1;
      }
      // if(chi2up_pval>=threshold && chi2dn_pval>=threshold){
      //   std::cout << "Pruning 1 : " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
      //   std::cout << "pval Up : " << chi2up_pval << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
      //   std::cout << "pval Dn : " << chi2dn_pval << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
      //   retvalue=1;
      // }
    }
    else{
      //std::cout << "Chi2 Up : " << chi2up   << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
      //std::cout << "Chi2 Dn : " << chi2down << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
      if(chi2up/(systematic->getUpHistogram()->GetNbinsX()-1.)<=1E-10 && chi2down/(systematic->getUpHistogram()->GetNbinsX()-1.)<=1E-10){
        //std::cout << "Pruning 1 : " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
        //std::cout << "Chi2 Up : " << chi2up   << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
        //std::cout << "Chi2 Dn : " << chi2down << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
        retvalue=1;
        //if(systematic->name()=="lh_fake_stat")
        //std::cout << "JULIAN: Pruning chi2 up="<<(chi2up/systematic->getUpHistogram()->GetNbinsX()-1.) << " down="<<(chi2down/systematic->getUpHistogram()->GetNbinsX()-1.)<< std::endl;
      }
    }

    //Pruning 2: if maximum relative variation between nominal and up/down in any bin is <0.5, the shape is ignored
    float prune2_threshold = 0.1; //0.5
    double maxrelvariation=0.;
    for(int ibin=1; ibin<systematic->getUpHistogram()->GetNbinsX()+1; ibin++){
      double diff=systematic->getUpHistogram()->GetBinContent(ibin)-sample->getFinalHistogram()->GetBinContent(ibin);
      double variation=fabs(diff/sample->getFinalHistogram()->GetBinError(ibin));
      if(variation>maxrelvariation)
        maxrelvariation=variation;

      diff=systematic->getDownHistogram()->GetBinContent(ibin)-sample->getFinalHistogram()->GetBinContent(ibin);
      variation=fabs(diff/sample->getFinalHistogram()->GetBinError(ibin));
      if(variation>maxrelvariation)
        maxrelvariation=variation;
    }

    m_pruning_maxrelvariation->Fill(maxrelvariation);

    if(maxrelvariation< prune2_threshold) {
      std::cout << "Pruning 2 : " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
      retvalue=1;
    }

    //Pruning 3: For a potential shape systematic to be considered significant and thus used in the fit, it must also fulfill the following additional pruning criterion, which is checked individually for each systematic for each background sample: the maximum bin-by-bin variation significance, maxi S i should be at least 0.1, where the variation significance S i is defined as S i = |ui − di|/σtoti, with ui (di) being the upwards (downwards) variation in bin i for a given background sample, while σtot is the statistical uncertainty for the total background estimation (i.e. for all samples) in bin i. Thus if for a given background sample, for a given systematic uncertainty, S i < 0.1 for all bins of the discriminating variable, then the shape variation is considered non-significant (for this particular background sample), and not considered further

    std::map<std::string,double> parameters;
    parameters.insert(std::pair<std::string,double>("SigXsecOverSM",0.));
    TH1F* sumofmc=channel->getSumOfMCHistogram(parameters);

    double maxvariation=0.;
    for(int ibin=1; ibin<systematic->getUpHistogram()->GetNbinsX()+1; ibin++){
      double diff=systematic->getUpHistogram()->GetBinContent(ibin)-systematic->getDownHistogram()->GetBinContent(ibin);
      double variation=fabs(diff)/fabs(sumofmc->GetBinError(ibin));
      if(variation>maxvariation)
        maxvariation=variation;
    }

    m_pruning_maxvariation->Fill(maxvariation);

    //if(maxvariation<0.1 && !(sample->isSignal())){ // 0.5% and 10% value obtained from Nils thesis
    //  retvalue=1; //use as histosys and overallsys
    //}

    // Incorrect logic, this retains if already pruned, it does not prune further
    //if((maxvariation>0.1 || sample->isSignal()) && maxrelvariation>0.005){ // 0.5% and 10% value obtained from Nils thesis
    //  retvalue=3; //use as histosys and overallsys
    //}

    // Pruning OVERALLSYSTS
    double MCuncert=0.;
    TH1F* nominal=sample->getFinalHistogram();
    for(int i=0; i<nominal->GetNbinsX()+2; i++){
      MCuncert+=nominal->GetBinError(i)*nominal->GetBinError(i);
    }

    MCuncert=sqrt(MCuncert)/nominal->GetSumOfWeights();

    bool pruneNorm = false;

    bool doflatprune = true;
    bool doonesideprune = true;
    bool dozttpruning = true;

    // prune using MC uncertainty
    //if (fabs(systematic->getUpNormalisation()-1.)<0.2*MCuncert && fabs(systematic->getDownNormalisation()-1.)<0.2*MCuncert) pruneNorm = true;
    //if (TString(sample->name()).Contains("HWW") || TString(sample->name()).Contains("ZH") || TString(sample->name()).Contains("WH") || TString(sample->name()).Contains("ttH")){
    //  if (fabs(systematic->getUpNormalisation()-1.)<0.2*MCuncert && fabs(systematic->getDownNormalisation()-1.)<0.2*MCuncert) pruneNorm = true;
    //}

    // prune using fixed 1%
    if (doflatprune){
      if (fabs(systematic->getDownNormalisation()-1.) < 0.01 && fabs(systematic->getUpNormalisation()-1.) < 0.01) pruneNorm = true;
    }

    if (doonesideprune){
      if ((systematic->getUpNormalisation()-1.)*(systematic->getDownNormalisation()-1.) > 0){
        if (fabs(systematic->getUpNormalisation()-1.) < 0.2*MCuncert && fabs(systematic->getDownNormalisation()-1.) < 0.2*MCuncert){
          pruneNorm = true;
        }
        if (fabs(systematic->getUpNormalisation() - systematic->getDownNormalisation()) < 0.2*MCuncert) pruneNorm = true;
        if (fabs(systematic->getUpNormalisation() - systematic->getDownNormalisation()) < 1E-3) pruneNorm = true;
      }
    }

    if (dozttpruning){
      if( TString(channel->name()).Contains("regztt") && !TString(sample->name()).Contains("Zll")){
        if (fabs(systematic->getUpNormalisation()-1.) < 0.2*MCuncert && fabs(systematic->getDownNormalisation()-1.) < 0.2*MCuncert){
          pruneNorm = true;
        }
      }
      if (pruneNorm && TString(channel->name()).Contains("regztt") && TString(sample->name()).Contains("Zll")){
        if (fabs(systematic->getUpNormalisation()-1.) > 0.2*MCuncert && fabs(systematic->getDownNormalisation()-1.) > 0.2*MCuncert) pruneNorm = false;
      }
    }
    //if (pruneNorm){
    //  if (fabs(systematic->getUpNormalisation()-1.) > 0.2*MCuncert && fabs(systematic->getDownNormalisation()-1.) > 0.2*MCuncert) pruneNorm = false;
    //}
    if(retvalue==1 && pruneNorm && !TString(systematic->name()).Contains("Madgraph")){
      retvalue=0;
    }

    if(retvalue==3 && pruneNorm && !TString(systematic->name()).Contains("Madgraph")){
      retvalue=2;
    }

    //document one-sided systematics into a txt file
    //if(retvalue==2 || retvalue==3){ //for shape systematics check if any bin is one-sided
    //  for(int ibin=1; ibin<systematic->getUpHistogram()->GetNbinsX()+1; ibin++){
    //    double diffup=systematic->getUpHistogram()->GetBinContent(ibin)-sample->getFinalHistogram()->GetBinContent(ibin);
    //    double diffdown=systematic->getDownHistogram()->GetBinContent(ibin)-sample->getFinalHistogram()->GetBinContent(ibin);
    //    if(diffdown*diffup>0.){
    //      m_onesidedsystematicsfile << "One-sided systematic " << channel->name() << " \t " << sample->name() << " \t " << systematic->name() << " \t " << ibin << " \t " << sample->getFinalHistogram()->GetBinContent(ibin) << " \t " << systematic->getUpHistogram()->GetBinContent(ibin) << " \t " << systematic->getDownHistogram()->GetBinContent(ibin) << std::endl;
    //    }
    //  }
    //}
    //if(retvalue==1 || retvalue==3){
    //  double diffup=systematic->getUpNormalisation()-1.;
    //  double diffdown=systematic->getDownNormalisation()-1.;
    //  if(diffdown*diffup>0.){
    //    m_onesidedsystematicsfile << "One-sided systematic " << channel->name() << " \t " << sample->name() << " \t " << systematic->name() << " \t NORM \t " << 1. << " \t " << systematic->getUpNormalisation() << " \t " << systematic->getDownNormalisation();
    //    if(sample->getYield()/channel->data()->getYield()>0.1 && fabs(diffup)>0.01 && fabs(diffdown)>0.01)
    //      m_onesidedsystematicsfile << "\t IMPORTANT";
    //    m_onesidedsystematicsfile << std::endl;
    //  }
    //}

    //one bin histograms can not have histosys
    if(systematic->getUpHistogram()->GetNbinsX()<=1){
      if(retvalue==2)
        retvalue=0;
      if(retvalue==3)
        retvalue=1;

    }

    //if(systematic->name()=="lh_fake_stat")
    //std::cout << "JULIAN: Pruning retvalue="<<retvalue<< std::endl;
    return retvalue;
  }

  //-------------------------------------------------------------------------------
  bool FakeUserTools::symmetrizeSystematic(Systematic* systematic, TH1F* nominalhist){
    if (systematic == 0 || nominalhist == 0) return false;
    //    if(systematic->symmetrize()){
    //      /*if(!systematic->isHistoSys() && systematic->isOverallSys()){
    //  // symmetrize for  overall systematic part
    //  //automatically correct for one-sided systematics as getDownNormalisation()==1 then.
    //  double upnormalisation=0.;
    //  double downnormalisation=0.;
    //  double bigger=systematic->getUpNormalisation();
    //  if(2.-systematic->getUpNormalisation()>bigger) bigger=2.-systematic->getUpNormalisation()>bigger;
    //  if(systematic->getDownNormalisation()>bigger) bigger=systematic->getDownNormalisation();
    //  if(2.-systematic->getDownNormalisation()>bigger) bigger=2.-systematic->getDownNormalisation();
    //  upnormalisation=bigger;
    //  if(bigger>2.){
    //    std::cout << "WARNING: The systematic \""<<systematic->name()<<"\" in channel \""<< systematic->channelname() << "\" and sample \""<< systematic->samplename() << "\" should be symmetrized and is > 100%. Setting value to 1E-6." <<std::endl;
    //    downnormalisation=1E-6;//quasi Kyle fix
    //  }
    //  else
    //    downnormalisation=2.-bigger;
    //  systematic->setUpNormalisation(upnormalisation);
    //  systematic->setDownNormalisation(downnormalisation);
    //
    //      }
    //
    //      // now treat histogram systematics
    //      if(systematic->isHistoSys()){*/
    //
    //
    //  TH1F *uphist=systematic->getUpHistogram();
    //  //uphist->Scale(systematic->getUpNormalisation()); //just symmetrise the shape!
    //  TH1F *downhist=systematic->getDownHistogram();
    //  //downhist->Scale(systematic->getDownNormalisation());
    //
    //
    //  for(int ibin=0; ibin<uphist->GetNbinsX()+2; ibin++){
    //    double upbin=uphist->GetBinContent(ibin);
    //    double downbin=downhist->GetBinContent(ibin);
    //    double nombin=nominalhist->GetBinContent(ibin);
    //            if(systematic->oneSided()){
    //              std::cout << "Systematic : " << systematic->name() << std::endl;
    //              std::cout << "Start Values" << std::endl;
    //              std::cout << uphist->Integral() << std::endl;
    //              std::cout << downhist->Integral() << std::endl;
    //              std::cout << "up : " << upbin << std::endl;
    //              std::cout << "nom : " << nombin << std::endl;
    //              std::cout << "down : " << downbin << std::endl;
    //            }
    //    double sigma=fabs(upbin-downbin)/2.; //set sigma to the maximum difference between any combination of nombin, upbin and down (should almost always be upbin-downbin)
    //    if(fabs(upbin-nombin)>sigma)
    //      sigma=fabs(upbin-nombin);// /2.;
    //    if(fabs(downbin-nombin)>sigma)
    //      sigma=fabs(downbin-nombin);// /2.;
    //    if(systematic->oneSided())
    //      sigma=fabs(upbin-nombin);
    //
    //    //if(systematic->oneSided()){
    //    if(upbin>nombin){
    //        uphist->SetBinContent(ibin,nombin+sigma);
    //        if(nombin-sigma<0)
    //          downhist->SetBinContent(ibin,1E-6);
    //        else
    //          downhist->SetBinContent(ibin,nombin-sigma);
    //      }
    //      else{
    //        downhist->SetBinContent(ibin,nombin+sigma);
    //        if(nombin-sigma<0)
    //          uphist->SetBinContent(ibin,1E-6);
    //        else
    //          uphist->SetBinContent(ibin,nombin-sigma);
    //      }
    //
    //            if(systematic->oneSided()){
    //              std::cout << "Final Values" << std::endl;
    //              std::cout << "up : "   << uphist->GetBinContent(ibin)   << std::endl;
    //              std::cout << "nom : "  << nominalhist->GetBinContent(ibin)  << std::endl;
    //              std::cout << "down : " << downhist->GetBinContent(ibin) << std::endl;
    //            }
    //
    //  }//loop over bins
    //  // now ensure histograms are normalised to nominal
    //  //double upnormalisation=uphist->GetSumOfWeights()/nominalhist->GetSumOfWeights();
    //  //double downnormalisation=downhist->GetSumOfWeights()/nominalhist->GetSumOfWeights();
    //        //uphist->Scale(1./upnormalisation);
    //        //downhist->Scale(1./downnormalisation);
    //  // If its a one sided systematic the down norm SHOULD be the mirror of UP
    //  // with the above procedure, if we get negative bins (then set to 1e-6) this is not the case
    //  // For non-one-sided systs, the down doesn't need to be the mirror of up, and we just dont symmetrise the norms...
    //  if(systematic->oneSided()){
    //    double dwnnorm = 2 - systematic->getUpNormalisation();
    //    if(dwnnorm<=0) dwnnorm = 1E-6;
    //    systematic->setDownNormalisation(dwnnorm);
    //  }
    //        //    if(systematic->oneSided()){
    //        //      std::cout << "Systematic : " << systematic->name() << std::endl;
    //  //for(int ibin=0; ibin<uphist->GetNbinsX()+2; ibin++){
    //  // std::cout << systematic->channelname() << " ibin "<<ibin << " up=" << uphist->GetBinContent(ibin) << " down=" << downhist->GetBinContent(ibin) << " upnorm=" << upnormalisation << " downnorm=" << downnormalisation << "up*norm=" << upnormalisation*uphist->GetBinContent(ibin) << " down*norm=" << downnormalisation*downhist->GetBinContent(ibin) << std::endl;
    //  //}
    //        //    }
    //
    //  systematic->setUpHistogram(uphist);
    //  systematic->setDownHistogram(downhist);
    //
    //
    //      }
    //}

    return true;
  }

  //-------------------------------------------------------------------------------
  std::string FakeUserTools::getMainParameter(){
    std::map<std::string, std::string>::iterator it=m_parameters.find("mass");
    if(it==m_parameters.end())
      return "125";
    return it->second;
  }

  //-------------------------------------------------------------------------------
  TString FakeUserTools::texify(TString input){
    input.ReplaceAll("_","\\_");
    return input;
  }

  //-------------------------------------------------------------------------------
  bool FakeUserTools::analyze(std::vector<Channel*> channels){
    TString dircmd="mkdir -p ";
    dircmd+=m_outputpath;
    dircmd+="/";
    dircmd+=m_version;
    dircmd+="/monitoring/";
    system(dircmd.Data());

    for(unsigned int ichannel=0; ichannel<channels.size(); ichannel++){
      Channel* channel=channels[ichannel];

      std::set<TString> s_systematics;
      std::set<TString> s_normfactors;
      for(unsigned int isample=0; isample<channel->samples()->size(); isample++){
        Sample* sample=channel->samples()->at(isample);
        for(unsigned int isystematic=0; isystematic<sample->systematics()->size(); isystematic++){
          Systematic* systematic=sample->systematics()->at(isystematic);
          s_systematics.insert(systematic->name());
        }
        for(unsigned int inormfactor=0; inormfactor<sample->normfactors()->size(); inormfactor++){
          NormFactor* normfactor=sample->normfactors()->at(inormfactor);
          s_normfactors.insert(normfactor->name());
        }
      }

      std::vector<TString> v_systematics;
      std::copy(s_systematics.begin(), s_systematics.end(), std::back_inserter(v_systematics));
      std::copy(s_normfactors.begin(), s_normfactors.end(), std::back_inserter(v_systematics));

      TString texfilename="workspaces/";
      texfilename+=m_version;
      texfilename+="/monitoring/pruning_";
      texfilename+=m_statanalysisname;
      texfilename+="_";
      texfilename+=channel->name();
      texfilename+=".tex";
      std::ofstream texfile;
      texfile.open (texfilename.Data());

      texfile << "\\begin{longtable}{r|";
      for(unsigned int isample=0; isample<channel->samples()->size(); isample++)
        texfile << "l";
      texfile<<"}\\hline\\hline\n";
      texfile << "Systematic ";
      for(unsigned int isample=0; isample<channel->samples()->size(); isample++)
        texfile << "& \\begin{turn}{-90}" << texify(channel->samples()->at(isample)->name()) <<"\\end{turn} ";
      texfile << "\\\\\\hline\\endhead\n";
      for(unsigned int isystematic=0; isystematic<v_systematics.size(); isystematic++){
        texfile << texify(v_systematics[isystematic]);
        for(unsigned int isample=0; isample<channel->samples()->size(); isample++){
          Sample* sample=channel->samples()->at(isample);
          Systematic* systematic=0;
          NormFactor* normfactor=0;
          //find systematic
          for(unsigned int isyst=0; isyst<sample->systematics()->size(); isyst++){
            if(sample->systematics()->at(isyst)->name()==v_systematics[isystematic]){
              systematic=sample->systematics()->at(isyst);
            }
          }
          for(unsigned int inormfactor=0; inormfactor<sample->normfactors()->size(); inormfactor++){
            if(sample->normfactors()->at(inormfactor)->name()==v_systematics[isystematic]){
              normfactor=sample->normfactors()->at(inormfactor);
            }
          }
          if(systematic==0 && normfactor==0){
            texfile << "& - ";
          }
          else{
            TString writestring="";
            if(normfactor!=0)
              writestring="Y";
            else if(systematic!=0 && systematic->isPrunedAway())
              writestring="N";
            else if(systematic!=0)
              writestring="Y";
            else
              writestring="?";

            if(systematic!=0 && systematic->isHistoSys())
              writestring=writestring+"H";
            if(systematic!=0 && systematic->isOverallSys())
              writestring=writestring+"O";
            texfile << "& " << writestring << " ";
          }
        }
        texfile << "\\\\\n";
      }
      texfile << "\\hline\\hline\n";
      texfile << "\\caption{Channel "<<texify(channels[ichannel]->name())<<": -=systematic not applied in this channel, N = systematic is pruned away, Y = systematic is not pruned away, H = HistoSyst is considered for this systematic, O = OverallSyst is considered for this systematic}\n";
      texfile << "\\label{pruning"<<channels[ichannel]->name()<<"}\n";
      texfile << "\\end{longtable} \n";
      texfile.close();
    }

    TString texfilename="workspaces/";
    texfilename+=m_version;
    texfilename+="/monitoring/pruning_";
    texfilename+=m_statanalysisname;
    texfilename+="_toplevel.tex";
    std::ofstream texfile;
    texfile.open (texfilename.Data());
    texfile << "\\documentclass[5pt,a4paper,landscape]{article} \n";
    texfile << "\\usepackage[a4paper]{geometry}\n\\usepackage[applemac]{inputenc}\n\\usepackage{indentfirst}\n\\usepackage{graphicx}\n\\usepackage[english]{babel}\n\\usepackage{pdfpages}\n\\usepackage{subfigure}\n\\usepackage{mathrsfs}\n\\usepackage{lscape}\n\\usepackage{rotating}\n\\usepackage{longtable}\n\n";
    texfile << "\\begin{document} \n";
    texfile << "\\title{Pruning result} \n";
    texfile << "\\maketitle \n";
    for(unsigned int ichannel=0; ichannel<channels.size(); ichannel++){
      //texfile << "\\begin{table}\n";
      //texfile << "\\begin{center} \n";
      texfile << "\\input{pruning_"<<m_statanalysisname<<"_"<<channels[ichannel]->name()<<".tex} \n";
      //texfile << "\\label{pruning"<<channels[ichannel]->name()<<"}\n";
      //texfile << "\\caption{Channel "<<texify(channels[ichannel]->name())<<": -=systematic not applied in this channel, N=systematic is pruned away, Y=systematic is not pruned away, S=shape is considered for this systematic}\n";
      //texfile << "\\end{center} \n";
      //texfile << "\\end{table}\n";
      texfile << "\\clearpage\n\n";
    }
    texfile << "\\end{document} \n";
    texfile.close();

    return true;
  }
} // namespace
