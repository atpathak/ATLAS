#include <WorkspaceBuilder/TESUserTools.h>
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
  TESUserTools::TESUserTools(std::string statanalysisname,
                             std::string outputpath,
                             std::string version,
                             std::vector<std::string> rootpaths,
                             std::vector<std::string> normfactortxtfilenames,
                             std::vector<std::string> overallsystematicfilenames,
                             std::map<std::string,std::string> parameters,
                             bool unblind,
                             bool info) : UserTools(rootpaths, parameters, unblind, info)
  {

    std::cout << "INFO: TESUserTools::TESUserTools() "<<statanalysisname<< " constructor..." << std::endl;
    setOutputPath(outputpath);
    m_statanalysisname=statanalysisname;
    m_version=version;

    //must be centralized using inheritance
    for(unsigned int i=0; i<normfactortxtfilenames.size(); i++){
      std::ifstream txtfile (normfactortxtfilenames.at(i).c_str());
      if (txtfile.is_open()){
        std::string normfactorname="";
        double nominalvalue=0.;
        double lowvalue=0.;
        double highvalue=0.;
        //while ( !txtfile.eof() ){
        std::string line;
        while (std::getline(txtfile, line)) {
          std::istringstream ss(line);
          if(line.find("#") != std::string::npos)
            continue;
          ss >> normfactorname >> nominalvalue >> lowvalue >> highvalue;
          m_normfactor_name.push_back(normfactorname);
          m_normfactor_value.push_back(nominalvalue);
          m_normfactor_low.push_back(lowvalue);
          m_normfactor_high.push_back(highvalue);
        }//loop
        txtfile.close();
      }//file
    }//inputs

    for(unsigned int i=0; i<overallsystematicfilenames.size(); i++){
      std::ifstream txtfile (overallsystematicfilenames.at(i).c_str());
      if (txtfile.is_open()){
        std::string overallsystematicname="";
        double value=0.;
        while ( !txtfile.eof() ){
          txtfile >> overallsystematicname >> value;
          std::cout << "Info: Using overallsyst: " << overallsystematicname << std::endl;
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
    m_pruning_alpha_overallsys_variation = new TH1F("pruning_alpha_overallsys_variation","pruning alpha overallsys variations", 600, 0, 0.3);
    m_pruning_alpha_overallsys_variation->SetDirectory(0);
    m_pruning_alpha_histosys_pvaluechi2 = new TH1F("m_pruning_alpha_histosys_pvaluechi2","pruning chi2 histosys variations", 1000, 0., 1.);
    m_pruning_alpha_histosys_pvaluechi2->SetDirectory(0);

    TString dircmd="mkdir -p ";
    TString dir=m_outputpath;
    dir+="/";
    dir+=m_version;
    dir+="/monitoring";

    TString onesidedsystematics_dir = dir+"/onesidedsystematics/";
    system( (dircmd + onesidedsystematics_dir).Data());
    TString filename_syst = onesidedsystematics_dir;
    filename_syst+="onesidedsystematics_";
    filename_syst+=m_statanalysisname;
    filename_syst+=".txt";
    m_onesidedsystematicsfile.open(filename_syst.Data(), std::ofstream::out);

    TString kylefix_dir = dir+"/kylefix/";
    system( (dircmd + kylefix_dir).Data());
    TString filename_kyle = kylefix_dir;
    filename_kyle+="kylefix_";
    filename_kyle+=m_statanalysisname;
    filename_kyle+=".txt";
    m_kylefixfile.open (filename_kyle.Data(), std::ofstream::out);


    //if( m_parameters["Smooth"].find("debug") != std::string::npos ){
    m_smooth_dir = dir+"/smoothing/";
    system( (dircmd + m_smooth_dir).Data());
    std::cout<<"INFO: TESUserTools::TESUserTools() - output directory created for smoothing '"<<m_smooth_dir<<"' ..."<<std::endl;

    m_smooth_scan.open( (m_smooth_dir+"smoothing_scan.txt").Data(), std::ofstream::out);
    m_smooth_errors.open( (m_smooth_dir+"smoothing_errors.txt").Data(), std::ofstream::out);
    m_smooth_applied.open( (m_smooth_dir+"smoothing_applied.txt").Data(), std::ofstream::out);
    m_smooth_skipped.open( (m_smooth_dir+"smoothing_skipped.txt").Data(), std::ofstream::out);


    //kyle
    m_nkylefixedbins_all=0;
    m_nkylefixedbins_nominal=0;
    m_sumwkylefixedbins_nominal=0;

    //make a list of all histograms in the root files so that later we can check if all of them have been used...
    for(unsigned int ifile=0; ifile<m_rootfiles.size(); ifile++){
      fillUsedHistogramsMap(m_rootfiles[ifile],"");
    }

    //print params
    if(m_parameters.size()){
      std::cout<<"INFO: TESUserTools::TESUserTools() Parameters: \n";
      for(const auto param : m_parameters){
        std::cout<<"\t'"<<param.first<<"' : '"<<param.second<<"'\n";
      }
      std::cout<<std::endl;
    }

  }

  //-------------------------------------------------------------------------------
  TESUserTools::~TESUserTools(void){
    TString dircmd="mkdir -p ";

    TString pruning_path = m_outputpath;
    pruning_path+="/";
    pruning_path+=m_version;
    pruning_path+="/";
    pruning_path+="monitoring";
    pruning_path+="/";
    pruning_path+="pruning";
    pruning_path+="/";
    system( (dircmd + pruning_path).Data()) ;

    TString rootfilename = pruning_path;
    rootfilename+=m_statanalysisname;
    rootfilename+=".root";

    TString prunefilename = pruning_path;
    prunefilename+=m_statanalysisname;
    prunefilename+=".txt";

    TFile *m_monitoringfile=new TFile(rootfilename,"RECREATE");
    TDirectory* dir=m_monitoringfile->mkdir("Pruning");
    m_pruning_maxvariation->SetDirectory(dir);
    m_pruning_maxrelvariation->SetDirectory(dir);
    m_pruning_pvaluechi2->SetDirectory(dir);
    m_pruning_alpha_overallsys_variation->SetDirectory(dir);
    m_pruning_alpha_histosys_pvaluechi2->SetDirectory(dir);
    m_monitoringfile->Write();
    m_monitoringfile->Close();
    if(m_monitoringfile)
      delete m_monitoringfile;

    std::ofstream m_pruningfile;
    m_pruningfile.open( prunefilename.Data(), std::ofstream::out);

    m_pruningfile << "INFO: The following histograms have not been used while building the workspace:" << std::endl;
    m_pruningfile << "They were not used in building this workspace. It might be worth checking if this is intended." << std::endl;
    for(std::map<TString, bool>::iterator it=m_usedhistograms.begin(); it!=m_usedhistograms.end(); it++)
      if(!it->second)
        m_pruningfile << it->first << std::endl;

    //kyle
    m_kylefixfile << m_nkylefixedbins_all << " bins have been kyle-fixed." << std::endl;
    m_kylefixfile << m_nkylefixedbins_nominal << " bins have been kyle-fixed for the nominal histograms (sumw="<<m_sumwkylefixedbins_nominal<<")." << std::endl;

    //close outstream files
    m_onesidedsystematicsfile.close();
    m_kylefixfile.close();
    m_pruningfile.close();
    m_smooth_scan.close();
    m_smooth_errors.close();
    m_smooth_applied.close();
    m_smooth_skipped.close();

    //info

  }
  //-------------------------------------------------------------------------------
  void TESUserTools::fillUsedHistogramsMap(TDirectory* dir, TString dirname){
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
        }
      }
      else{
        std::cout << "WARNING: Found object \"" << key->GetName() << "\" of type \"" << key->GetClassName() << "\" which is neither a TDirectory or a TH1*, will ignore this object." << std::endl;
      }
    }
  }

  //-------------------------------------------------------------------------------
  TH1F* TESUserTools::getHistogram(std::string channel, std::string sample, std::string systematic, int up){
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

  double TESUserTools::getOverallSys(std::string channel, std::string sample, std::string systematic, int up){

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

  double TESUserTools::getLumiInInvPb(std::string channel){
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
  std::string TESUserTools::description(){
    return "Available options with -p: \n\tNoSmoothing\n\tSymmetrize\n\tNoKyleFix\n\tNoPruning\n\t78TeVPruning for fake pruning to make 7/8 TeV input files usable\n\tPrintUnusedHistograms to print a list of histograms in the root files that are not used\n\tConstantNuisanceParameterFile <filename> to set the nuisance parameters listed in this file to constant (1 column, alpha_ can be omitted)\n\tCorrelatedNuisanceParameterFile <filename> to set the nuisance parameters listed in one line of this file to correlated (1. column: new name of NP, 2./3./4./... input nuisance parameter names)\n\tRemoveSignalOutside100150 sets the signal histograms to 0 outsideof [100,150] GeV.";
  }

  //-------------------------------------------------------------------------------
  double TESUserTools::getNormFactor(std::string normfactor, int up){
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
  bool TESUserTools::isNuisanceParameterConstant(TString npname){

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
  TString TESUserTools::correlatedSystematicName(TString name){
    std::map<TString,TString>::iterator it=m_correlatednuisanceparameter.find(name);
    if(it!=m_correlatednuisanceparameter.end())
      return it->second;

    return name;
  }

  //-------------------------------------------------------------------------------
  bool TESUserTools::smoothHistogram(TH1F* input, TH1F* nominal, std::string channel, std::string sample, std::string systematic, int up){
    /**
     *Notes:
     * --parameters Smooth=systematics,W,Fake,Zll,Top
     * add the 'alpha' keyword in order to include the alpha parameters in the smoothing procedure
     */

    //fundamental checks
    if(!input){
      m_smooth_errors <<" [channel, sample, systematic, variation] = ["
                      <<channel
                      <<", "<<sample
                      <<", "<<systematic
                      <<", "<<up
                      <<"] Error - Null systematic input "
                      <<std::endl;
      return false;
    }
    if(!nominal){
      m_smooth_errors <<" [channel, sample, systematic, variation] = ["
                      <<channel
                      <<", "<<sample
                      <<", "<<systematic
                      <<", "<<up
                      <<"] Error - Null nominal input "
                      <<std::endl;
      return false;
    }

    if(!m_smooth_errors.is_open()){
      std::cout<<"ERROR: TESUserTools::smoothHistogram - Error in opening the smooth errors file ..."<<std::endl;
    }

    //apply or not
    if(m_parameters.find("NoSmoothing") != m_parameters.end()){
      m_smooth_skipped << "No Smoothing Enabled"<<std::endl;
      m_smooth_applied << "No Smoothing Enabled"<<std::endl;
      return true;
    } else if(m_parameters.find("Smooth") != m_parameters.end()){


      std::string smoothing_keys = m_parameters["Smooth"];

      m_smooth_scan <<" [channel, sample, systematic, variation] = ["
                    <<channel
                    <<", "<<sample
                    <<", "<<systematic
                    <<", "<<up
                    <<"] keys = "<<smoothing_keys
                    <<std::endl;

      bool do_smooth_syst = smoothing_keys.find("systematics") != std::string::npos;
      if(up!=0 && systematic != "nominal" && !do_smooth_syst){

        m_smooth_skipped <<"[channel, sample, systematic, variation] = ["
                         <<channel
                         <<", "<<sample
                         <<", "<<systematic
                         <<", "<<up
                         <<"] since this systematic is subject to smoothing"
                         <<std::endl;

        return true;
      }

      bool do_this_sample = smoothing_keys.find(sample) != std::string::npos;
      if(!do_this_sample){
        m_smooth_skipped <<"[channel, sample, systematic, variation] = ["
                         <<channel
                         <<", "<<sample
                         <<", "<<systematic
                         <<", "<<up
                         <<"] since this sample is subject to smoothing"
                         <<std::endl;
        return true;
      }

      bool do_smooth_alpha = smoothing_keys.find("alpha") != std::string::npos;
      if(!do_smooth_alpha){
        if( systematic.find("ALPHA") != std::string::npos){
          m_smooth_skipped <<"[channel, sample, systematic, variation] = ["
                           <<channel
                           <<", "<<sample
                           <<", "<<systematic
                           <<", "<<up
                           <<"] since this is an alpha parameter"
                           <<std::endl;
          return true;
        }
      }

      if(input->GetNbinsX() < 3 ){
        m_smooth_skipped <<"[channel, sample, systematic, variation] = ["
                         <<channel
                         <<", "<<sample
                         <<", "<<systematic
                         <<", "<<up
                         <<"] since smoothing is only supported for >=3 bins"
                         <<std::endl;
        return true;
      }

      m_smooth_applied <<"[channel, sample, systematic, variation] = "
                       <<channel<<", "
                       <<sample<<", "
                       <<systematic<<", "
                       <<up
                       <<std::endl;


      //debugging
      TH1F* bck = NULL;
      bck=(TH1F*)input->Clone();
      bck->SetDirectory(0);

      //entries
      double entries=input->GetEntries();

      //Dividing myself to make contents to be one when the nominal has zero entries.
      for(int ibin=0; ibin<input->GetNbinsX()+2; ibin++){
        if(nominal->GetBinContent(ibin)!=0){
          input->SetBinContent(ibin, input->GetBinContent(ibin)/nominal->GetBinContent(ibin));
        }else{
          input->SetBinContent(ibin, 1.);
        }
      }

      //weights before
      double before = input->GetSumOfWeights();

      //smooth
      input->GetXaxis()->SetRange(2,input->GetNbinsX()-1); //This is needed because the first and last bin shouldn't be smoothed.
      input->Smooth(1,"R"); //R smoothes only visible bins, i.e. no overflow or underflow
      input->GetXaxis()->SetRange(1,input->GetNbinsX()); //Come back to the default x-axis range

      //weights after
      double after = input->GetSumOfWeights();

      if(std::fabs((before-after)/before)>0.05)
        m_smooth_applied <<"[channel, sample, systematic, variation] = "
                         <<channel<<", "
                         <<sample<<", "
                         <<systematic<<", "
                         <<up
                         <<"Smoothing changed integral by 5%"
                         << " before:" << before
                         << " after:" << after
                         << std::endl;

      // Do we want to keep integral as the original?
      // input->Scale(before/after);//This changes shape dramatically when |after-before| is big.

      //get smoothed shape
      input->Multiply(nominal);
      input->SetEntries(entries);

      /* the following is only used to debug the smoothing*/
      if(bck){
        TCanvas *c1=new TCanvas("c1","c1",800,600);
        c1->Divide(2,2);

        c1->cd(1);
        nominal->SetLineColor(kBlue);
        nominal->Draw();
        bck->SetLineColor(kBlack);
        bck->Draw("SAME");

        c1->cd(2);
        TH1F* nombckratio=(TH1F*)bck->Clone();
        nombckratio->SetDirectory(0);
        nombckratio->Divide(nominal);
        nombckratio->Draw();

        c1->cd(3);
        bck->SetLineColor(kBlack);
        bck->Draw();
        input->SetLineColor(kRed);
        input->Draw("SAME");

        c1->cd(4);
        TH1F* ratio=(TH1F*)bck->Clone();
        ratio->SetDirectory(0);
        ratio->Divide(input);
        ratio->Draw();

        TString filename=m_smooth_dir;
        filename+="/";
        filename+="debug_smoothing_";
        filename+=channel;
        filename+="_";
        filename+=sample;
        filename+="_";
        filename+=systematic;
        filename+="_";
        filename+=up;
        filename+=".png";

        c1->Modified();
        c1->Update();
        c1->Print(filename);
        if(c1) delete c1;
        if(bck) delete bck;
        if(nombckratio) delete nombckratio;
        if(ratio) delete ratio;
      }


    }//smoothing


    return true;
  }

  //-------------------------------------------------------------------------------
  bool TESUserTools::kyleFixHistogram(TH1F* input, std::string channel, std::string sample, std::string systematic, int up, bool issignal){

    if(m_parameters.find("NoKyleFix")==m_parameters.end() && !issignal && input->GetEntries()>0){

      // don't kyle-fix signal as this is not conservative and don't kyle fix histograms which have no entry at all as kyle-fixing will fail
      double sumW2TotBin = 0.;

      for(int j=1;j<input->GetNbinsX()+1;j++){ //No OVER/UNDER-FLOW bins considered!
        sumW2TotBin += pow( input->GetBinError(j) , 2);
      }

      double avWeightBin = input->GetSumOfWeights() /(double)input->GetEntries();
      double avW2Bin = sumW2TotBin / (double)input->GetEntries();

      if(input->GetEntries() == 0.0){
        std::cout<<"ERROR! Histogram has 0 entries! Not meaningful to Kyle-fix, please look this over!! Will fail now. We are at channel \""
                 <<channel << "\", sample \"" << sample << "\", systematic \""<<systematic<<"\" ("<<up<<")"<<std::endl;
        return false;
      }

      if(avWeightBin <= 0){
        std::cout<<"WARNING! Potential Kyle-fix value is NEGATIVE (avWeightBin="<<avWeightBin<<", GetEntries()=" << input->GetEntries()
                 << ", GetSumOfWeights()=" << input->GetSumOfWeights()
                 <<"). This can happen if the integral of the histogram is negative. Clearly pathological, needs to be fixed at an earlier stage... For now setting it to 0.0001, but look into this!!!! We are at channel \""
                 <<channel << "\", sample \"" << sample << "\", systematic \""<<systematic<<"\" ("<<up<<")"<<std::endl;
        if(m_info) std::cout<<"GetSumOfWeights="<<input->GetSumOfWeights()<<" GetEntries="<<input->GetEntries()<<" avWeightBin="<<avWeightBin<<std::endl;
        avWeightBin = 0.0001;
      }

      if(avWeightBin>2){
        //std::cout << "avWeightBin="<<avWeightBin<<", input->GetEntries()==" << input->GetEntries() << ", input->GetSumOfWeights()==" << input->GetSumOfWeights() << std::endl;
        std::cout<<"WARNING! Potential Kyle-fix value is rather large, >2 (avWeightBin="<<avWeightBin<<", GetEntries()=" << input->GetEntries()
                 << ", GetSumOfWeights()=" << input->GetSumOfWeights()<<"). Continuing for now, but you may want to look into it. We are at channel \""
                 <<channel << "\", sample \"" << sample << "\", systematic \""<<systematic<<"\"("<<up<<")"<<std::endl;
      }

      std::vector<double> kylefixedweights;
      for(int j=1;j<input->GetNbinsX()+1;j++){ //No OVER/UNDER-FLOW bins considered!
        if(input->GetBinContent(j) <= 0){
          if(m_info) std::cout<<"INFO: Need to apply KyleFix in channel \""<<channel << "\", sample \"" << sample << "\", systematic \""<<systematic<<"\" ("<<up<<") bin nr"<< j <<"!! Kyle-fixed bin-content: "<<avWeightBin<<" and error: "<<avW2Bin<<" Original bin-content:"<<input->GetBinContent(j)<<std::endl;
          if(channel=="Htt_year2016_chanll_catvbf_regztt_selCBA" && sample=="Ztt" && (systematic=="LumiUnc2016" || systematic=="nominal")){
            std::cout << "INFO! Applying kyle-fix in bin " << j << " systematic="<<systematic<<" weight=" << avWeightBin << " sum="<<input->GetSumOfWeights() <<" entries="<<input->GetEntries()<<std::endl;
          }
          input->SetBinContent(j,avWeightBin);
          input->SetBinError(j,sqrt(avW2Bin));
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
  int TESUserTools::pruneSystematic(Systematic* systematic, Channel* channel, Sample* sample){

    int retvalue=3;

    // Sample class:
    //enum PrunningRetVals{PrunedAway, OnlyOverallSys, OnlyHistSys, NotPruned};
    // 0 : Everything pruned
    // 1 : Shape pruned
    // 2 : Normalisation pruned
    // 3 : Nothing pruned

    ///////////////////////////////////////////////////////////
    // pre-checks
    ///////////////////////////////////////////////////////////
    //no pruning
    if(m_parameters.find("NoPruning")!=m_parameters.end()){
      std::cout<<"INFO: TESUserTools::pruneSystematic() - No Pruning will be applied!\n";
      return Systematic::NotPruned;
    }

    //keep NPs in any case
    if(m_parameters.find("KeepNPwithKey")!=m_parameters.end()){
      if (systematic->name().find( m_parameters["KeepNPwithKey"] ) != std::string::npos) {
        std::cout<<"INFO: TESUserTools::pruneSystematic() - Pruning won't be applied '"
          +systematic->name()+"'/'"+channel->name()+"'/'"+sample->name()+"' (protected NP with key)..."<<std::endl;
        return Systematic::NotPruned;
      }
    }


    //kep NPs for certain samples with keys
    if(m_parameters.find("KeepNuisApply") != m_parameters.end()){

      bool kSample = false; bool kKey = false; bool kChannel = false;

      if(m_parameters.find("KeepNuisWithKeys") != m_parameters.end()){
        auto tokens = split(m_parameters["KeepNuisWithKeys"], ',');
        for(const auto &token : tokens){
          if(systematic->name().find(token) != std::string::npos){
            kKey = true;
            break;
          }
        }
      }

      if(m_parameters.find("KeepNuisForSamples") != m_parameters.end()){
        auto tokens = split(m_parameters["KeepNuisForSamples"], ',');
        for(const auto &token : tokens){
          if(sample->name().find(token) != std::string::npos){
            kSample = true;
            break;
          }
        }
      }

      if(m_parameters.find("KeepNuisForChannels") != m_parameters.end()){
        auto tokens = split(m_parameters["KeepNuisForChannels"], ',');
        for(const auto &token : tokens){
          if(channel->name().find(token) != std::string::npos){
            kChannel = true;
            break;
          }
        }
      }

      if(kSample && kKey && kChannel){
        std::cout<<"INFO: TESUserTools::pruneSystematic() - Pruning NOT applied '"
          +systematic->name()+"'/'"+channel->name()+"'/'"+sample->name()+"' (protected NP for samples+keys)..."<<std::endl;
        return Systematic::NotPruned;
      }

    }

    ///////////////////////////////////////////////////////////
    //If we find systematics with an empty histogram, prune away (maybe due to low stat MC sample).
    ///////////////////////////////////////////////////////////
    if((systematic->getUpNormalisation() < 1E-4 || systematic->getDownNormalisation() < 1E-4) && !(systematic->oneSided())){
      std::cout<<"INFO: TESUserTools::pruneSystematic() - Pruning '"
        +systematic->name()+"'/'"+channel->name()+"'/'"+sample->name()+"' (empty histo)..."<<std::endl;
      systematic->setSysPrunedBy(PruneUtil::EmptyHist);
      return Systematic::PrunedAway;
    }

    ///////////////////////////////////////////////////////////
    /// Special pruning for alpha parameters
    ///////////////////////////////////////////////////////////

    if(m_parameters.find("AlphaPruningApply") != m_parameters.end()){ //if applied
      if (systematic->name().find( m_parameters["AlphaPruningKey"] ) != std::string::npos) { //if syst names contains the parsed key

        bool pruneAlpha_OS = false;
        bool pruneAlpha_HS = false;
        const float alpha_overallsys_threshold = 0.01;
        const float alpha_histosys_threshold = 0.99;

        //////////////////////////////////////////
        //one bin histograms can not have histosys
        //////////////////////////////////////////
        if(systematic->getUpHistogram()->GetNbinsX()<=1){
          pruneAlpha_HS = true;
          std::cout<<"INFO: TESUserTools::pruneSystematic() - alpha HSys '"
            +systematic->name()+"'/'"+channel->name()+"'/'"+sample->name()+"' (HistoSys - 1-bin histo)..."<<std::endl;
        }

        ////////////
        //overallsys
        ////////////
        float AbsNormUp = std::fabs(systematic->getUpNormalisation()-1.);
        float AbsNormDown = std::fabs(systematic->getDownNormalisation()-1.);
        m_pruning_alpha_overallsys_variation->Fill(AbsNormDown);
        m_pruning_alpha_overallsys_variation->Fill(AbsNormUp);
        if ( AbsNormUp <  alpha_overallsys_threshold && AbsNormDown <  alpha_overallsys_threshold) {
          systematic->setSysPrunedBy(PruneUtil::FlatPrune);
          pruneAlpha_OS = true;
          //retvalue=Systematic::OnlyHistSys;
          std::cout<<"INFO: TESUserTools::pruneSystematic() - Alpha OSys '"
            +systematic->name()+"' : '"
            +channel->name()+"' : '"
            +sample->name()
            +"' (OverallSys - SameNorm max|DeltaNorm|="<<std::max(AbsNormUp, AbsNormDown)<<")..."<<std::endl;
        }//OS

        ////////////
        //histosys
        ////////////
        /* Note: If neither the upward nor the downward variation’s probability is lower than that threshold, the shape variation is
           considered to not be significant for the given background sample, and the shape NP is not used in the fit
           --> the corresponding normalization uncertainty is still kept however. */
        double chi2up=0.;
        double chi2down=0.;
        for(int ibin=1; ibin < systematic->getUpHistogram()->GetNbinsX()+1; ibin++){ //loop over bins
          double statuncert = std::fabs(sample->getFinalHistogram()->GetBinError(ibin));
          //up
          double diffup =  std::fabs(systematic->getUpHistogram()->GetBinContent(ibin)-sample->getFinalHistogram()->GetBinContent(ibin));
          double statuncertup =  std::fabs(systematic->getUpHistogram()->GetBinError(ibin));
          if(diffup!=0 && (statuncertup!=0 || statuncert!=0))
            chi2up += std::pow(diffup, 2.)/std::pow(std::max(statuncertup, statuncert), 2.);
          //down
          double diffdown =  std::fabs(systematic->getDownHistogram()->GetBinContent(ibin)-sample->getFinalHistogram()->GetBinContent(ibin));
          double statuncertdown =  std::fabs(systematic->getDownHistogram()->GetBinError(ibin));
          if(diffdown!=0 && (statuncertdown!=0 || statuncert!=0))
            chi2down += std::pow(diffdown,2.)/std::pow(std::max(statuncertdown, statuncert), 2.);
        }//HS loop over bins

        //pvalue
        if( systematic->hasTag("FourVectorSystematic") ){
          int nbinsup =  systematic->getUpHistogram()->GetNbinsX()-1;
          int nbinsdown =  systematic->getDownHistogram()->GetNbinsX()-1;
          double pvalue_chi2up   = 1.0 - ROOT::Math::chisquared_cdf(chi2up, nbinsup);
          double pvalue_chi2down = 1.0 - ROOT::Math::chisquared_cdf(chi2down, nbinsdown);

          m_pruning_alpha_histosys_pvaluechi2->Fill(pvalue_chi2up);
          m_pruning_alpha_histosys_pvaluechi2->Fill(pvalue_chi2down);

          double max_chi2p = std::max(pvalue_chi2up, pvalue_chi2up);
          //if the p-value is > X, then the hypothesis of identity of the two histograms can be accepted for X significant level --> prune
          //if the p-value is < X, then the hypothesis of identity of the two histograms is rejected for X significant level -> retain
          if(max_chi2p >= alpha_histosys_threshold){
            pruneAlpha_HS = true;
            systematic->setSysPrunedBy(PruneUtil::Chi2);
            retvalue=Systematic::OnlyOverallSys;
            std::cout<<"INFO: TESUserTools::pruneSystematic() - Alpha HSys '"
              +systematic->name()+"' : '"+channel->name()+"' : '"+sample->name()+"' (HSys - chi2 p-val = "<<max_chi2p<<")..."<<std::endl;
          }//chi2p

        }//it's 4-vector syst

        //take out OverallSys
        if(pruneAlpha_OS){
          Systematic::removeOverallSys(retvalue);
        }

        //take out HistoSys
        if(pruneAlpha_HS){
         Systematic::removeHistSys(retvalue);
        }

        return retvalue;

      }//alpha is present
    }//apply alpha pruning

    /*
     */

    ///////////////////////////////////////////////////////////
    // Pruning for HistoSys
    ///////////////////////////////////////////////////////////

    //Pruning 0: HistoSys won't be considered if the original histogram don't have enough statistics.
    double MCuncert=0.;
    double minMCuncertByBin=999.;
    TH1F* nominal=sample->getFinalHistogram();
    for(int i=0; i<nominal->GetNbinsX()+2; i++){
      double binErr = nominal->GetBinError(i);
      double binCont = nominal->GetBinContent(i);
      MCuncert+=binErr*binErr;
      if(binErr!=0. && binCont!=0.){
        double binRelErr = binErr/binCont;
        if(minMCuncertByBin>binRelErr) minMCuncertByBin = binRelErr;
      }
    }

    MCuncert=sqrt(MCuncert)/nominal->GetSumOfWeights();

    if( systematic->hasTag("FourVectorSystematic") ){
      Double_t StatErrThresForShape = 0.10;
      std::map<std::string, std::string>::iterator it=m_parameters.find("StatErrReqForShape");
      if(it!=m_parameters.end()) StatErrThresForShape = TString(it->second).Atof();

      if(MCuncert>StatErrThresForShape){
        //      if(minMCuncertByBin>StatErrThresForShape){
        //      std::cout << "LowStat to evaluate HistSys: " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
        std::cout<<"INFO: TESUserTools::pruneSystematic() - Pruning '"
          +systematic->name()+"'/'"+channel->name()+"'/'"+sample->name()+"' (HistoSys - low stats)..."<<std::endl;

        systematic->setSysPrunedBy(PruneUtil::LowStat);
        //        retvalue = Systematic::PrunedAway;//more aggressive ...
        Systematic::removeHistSys(retvalue);
      }
      //      std::cout<<"MC stat. uncertainty: "<<MCuncert<<", min MC stat. uncertainty by-bin: "<<minMCuncertByBin<<", Threshold="<<StatErrThresForShape<<std::endl;
    }

    //Pruning 1: A χ^2 test between the upwards and the downwards fluctuated shape with respect to the nominal is performed, for each potential shape systematic NP and for each sample. It should be noted that in this calculation the statistical uncertainty that enters is only the largest of the nominal or varied one, rather than both (since they are typically very strongly correlated). The shape systematic is retained if the result of the χ^2 test is less than a threshold value, for either of the upwards or downwards fluctuated shape, where the exact threshold value differs between the channels. If neither the upward nor the downward variation’s probability is lower than that threshold, the shape variation is considered to not be significant for the given background sample, and the shape NP is not used in the fit (the corresponding normalization uncertainty is still kept however). This pruning criterion is not applied to those systematics whose variation only occurs through the variation of weights of the MC events – these are always kept.

    double chi2up=0.;
    double chi2down=0.;
    const double threshold=0.10;
    for(int ibin=1; ibin<systematic->getUpHistogram()->GetNbinsX()+1; ibin++){
      double statuncert = std::fabs(sample->getFinalHistogram()->GetBinError(ibin));
      //up
      double diffup = std::fabs(systematic->getUpHistogram()->GetBinContent(ibin)-sample->getFinalHistogram()->GetBinContent(ibin) );
      double statuncertup = std::fabs(systematic->getUpHistogram()->GetBinError(ibin));
      if(diffup!=0 && (statuncertup!=0 || statuncert!=0))
        chi2up+=pow(diffup,2.)/pow(std::max(statuncertup,statuncert),2.);
      //down
      double diffdown = std::fabs(systematic->getDownHistogram()->GetBinContent(ibin)-sample->getFinalHistogram()->GetBinContent(ibin));
      double statuncertdown = std::fabs(systematic->getDownHistogram()->GetBinError(ibin));
      if(diffdown!=0 && (statuncertdown!=0 || statuncert!=0))
        chi2down+=pow(diffdown,2.)/pow(std::max(statuncertdown,statuncert),2.);
    }
    //chi2up/=systematic->getUpHistogram()->GetNbinsX()-1;
    //chi2down/=systematic->getDownHistogram()->GetNbinsX()-1;
    if( systematic->hasTag("FourVectorSystematic") ){
      double pvalue_chi2up   = 1.0 - ROOT::Math::chisquared_cdf(chi2up,systematic->getUpHistogram()->GetNbinsX()-1);
      double pvalue_chi2down = 1.0 - ROOT::Math::chisquared_cdf(chi2down,systematic->getDownHistogram()->GetNbinsX()-1);

      m_pruning_pvaluechi2->Fill(pvalue_chi2up);
      m_pruning_pvaluechi2->Fill(pvalue_chi2down);
      //double chi2up_pval = (sample->getFinalHistogram())->Chi2Test(systematic->getUpHistogram(),"WW");
      //double chi2dn_pval = (sample->getFinalHistogram())->Chi2Test(systematic->getDownHistogram(),"WW");
      //std::cout << "Chi2 Up : " << pvalue_chi2up   << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
      //std::cout << "Chi2 Dn : " << pvalue_chi2down << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
      //std::cout << "pval Up : " << chi2up_pval << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
      //std::cout << "pval Dn : " << chi2dn_pval << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;

      //if the p-value is > X, then the hypothesis of identity of the two histograms can be accepted for X significant level --> prune
      if(pvalue_chi2up >= threshold || pvalue_chi2down >= threshold){
        std::cout<<"INFO: TESUserTools::pruneSystematic() - Pruning '"
          +systematic->name()+"'/'"+channel->name()+"'/'"+sample->name()+"' (HistoSys - chi2 p-val="<<pvalue_chi2up<<")..."<<std::endl;

        systematic->setSysPrunedBy(PruneUtil::Chi2);
        retvalue=Systematic::OnlyOverallSys;
      }
      // if(chi2up_pval>=threshold && chi2dn_pval>=threshold){
      //   std::cout << "Pruning 1 : " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
      //   std::cout << "pval Up : " << chi2up_pval << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
      //   std::cout << "pval Dn : " << chi2dn_pval << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
      //   retvalue=1;
      // }
    } else{
      //std::cout << "Chi2 Up : " << chi2up   << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
      //std::cout << "Chi2 Dn : " << chi2down << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
      if(chi2up/(systematic->getUpHistogram()->GetNbinsX()-1.) >=1E-10 || chi2down/(systematic->getUpHistogram()->GetNbinsX()-1.) >= 1E-10){
        //std::cout << "Chi2 Up : " << chi2up   << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
        //std::cout << "Chi2 Dn : " << chi2down << " " << systematic->name() << " in category " << channel->name() << " for sample " << sample->name() << std::endl;
        systematic->setSysPrunedBy(PruneUtil::Chi2);
        retvalue=Systematic::OnlyOverallSys;

        std::cout<<"INFO: TESUserTools::pruneSystematic() - Pruning '"
          +systematic->name()+"'/'"+channel->name()+"'/'"+sample->name()+"' (HistoSys - chi2/N)..."<<std::endl;
      }
    }

    //Pruning 2: if maximum relative variation between nominal and up/down in any bin is <0.5, the shape is ignored
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

    if(maxrelvariation<0.5) {

      std::cout<<"INFO: TESUserTools::pruneSystematic() - Pruning '"
        +systematic->name()+"'/'"+channel->name()+"'/'"+sample->name()+"' (HistoSys - max. rel. var.)..."<<std::endl;

      systematic->setSysPrunedBy(PruneUtil::MaxVar);
      retvalue=Systematic::OnlyOverallSys;
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
    //  systematic->setSysPrunedBy(PruneUtil::DiffUpDownSig);
    //  retvalue=Systematic::OnlyOverallSys;
    //}

    // Incorrect logic, this retains if already pruned, it does not prune further
    //if((maxvariation>0.1 || sample->isSignal()) && maxrelvariation>0.005){ // 0.5% and 10% value obtained from Nils thesis
    //std::cout<<"INFO: TESUserTools::pruneSystematic() - Pruning '"
    //     +systematic->name()+"'/'"+channel->name()+"'/'"+sample->name()+"' (HistoSys - max. rel. var.)..."<<std::endl;

    //  systematic->setSysPrunedBy(PruneUtil::DiffUpDownSig);
    //  retvalue=Systematic::NotPruned; //use as histosys and overallsys
    //}


    //one bin histograms can not have histosys
    if(systematic->getUpHistogram()->GetNbinsX()<=1){
      Systematic::removeHistSys(retvalue);

      std::cout<<"INFO: TESUserTools::pruneSystematic() - Pruning '"
        +systematic->name()+"'/'"+channel->name()+"'/'"+sample->name()+"' (HistoSys - 1-bin histo)..."<<std::endl;

    }


    ///////////////////////////////////////////////////////////
    // Pruning for OverallSys
    ///////////////////////////////////////////////////////////
    bool pruneNorm = false; //Default: not pruned
    bool doflatprune = true;
    bool doonesideprune = true;

    // prune using fixed threshold
    float overallsys_threshold = 0.01;
    if (doflatprune){
      float AbsNormUp = std::fabs(systematic->getUpNormalisation()-1.);
      float AbsNormDown = std::fabs(systematic->getDownNormalisation()-1.);
      if ( AbsNormUp <  overallsys_threshold && AbsNormDown <  overallsys_threshold) {
        systematic->setSysPrunedBy(PruneUtil::FlatPrune);
        pruneNorm = true;

        std::cout<<"INFO: TESUserTools::pruneSystematic() - Pruning '"
          +systematic->name()
          +"'/'"+channel->name()
          +"'/'"+sample->name()
          +"' (OverallSys - SameNorm max|DeltaNorm|="<<std::max(AbsNormUp, AbsNormDown)<<")..."<<std::endl;
      }
    }

    if (doonesideprune){
      if ((systematic->getUpNormalisation()-1.)*(systematic->getDownNormalisation()-1.) > 0){
        if (fabs(systematic->getUpNormalisation()-1.) < 0.2*MCuncert && fabs(systematic->getDownNormalisation()-1.) < 0.2*MCuncert){
          systematic->setSysPrunedBy(PruneUtil::OneSideSmall);
          pruneNorm = true;

          std::cout<<"INFO: TESUserTools::pruneSystematic() - Pruning '"
            +systematic->name()+"'/'"+channel->name()+"'/'"+sample->name()+"' (OverallSys - Up or Down MC uncert)..."<<std::endl;

        }

        if (fabs(systematic->getUpNormalisation() - systematic->getDownNormalisation()) < 0.2*MCuncert){
          systematic->setSysPrunedBy(PruneUtil::OneSideSmallDiff);
          pruneNorm = true;

          std::cout<<"INFO: TESUserTools::pruneSystematic() - Pruning '"
            +systematic->name()+"'/'"+channel->name()+"'/'"+sample->name()+"' (OverallSys - DeltaNorm MC uncert)..."<<std::endl;

        }

        if (fabs(systematic->getUpNormalisation() - systematic->getDownNormalisation()) < 1E-3){
          systematic->setSysPrunedBy(PruneUtil::OneSideSmallAbsDiff);
          pruneNorm = true;

          std::cout<<"INFO: TESUserTools::pruneSystematic() - Pruning '"
            +systematic->name()+"'/'"+channel->name()+"'/'"+sample->name()+"' (OverallSys - OneSided MC uncert)..."<<std::endl;

        }
      }
    }

    if(pruneNorm){
      Systematic::removeOverallSys(retvalue);
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

    //if(systematic->name()=="lh_fake_stat")
    //std::cout << "JULIAN: Pruning retvalue="<<retvalue<< std::endl;

    // for(Int_t itype=0; itype<PruneUtil::nPruneType; itype++){
    //   if(systematic->isSysPrunedBy(itype)){
    //     std::cout<<"Pruned by criteria: "<<PruneUtil::getCriteriaName(itype)<<std::endl;
    //     //        getchar();
    //   }
    // }

    return retvalue;
  }

  //-------------------------------------------------------------------------------
  bool TESUserTools::symmetrizeSystematic(Systematic* systematic, TH1F* nominalhist){
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
  std::string TESUserTools::getMainParameter(){
    std::map<std::string, std::string>::iterator it=m_parameters.find("mass");
    if(it==m_parameters.end())
      return "125";
    return it->second;
  }

  //-------------------------------------------------------------------------------
  TString TESUserTools::texify(TString input){
    input.ReplaceAll("_","\\_");
    return input;
  }

  //-------------------------------------------------------------------------------
  bool TESUserTools::analyze(std::vector<Channel*> channels){
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

      TString texfilename;
      texfilename+=m_outputpath;
      texfilename+="/";
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

    TString texfilename;
    texfilename+=m_outputpath;
    texfilename+="/";
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
