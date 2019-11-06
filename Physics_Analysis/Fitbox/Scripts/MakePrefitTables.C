//
//  Test.C
//  
//
//  Created by Antonio De Maria on 21/12/16.
//
//

#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2F.h"
#include "TF1.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TTree.h"
#include "TGraph.h"
#include "TMath.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TProfile.h"
#include <TGraphErrors.h>
#include "TH2.h"
#include "TLatex.h"
#include "TH1I.h"


#include "iostream"
#include "string"
#include "TCut.h"
#include "map"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>

int MakeTables(char *ws_input_filename){
    
    TFile *file = new TFile(ws_input_filename, "READ");
    
    std::string samples[] = {"Data","Zll","Ztt","Top","W","Fake"};
    std::vector< std::string > vec_samples( samples, samples + ( sizeof ( samples ) /  sizeof ( std::string ) ) );
    std::string regions[] = {"mh_1p_InclEta_FailLooseID_signal","mh_1p_InclEta_PassLooseFailMediumID_signal","mh_1p_InclEta_PassMediumFailTightID_signal","mh_1p_InclEta_PassTightID_signal"};
   
    std::vector< std::string > vec_regions( regions, regions + ( sizeof ( regions ) /  sizeof ( std::string ) ) );
    
    TH1F *histo = new TH1F();
    TH1F *sum_histo = new TH1F();
    
   // histo->Sumw2();
    sum_histo->Sumw2();
    
    int first = 0;
    bool check = false;
    double data_int = 0;
    double data_err = 0;
    
    // insert name of the region, integral and integral error in maps
    std::map<std::string,double> yields;
    std::map<std::string,double> errors;
    
    for(std::vector<string>::size_type i = 0; i != vec_regions.size(); i++) {
        
        // create a txt file with the yields for each region
       
        std::string fnameTxt = regions[i]+"_yields.txt";
        std::ofstream out_file;
        out_file.open(fnameTxt.c_str());
    
        out_file << "\\begin{table}" << std::endl;
        out_file << "\\renewcommand{\\arraystretch}{1.3}" << std::endl;
        out_file << "\\begin{center}" << std::endl;
        out_file << "\\begin{tabular}{ll}\\hline" << std::endl;
        out_file << "Sample & Prefit \\\\ \\hline \\hline " << std::endl;
        
        for(std::vector<std::string>::size_type j = 0; j != vec_samples.size(); j++) {
        
            std::string histo_path = vec_regions[i] + "/" + vec_samples[j] + "/nominal";
            file->GetObject(histo_path.c_str(), histo);
            
            TString sample_name(vec_samples[j]);
            if(! sample_name.Contains("Data"))
            {
                if(first == 0){(*sum_histo) = (*histo); first++;}
                else{check = sum_histo->Add(histo); first++;}
            
                double calc_err = 0;
                double calc_int = histo->IntegralAndError(0,histo->GetNbinsX(),calc_err);
            
                out_file << vec_samples[j] << " & " << calc_int << " $\\pm$ " << calc_err << " \\\\ " << std::endl;
        
            } else{
                
                data_int = histo->IntegralAndError(0,histo->GetNbinsX(),data_err);
                
            }
        }
        double calc_err_tot = 0;
        double calc_int_tot = sum_histo->IntegralAndError(0,sum_histo->GetNbinsX(),calc_err_tot);
       
        out_file << "\\hline" << std::endl;
        out_file << " Total " << " & " << calc_int_tot << " $\\pm$ " << calc_err_tot << " \\\\ " << std::endl;
        out_file << "\\hline" << std::endl;
        out_file << " Data  " << " & " <<  data_int << " $\\pm$ " << data_err << "\\\\" << std::endl;
        out_file << "\\hline" << std::endl;
        out_file << "\\hline" << std::endl;
        out_file << "\\end{tabular}" << std::endl;
        out_file << "\\end{center}" << std::endl;
        out_file << "\\caption{Prefit yields}" << std::endl;
        out_file << "\\label{tb:yields_Prefit}" << std::endl;
        out_file << "\\end{table}" << std::endl;
        
        if( out_file.is_open() ) out_file.close();
        
        sum_histo->Reset();
        first = 0;

        
    }
    

    return 0;

}
