#include "TFile.h"
#include "TString.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>     /* atof */
#include "TROOT.h"
#include "TStyle.h"
#include "TMath.h"
#include "TF1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TGraphErrors.h"
#include "TFrame.h"
#include <TPaveStats.h>
#include "TAttText.h"
#include "TMultiGraph.h"
#include "TChain.h"
#include "TTree.h"
#include "TLatex.h"
#include <TCut.h>
#include <TLegend.h>
#include "TLorentzVector.h"
using namespace std;

int main(int argc, char* argv[]) {
  //
  cout << "Running in batch mode ... " << endl; gROOT->SetBatch(kTRUE);
  //
  int      n_truth_tau;//!
  bool     IsHadronicTau; //!
  int      numCharged; //!
  int      numNeutral; //!
  Double_t angle_vis_neu; //!
  Double_t tau_momentum; //!
  //
  TFile *fout = TFile::Open("Ztautau_8TeV.root","recreate");
  TTree *tree = new TTree ("tree", "tree");
  tree->SetDirectory (fout);
  tree->Branch("n_truth_tau",     &n_truth_tau);
  tree->Branch("angle_vis_neu",   &angle_vis_neu);
  tree->Branch("tau_momentum",    &tau_momentum);
  tree->Branch("IsHadronicTau",   &IsHadronicTau);
  tree->Branch("numCharged",      &numCharged);
  tree->Branch("numNeutral",      &numNeutral);
  //  
  TString treename="newtree"; 
  TChain* chain = new TChain(treename);
  //
  for (unsigned int ifile = 0 ; ifile < 1; ++ifile) {
    TString RootFiles = Form("root://eosatlas//eos/atlas/user/s/swaban/TauMCGen500M/TauMCGen_147818_%d_%d.root",ifile*100+1,ifile*100+100);
    cout << "Adding : " << RootFiles << endl;
    chain->Add(RootFiles);
  }
  //
  TTree *oldtree = chain;
  // Declaration of leaf types
  Int_t           event;
  Float_t         x_pt;
  Float_t         x_eta;
  Float_t         x_phi;
  Float_t         x_m;
  Int_t           x_id;
  Float_t         tau1_pt;
  Float_t         tau1_eta;
  Float_t         tau1_phi;
  Float_t         tau1_m;
  Int_t           tau1_id;
  Int_t           tau1_mode;
  Int_t           tau1_ndau;
  Float_t         tau2_pt;
  Float_t         tau2_eta;
  Float_t         tau2_phi;
  Float_t         tau2_m;
  Int_t           tau2_id;
  Int_t           tau2_mode;
  Int_t           tau2_ndau;
  Float_t         dau1_pt[5];
  Float_t         dau1_eta[5];
  Float_t         dau1_phi[5];
  Float_t         dau1_m[5];
  Int_t           dau1_id[5];
  Float_t         dau2_pt[5];
  Float_t         dau2_eta[5];
  Float_t         dau2_phi[5];
  Float_t         dau2_m[5];
  Int_t           dau2_id[5];
  //
  oldtree->SetBranchAddress("event",     &event);
  oldtree->SetBranchAddress("x_pt",      &x_pt);
  oldtree->SetBranchAddress("x_eta",     &x_eta);
  oldtree->SetBranchAddress("x_phi",     &x_phi);
  oldtree->SetBranchAddress("x_m",       &x_m);
  oldtree->SetBranchAddress("x_id",      &x_id);
  oldtree->SetBranchAddress("tau1_pt",   &tau1_pt);
  oldtree->SetBranchAddress("tau1_eta",  &tau1_eta);
  oldtree->SetBranchAddress("tau1_phi",  &tau1_phi);
  oldtree->SetBranchAddress("tau1_m",    &tau1_m);
  oldtree->SetBranchAddress("tau1_id",   &tau1_id);
  oldtree->SetBranchAddress("tau1_mode", &tau1_mode);
  oldtree->SetBranchAddress("tau1_ndau", &tau1_ndau);
  oldtree->SetBranchAddress("tau2_pt",   &tau2_pt);
  oldtree->SetBranchAddress("tau2_eta",  &tau2_eta);
  oldtree->SetBranchAddress("tau2_phi",  &tau2_phi);
  oldtree->SetBranchAddress("tau2_m",    &tau2_m);
  oldtree->SetBranchAddress("tau2_id",   &tau2_id);
  oldtree->SetBranchAddress("tau2_mode", &tau2_mode);
  oldtree->SetBranchAddress("tau2_ndau", &tau2_ndau);
  oldtree->SetBranchAddress("dau1_pt",    dau1_pt);
  oldtree->SetBranchAddress("dau1_eta",   dau1_eta);
  oldtree->SetBranchAddress("dau1_phi",   dau1_phi);
  oldtree->SetBranchAddress("dau1_m",     dau1_m);
  oldtree->SetBranchAddress("dau1_id",    dau1_id);
  oldtree->SetBranchAddress("dau2_pt",    dau2_pt);
  oldtree->SetBranchAddress("dau2_eta",   dau2_eta);
  oldtree->SetBranchAddress("dau2_phi",   dau2_phi);
  oldtree->SetBranchAddress("dau2_m",     dau2_m);
  oldtree->SetBranchAddress("dau2_id",    dau2_id);
  //
  //If nevtdo was provided as an argument, do at most that many events
  int ifirst   = 0;
  int nevtdo   = 0;
  int ndebug   = 0;
  Stat_t nevtot = (nevtdo==0) ? oldtree->GetEntries()-ifirst : TMath::Min(oldtree->GetEntries()-ifirst,(long long int)nevtdo);
  cout << " GetEntries = " << oldtree->GetEntries() << " nevtot = " << nevtot << " nevtdo = " << nevtdo << " ifirst = " << ifirst << endl;
  //
  int thisfile=0;
  int idummy=-1;
  bool verbose = false;
  for (unsigned int ievt = ifirst; ievt < ifirst+nevtot; ++ievt) { //Process each event
    //In case of a TChain, ientry is the entry number in the current file
    int ientry = oldtree->LoadTree(ievt);
    //In this case, it is the first entry in a root file, so print out its name
    if (ientry == 0) {
      ++thisfile;
      cout << "Reading from file (" << thisfile << ") : " << oldtree->GetCurrentFile()->GetName() << endl;
    }
    /* UNCOMMENT THIS LINE ONLY FOR VERY SHORT TESTS: */
    if (ievt==(ifirst+10)) break;
    //Read in this event
    oldtree->GetEntry(ievt);
    //
    verbose = false;
    ++idummy;
    if (idummy>-1 && idummy<ndebug) verbose=true;
    //
    if (ievt % 1000 == 0 ) verbose = true;
    //
    if (verbose) cout << "========> ievt = " << ievt << " idummy = " << idummy << " ndebug = " << ndebug << " " << tau1_mode << " " << tau2_mode << endl;
    //
    n_truth_tau = 2;
    for (unsigned int itau=0;itau<2;++itau) {
      // load each tau
      bool first = itau==0;
      Float_t         tau_pt = first ? tau1_pt : tau2_pt;
      Float_t         tau_eta = first ? tau1_eta : tau2_eta;
      Float_t         tau_phi = first ? tau1_phi : tau2_phi;
      Float_t         tau_m = first ? tau1_m : tau2_m;
      Int_t           tau_id = first ? tau1_id : tau2_id;
      Int_t           tau_mode = first ? tau1_mode : tau2_mode;
      Int_t           tau_ndau = first ? tau1_ndau : tau2_ndau;
      Float_t         dau_pt[5];
      Float_t         dau_eta[5];
      Float_t         dau_phi[5];
      Float_t         dau_m[5];
      Int_t           dau_id[5];
      for (unsigned int i = 0 ; i < 5 ; ++i ){
	dau_pt[i] = first ? dau1_pt[i] : dau2_pt[i];
	dau_eta[i] = first ? dau1_eta[i] : dau2_eta[i];
	dau_phi[i] = first ? dau1_phi[i] : dau2_phi[i];
	dau_m[i] = first ? dau1_m[i] : dau2_m[i];
	dau_id[i] = first ? dau1_id[i] : dau2_id[i];
      }
      // analyze each tau
      // first find out if it is hadronic tau decay or not
      IsHadronicTau = false;
      int nneu = 0;
      for (unsigned int i = 0 ; i < 5 ; ++i ){
	if (abs(dau_id[i]) == 12 || abs(dau_id[i]) == 14 || abs(dau_id[i]) == 16) nneu++;
      }
      if (nneu==1) {
	IsHadronicTau = true;
	if (tau_mode == 6) numCharged = 3;
	if (tau_mode == 3) numNeutral = 0;
	if (tau_mode == 4) numNeutral = 1;
	if (tau_mode == 5) numNeutral = 2;
	

      } else {	
	IsHadronicTau = false;
      }	
      //
      
    }
    
    if (tau1_mode==6 && tau2_mode==6) { // 3-prong-vs-3-prong
      
      TLorentzVector tau_p4[2]; 
      tau_p4[0].SetPtEtaPhiM(tau1_pt,tau1_eta,tau1_phi,tau1_m);
      tau_p4[1].SetPtEtaPhiM(tau2_pt,tau2_eta,tau2_phi,tau2_m);
      
      TLorentzVector neu_p4[2]     = {TLorentzVector(0,0,0,0), TLorentzVector(0,0,0,0)};
      TLorentzVector vis_p4[2]     = {TLorentzVector(0,0,0,0), TLorentzVector(0,0,0,0)};
      vector<TLorentzVector> dau_p4[2]  ;
      
      if (verbose) cout << "tau1_ndau = "  << tau1_ndau << endl;
      if (verbose) cout << "dau1_id = ";
      for (unsigned int i=0;i<tau1_ndau;++i) {
	//cout << dau1_id[i] << " ";
	if ( abs(dau1_id[i]) == 211 || abs(dau1_id[i]) == 111 ){
	  TLorentzVector tmp_p4; tmp_p4.SetPtEtaPhiM(dau1_pt[i],dau1_eta[i],dau1_phi[i],dau1_m[i]);
	  dau_p4[0].push_back(tmp_p4);
	  vis_p4[0] +=  tmp_p4;
	} else if ( abs(dau1_id[i]) == 16 ) {
	  TLorentzVector tmp_p4; tmp_p4.SetPtEtaPhiM(dau1_pt[i],dau1_eta[i],dau1_phi[i],dau1_m[i]);
	  neu_p4[0] +=  tmp_p4;
	}
      }
      if (verbose) cout << endl;
      
      if (verbose) cout << "vis_p4[0].Momentum : " << vis_p4[0].P() << endl;
      if (verbose) cout << "neu_p4[0].Momentum : " << neu_p4[0].P() << endl;
      
      if (verbose) cout << "tau2_ndau = "  << tau2_ndau << endl;
      if (verbose) cout << "dau2_id = ";
      for (unsigned int i=0;i<tau2_ndau;++i) {
	//cout << dau2_id[i] << " ";
	if ( abs(dau1_id[i]) == 211 || abs(dau1_id[i]) == 111 ){
	  TLorentzVector tmp_p4; tmp_p4.SetPtEtaPhiM(dau2_pt[i],dau2_eta[i],dau2_phi[i],dau2_m[i]);
	  dau_p4[1].push_back(tmp_p4);
	  vis_p4[1] +=  tmp_p4;
	} else if ( abs(dau1_id[i]) == 16 ) {
	  TLorentzVector tmp_p4; tmp_p4.SetPtEtaPhiM(dau1_pt[i],dau1_eta[i],dau1_phi[i],dau1_m[i]);
	  neu_p4[1] +=  tmp_p4;
	}
      }
      if (verbose) cout << endl;
      
      if (verbose) cout << "vis_p4[1].Momentum : " << vis_p4[1].P() << endl;
      if (verbose) cout << "neu_p4[1].Momentum : " << neu_p4[1].P() << endl;
      tree->Fill();
    }
    
  }
  
  fout->Close();
  
  delete chain;
  chain = 0;
  
}
