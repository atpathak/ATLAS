//
//  Make_Ztt_MurMuf_Theory_Envelope
//  
//
//  Created by Antonio De Maria on 26/04/17.
// 
//

#include "TFile.h"
#include "TKey.h"
#include "TMacro.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "TH1.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include <sys/stat.h>

int MakeDebugPlots(std::string  t_vec_processes, std::string t_vec_region, TH1F* hnom, TH1F* hup, TH1F* hdn, std::string variation)
{
    std::string shower("shower_sys"); 

    // create a canvas with the nominal mc, the old variation and the new variation
    std::string canvas_title = t_vec_processes+"_"+t_vec_region+"_"+variation+"_evenlope";
    TCanvas *canvas = new TCanvas( canvas_title.c_str(), canvas_title.c_str(), 800,600);
    TPad pad1("pad1","",0.,0.3,1.0,1.0,0,0.,0);
    TPad pad2("pad2","",0.,0.0,1.0,0.3,0,0.,0);

    TH1F* t_nom_histo_mc=  (TH1F*) hnom->Clone();
    TH1F* t_var_histo_up=  (TH1F*) hup->Clone();
    TH1F* t_var_histo_dn=  (TH1F*) hdn->Clone();

    t_nom_histo_mc->Sumw2();
    t_var_histo_up->Sumw2();
    t_var_histo_dn->Sumw2();
				
    pad1.SetBottomMargin(0.002);
    pad2.SetTopMargin(0.00);
    pad2.SetBottomMargin(0.40);
    pad1.Draw();
    pad1.SetNumber(1);
    pad2.Draw();
    pad2.SetNumber(2);
                                
    canvas->cd(1);
    t_nom_histo_mc->SetMarkerColor(kBlack);
    t_var_histo_up->SetMarkerColor(kRed);
    t_var_histo_dn->SetMarkerColor(kBlue);

    t_nom_histo_mc->SetMaximum(1.4*t_nom_histo_mc->GetMaximum());
    t_nom_histo_mc->SetMinimum(0.6*t_nom_histo_mc->GetMinimum());
				
    t_nom_histo_mc->Draw();
    t_var_histo_up->Draw("SAME");
    if (variation.compare(shower) != 0) {t_var_histo_dn->Draw("SAME");}
                                
    TLegend *legend = new TLegend(0.60,0.67,0.9,0.85);
    legend->AddEntry(t_nom_histo_mc, "Nominal", "lep");
    legend->AddEntry(t_var_histo_up, "Up Variation", "lep");
    if (variation.compare(shower) != 0){legend->AddEntry(t_var_histo_dn, "Down Variation", "lep");}
    legend->SetBorderSize(0);
    legend->Draw();
    canvas->Update();
                                
    canvas->cd(2);
                                
    TH1F* ratio_error  =  (TH1F*) t_nom_histo_mc->Clone("error");
    TH1F* ratio_up  =  (TH1F*) t_var_histo_up->Clone("up_ratio");
    TH1F* ratio_dn =  (TH1F*) t_var_histo_dn->Clone("dn_ratio");
                                
    ratio_error->SetTitle("");
    ratio_error->SetTitleOffset(3.5);
    ratio_error->GetYaxis()->SetNdivisions(5);
    ratio_error->Divide(t_nom_histo_mc);
    ratio_up->Divide(t_nom_histo_mc);
    ratio_dn->Divide(t_nom_histo_mc);
    ratio_up->SetMarkerColor(kRed);
    ratio_dn->SetMarkerColor(kBlue);

    ratio_error->SetFillColor(1);
    ratio_error->SetLineColor(0);
    ratio_error->SetMarkerStyle(1);
    ratio_error->SetMarkerSize(0.0001);
    ratio_error->SetMarkerColor(1);
    ratio_error->SetFillStyle(3004);

    ratio_error->SetMaximum(2.0);
    ratio_error->SetMinimum(0.0);
				
    ratio_error->Draw("E2,same");
    ratio_up->Draw("SAME");
    if (variation.compare(shower) != 0){ratio_dn-> Draw("SAME");}
                                
    canvas->Update();
                                
    canvas_title += ".png";
    canvas->SaveAs(canvas_title.c_str());

    return 0;

}


