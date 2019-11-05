#include <TString.h>
#include <string.h>
#include <TROOT.h>
//#include <TChain.h>
#include <TFile.h>
#include <TList.h>
#include <TObject.h>
#include <TSystem.h>
#include <TAxis.h>
//#include <TLorentzVector.h>
//#include <iostream>
#include <vector>
//#include <TVector3.h>
#include <TH1.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <iostream>
#include <fstream>
void Plot(TString InputRootFile, TString InputRootDirName, TString HistName) {

  TH1* hist(0);
  
  TFile* file=TFile::Open(InputRootFile);
  TString FullHistName = Form("%s/%s", InputRootDirName.Data(), HistName.Data());
  hist = (TH1*) file->Get(FullHistName);
  hist->SetDirectory(0);
  //  hist->Print();
  file->Close();

  double xmin = 2.5;
  double xmax = 3.0;
  TAxis *axis = hist->GetXaxis();
  int bmin = axis->FindBin(xmin); 
  int bmax = axis->FindBin(xmax); 
  double integral = hist->Integral(bmin,bmax);

  if (integral > .25) {
    
    
    TCanvas* c1=new TCanvas("c1","c1");
    hist->GetXaxis()->SetRangeUser(0,6);
    hist->Draw();
    c1->SaveAs(Form("PlotBookDir/%s.pdf",HistName.Data()));
    delete c1;
    c1=0;

  }

}
int PlotBook(string fileNameIn = "ID",string slice = "all", string InputTextFile = "x0_plots_3_ID_Geantino_all_Services.txt"){

  TString InputRootFile = Form("../Simulate/%s/Geantino_%s/rad_intLength.histo.root",fileNameIn.c_str(),slice.c_str());

  cout << "InputRootFile : " << InputRootFile << endl;

  TString InputRootDirName = "radLen";
  
  cout << "InputRootDirName = " << InputRootDirName << endl;

  gSystem->Exec("rm -rf PlotBookDir ; mkdir -vp PlotBookDir");
  
  string line;
  ifstream myfile (InputTextFile);
  if (myfile.is_open()){
    while ( getline (myfile,line) ) {
      //      cout << line << '\n';
      TString HistName = line;
      HistName.Remove(TString::EStripType::kLeading,' ');
      HistName.Remove(TString::EStripType::kTrailing,' ');
      Plot(InputRootFile,InputRootDirName, HistName);
    }
    myfile.close();
  }
  //
  TString doit=Form("gs -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile=\"PlotBook.pdf\" PlotBookDir/*.pdf");
  cout << "doit : " << doit << endl;
  gSystem->Exec(doit);

  
  return 0;
  
  
}
