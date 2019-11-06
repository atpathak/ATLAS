//
//  Make_JER_Envelope
//  
//
//  Created by Antonio De Maria
// 
//  description: inject correct 11 NP JER variations in a WS
//


#include "TFile.h"
#include "TKey.h"
#include "TMacro.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include "TH1.h"
#include "TH1F.h"
#include <sys/stat.h>
#include "TCanvas.h"
#include "TLegend.h"


#include "string"
#include "map"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>

int MakeJERFinalVar(const char* input_filename,bool debug = false)
{
   
    std::string c_filename(input_filename);
    std::string filename = c_filename.substr(0, c_filename.size()-5);
    filename += "_WithModJERVar.root";

    TString command="cp -r ";
    command+=input_filename;
    command+=" ";
    command+=filename;   
    system(command.Data());

    TFile *file = new TFile(filename.c_str(),"UPDATE");
   

    // insert the folders' name of your regions
    // lh
   // std::string folders[] = {"llAll_cba_boost_tight_signal","llAll_cba_boost_loose_signal", "llAll_cba_vbf_tight_signal","llAll_cba_vbf_loose_signal","llAll_cba_boost_top","llAll_cba_vbf_top","llAll_cba_boost_zll","llAll_cba_vbf_zll",};
  std::string folders[] = {
  //ll CBA
  "llAll_cba_boost_loose_signal", "llAll_cba_boost_tight_signal", "llAll_cba_boost_top", "llAll_cba_boost_zll", "llAll_cba_vbf_loose_signal", "llAll_cba_vbf_tight_signal", "llAll_cba_vbf_top", "llAll_cba_vbf_zll",\
  //lh CBA
  "mhAll_cba_boost_loose_signal","mhAll_cba_boost_tight_signal","ehAll_cba_boost_loose_signal","ehAll_cba_boost_tight_signal","mhAll_cba_vbf_loose_signal","ehAll_cba_vbf_loose_signal","mhAll_cba_vbf_tight_signal","ehAll_cba_vbf_tight_signal","mhAll_cba_vbf_top","ehAll_cba_vbf_top","mhAll_cba_boost_top","ehAll_cba_boost_top","mhAll_mva_boost_signal","ehAll_mva_boost_signal","mhAll_mva_vbf_signal","ehAll_mva_vbf_signal","mhAll_mva_boost_top","ehAll_mva_boost_top","mhAll_mva_vbf_top","ehAll_mva_vbf_top",\
  //hh CBA
  "hhAll_cba_boost_loose_signal","hhAll_cba_boost_tight_signal","hhAll_cba_vbf_highdr_tight_signal","hhAll_cba_vbf_signal","hhAll_cba_boost_signal","hhAll_cba_vbf_highdr_loose_signal", "hhAll_cba_vbf_lowdr_signal","hhAll_mva_rest_control","hhAll_mva_vbf_signal","hhAll_mva_boost_signal","hhAll_preselection","hhAll_cba_boost_super_tight_signal","hhAll_cba_boost_tight_notsupertight_signal",\
  //hh MVA
  "mva_rest_control","mva_restmmc_control","mva_restdphi_control","mva_vbf_signal","mva_boost_signal",
  };
    vector< std::string > vec_folders( folders, folders + ( sizeof ( folders ) /  sizeof ( std::string ) ) );

    std::string processes[] = {"Ztt","Data","ggHWW","VBFHWW","Zll","ZllEWK","ZttEWK","VV","Top","ggH","VBFH","WH","ZH","ttH","Fake","W"};
    vector< std::string > vec_processes( processes, processes + ( sizeof ( processes ) /  sizeof ( std::string ) ) );
    
    // name of the variations
    std::string variation[] = {"jet_jer_crosscalibfwd_high","jet_jer_noisefwd_high","jet_jer_np0_low","jet_jer_np0_high","jet_jer_np1_low","jet_jer_np1_high","jet_jer_np2_low","jet_jer_np2_high","jet_jer_np3_low","jet_jer_np3_high","jet_jer_np4_low","jet_jer_np4_high","jet_jer_np5_low","jet_jer_np5_high","jet_jer_np6_low","jet_jer_np6_high","jet_jer_np7_low","jet_jer_np7_high","jet_jer_np8_low","jet_jer_np8_high"};
    std::vector<std::string> vec_variations( variation, variation + ( sizeof ( variation ) /  sizeof ( std::string ) ) );

    TH1F* var_histo_data;
    TH1F* var_histo_mc;
    TH1F* nom_histo_data;
    TH1F* nom_histo_mc;
   
    // create a txt file to save results
    std::string fnameTxt = "debug_file.txt";
    std::ofstream out_file;
    out_file.open(fnameTxt.c_str());
    
    TDirectory *cur_dir = gDirectory;
    TDirectory *sub_dir = gDirectory;
    TIter next(file->GetListOfKeys());
    TKey *key;
    while((key = (TKey*)next() ) )
    {
        // get the different regions
        std::string dir_name( key->GetName() );
        if (key->IsFolder() && std::find(vec_folders.begin(), vec_folders.end(), dir_name) != vec_folders.end()  ) {
            
            if(cur_dir->cd(key->GetName())){
                
                if(debug) out_file << " current region " << key->GetName() << std::endl;
  
                string nom_histo_data_name = dir_name + "/Data/nominal";
                file->GetObject(nom_histo_data_name.c_str(), nom_histo_data);
                
                if (nom_histo_data==0) {
                  std::cout << nom_histo_data_name.c_str() << " not found" << std::endl;
                }


                // loop over the variations
                for(std::vector<string>::size_type i = 0; i != vec_variations.size(); i++) {
                    
                    if(debug) out_file << " considering the variation " << vec_variations[i] << std::endl;
                    
                    // get the variation histogram for data
                    std::string histo_data_name = dir_name + "/Data/" + vec_variations[i];
                    file->GetObject(histo_data_name.c_str(), var_histo_data);
                    if (var_histo_data==0) {
                         std::cout << histo_data_name.c_str() << " not found, using nominal" << std::endl;
                         var_histo_data = (TH1F*) nom_histo_data->Clone(histo_data_name.c_str());
                    }
                    
                    //loop over the samples
                    for(std::vector<string>::size_type j = 0; j != vec_processes.size(); j++) {
 
                        if(debug)  out_file << " considering the sample " << vec_processes[j] << std::endl;
                       
                        std::string path_process_name = dir_name + "/" + vec_processes[j];
                                        
                        if(sub_dir->cd(path_process_name.c_str())){
                            
                            string nom_histo_mc_name = dir_name + "/" + vec_processes[j] + "/nominal";
                            file->GetObject(nom_histo_mc_name.c_str(), nom_histo_mc);
                            
                            // get the variation histogram for the different processes
                            std::string histo_process_name = dir_name + "/" + vec_processes[j] + "/" + vec_variations[i];
                            file->GetObject(histo_process_name.c_str(), var_histo_mc);
                            if  (var_histo_mc==0) {
                               std::cout << histo_process_name.c_str() << " not found, using nominal" << std::endl;
                               var_histo_mc = (TH1F*) nom_histo_mc->Clone(histo_process_name.c_str());
                              }
                        
                            std::string final_histo_name = "final_" + vec_variations[i];
                            TH1F* final_histo = (TH1F*) var_histo_mc->Clone(final_histo_name.c_str());
                    
                            // loop over the bins
                            for(int t=1; t<= nom_histo_data->GetNbinsX(); t++){
			      double rel_diff_data =  (TMath::Abs(nom_histo_data->GetBinContent(t))<1e-7)? 0. : (var_histo_data->GetBinContent(t) -  nom_histo_data->GetBinContent(t))/nom_histo_data->GetBinContent(t);
			      double rel_diff_mc   =   (TMath::Abs(nom_histo_mc->GetBinContent(t))<1e-7) ? 0. : (var_histo_mc->GetBinContent(t) - nom_histo_mc->GetBinContent(t))/nom_histo_mc->GetBinContent(t);
                                
                                double sigma;
                                if(rel_diff_data > 0) sigma = rel_diff_mc - rel_diff_data;
                                else{ sigma = rel_diff_mc + rel_diff_data;}
                                
                                if(debug)
                                {
                                    out_file << std::endl;
                                    out_file << " bin " << t << " : " << " MMC bin [ " << nom_histo_data->GetBinLowEdge(t) << " ; " << nom_histo_data->GetBinLowEdge(t+1) << "]" << std::endl;
                                    out_file << " data nominal " << nom_histo_data->GetBinContent(t) << " mc nominal " << nom_histo_mc->GetBinContent(t) << std::endl;
                                    out_file << " data variation " << var_histo_data->GetBinContent(t) << " mc variation " << var_histo_mc->GetBinContent(t) << std::endl;
                                    out_file << " data rel var " << rel_diff_data << " mc rel val " << rel_diff_mc << std::endl << " -> sigma " << sigma << std::endl;
                                    out_file << " new mc bin content " <<  sigma*nom_histo_mc->GetBinContent(t) + nom_histo_mc->GetBinContent(t) << std::endl;
                                    out_file << std::endl;
                                }
                                
                                final_histo->SetBinContent(t, sigma*nom_histo_mc->GetBinContent(t) + nom_histo_mc->GetBinContent(t) );
                            }
                            final_histo->Write();
                           
                            if(debug){
                                // create a canvas with the nominal mc, the old variation and the new variation
                                std::string canvas_title = vec_processes[j]+"_"+vec_variations[i];
                                TCanvas *canvas = new TCanvas( canvas_title.c_str(), canvas_title.c_str(), 800,600);
                                TPad pad1("pad1","",0.,0.3,1.0,1.0,0,0.,0);
                                TPad pad2("pad2","",0.,0.0,1.0,0.3,0,0.,0);
                                pad1.SetBottomMargin(0.002);
                                pad2.SetTopMargin(0.00);
                                pad2.SetBottomMargin(0.40);
                                pad1.Draw();
                                pad1.SetNumber(1);
                                pad2.Draw();
                                pad2.SetNumber(2);
                                
                                canvas->cd(1);
                                nom_histo_mc->SetMarkerColor(kBlack);
                                var_histo_mc->SetMarkerColor(kRed);
                                final_histo->SetMarkerColor(kBlue);
                                nom_histo_mc->Draw();
                                var_histo_mc->Draw("SAME");
                                final_histo->Draw("SAME");
                                
                                TLegend *legend = new TLegend(0.60,0.67,0.9,0.85);
                                legend->AddEntry(nom_histo_mc, "Nominal", "lep");
                                legend->AddEntry(var_histo_mc, "Initial Variation", "lep");
                                legend->AddEntry(final_histo, "Final Variation", "lep");
                                legend->SetBorderSize(0);
                                legend->Draw();
                                canvas->Update();
                                
                                canvas->cd(2);
                                
                                TH1F* ratio_error  =  (TH1F*) nom_histo_mc->Clone("error");
                                TH1F* ratio_input  =  (TH1F*) var_histo_mc->Clone("input_ratio");
                                TH1F* ratio_output =  (TH1F*) final_histo->Clone("output_ratio");
                                
                                ratio_error->SetTitle("");
                                ratio_error->SetTitleOffset(3.5);
                                ratio_error->GetYaxis()->SetNdivisions(5);
                                ratio_error->Divide(nom_histo_mc);
                                ratio_input->Divide(nom_histo_mc);
                                ratio_output->Divide(nom_histo_mc);
                                ratio_input->SetMarkerColor(kRed);
                                ratio_output->SetMarkerColor(kBlue);
                                
                                ratio_error->SetFillColor(22);
                                ratio_error->Draw("E3");
                                ratio_input->Draw("SAME");
                                ratio_output-> Draw("SAME");
                                
                                canvas->Update();
                                
                                canvas_title += ".png";
                                canvas->SaveAs(canvas_title.c_str());
                            }
                            
                        }
                    } // end of loop over processes
                } // end of loop over the variations
            }
        }
    } // end of loop over regions
    if(debug && out_file.is_open()) out_file.close();
    file->Close();


    return 0;
}

// TODO
// 1) finish the debug part with a txt file with the results
// 2) maybe a ratio hist in the debug mode
