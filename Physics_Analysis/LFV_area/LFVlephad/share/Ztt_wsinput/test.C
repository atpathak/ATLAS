void test(){
  TFile*_f1 = TFile::Open("/afs/cern.ch/user/n/ndang/work/public/WSinput/lfv_lephad_WSinput_18_10_16_UnblindedData_fixedMGvsShZtt.root");
  TFile*_f2 = TFile::Open("lfv_lephad_ws_nominal_test1.root");
  TH1*h1 = (TH1F*)_f1->Get("mtau_cba_sr1_vbf_signal/Ztt/theory_ztt_MGvsSh_constrained_high"); h1->SetLineColor(1); h1->Print();
  TH1*h2 = (TH1F*)_f1->Get("mtau_cba_sr1_vbf_signal/Ztt/theory_ztt_MGvsSh_high"); h2->SetLineColor(4); h2->Print();
  TH1*h3 = (TH1F*)_f2->Get("mtau_cba_sr1_vbf_signal/Ztt/theory_ztt_MGvsSh_constrained_high"); h3->SetLineColor(2); h3->Print();
  TH1*h4 = (TH1F*)_f2->Get("mtau_cba_sr1_vbf_signal/Ztt/nominal"); h4->SetLineColor(2); h4->Print();

  //h1->Scale(1./h1->Integral());
  //h3->Scale(1./h3->Integral());

  TCanvas*c1=new TCanvas("c1");
  h1->Draw();
  h2->Draw("SAME");
  h3->Draw("SAME");
  h4->Draw("SAME");
  c1->SaveAs("test_mtau_cba_sr1_vbf_Ztt_comparision_2.png");
}