int MakeTheoryAndShowerEnvelope(const char* input_filename, int option=1, bool debug=false)
{
   
    std::string c_filename(input_filename);
    std::string filename = c_filename.substr(0, c_filename.size()-5);
    filename += "_WithTheoryEnv.root";

    TString command="cp -r ";
    command+=input_filename;
    command+=" ";
    command+=filename;   
    system(command.Data());

    TFile *file = new TFile(filename.c_str(),"UPDATE");
   

    // insert the folders' name of your regions
    
    std::string folders[] = {"llAll_cba_boost_loose_signal", "llAll_cba_boost_tight_signal", "llAll_cba_vbf_loose_signal", "llAll_cba_vbf_tight_signal", "llAll_cba_boost_top", "llAll_cba_boost_zll", "llAll_cba_vbf_top", "llAll_cba_vbf_zll","llAll_cba_boost_ztt","llAll_cba_vbf_ztt","llAll_lfv_inc_signal","llAll_lfv_inc_top","llAll_lfv_vbf_signal","llAll_lfv_vbf_top","llAll_cba_boost_loose_df_signal", "llAll_cba_boost_tight_df_signal", "llAll_cba_vbf_loose_df_signal", "llAll_cba_vbf_tight_df_signal", "llAll_cba_boost_df_top", "llAll_cba_boost_df_zll", "llAll_cba_vbf_df_top", "llAll_cba_df_vbf_zll","llAll_cba_boost_loose_ee_signal", "llAll_cba_boost_tight_ee_signal", "llAll_cba_vbf_loose_ee_signal", "llAll_cba_vbf_tight_ee_signal", "llAll_cba_boost_ee_top", "llAll_cba_boost_ee_zll", "llAll_cba_vbf_ee_top", "llAll_cba_ee_vbf_zll","llAll_cba_boost_loose_mumu_signal", "llAll_cba_boost_tight_mumu_signal", "llAll_cba_vbf_loose_mumu_signal", "llAll_cba_vbf_tight_mumu_signal", "llAll_cba_boost_mumu_top", "llAll_cba_boost_mumu_zll", "llAll_cba_vbf_mumu_top", "llAll_cba_mumu_vbf_zll","llAll_cba_boost_ztt","llAll_cba_vbf_ztt","llAll_lfv_inc_signal","llAll_lfv_inc_top","llAll_lfv_vbf_signal","llAll_lfv_vbf_top", "llAll_lfv_vbf_ztt","llAll_lfv_inc_ztt","llAll_mva_boost_sf_signal","llAll_mva_boost_df_signal","llAll_mva_vbf_signal","llAll_mva_boost_zll","llAll_mva_boost_top","llAll_mva_vbf_zll","llAll_mva_vbf_top"};
    
    std::vector< std::string > vec_folders( folders, folders + ( sizeof ( folders ) /  sizeof ( std::string ) ) );
    std::vector<std::string>::iterator it_folders;
    
    std::string processes[] = {"Ztt","VBFH","WH","ZH","ggH","LFV_VBFH","LFV_WH","LFV_ZH","LFV_ggH"};
    std::vector< std::string > vec_processes( processes, processes + ( sizeof ( processes ) /  sizeof ( std::string ) ) );
    
    // name of the variations for Ztt
    std::string variation_ztt[] = {"mur2_muf1","mur1_muf05","mur05_muf05","mur1_muf2","mur2_muf2","mur05_muf1"};
    std::vector<std::string> vec_variations_ztt( variation_ztt, variation_ztt + ( sizeof ( variation_ztt ) /  sizeof ( std::string ) ) );
    
    // name of the variations for VBF
    std::string variation_vbf[] = {"theory_sig_qcd_0_high","theory_sig_qcd_1_high","theory_sig_qcd_2_high","theory_sig_qcd_3_high","theory_sig_qcd_4_high","theory_sig_qcd_5_high",};
    std::vector<std::string> vec_variations_vbf( variation_vbf, variation_vbf + ( sizeof ( variation_vbf ) /  sizeof ( std::string ) ) );
    
    // name of the variations for VH: neglect crossterms qcd_2 and qcd_5
    std::string variation_vh[] = {"theory_sig_qcd_0_high","theory_sig_qcd_1_high"/*,"theory_sig_qcd_2_high"*/,"theory_sig_qcd_3_high","theory_sig_qcd_4_high"/*,"theory_sig_qcd_5_high"*/,"theory_sig_qcd_6_high","theory_sig_qcd_7_high"};
    std::vector<std::string> vec_variations_vh( variation_vh, variation_vh + ( sizeof ( variation_vh ) /  sizeof ( std::string ) ) );
    
    vector<TH1F*> vec_histo_mur_muf;
    TH1F* histo_shower_herwig;
    TH1F* histo_shower_powheg;
    TH1F* nominal = new TH1F();

    // create a txt file to save results
    std::string fnameTxt = "debug_file.txt";
    std::ofstream out_file;
    out_file.open(fnameTxt.c_str());

    
    TDirectory *cur_dir = gDirectory;
    TDirectory *sub_dir = gDirectory;
    TIter next(file->GetListOfKeys());
    TKey *key;
    Int_t nHistWithNegBins = 0;
    while((key = (TKey*)next() ) )
    {
        std::string dir_name( key->GetName() );
        if (key->IsFolder() && std::find(vec_folders.begin(), vec_folders.end(), dir_name) != vec_folders.end()  ) {
            
            if(cur_dir->cd(key->GetName())){
                std::string regionName=key->GetName();
                std::cout << "--------------------------------------------------" << std::endl;
                std::cout << "Current region: " << regionName << std::endl;

                if(debug) out_file << " current region " << key->GetName() << std::endl;
		
                //loop over the samples
                for(std::vector<string>::size_type j = 0; j != vec_processes.size(); j++) {
                    
                    std::cout << "--------------------------------------------------" << std::endl;
                    std::cout << " Considering the sample " << vec_processes[j] << std::endl;

                    if(debug) out_file << " considering the sample " << vec_processes[j] << std::endl;
		    
                    std::string path_process_name = dir_name + "/" + vec_processes[j];
                    
                    if(sub_dir->cd(path_process_name.c_str())){

                        vec_histo_mur_muf.clear();
                        std::string histo_path_nominal = path_process_name + "/nominal";
                        std::cout<<"Trying to retrieve "<<histo_path_nominal<<"..."<<std::endl;
                        nominal =  (TH1F*) file->Get(histo_path_nominal.c_str());
			
                        if(nominal==NULL){
                            std::cout<<"ERROR: cannot find histogram of "<<histo_path_nominal<<"."<<std::endl;
                            return 1;
                        }


                        if(vec_processes[j].compare("Ztt") == 0){
			  std::cout << "Doing Ztt" << std::endl;
                            // getting the variations for Ztt
                            for(std::vector<string>::size_type i = 0; i != vec_variations_ztt.size(); i++) {
                                TH1F* curr_histo = new TH1F();
                                
                                std::string histo_path_var = path_process_name + "/theory_ztt_lhe3weight_"+vec_variations_ztt[i]+"_pdf261000"; // should be  pdf261000_high

                                if(debug) out_file << " considering the variation " << histo_path_var << std::endl;
				
                                std::cout<<"Trying to retrieve "<<histo_path_var<<"..."<<std::endl;
                                curr_histo= (TH1F*) file->Get(histo_path_var.c_str());
                                if(curr_histo==NULL){
				  histo_path_var=histo_path_var+"_high";
				  curr_histo= (TH1F*) file->Get(histo_path_var.c_str());
				}
				if(curr_histo==NULL)
				  {
                                    std::cout<<"ERROR: cannot find histogram of "<<histo_path_var<<"."<<std::endl;
                                    return 1;
				  }
                                vec_histo_mur_muf.push_back(curr_histo);
                            }
                        }
                        else if(vec_processes[j].compare("VBFH") == 0 || vec_processes[j].compare("LFV_VBFH") == 0){
			  std::cout << "Doing VBF" << std::endl;
                            // get the shower sys histograms
                            std::string histo_path_shower_var = path_process_name + "/theory_sig_shower_py";
                            std::cout<<"Trying to retrieve "<<histo_path_shower_var<<"..."<<std::endl;
			    histo_shower_powheg= (TH1F*) file->Get(histo_path_shower_var.c_str());
                            
                            histo_path_shower_var = path_process_name + "/theory_sig_shower_hw";
                            std::cout<<"Trying to retrieve "<<histo_path_shower_var<<"..."<<std::endl;
                            histo_shower_herwig= (TH1F*) file->Get(histo_path_shower_var.c_str());
                            
                            if( histo_shower_powheg == NULL || histo_shower_herwig == NULL){
                                std::cout<<"ERROR: cannot find histogram of for shower sys uncertainties (theory_sig_shower_py or theory_sig_shower_hw)" <<std::endl;
                                return 1;
                            }
                            
                            
                            // get the variations MUR-MUF for VBFH
                            for(std::vector<string>::size_type i = 0; i != vec_variations_vbf.size(); i++) {
                                TH1F* curr_histo = new TH1F();
                                std::string histo_path_var = path_process_name + "/" + vec_variations_vbf[i];
				
                                if(debug) out_file << " considering the variation " << histo_path_var << std::endl;
				
                                std::cout<<"Trying to retrieve "<<histo_path_var<<"..."<<std::endl;
                                curr_histo= (TH1F*) file->Get(histo_path_var.c_str());
                                if(curr_histo==NULL){
                                    std::cout<<"ERROR: cannot find histogram of "<<histo_path_var<<"."<<std::endl;
                                    return 1;
                                }
                                vec_histo_mur_muf.push_back(curr_histo);
                            }
                        }
                        else if(vec_processes[j].compare("ggH") == 0 || vec_processes[j].compare("LFV_ggH") == 0){
			  std::cout << "Doing ggH" << std::endl;
                            // get the shower sys histograms
                            std::string histo_path_shower_var = path_process_name + "/theory_sig_shower_py";
                            std::cout<<"Trying to retrieve "<<histo_path_shower_var<<"..."<<std::endl;
                            histo_shower_powheg=  (TH1F*) file->Get(histo_path_shower_var.c_str());
                            
                            histo_path_shower_var = path_process_name + "/theory_sig_shower_hw";
                            std::cout<<"Trying to retrieve "<<histo_path_shower_var<<"..."<<std::endl;
                            histo_shower_herwig=  (TH1F*) file->Get(histo_path_shower_var.c_str());
                            
                            if( histo_shower_powheg == NULL || histo_shower_herwig == NULL){
                                std::cout<<"ERROR: cannot find histogram of for shower sys uncertainties (theory_sig_shower_py or theory_sig_shower_hw)" <<std::endl;
                                return 1;
                            }
                        }
                        else{
			  std::cout << "Doing something else" << std::endl;
                            // get the shower sys histograms
                            std::string histo_path_shower_var = path_process_name + "/theory_sig_shower_py";
                            std::cout<<"Trying to retrieve "<<histo_path_shower_var<<"..."<<std::endl;
                            histo_shower_powheg= (TH1F*) file->Get(histo_path_shower_var.c_str());
                            
                            histo_path_shower_var = path_process_name + "/theory_sig_shower_hw";
                            std::cout<<"Trying to retrieve "<<histo_path_shower_var<<"..."<<std::endl;
                            histo_shower_herwig= (TH1F*) file->Get(histo_path_shower_var.c_str());
                            
                            if( histo_shower_powheg == NULL || histo_shower_herwig == NULL){
                                std::cout<<"ERROR: cannot find histogram of for shower sys uncertainties (theory_sig_shower_py or theory_sig_shower_hw)" <<std::endl;
                                return 1;
                            }
                            
                            // get the variations  for VH
                            for(std::vector<string>::size_type i = 0; i != vec_variations_vh.size(); i++) {
                                TH1F* curr_histo = new TH1F();
                                std::string histo_path_var = path_process_name + "/" + vec_variations_vh[i];

                                if(debug) out_file << " considering the variation " << histo_path_var << std::endl;

                                std::cout<<"Trying to retrieve "<<histo_path_var<<"..."<<std::endl;
                                curr_histo= (TH1F*) file->Get(histo_path_var.c_str());
                                if(curr_histo==NULL){
                                    std::cout<<"ERROR: cannot find histogram of "<<histo_path_var<<"."<<std::endl;
                                    return 1;
                                }
                                vec_histo_mur_muf.push_back(curr_histo);
                            }
                        }
                        
                        std::cout<<"All histograms were successfully retrieved."<<std::endl;
                        std::cout<<"Going to make new MUR-MUF envelope plots and shower sys plots..."<<std::endl;
                        
                        Double_t orig_entries = nominal->GetEntries();

                        std::string np_process_name;
                        if  ( vec_processes[j].find("WH") != std::string::npos ||  vec_processes[j].find("ZH") != std::string::npos )
                            np_process_name= "VH";
                        else if ( vec_processes[j].find("Ztt") != std::string::npos )
                            np_process_name= "ztt";
			else if ( vec_processes[j].find("VBFH") != std::string::npos )
                          np_process_name= "VBFH";
			else if ( vec_processes[j].find("ggH") != std::string::npos )
                          np_process_name= "ggH";
                        else
                            np_process_name= vec_processes[j];
			
                        std::string envelope_high_name = "theory_" + np_process_name + "_mur_muf_envelope_high";
                        std::string envelope_down_name = "theory_" + np_process_name + "_mur_muf_envelope_low";
                        std::string final_shower_sys_histo_up_name = "theory_" + np_process_name + "_shower_high";
                         
                        TH1F* final_envelope_up = (TH1F*) nominal->Clone(envelope_high_name.c_str());
                        TH1F* final_envelope_down = (TH1F*) nominal->Clone(envelope_down_name.c_str());
                        TH1F* final_shower_sys_histo_up = (TH1F*) nominal->Clone(final_shower_sys_histo_up_name.c_str());
                        
                        // loop over the bins to get final variations
                        double ratio_hw_ph;
                        for(int l=1; l<= nominal->GetNbinsX(); l++){

			  if(vec_processes[j].compare("Ztt") != 0){ //skip Ztt
                            // shower sys section
                            std::cout << " starting the shower sys part " << std::endl;
                            if ( histo_shower_herwig &&  histo_shower_powheg) {
			    std::cout << histo_shower_herwig << " " << histo_shower_powheg <<  std::endl;
			    std::cout << histo_shower_herwig->GetName() << /*" " << histo_shower_powheg->GetName() <<*/  std::endl;
			    std::cout << nominal << " " << final_shower_sys_histo_up <<  std::endl;
                            if(histo_shower_herwig->GetBinContent(l) != 0 && histo_shower_powheg->GetBinContent(l) != 0){

			      std::cout << "1" << std::endl;
			      ratio_hw_ph = histo_shower_herwig->GetBinContent(l)/histo_shower_powheg->GetBinContent(l);
			      std::cout << "2" << std::endl;
			      final_shower_sys_histo_up->SetBinContent(l, nominal->GetBinContent(l)*ratio_hw_ph);
			      std::cout << "3" << std::endl;
                            }
                            else{ // shower sys histo with an empty bin
 			      std::cout << "4" << std::endl;
			      ratio_hw_ph = histo_shower_herwig->Integral(1,histo_shower_herwig->GetNbinsX())/histo_shower_powheg->Integral(1,histo_shower_powheg->GetNbinsX());
                                std::cout << " bin " << l << " has bin fixed to total integral ratio herwig/powheg " << ratio_hw_ph << std::endl;
                                final_shower_sys_histo_up->SetBinContent(l, nominal->GetBinContent(l)*ratio_hw_ph);         
 			      std::cout << "5" << std::endl;
                            }
                            
                            std::cout << " finished the shower sys part and starting the MUR-MUF section" << std::endl;
			  }
                          }
                            // MUR-MUF section
                            if(option == 1)
                            {
                                double max_dev = 0;
                                double max_dev_tmp = 0;
                                // loop over the variations to get maximum shift from nominal
                                for(std::vector<TH1F*>::size_type t = 0; t != vec_histo_mur_muf.size(); t++) {
                                    max_dev_tmp = std::fabs( nominal->GetBinContent(l) - vec_histo_mur_muf[t]->GetBinContent(l));
                                    if(max_dev_tmp > max_dev)
                                    {
                                        max_dev = max_dev_tmp;
                                        final_envelope_up->SetBinContent(l, nominal->GetBinContent(l) + max_dev);
                                        final_envelope_down->SetBinContent(l, nominal->GetBinContent(l) - max_dev);
                                    }
                                }
                                // print yield for debug
                                std::cout << " bin : " << l << " nominal : " << nominal->GetBinContent(l) << " envelope up " << final_envelope_up->GetBinContent(l) << " envelope down " << final_envelope_down->GetBinContent(l)  << std::endl;
                                max_dev = 0;
                                max_dev_tmp = 0;
                            }
                            else{
                                // less conservative solution up = max(up_var), down = max(down_var)
                                double max_dev_up = 0;
                                double max_dev_down = 0;
                                double max_dev_tmp_up = 0;
                                double max_dev_tmp_down = 0;
                                for(std::vector<TH1F*>::size_type i = 0; i != vec_histo_mur_muf.size(); i++) {
                                    // it's down variation
                                    if( nominal->GetBinContent(l) - vec_histo_mur_muf[i]->GetBinContent(l) > 0)
                                    {
                                        max_dev_tmp_down = nominal->GetBinContent(l) - vec_histo_mur_muf[i]->GetBinContent(l);
                                        if( max_dev_tmp_down > max_dev_down){
                                            max_dev_down = max_dev_tmp_down;
                                            final_envelope_down->SetBinContent(l, nominal->GetBinContent(l) - max_dev_down);
                                        }
                                    }
                                    else{ // it's up variation
                                        max_dev_tmp_up = vec_histo_mur_muf[i]->GetBinContent(l) - nominal->GetBinContent(l);
                                        if( max_dev_tmp_up > max_dev_up){
                                            max_dev_up = max_dev_tmp_up;
                                            final_envelope_up->SetBinContent(l, nominal->GetBinContent(l) + max_dev_up);
                                        }
                                    }
                                }

                                max_dev_up = 0;
                                max_dev_down = 0;
                                max_dev_tmp_up = 0;
                                max_dev_tmp_down = 0;
                                
                            }

                            std::cout << " bin : " << l << " nominal : " << nominal->GetBinContent(l) << " envelope up " << final_envelope_up->GetBinContent(l) << " envelope down " << final_envelope_down->GetBinContent(l)  << std::endl;
                            
                             std::cout << " bin : " << l << " nominal : " << nominal->GetBinContent(l) << " shower sys up " << final_shower_sys_histo_up->GetBinContent(l) << std::endl;

                            if(debug){
                                out_file <<  " Shower sys part " << std::endl;
                                out_file <<  " bin : " << l << " nominal : " << nominal->GetBinContent(l) << " shower sys up " << final_shower_sys_histo_up->GetBinContent(l) << std::endl;
                                out_file << " bin : " << l << " up - nom : " <<  final_shower_sys_histo_up->GetBinContent(l) -  nominal->GetBinContent(l) << std::endl;
                                
                                
                                out_file <<  " MUR-MUF variations " << std::endl;
                                out_file <<  " bin : " << l << " nominal : " << nominal->GetBinContent(l) << " envelope up " << final_envelope_up->GetBinContent(l) << " envelope down " << final_envelope_down->GetBinContent(l)  << std::endl;
                                out_file << " bin : " << l << " up - nom : " <<  final_envelope_up->GetBinContent(l) -  nominal->GetBinContent(l) <<  " down - nom : " <<  final_envelope_down->GetBinContent(l) -  nominal->GetBinContent(l) << std::endl;
                            }

			    
                        } // end of bins
                        //Set entries to the original value
                        final_envelope_up->SetEntries(orig_entries);
                        final_envelope_down->SetEntries(orig_entries);
                        final_shower_sys_histo_up->SetEntries(orig_entries);
                        
                        //Check if there are negative bins...
                        for(int l=1; l<= nominal->GetNbinsX(); l++){
                            if(final_envelope_up  ->GetBinContent(l)<0.) nHistWithNegBins++;
                            if(final_envelope_down->GetBinContent(l)<0.) nHistWithNegBins++;
                        }
                        
                        std::cout << "Writing : " << final_envelope_up->GetName() << " and " << final_envelope_down->GetName()<< std::endl;
                        final_envelope_up->Write();
                        final_envelope_down->Write();
                        
                        std::cout << " Writing : " << final_shower_sys_histo_up->GetName() << std::endl;
                        final_shower_sys_histo_up->Write();
                      
                        
                        if(debug){
                            MakeDebugPlots(np_process_name,regionName,nominal, final_envelope_up, final_envelope_down,"mur_muf");
                            MakeDebugPlots(np_process_name,regionName,nominal, final_shower_sys_histo_up, nominal,"shower_sys");
                        }
                    }
                }
            }
            std::cout<<std::endl;
        }
    }
    if(nHistWithNegBins>0){
        std::cout<<"=========================================================================================="<<std::endl;
        std::cout<<"WARNING: "<<nHistWithNegBins<<" histograms with negative bin contents were detected."<<std::endl;
        std::cout<<"         Make sure if it's a problem or not."<<std::endl;
    }

    if(debug && out_file.is_open()) out_file.close();
    file->Close();
    std::cout<<"=========================================================================================="<<std::endl;
    std::cout<<"New file was saved: "<<filename<<std::endl;
    std::cout<<"=========================================================================================="<<std::endl;
    
    return 0;



    return 0;
}
