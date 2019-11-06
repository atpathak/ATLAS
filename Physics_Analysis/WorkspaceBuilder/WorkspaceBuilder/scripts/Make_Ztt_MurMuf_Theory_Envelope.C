//////////////////////////////////////////////////////////////////////////////////////
//  Make_Ztt_MurMuf_Theory_Envelope
//  Usage:
//  1. Load this macro in your ROOT interactive session.
//  2. Execute MakeTheoryEnvelope() with input root file as an argument.
//  3. You'll find a new file with "_WithTheoryEnv.root" in the same directory as your input file.
//  i.e.
//  root[] .L WorkspaceBuilder/scripts/Make_Ztt_MurMuf_Theory_Envelope.C
//  root[] MakeTheoryEnvelope("your_input.root")
//
//  NOTE: option=1  --> the maximum absolute difference is used both up/down variations.
//        option!=1 --> the maximum of upward(downward) variations is used both up(down) variations.
//
//  Created by Antonio De Maria on 26/04/17.
//////////////////////////////////////////////////////////////////////////////////////
#include "TFile.h"
#include "TKey.h"
#include "TMacro.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include "TH1.h"
#include "TH1F.h"
#include <sys/stat.h>

int MakeTheoryEnvelope(const char* input_filename, int option=1)
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
  // lh+hh
  std::string folders[] = {
  //ll CBA
  "llAll_cba_boost_loose_signal", "llAll_cba_boost_tight_signal", "llAll_cba_boost_top", "llAll_cba_boost_zll", "llAll_cba_vbf_loose_signal", "llAll_cba_vbf_tight_signal", "llAll_cba_vbf_top", "llAll_cba_vbf_zll",\
  //lh CBA
  "mhAll_cba_boost_loose_signal","mhAll_cba_boost_tight_signal","ehAll_cba_boost_loose_signal","ehAll_cba_boost_tight_signal","mhAll_cba_vbf_loose_signal","ehAll_cba_vbf_loose_signal","mhAll_cba_vbf_tight_signal","ehAll_cba_vbf_tight_signal","mhAll_cba_vbf_top","ehAll_cba_vbf_top","mhAll_cba_boost_top","ehAll_cba_boost_top","mhAll_mva_boost_signal","ehAll_mva_boost_signal","mhAll_mva_vbf_signal","ehAll_mva_vbf_signal","mhAll_mva_boost_top","ehAll_mva_boost_top","mhAll_mva_vbf_top","ehAll_mva_vbf_top",\
  //hh CBA
  "hhAll_cba_boost_loose_signal","hhAll_cba_boost_tight_signal","hhAll_cba_vbf_highdr_tight_signal","hhAll_cba_vbf_signal","hhAll_cba_boost_signal","hhAll_cba_vbf_highdr_loose_signal", "hhAll_cba_vbf_lowdr_signal","hhAll_mva_rest_control","hhAll_mva_vbf_signal","hhAll_mva_boost_signal","hhAll_preselection","hhAll_cba_boost_super_tight_signal","hhAll_cba_boost_tight_notsupertight_signal",\
  //hh MVA
  "hhAll_mva_rest_control","hhAll_mva_restmmc_control","hhAll_mva_restdphi_control","hhAll_mva_vbf_signal","hhAll_mva_boost_signal",
  };
  vector< std::string > vec_folders( folders, folders + ( sizeof ( folders ) /  sizeof ( std::string ) ) );
  std::vector<std::string>::iterator it_folders;

  // name of the variations
  std::string variation[] = {"mur2_muf1","mur1_muf05","mur05_muf05","mur1_muf2","mur2_muf2","mur05_muf1"};
  std::vector<std::string> vec_variations( variation, variation + ( sizeof ( variation ) /  sizeof ( std::string ) ) );

  vector<TH1F*> vec_histo;
  TH1F* nominal = new TH1F();
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

          // access to the Ztt folder for the specified region
          dir_name = dir_name + "/Ztt";
          if(sub_dir->cd(dir_name.c_str())){
            vec_histo.clear();
            std::string histo_path_nominal = dir_name + "/nominal";
            std::cout<<"Trying to retrieve "<<histo_path_nominal<<"..."<<std::endl;
            file->GetObject(histo_path_nominal.c_str(), nominal);
            if(nominal==NULL){
              std::cout<<"ERROR: cannot find histogram of "<<histo_path_nominal<<"."<<std::endl;
              return 1;
            }

            // get the variations
            for(std::vector<string>::size_type i = 0; i != vec_variations.size(); i++) {
              TH1F* curr_histo = new TH1F();
              std::string histo_path_var = dir_name + "/theory_ztt_lhe3weight_"+vec_variations[i]+"_pdf261000_high";
              std::cout<<"Trying to retrieve "<<histo_path_var<<"..."<<std::endl;
              file->GetObject(histo_path_var.c_str(), curr_histo);
              if(curr_histo==NULL){
                std::cout<<"ERROR: cannot find histogram of "<<histo_path_var<<"."<<std::endl;
                return 1;
              }
              vec_histo.push_back(curr_histo);
            }
            std::cout<<"All histograms were successfully retrieved."<<std::endl;
            std::cout<<"Going to make new envelope plots..."<<std::endl;

            std::string histo_envelope_path = dir_name + "/theory_ztt_mur_muf_envelope";
            Double_t orig_entries = nominal->GetEntries();
            TH1F* final_envelope_up = (TH1F*) nominal->Clone("theory_ztt_mur_muf_envelope_high");
            TH1F* final_envelope_down = (TH1F*) nominal->Clone("theory_ztt_mur_muf_envelope_low");
            // loop over the bins
            for(int j=1; j<= nominal->GetNbinsX(); j++){
              if(option == 1)
                {
                  double max_dev = 0;
                  double max_dev_tmp = 0;
                  // loop over the variations to get maximum shift from nominal
                  for(std::vector<TH1F*>::size_type i = 0; i != vec_histo.size(); i++) {
                    max_dev_tmp = std::fabs( nominal->GetBinContent(j) - vec_histo[i]->GetBinContent(j));
                    if(max_dev_tmp > max_dev)
                      {
                        max_dev = max_dev_tmp;
                        final_envelope_up->SetBinContent(j, nominal->GetBinContent(j) + max_dev);
                        final_envelope_down->SetBinContent(j, nominal->GetBinContent(j) - max_dev);
                      }
                  }
                  // print yield for debug
                  std::cout << " Bin : " << j << " nominal : " << nominal->GetBinContent(j) << " envelope up " << final_envelope_up->GetBinContent(j) << " envelope down " << final_envelope_down->GetBinContent(j)  << std::endl;
                  max_dev = 0;
                  max_dev_tmp = 0;
                }
              else{
                // less conservative solution up = max(up_var), down = max(down_var)
                double max_dev_up = 0;
                double max_dev_down = 0;
                double max_dev_tmp_up = 0;
                double max_dev_tmp_down = 0;
                for(std::vector<TH1F*>::size_type i = 0; i != vec_histo.size(); i++) {
                  // it's down variation
                  if( nominal->GetBinContent(j) - vec_histo[i]->GetBinContent(j) > 0)
                    {
                      max_dev_tmp_down = nominal->GetBinContent(j) - vec_histo[i]->GetBinContent(j);
                      if( max_dev_tmp_down > max_dev_down){
                        max_dev_down = max_dev_tmp_down;
                        final_envelope_down->SetBinContent(j, nominal->GetBinContent(j) - max_dev_down);
                      }
                    }
                  else{ // it's up variation
                    max_dev_tmp_up = vec_histo[i]->GetBinContent(j) - nominal->GetBinContent(j);
                    if( max_dev_tmp_up > max_dev_up){
                      max_dev_up = max_dev_tmp_up;
                      final_envelope_up->SetBinContent(j, nominal->GetBinContent(j) + max_dev_up);
                    }
                  }
                }
                std::cout << " bin : " << j << " nominal : " << nominal->GetBinContent(j) << " envelope up " << final_envelope_up->GetBinContent(j) << " envelope down " << final_envelope_down->GetBinContent(j)  << std::endl;
                max_dev_up = 0;
                max_dev_down = 0;
                max_dev_tmp_up = 0;
                max_dev_tmp_down = 0;
              }
            }
            //Set entries to the original value
            final_envelope_up->SetEntries(orig_entries);
            final_envelope_down->SetEntries(orig_entries);

            //Check if there are negative bins...
            for(int j=1; j<= nominal->GetNbinsX(); j++){
              if(final_envelope_up  ->GetBinContent(j)<0.) nHistWithNegBins++;
              if(final_envelope_down->GetBinContent(j)<0.) nHistWithNegBins++;
            }

            std::cout << "Writing : " << final_envelope_up->GetName() << " and " << final_envelope_down->GetName()<< std::endl;
            final_envelope_up->Write();
            final_envelope_down->Write();

          }
        }
      }
      std::cout<<std::endl;
    }

  if(nHistWithNegBins>0){
    std::cout<<"=========================================================================================="<<std::endl;
    std::cout<<"WARNING: "<<nHistWithNegBins<<" histograms with negative bin contents were detected."<<std::endl;
    std::cout<<"         Make sure if it's a problem or not."<<std::endl;
  }

  file->Close();
  std::cout<<"=========================================================================================="<<std::endl;
  std::cout<<"New file was saved: "<<filename<<std::endl;
  std::cout<<"=========================================================================================="<<std::endl;

  return 0;
}
