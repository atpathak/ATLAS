#ifndef MyAnalysis_MyxAODAnalysis_H
#define MyAnalysis_MyxAODAnalysis_H

#include <EventLoop/Algorithm.h>
#include "AsgTools/AnaToolHandle.h"
#include "TH1.h"
#include "TH2.h"
#include "TString.h"
#include <string>
#include <TTree.h>
#include <TFile.h>

class MyxAODAnalysis : public EL::Algorithm
{
  // put your configuration variables here as public variables.
  // that way they can be set directly from CINT and python.
public:
  // float cutValue;

  int m_eventCounter; //!
  int m_numCleanEvents; //!
  

  // variables that don't get filled at submission time should be
  // protected from being send from the submission node to the worker
  // node (done by the //!)


 public:
  // Tree *myTree; //!
  // TH1 *myHist; //!
  // defining the output file name and tree that we will put in the output ntuple, also the one branch that will be in that tree 
  std::string outputName;
  TTree *tree; //!
  int      n_truth_tau;//!
  Double_t angle_vis_neu; //!
  Double_t angle_tau_vis; //!
  Double_t angle_tau_neu; //!
  Double_t tau_momentum; //!
  Double_t tau_pt; //!
  Double_t tau_m; //!
  Double_t tau_eta; //!
  Double_t tau_phi; //!
  double   vis_m ; //!
  double   vis_pt; //!
  double   vis_eta; //!
  double   vis_phi; //!
  Double_t vis_momentum; //!
  Double_t neu_pt; //!
  Double_t neu_m; //!
  Double_t neu_eta; //!
  Double_t neu_phi; //!
  Double_t neu_momentum; //!
  bool     IsHadronicTau; //!
  int      numCharged; //!
  int      numChargedPion; //!
  int      numNeutral; //!
  int      numNeutralPion; //!
  int      DecayMode; //!
 
  



  // this is a standard constructor
  MyxAODAnalysis ();

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();

  // this is needed to distribute the algorithm to the workers
  ClassDef(MyxAODAnalysis, 1);
};

#endif
