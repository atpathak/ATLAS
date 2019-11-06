//
//  SMPlotRebinning.C
//  
//
//  Created by Antonio De Maria on 30/03/2017
//
//

#include <stdio.h>
#include "TFile.h"
#include "TDirectory.h"
#include "TKey.h"
#include "TMacro.h"
#include "TString.h"
#include "TH1F.h"
#include "vector"
#include "TCanvas.h"
#include "TGraphAsymmErrors.h"
#include "TClass.h"
#include <iostream>
#include "TKey.h"
#include "TROOT.h"
#include "TObject.h"
#include "TLegend.h"
#include "TGraph.h"


int rebinning(char* plot_filename, char* binning_filename, char* output_filename)
{
    TFile *plot_file = new TFile(plot_filename,"READ");
    TFile* output_file = new TFile(output_filename,"RECREATE");
    
    TIter next(plot_file->GetListOfKeys());
    TKey *key;

    while ((key = (TKey*)next())){
        TClass *c1 = gROOT->GetClass(key->GetClassName());
        if( ! c1->InheritsFrom("TH1")) continue;

        TH1* original_histo = (TH1*) key->ReadObj();
        
        string histo_name = key->GetName();
        string mystr = histo_name.substr(0, histo_name.find("lfv", 0));
        std::string::size_type i = histo_name.find(mystr); 
        if (i != std::string::npos) histo_name.erase(i, mystr.length());
        histo_name = "bins_"+histo_name;     
 
        // access to the binning file  to get the original binning
        TFile *binning_file = new TFile(binning_filename,"READ");
        TDirectory *dir = gDirectory;
        dir->cd("binning");
       
        TH1F* histo = (TH1F*) gDirectory->Get(histo_name.c_str());
        double* bins = new double[histo->GetNbinsX()+1];
        histo->GetXaxis()->GetLowEdge(bins);
        bins[histo->GetNbinsX()] = histo->GetBinLowEdge(histo->GetNbinsX()) + histo->GetBinWidth(histo->GetNbinsX());
         
        original_histo->SetBins(histo->GetNbinsX(),bins);
        original_histo->GetXaxis()->SetRange(bins[0],bins[histo->GetNbinsX()+1]);
        
        
        output_file->cd();    
	original_histo->Write();  
    }
    return 0;
}

 
