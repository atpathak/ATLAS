#include "LFVlephad/MXLG.h"
#include <limits>

// uncomment the following line to enable debug printouts
// #define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);

// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"

ClassImp(MXLG)

//______________________________________________________________________________________________

MXLG::MXLG(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

MXLG::~MXLG(){
  // default destructor
  DEBUGclass("destructor called");
} 

//______________________________________________________________________________________________
/*
void MXLG::load_pdf_xlg() const{
  TFile* file_pdf = new TFile("mtt_package/pdf_xlg.root", "read");
  for(int i=0; i<3; i++) {
    h_Minvvis_pperp[i] = (TH2F*) file_pdf -> Get(Form("h_Minvvis_pperp_%i", i));
  }
}
*/
//______________________________________________________________________________________________

double fitfunc(double* x, double* par){
  double mass = x[0];
  double massfit = par[0];
  double a = par[1];
  double b = par[2];
  double y = a*(1-b*pow(mass-massfit,2));
  return y;
}

//______________________________________________________________________________________________

void MXLG::cal_mtt_xlg(double& mtt_maxb, double& mtt_maxw, bool lfv, TLorentzVector P_vis[2], int vis_type[2], double MET_et[2], double sigma_MET, double mmax, double binw, int nsampling) const{

  TH2F* h_Minvvis_pperp[3];

  TFile* file_pdf = new TFile(m_filepath, "read");
  for(int i=0; i<3; i++) {
    h_Minvvis_pperp[i] = (TH2F*) file_pdf -> Get(Form("h_Minvvis_pperp_%i", i));
    h_Minvvis_pperp[i]->SetDirectory(0);
  }
  file_pdf->Close();

  double vis_pt[2];
  double vis_eta[2];
  double vis_phi[2];
  for(int i=0; i<2; i++){
    vis_pt[i] = P_vis[i].Perp();
    vis_eta[i]= P_vis[i].Eta();
    vis_phi[i]= P_vis[i].Phi();
  }
  double MET_etx = MET_et[0];
  double MET_ety = MET_et[1];


  int Nsampling = nsampling;
  if(lfv==0 && Nsampling<10000) Nsampling = 10000;
  if(lfv==1 && Nsampling<1000) Nsampling = 1000;
  double Minvov[2];
  double invp[2];
  double vispz[2];
  double invE[2];
  double visp[2];
  double visE[2];
  double visEcm[2];
  double cosvl[2];
  double ptau[2];
  double mtt;
  double costt = cos((P_vis[0].Vect()).Angle(P_vis[1].Vect()));
  double gammabeta[2];
  double pcm[2];
  double pzcm[2];
  double ptcm[2];
  int option[2] = {1, -1};
  double missetx;
  double missety;
  double MET_resolution = -1;
  MET_resolution = sigma_MET;
  int Nbins = mmax/binw;
  TH1F* h1 = new TH1F("h1", "", Nbins, 0, mmax);
  h1 -> Sumw2();
  TF1* func1 = new TF1("func1", fitfunc, 0, mmax, 3);
  
  for(int i1=0; i1<Nsampling; i1++){
    for(int j=0; j<2; j++){
      if(lfv == 1 && j == 1) continue;
      visp[j] = vis_pt[j]*cosh(vis_eta[j]);
      if(vis_type[j]<0) {
	h_Minvvis_pperp[0] -> GetRandom2(Minvov[j], ptcm[j]);
      }
      else if(vis_type[j]==1) {
	h_Minvvis_pperp[1] -> GetRandom2(Minvov[j], ptcm[j]);
      }
      else if(vis_type[j]==3) {
	h_Minvvis_pperp[2] -> GetRandom2(Minvov[j], ptcm[j]);
      }
      pcm[j] = (1.777*1.777 - Minvov[j]*Minvov[j])/2.0/1.777;
      pzcm[j] = sqrt(pcm[j]*pcm[j] - ptcm[j]*ptcm[j]);
      vispz[j] = sqrt(visp[j]*visp[j] - ptcm[j]*ptcm[j]);
      if(vis_type[j]<0) {
	visEcm[j] = pcm[j];
	visE[j] = visp[j];
      }
      else {
	visEcm[j] = sqrt(pcm[j]*pcm[j] + Minvov[j]*Minvov[j]);
	visE[j] = sqrt(visp[j]*visp[j] + Minvov[j]*Minvov[j]);
      }
    }

    
    for(int j0=0; j0<2; j0++){ 
      gammabeta[0] = (visEcm[0]*vispz[0]+option[j0]*visE[0]*pzcm[0])/(visEcm[0]*visEcm[0]-pzcm[0]*pzcm[0]);
      if(gammabeta[0]!=gammabeta[0] || gammabeta[0]<0) continue;
      ptau[0] = gammabeta[0]*1.777;
      invE[0] = sqrt(ptau[0]*ptau[0] + 1.777*1.777) - visE[0];
      if(vis_type[0]<0){
	invp[0] = sqrt(invE[0]*invE[0]-Minvov[0]*Minvov[0]);
      }
      else invp[0] = invE[0];

      for(int j1=0; j1<2; j1++){
	if(lfv == 1 && j1>0) continue;
	if(lfv == 0){
	  gammabeta[1] = (visEcm[1]*vispz[1]+option[j1]*visE[1]*pzcm[1])/(visEcm[1]*visEcm[1]-pzcm[1]*pzcm[1]);
	  if(gammabeta[1]!=gammabeta[1] || gammabeta[1]<0) continue;
	  //if(j0!=0 || j1!=0) continue;
	  ptau[1] = gammabeta[1]*1.777;
	  invE[1] = sqrt(ptau[1]*ptau[1] + 1.777*1.777) - visE[1];
	  if(vis_type[1]<0){
	    invp[1] = sqrt(invE[1]*invE[1]-Minvov[1]*Minvov[1]);
	  }
	  else invp[1] = invE[1];
	}
	double weight = 1.0;
	if(lfv == 0){
	  mtt = sqrt(2*1.777*1.777+2*sqrt(ptau[0]*ptau[0]+1.777*1.777)*sqrt(ptau[1]*ptau[1]+1.777*1.777)-2*ptau[0]*ptau[1]*costt);
	  missetx = invp[0]/cosh(vis_eta[0])*cos(vis_phi[0]) + invp[1]/cosh(vis_eta[1])*cos(vis_phi[1]);
	  missety = invp[0]/cosh(vis_eta[0])*sin(vis_phi[0]) + invp[1]/cosh(vis_eta[1])*sin(vis_phi[1]);
	}
	else {
	  mtt = sqrt(1.777*1.777+P_vis[1].M2()+2*sqrt(ptau[0]*ptau[0]+1.777*1.777)*P_vis[1].E()-2*ptau[0]*P_vis[1].Rho()*costt);  
	  missetx = invp[0]/cosh(vis_eta[0])*cos(vis_phi[0]);
	  missety = invp[0]/cosh(vis_eta[0])*sin(vis_phi[0]);
	}
	weight = 1.0/sqrt(2.0*M_PI)/MET_resolution*exp(-0.5*pow(missetx-MET_etx,2)/pow(MET_resolution,2))*1.0/sqrt(2.0*M_PI)/MET_resolution*exp(-0.5*pow(missety-MET_ety,2)/pow(MET_resolution,2));
	h1 -> Fill(mtt, weight);
      }
    }
  }
  
  if(h1->GetMaximumBin() == 1) {
    mtt_maxb = 0;
    mtt_maxw = 0;
    //cout<<"bad mtt_maxb = "<<mtt_maxb<<endl;
  }
  else {
    h1 -> Smooth();
    //two masses
    //1) maximum bin, thus depend upon binning
    int max_bin=h1->GetMaximumBin();
    mtt_maxb = h1 -> GetBinCenter(max_bin);
    //cout<<"mtt_maxb = "<<mtt_maxb<<endl;
    //2) calculate m(tautau) in a window around the maximum 
    //just like sliding window in ATLAS MMC
    int winHalfWidth = 2;

    int firstbin_nonzero = -1;
    int lastbin_nonzero = -1;
    for(int i=1; i<Nbins; i++){
      if(h1 -> GetBinContent(i)<=0) continue;
      firstbin_nonzero = i;
      break;
    }
    for(int i=Nbins; i>0; i--){
      if(h1->GetBinContent(i)<=0) continue;
      lastbin_nonzero = i;
      break;
    }
    firstbin_nonzero = firstbin_nonzero - winHalfWidth - 1;
    if(firstbin_nonzero < 1) firstbin_nonzero = 1;
    lastbin_nonzero = lastbin_nonzero + winHalfWidth + 1;
    if(lastbin_nonzero > Nbins) lastbin_nonzero = Nbins;
    //sliding widnow now to find max_bin with the maximal sum of weights in the window [max_bin-winHalfWidth, max_bin+winHalfWidth]
    const int nwidth = 2*winHalfWidth + 1;
    double winsum = 0;
    double winmax = 0;
    for(int i=1; i<=nwidth; i++) winsum += h1 -> GetBinContent(i);
    winmax = winsum;
    max_bin = 0;
    int ibinL = firstbin_nonzero;
    int ibinR = ibinL + 2*winHalfWidth;
    max_bin = (ibinL + ibinR)/2;
    while(ibinR<=lastbin_nonzero){
      ibinL ++;
      ibinR ++;
      double dweight = h1 -> GetBinContent(ibinR) - h1 -> GetBinContent(ibinL-1);
      winsum += dweight;
      if(winsum > winmax){
	winmax = winsum;
	max_bin = (ibinL+ibinR)/2;
      }
    }
    
    int iBinMin=max_bin-winHalfWidth;
    if (iBinMin<1) iBinMin=1;
    int iBinMax=max_bin+winHalfWidth;
    if (iBinMax>Nbins) iBinMax=Nbins;
    double sumw=0;
    double sumx=0;
    for (int iBin=iBinMin ; iBin<=iBinMax ; iBin++){
      double weight=h1->GetBinContent(iBin);
      sumw+=weight;
      sumx+=weight*h1->GetBinCenter(iBin);
    }
    double mtt_maxw_win=sumx/sumw;
    mtt_maxw = mtt_maxw_win;

    //to fit within the window
    /*double fitWidth = (winHalfWidth + 0.5)*binw;
    fitWidth = 15;//close to the mass resolution 15 GeV
    fitWidth = ((int)(h1 -> GetRMS()/5)+1)*5.0;
    fitWidth = int(h1->GetRMS()) + 1.0;

    func1 = new TF1("func1", fitfunc, mtt_maxw_win - fitWidth, mtt_maxw_win + fitWidth, 3);
    func1 -> SetParameters(mtt_maxw_win, sumw/binw, 0.001);
    func1 -> SetParLimits(0, 0, mmax);
    //TString fitOption = debug? "S" : "QNS";
    TString fitOption = "QNSL";
    //cout<<"before fit"<<endl;
    TFitResultPtr fitRes = h1 -> Fit(func1, fitOption, "", mtt_maxw_win - fitWidth, mtt_maxw_win + fitWidth);
    //cout<<"after fit"<<endl;
    double mtt_maxw_fit =-1;
    double a = 0;
    double b = 0;
    if(int(fitRes)==0) {
      mtt_maxw_fit = fitRes -> Parameter(0); 
      a = fitRes -> Parameter(1);
      b = fitRes -> Parameter(2);
    }
  
    if(mtt_maxw_fit > 0 && a*b>0 && fabs(mtt_maxw_fit- mtt_maxw_win)<fitWidth) mtt_maxw = mtt_maxw_fit;
    */
  }
  
  h_Minvvis_pperp[0]->Delete();
  h_Minvvis_pperp[1]->Delete();
  h_Minvvis_pperp[2]->Delete();
  h1->Delete();
  func1->Delete();
}

//______________________________________________________________________________________________

double MXLG::getValue() const{
  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering function");

  // the contents of this function highly depend on the way your
  // observable is supposed to retrieve (or create?) data
  // good luck, you're on your own here!

  int tau_0_n_tracks = this->tau_0_n_tracks->EvalInstance();
  double tau_0_pt = this->tau_0_pt->EvalInstance();
  double tau_0_eta = this->tau_0_eta->EvalInstance();
  double tau_0_phi = this->tau_0_phi->EvalInstance();
  double tau_0_m = this->tau_0_m->EvalInstance();
  double lep_0_pt = this->lep_0_pt->EvalInstance();
  double lep_0_eta = this->lep_0_eta->EvalInstance();
  double lep_0_phi = this->lep_0_phi->EvalInstance();
  double lep_0_m = this->lep_0_m->EvalInstance();
  double met_reco_sumet = this->met_reco_sumet->EvalInstance();
  double lephad_dphi = this->lephad_dphi->EvalInstance();
  double met_reco_etx = this->met_reco_etx->EvalInstance();
  double met_reco_ety = this->met_reco_ety->EvalInstance();

  int vis_type[2];
  TLorentzVector P_vis[2];
  double MET_et[2];
  double sigma_MET,mmax,binw,mtt_maxb,mtt_maxw;
  int nsampling;
  bool lfv;

  vis_type[0] = tau_0_n_tracks;
  vis_type[1] = -1;
  P_vis[0].SetPtEtaPhiM(tau_0_pt, tau_0_eta, tau_0_phi, tau_0_m);
  P_vis[1].SetPtEtaPhiM(lep_0_pt, lep_0_eta, lep_0_phi, lep_0_m);
  //sigma_MET = 1.0*sqrt(met_reco_sumet);
  sigma_MET = (4.42+0.64*sqrt(met_reco_sumet))*(0.89+0.1*lephad_dphi-0.14*log(lephad_dphi));
  MET_et[0] = met_reco_etx;
  MET_et[1] = met_reco_ety;
  nsampling = 1000;
  mmax = 500;
  binw = 2;
  mtt_maxb = 0;
  mtt_maxw = 0;
  lfv = 1;

  //this->load_pdf_xlg();

  this->cal_mtt_xlg(mtt_maxb, mtt_maxw, lfv, P_vis, vis_type, MET_et, sigma_MET, mmax, binw, nsampling);
  
  return mtt_maxw;
}

//______________________________________________________________________________________________

Long64_t MXLG::getCurrentEntry() const {
  // retrieve the current entry from the tree

  // since we don't have any tree or event pointer, there is usually
  // no way for us to know what entry we are currently looking
  // at. hence, we return -1 by default
  
  return -1;
}

//______________________________________________________________________________________________

TObjArray* MXLG::getBranchNames() const {
  // retrieve the list of branch names for this observable
  // ownership of the list belongs to the caller of the function
  DEBUGclass("retrieving branch names");
  TObjArray* bnames = LepHadObservable::getBranchNames();

  bnames->Add(new TObjString("tau_0_n_tracks"));
  bnames->Add(new TObjString("tau_0_pt"));
  bnames->Add(new TObjString("tau_0_eta"));
  bnames->Add(new TObjString("tau_0_phi"));
  bnames->Add(new TObjString("tau_0_m"));
  bnames->Add(new TObjString("lep_0_pt"));
  bnames->Add(new TObjString("lep_0_eta"));
  bnames->Add(new TObjString("lep_0_phi"));
  bnames->Add(new TObjString("lep_0_m"));
  bnames->Add(new TObjString("met_reco_sumet"));
  bnames->Add(new TObjString("lephad_dphi"));
  bnames->Add(new TObjString("met_reco_etx"));
  bnames->Add(new TObjString("met_reco_ety"));

  // since we don't have a tree pointer, we probably also don't need any branches
  return bnames;
}
//______________________________________________________________________________________________

bool MXLG::initializeSelf(){
  DEBUGclass("initializing");
  // initialize this observable

  if (!LepHadObservable::initializeSelf()) {
    return false;
  }

  this->tau_0_n_tracks = new TTreeFormula("tau_0_n_tracks", "tau_0_n_tracks", this->fTree);
  this->tau_0_pt = new TTreeFormula("tau_0_pt", "tau_0_pt", this->fTree);
  this->tau_0_eta = new TTreeFormula("tau_0_eta", "tau_0_eta", this->fTree);
  this->tau_0_phi = new TTreeFormula("tau_0_phi", "tau_0_phi", this->fTree);
  this->tau_0_m = new TTreeFormula("tau_0_m", "tau_0_m", this->fTree);
  this->lep_0_pt = new TTreeFormula("lep_0_pt", "lep_0_pt", this->fTree);
  this->lep_0_eta = new TTreeFormula("lep_0_eta", "lep_0_eta", this->fTree);
  this->lep_0_phi = new TTreeFormula("lep_0_phi", "lep_0_phi", this->fTree);
  this->lep_0_m = new TTreeFormula("lep_0_m", "lep_0_m", this->fTree);
  this->met_reco_sumet = new TTreeFormula("met_reco_sumet", "met_reco_sumet", this->fTree);
  this->lephad_dphi = new TTreeFormula("lephad_dphi", "lephad_dphi", this->fTree);
  this->met_reco_etx = new TTreeFormula("met_reco_etx", "met_reco_etx", this->fTree);
  this->met_reco_ety = new TTreeFormula("met_reco_ety", "met_reco_ety", this->fTree);

  return true;
}

//______________________________________________________________________________________________

bool MXLG::finalizeSelf(){
  // initialize this observable
  DEBUGclass("finalizing");
  // remember to undo anything you did in initializeSelf() !

  delete this->tau_0_n_tracks;
  delete this->tau_0_pt;
  delete this->tau_0_eta;
  delete this->tau_0_phi;
  delete this->tau_0_m;
  delete this->lep_0_pt;
  delete this->lep_0_eta;
  delete this->lep_0_phi;
  delete this->lep_0_m;
  delete this->met_reco_sumet;
  delete this->lephad_dphi;
  delete this->met_reco_etx;
  delete this->met_reco_ety;

  tau_0_n_tracks = NULL;
  tau_0_pt = NULL;
  tau_0_eta = NULL;
  tau_0_phi = NULL;
  tau_0_m = NULL;
  lep_0_pt = NULL;
  lep_0_eta = NULL;
  lep_0_phi = NULL;
  lep_0_m = NULL;
  met_reco_sumet = NULL;
  lephad_dphi = NULL;
  met_reco_etx = NULL;
  met_reco_ety = NULL;

  return true;
}
//______________________________________________________________________________________________

MXLG::MXLG(const TString& name, TString filepath):
LepHadObservable(name)
{
  // constructor with name argument
  DEBUGclass("constructor called with '%s'",name.Data());
  m_filepath = filepath;
}
